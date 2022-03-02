/*************************************************************************************************/
/*
 *  Copyright (c) 2021, PACKETCRAFT, INC.
 *  All rights reserved.
 */
/*************************************************************************************************/

/*
 * Redistribution and use of the Audio subsystem for nRF5340 Software, in binary
 * and source code forms, with or without modification, are permitted provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code form must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary code form, except as embedded into a Nordic
 *    Semiconductor ASA nRF53 chip or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of Packetcraft, Inc. nor Nordic Semiconductor ASA nor
 *    the names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA nRF53 chip.
 *
 * 5. Any software provided in binary or source code form under this license
 *    must not be reverse engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY PACKETCRAFT, INC. AND NORDIC SEMICONDUCTOR ASA
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL PACKETCRAFT, INC.,
 * NORDIC SEMICONDUCTOR ASA, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "audio_datapath.h"

#include <zephyr.h>
#include <nrfx_clock.h>

#include "macros_common.h"
#include "board.h"
#include "led.h"
#include "audio_i2s.h"
#include "sw_codec_select.h"
#include "audio_sync_timer.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(audio_datapath, CONFIG_LOG_AUDIO_DATAPATH_LEVEL);

/*
 * Terminology
 *   - sample: signed integer of audio waveform amplitude
 *   - sample FIFO: circular array of raw audio samples
 *   - block: set of raw audio samples exchanged with I2S
 *   - frame: encoded audio packet exchanged with connectivity
 */

#if (CONFIG_SW_CODEC_LC3_7_5_MS_FRAMESIZE)
#define FRAME_DURATION_US 7500
#else
#define FRAME_DURATION_US 10000
#endif /* (CONFIG_SW_CODEC_LC3_7_5_MS_FRAMESIZE) */

#define TIMESTAMP_DELTA_MAX_ERR_US (int)(FRAME_DURATION_US * 0.001)

#define BLK_PERIOD_US 1000

/* Total sample FIFO period in microseconds */
#define FIFO_SMPL_PERIOD_US 40000
#define FIFO_NUM_BLKS	    NUM_BLKS(FIFO_SMPL_PERIOD_US)
#define MAX_FIFO_SIZE	    (FIFO_NUM_BLKS * BLK_SIZE_SAMPLES(CONFIG_AUDIO_SAMPLE_RATE_HZ) * 2)

/* Number of audio blocks given a duration */
#define NUM_BLKS(d) ((d) / BLK_PERIOD_US)
/* Single audio block size in number of samples (stereo) */
#define BLK_SIZE_SAMPLES(r) (((r)*BLK_PERIOD_US) / 1000000)
/* Increment sample FIFO index by one block */
#define NEXT_IDX(i) (((i) < (FIFO_NUM_BLKS - 1)) ? ((i) + 1) : 0)
/* Decrement sample FIFO index by one block */
#define PREV_IDX(i) (((i) > 0) ? ((i)-1) : (FIFO_NUM_BLKS - 1))

#define NUM_BLKS_IN_FRAME     NUM_BLKS(FRAME_DURATION_US)
#define NUM_SAMPS_IN_BLK_MONO BLK_SIZE_SAMPLES(CONFIG_AUDIO_SAMPLE_RATE_HZ)
/* Number of octets in a single audio block */
#define BLK_MONO_SIZE_OCTETS (NUM_SAMPS_IN_BLK_MONO * sizeof(int16_t))
/* How much data to be collected before moving on with drift compensation */
#define LOCAL_NUM_DATA_PTS (DRIFT_MEAS_PERIOD_US / BLK_PERIOD_US)
/* How much data to be collected before moving on with presentation compensation */
#define REMOTE_NUM_DATA_PTS (DRIFT_MEAS_PERIOD_US / FRAME_DURATION_US)

/* Audio clock - nRF5340 Analog Phase-Locked Loop (APLL) */
#define APLL_FREQ_CENTER 39854
#define APLL_FREQ_MIN	 36834
#define APLL_FREQ_MAX	 42874
/* Use nanoseconds to reduce rounding errors */
#define APLL_FREQ_ADJ(t) (-((t)*1000) / 331)

