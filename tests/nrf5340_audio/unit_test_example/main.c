/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

int func_add(int a, int b)
{
	return a + b;
}

void test_unit_1(void)
{
	zassert_equal(func_add(1, 2), 3, "fail");
}

void test_main(void)
{
	ztest_test_suite(test_suite_unit, ztest_unit_test(test_unit_1));
	ztest_run_test_suite(test_suite_unit);
}
