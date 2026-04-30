## RISC-V Emulator
usage:
```bash
./riscv_emu.bin program.hex
```
where `program.hex` is expected to be an [Intel HEX (wiki)](https://en.wikipedia.org/wiki/Intel_HEX) binary compiled for the RV32I instruction set.

## UI Controls
F9 - step<br>
F3 - cycle register file data representation hex/decimal/binary<br>
F2 - enter address for start of memory display window<br>
F5 - dump memory contents to a `memdump.hex` file<br>
F12 - restart emulator<br>
F1 - exit<br>

## Building the Emulator
### Dependencies
The UI is implemented with `ncurses`.
To compile the test cases `cunit` is required.<br>
<br>
Debian, Ubuntu:
```bash
apt-get update && apt-get install -y libcunit1-dev libncurses-dev
```
Arch:
```bash
pacman -Sy cunit ncurses
```
### Build Emulator & Build Tests
For the main binary run `make` inside the project root folder.
To compile the unit tests:
```bash
cd cunit && make
```