#define DRIFT_COMP		true
#define DRIFT_MEAS_PERIOD_US	100000
#define DRIFT_ERR_THRESH_LOCK	16
#define DRIFT_ERR_THRESH_UNLOCK 32

#define PRES_COMP_ENABLE true
/* Presentation delay in microseconds */
#define PRES_DLY_US		10000
#define PRES_ERR_THRESH_LOCK_US 1000

typedef enum {
	DRFT_STATE_INIT, /* Wireless data path initialization - Initialize drift compensation */
	DRFT_STATE_CALIB, /* Calibrate and zero out local delay */
	DRFT_STATE_OFFSET, /* Adjust I2S offset relative to SDU Reference */
	DRFT_STATE_LOCKED /* Drift compensation locked - Minor corrections */
} drift_comp_state_t;

static const char *const drift_comp_state_names[] = {
	"INIT",
	"CALIB",
	"OFFSET",
	"LOCKED",
};

typedef enum {
	PRES_STATE_INIT, /* Wireless data path initialization - Initialize presentation compensation */
	PRES_STATE_MEAS, /* Measure presentation delay */
	PRES_STATE_WAIT, /* Wait for some time */
	PRES_STATE_LOCKED /* Presentation compensation locked */
} pres_comp_state_t;

static const char *const pres_comp_state_names[] = {
	"INIT",
	"MEAS",
	"WAIT",
	"LOCKED",
};

static struct {
	bool datapath_initialized;
	bool stream_started;

	void *decoded_data;

	struct {
		data_fifo_t *fifo;
	} in;

	struct {
		int16_t __aligned(sizeof(uint32_t)) fifo[MAX_FIFO_SIZE];
		uint16_t prod_blk_idx; /* Output producer audio block index */
		uint16_t cons_blk_idx; /* Output consumer audio block index */
		uint32_t meas_pres_dly_us;
		uint32_t prod_blk_ts[FIFO_NUM_BLKS];
		/* Statistics */
		uint32_t total_frames;
		uint32_t total_prod_blks;
		uint32_t total_cons_blks;
		uint32_t total_blk_underruns;
	} out;

	struct {
		uint32_t last_ts;
	} remote;

	struct {
		uint32_t last_ts;
	} local;

	struct {
		drift_comp_state_t state : 8;
		uint16_t ctr; /* Counter for collected data points */
		uint32_t meas_start_time_us;
		uint32_t center_freq;
	} drift_adj;

	struct {
		pres_comp_state_t state : 8;
		uint16_t ctr; /* Counter for collected data points (counter also used in waiting state) */
		int32_t err_dly_us;
		int32_t sum_err_dly_us;
	} pres_adj;
} ctrl_blk;

static void hfclkaudio_set(uint16_t freq_value)
{
#if (DRIFT_COMP)
	uint16_t freq_val = freq_value;

	freq_val = MIN(freq_val, APLL_FREQ_MAX);
	freq_val = MAX(freq_val, APLL_FREQ_MIN);
	nrfx_clock_hfclkaudio_config_set(freq_val);
#endif /* (DRIFT_COMP) */
}

static void drift_comp_state_set(drift_comp_state_t new_state)
{
	if (new_state == ctrl_blk.drift_adj.state) {
		return;
	}

	ctrl_blk.drift_adj.state = new_state;
	LOG_INF("Drft comp state: %s", drift_comp_state_names[new_state]);
}

