# Unit test example
In order to run a unit test locally:
west build -b qemu_cortex_m3 -t run

Alternatively:

west build -b qemu_cortex_m3
cd build
ninja run