static void audio_datapath_drift_compensation(void)
{
	switch (ctrl_blk.drift_adj.state) {
	case DRFT_STATE_INIT: {
		if (++ctrl_blk.drift_adj.ctr < LOCAL_NUM_DATA_PTS) {
			/* Same state - Allow wireless to initialize data path */
			return;
		}

		if (ctrl_blk.remote.last_ts) {
			ctrl_blk.drift_adj.ctr = 0;
			ctrl_blk.drift_adj.meas_start_time_us = ctrl_blk.remote.last_ts;

			drift_comp_state_set(DRFT_STATE_CALIB);
		}
		break;
	}
	case DRFT_STATE_CALIB: {
		if (++ctrl_blk.drift_adj.ctr < LOCAL_NUM_DATA_PTS) {
			/* Same state - Collect more data */
			return;
		}

		ctrl_blk.drift_adj.ctr = 0;

		int32_t err_us = DRIFT_MEAS_PERIOD_US -
				 (ctrl_blk.remote.last_ts - ctrl_blk.drift_adj.meas_start_time_us);
		int32_t freq_adj = APLL_FREQ_ADJ(err_us);

		ctrl_blk.drift_adj.center_freq = APLL_FREQ_CENTER + freq_adj;

		hfclkaudio_set(ctrl_blk.drift_adj.center_freq);

		drift_comp_state_set(DRFT_STATE_OFFSET);
		break;
	}
	case DRFT_STATE_OFFSET: {
		if (++ctrl_blk.drift_adj.ctr < LOCAL_NUM_DATA_PTS) {
			/* Same state - Collect more data */
			return;
		}

		ctrl_blk.drift_adj.ctr = 0;

		int32_t err_us = (ctrl_blk.remote.last_ts - ctrl_blk.local.last_ts) % BLK_PERIOD_US;

		if (err_us > (BLK_PERIOD_US / 2)) {
			err_us = err_us - BLK_PERIOD_US;
		}

		int32_t freq_adj = APLL_FREQ_ADJ(err_us);

		hfclkaudio_set(ctrl_blk.drift_adj.center_freq + freq_adj);

		if ((err_us < DRIFT_ERR_THRESH_LOCK) && (err_us > -DRIFT_ERR_THRESH_LOCK)) {
			drift_comp_state_set(DRFT_STATE_LOCKED);
		}

		break;
	}
	case DRFT_STATE_LOCKED: {
		if (++ctrl_blk.drift_adj.ctr < LOCAL_NUM_DATA_PTS) {
			/* Same state - Collect more data */
			return;
		}

		ctrl_blk.drift_adj.ctr = 0;

		int32_t err_us = (ctrl_blk.remote.last_ts - ctrl_blk.local.last_ts) % BLK_PERIOD_US;

		if (err_us > (BLK_PERIOD_US / 2)) {
			err_us = err_us - BLK_PERIOD_US;
		}

		/* Use asymptotic correction with small errors */
		err_us /= 2;
		int32_t freq_adj = APLL_FREQ_ADJ(err_us);

		hfclkaudio_set(ctrl_blk.drift_adj.center_freq + freq_adj);

		if ((err_us > DRIFT_ERR_THRESH_UNLOCK) || (err_us < -DRIFT_ERR_THRESH_UNLOCK)) {
			drift_comp_state_set(DRFT_STATE_OFFSET);
		}

		break;
	}
	default: {
		break;
	}
	}
}

static void pres_comp_state_set(pres_comp_state_t new_state)
{
	int ret;

	if (new_state == ctrl_blk.pres_adj.state) {
		return;
	}

	ctrl_blk.pres_adj.state = new_state;
	LOG_INF("Pres comp state: %s", pres_comp_state_names[new_state]);
	if (new_state == PRES_STATE_LOCKED) {
		ret = led_on(LED_APP_2_GREEN);
	} else {
		ret = led_off(LED_APP_2_GREEN);
	}
	ERR_CHK(ret);
}

static int32_t audio_datapath_presentation_compensation(uint32_t exp_dly_us)
{
	if (ctrl_blk.drift_adj.state != DRFT_STATE_LOCKED) {
		/* Unconditionally reset state machine if drift compensation looses lock */
		pres_comp_state_set(PRES_STATE_INIT);
		return 0;
	}

	int32_t pres_adj_us = 0;

	switch (ctrl_blk.pres_adj.state) {
	case PRES_STATE_INIT: {
		ctrl_blk.pres_adj.ctr = 0;
		ctrl_blk.pres_adj.sum_err_dly_us = 0;
		pres_comp_state_set(PRES_STATE_MEAS);
		break;
	}
	case PRES_STATE_MEAS: {
		if (ctrl_blk.pres_adj.ctr++ < REMOTE_NUM_DATA_PTS) {
			ctrl_blk.pres_adj.sum_err_dly_us +=
				exp_dly_us - ctrl_blk.out.meas_pres_dly_us;

			/* Same state - Collect more data */
			break;
		}

		ctrl_blk.pres_adj.ctr = 0;

		ctrl_blk.pres_adj.err_dly_us =
			ctrl_blk.pres_adj.sum_err_dly_us / REMOTE_NUM_DATA_PTS;

#if (PRES_COMP_ENABLE)
		pres_adj_us = ctrl_blk.pres_adj.err_dly_us;
#endif /* (PRES_COMP_ENABLE) */

		/* Restart measurement */
		ctrl_blk.pres_adj.sum_err_dly_us = 0;

		if ((pres_adj_us >= (BLK_PERIOD_US / 2)) || (pres_adj_us <= -(BLK_PERIOD_US / 2))) {
			pres_comp_state_set(PRES_STATE_WAIT);
			break;
		}

		/* Advance state if presentation compensation result is within bounds otherwise restart measurement */
		if ((ctrl_blk.pres_adj.err_dly_us < PRES_ERR_THRESH_LOCK_US) &&
		    (ctrl_blk.pres_adj.err_dly_us > -PRES_ERR_THRESH_LOCK_US)) {
			/* Drift compensation will always be in DRFT_STATE_LOCKED here */

			pres_comp_state_set(PRES_STATE_LOCKED);
		}

		break;
	}
	case PRES_STATE_WAIT: {
		if (ctrl_blk.pres_adj.ctr++ > (FIFO_SMPL_PERIOD_US / FRAME_DURATION_US)) {
			pres_comp_state_set(PRES_STATE_INIT);
		}

		break;
	}
	case PRES_STATE_LOCKED: {
		/*
		 * Presentation delay compensation moves into PRES_STATE_WAIT if an audio
		 * frame is missed or into PRES_STATE_INIT if drift compensation unlocks.
		 */

		break;
	}
	default: {
		break;
	}
	}

	return pres_adj_us;
}

/*
 * This handler function is called every time I2S needs new buffers for
 * TX and RX data.
 *
 * The new TX data buffer is the next consumer block in out.fifo.
 *
 * The new RX data buffer is the first empty slot of in.fifo.
 * New I2S RX data is located in rx_buf_released, and is locked into
 * the in.fifo message queue.
 */
static void audio_datapath_i2s_blk_complete(uint32_t ts, uint32_t *rx_buf_released)
{
	static bool underrun_condition;

	/*** Presentation delay measurement ***/

	ctrl_blk.out.meas_pres_dly_us = ts - ctrl_blk.out.prod_blk_ts[ctrl_blk.out.cons_blk_idx];

	/*** TX ***/

	/* Double buffered index */
	uint32_t next_out_blk_idx = NEXT_IDX(ctrl_blk.out.cons_blk_idx);

	if (next_out_blk_idx != ctrl_blk.out.prod_blk_idx) {
		/* Only increment if not in underrun condition */
		ctrl_blk.out.cons_blk_idx = next_out_blk_idx;
		if (underrun_condition) {
			underrun_condition = false;
			LOG_WRN("Data received, total underruns: %d",
				ctrl_blk.out.total_blk_underruns);
		}
	} else {
		/* Repeat audio block */
		next_out_blk_idx = ctrl_blk.out.cons_blk_idx;
		/* Don't print if we haven't started receiving data yet */
		if (ctrl_blk.remote.last_ts != 0) {
			if (!underrun_condition ||
			    ((ctrl_blk.out.total_blk_underruns % 1000) == 0)) {
				LOG_WRN("In I2S TX underrun condition, total: %d",
					ctrl_blk.out.total_blk_underruns);
			}
			underrun_condition = true;
			ctrl_blk.out.total_blk_underruns++;
		}
	}

	uint8_t *tx_buf =
		(uint8_t *)&ctrl_blk.out.fifo[next_out_blk_idx * NUM_SAMPS_IN_BLK_MONO * 2];

	/*** RX ***/

	int ret;
	static int prev_ret;
	uint32_t *rx_buf;

	/* Lock last filled buffer into message queue */
	if (rx_buf_released != NULL) {
		ret = data_fifo_block_lock(ctrl_blk.in.fifo, (void **)&rx_buf_released,
					   BLOCK_SIZE_BYTES);

		ERR_CHK_MSG(ret, "Unable to lock block RX");
	}

	/* Get new empty buffer to send to I2S HW */
	ret = data_fifo_pointer_first_vacant_get(ctrl_blk.in.fifo, (void **)&rx_buf, K_NO_WAIT);
	if (ret == 0 && prev_ret == -ENOMEM) {
		LOG_WRN("I2S RX continuing stream");
		prev_ret = ret;
	}

	/* If RX FIFO is filled up */
	if (ret == -ENOMEM) {
		void *data;
		size_t size;

		if (ret != prev_ret) {
			LOG_WRN("I2S RX overrun. Single msg");
			prev_ret = ret;
		}

		ret = data_fifo_pointer_last_filled_get(ctrl_blk.in.fifo, &data, &size, K_NO_WAIT);
		ERR_CHK(ret);

		ret = data_fifo_block_free(ctrl_blk.in.fifo, &data);
		ERR_CHK(ret);

		ret = data_fifo_pointer_first_vacant_get(ctrl_blk.in.fifo, (void **)&rx_buf,
							 K_NO_WAIT);
	}

	ERR_CHK_MSG(ret, "RX failed to get block");

	/*** Data exchange ***/

	audio_i2s_set_next_buf(tx_buf, rx_buf);

	/*** Drift compensation ***/

	ctrl_blk.local.last_ts = ts;
	audio_datapath_drift_compensation();
}

static void audio_datapath_i2s_start(void)
{
	int ret;

	/* Double buffer I2S */
	uint8_t *tx_buf_one;
	uint8_t *tx_buf_two;
	uint32_t *rx_buf_one;
	uint32_t *rx_buf_two;

	/* TX */

	ctrl_blk.out.cons_blk_idx = PREV_IDX(ctrl_blk.out.cons_blk_idx);
	tx_buf_one =
		(uint8_t *)&ctrl_blk.out.fifo[ctrl_blk.out.cons_blk_idx * NUM_SAMPS_IN_BLK_MONO * 2];

	ctrl_blk.out.cons_blk_idx = PREV_IDX(ctrl_blk.out.cons_blk_idx);
	tx_buf_two =
		(uint8_t *)&ctrl_blk.out.fifo[ctrl_blk.out.cons_blk_idx * NUM_SAMPS_IN_BLK_MONO * 2];

	/* RX */

	uint32_t alloced_cnt;
	uint32_t locked_cnt;

	ret = data_fifo_num_used_get(ctrl_blk.in.fifo, &alloced_cnt, &locked_cnt);
	if (alloced_cnt || locked_cnt || ret) {
		ERR_CHK_MSG(-ENOMEM, "Fifo is not empty!");
	}

	ret = data_fifo_pointer_first_vacant_get(ctrl_blk.in.fifo, (void **)&rx_buf_one, K_NO_WAIT);
	ERR_CHK_MSG(ret, "RX failed to get block");
	ret = data_fifo_pointer_first_vacant_get(ctrl_blk.in.fifo, (void **)&rx_buf_two, K_NO_WAIT);
	ERR_CHK_MSG(ret, "RX failed to get block");

	/* Start I2S */

	audio_i2s_start(tx_buf_one, rx_buf_one);
	audio_i2s_set_next_buf(tx_buf_two, rx_buf_two);
}

static void audio_datapath_i2s_stop(void)
{
	audio_i2s_stop();
}

void audio_datapath_stream_out(const uint8_t *buf, size_t size, uint32_t sdu_ref, bool bad_frame)
{
	if (ctrl_blk.stream_started) {
		uint32_t cur_time = audio_sync_timer_curr_time_get();

		if (sdu_ref == ctrl_blk.remote.last_ts) {
			LOG_WRN("Duplicate timestamp - Dropping audio frame: sdu_ref=%d", sdu_ref);
			return;
		}

		/*** Drift control ***/

		uint32_t missing_blk = 0;

		if (buf && ctrl_blk.remote.last_ts) {
			uint32_t last_out_delta_us = sdu_ref - ctrl_blk.remote.last_ts;

			/* Guard against invalid timestamps */
			if (last_out_delta_us < (FRAME_DURATION_US + (FRAME_DURATION_US / 2))) {
				if ((last_out_delta_us >
				     (FRAME_DURATION_US + TIMESTAMP_DELTA_MAX_ERR_US)) ||
				    (last_out_delta_us <
				     (FRAME_DURATION_US - TIMESTAMP_DELTA_MAX_ERR_US))) {
					LOG_WRN("Invalid timestamp delta: %d", last_out_delta_us);

					/* Estimate timestamp */
					last_out_delta_us = FRAME_DURATION_US;
					sdu_ref = ctrl_blk.remote.last_ts + last_out_delta_us;
				}
			}

			if (last_out_delta_us > (BLK_PERIOD_US / 2)) {
				uint32_t last_out_blk =
					(last_out_delta_us - (BLK_PERIOD_US / 2)) /
					BLK_PERIOD_US; /* Ensure use of low average */
				missing_blk =
					(last_out_blk / NUM_BLKS_IN_FRAME) * NUM_BLKS_IN_FRAME;
			} else {
				return;
			}
		}

		/* Move presentation compensation into PRES_STATE_WAIT if audio frame is missed */
		if (missing_blk) {
			LOG_WRN("Missed audio frame");
			ctrl_blk.pres_adj.ctr = 0;
			pres_comp_state_set(PRES_STATE_WAIT);
		}

		if (buf) {
			ctrl_blk.remote.last_ts = sdu_ref;
		} else {
			LOG_WRN("Missed audio packet");

			/* Estimate timestamp */
			ctrl_blk.remote.last_ts += FRAME_DURATION_US;
		}

		/* When to play received audio */
		int32_t exp_dly_us = PRES_DLY_US - (cur_time - sdu_ref);
		int32_t pres_adj_us = audio_datapath_presentation_compensation(exp_dly_us);

		if (pres_adj_us >= 0) {
			pres_adj_us += (BLK_PERIOD_US / 2);
		} else {
			pres_adj_us += -(BLK_PERIOD_US / 2);
		}

		/* The number of adjustment blocks is 0 as long as |pres_adj_us| < BLK_PERIOD_US */
		int32_t pres_adj_blks = pres_adj_us / BLK_PERIOD_US;

		if (pres_adj_blks > (FIFO_NUM_BLKS / 2)) {
			/* Limit adjustment */
			pres_adj_blks = FIFO_NUM_BLKS / 2;

			LOG_WRN("Requested presentation delay out of range: pres_adj_us=%d, total_frames=%u",
				pres_adj_us, ctrl_blk.out.total_frames);
		} else if (pres_adj_blks < -(FIFO_NUM_BLKS / 2)) {
			/* Limit adjustment */
			pres_adj_blks = -(FIFO_NUM_BLKS / 2);

			LOG_WRN("Requested presentation delay out of range: pres_adj_us=%d, total_frames=%u",
				pres_adj_us, ctrl_blk.out.total_frames);
		}

		/* Adjust Sample FIFO with missing blocks */
		pres_adj_blks += missing_blk;

		if (pres_adj_blks > 0) {
			LOG_DBG("Presentation delay inserted to output audio stream: pres_adj_blks=%d, total_frames=%u",
				pres_adj_blks, ctrl_blk.out.total_frames);

			/* Increase presentation delay */
			for (int i = 0; i < pres_adj_blks; i++) {
				/* Mute audio frame */
				memset(&ctrl_blk.out.fifo[ctrl_blk.out.prod_blk_idx *
							  NUM_SAMPS_IN_BLK_MONO * 2],
				       0, BLK_MONO_SIZE_OCTETS * 2);

				/* Record producer block start reference */
				ctrl_blk.out.prod_blk_ts[ctrl_blk.out.prod_blk_idx] =
					cur_time - ((pres_adj_blks - i) * BLK_PERIOD_US);

				ctrl_blk.out.prod_blk_idx = NEXT_IDX(ctrl_blk.out.prod_blk_idx);
				ctrl_blk.out.total_prod_blks++;
			}
		} else if (pres_adj_blks < 0) {
			LOG_DBG("Presentation delay removed from output audio stream: pres_adj_blks=%d, total_frames=%u",
				pres_adj_blks, ctrl_blk.out.total_frames);

			/* Reduce presentation delay */
			for (int i = 0; i > pres_adj_blks; i--) {
				ctrl_blk.out.prod_blk_idx = PREV_IDX(ctrl_blk.out.prod_blk_idx);
			}
		}

		/*** Decode ***/

		if (buf) {
			int ret;
			size_t pcm_size; /* Not currently in use */

			ret = sw_codec_decode(buf, size, bad_frame, &ctrl_blk.decoded_data,
					      &pcm_size);

			if (ret && !bad_frame) {
				LOG_WRN("SW codec decode error: %d", ret);
			}
		}

		ctrl_blk.out.total_frames++;

		/*** Output ***/

		int32_t out_dly_blks = ctrl_blk.out.prod_blk_idx - ctrl_blk.out.cons_blk_idx;

		if ((out_dly_blks + NUM_BLKS_IN_FRAME) > FIFO_NUM_BLKS) {
			LOG_WRN("Output audio stream overrun: total_prod_blks=%u",
				ctrl_blk.out.total_prod_blks);

			/* Discard frame to allow consumer to catch up */
			return;
		}

		uint32_t out_blk_idx = ctrl_blk.out.prod_blk_idx;

		for (uint32_t i = 0; i < NUM_BLKS_IN_FRAME; i++) {
			memcpy(&ctrl_blk.out.fifo[out_blk_idx * NUM_SAMPS_IN_BLK_MONO * 2],
			       &((int16_t *)ctrl_blk.decoded_data)[i * NUM_SAMPS_IN_BLK_MONO * 2],
			       BLK_MONO_SIZE_OCTETS * 2);

			/* Record producer block start reference */
			ctrl_blk.out.prod_blk_ts[out_blk_idx] = cur_time + (i * BLK_PERIOD_US);

			out_blk_idx = NEXT_IDX(out_blk_idx);
		}

		/* Advance */
		ctrl_blk.out.prod_blk_idx = out_blk_idx;
		ctrl_blk.out.total_prod_blks += NUM_BLKS_IN_FRAME;
	} else {
		LOG_WRN("Stream not started");
	}
}

int audio_datapath_start(data_fifo_t *fifo_rx)
{
	__ASSERT_NO_MSG(fifo_rx != NULL);

	if (ctrl_blk.datapath_initialized) {
		if (!ctrl_blk.stream_started) {
			ctrl_blk.in.fifo = fifo_rx;

			/* Clear counters and mute initial audio */
			memset(&ctrl_blk.out, 0, sizeof(ctrl_blk.out));

			audio_datapath_i2s_start();
			ctrl_blk.stream_started = true;

			return 0;
		} else {
			return -EALREADY;
		}
	} else {
		LOG_WRN("Audio datapath not initialized");
		return -ECANCELED;
	}
}

int audio_datapath_stop(void)
{
	if (ctrl_blk.stream_started) {
		ctrl_blk.stream_started = false;
		audio_datapath_i2s_stop();
		ctrl_blk.remote.last_ts = 0;

		pres_comp_state_set(PRES_STATE_INIT);

		return 0;
	} else {
		return -EALREADY;
	}
}

int audio_datapath_init(void)
{
	memset(&ctrl_blk, 0, sizeof(ctrl_blk));
	audio_i2s_blk_comp_cb_register(audio_datapath_i2s_blk_complete);
	ctrl_blk.datapath_initialized = true;

	return 0;
}
