


aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/_start.S -o build_tmp/start.o

aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/core0.c -o build_tmp/core0.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/core1.c -o build_tmp/core1.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/core2.c -o build_tmp/core2.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/core3.c -o build_tmp/core3.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/memory.c -o build_tmp/memory.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/gpio.c -o build_tmp/gpio.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/screen.c -o build_tmp/screen.o

aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/eq.c -o build_tmp/eq.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/comp.c -o build_tmp/comp.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/ngate.c -o build_tmp/ngate.o


aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/comm.c -o build_tmp/comm.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/timing.c -o build_tmp/timing.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/mbox.c -o build_tmp/mbox.o
aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a72+fp+simd -c src/math.c -o build_tmp/math.o



aarch64-none-elf-ld -r -b binary -o build_tmp/font_psf.o src/font.psf

aarch64-none-elf-ld -nostdlib -nostartfiles build_tmp/start.o build_tmp/font_psf.o build_tmp/core0.o build_tmp/core1.o build_tmp/core2.o build_tmp/core3.o build_tmp/memory.o build_tmp/math.o build_tmp/screen.o build_tmp/gpio.o build_tmp/eq.o build_tmp/comp.o build_tmp/ngate.o build_tmp/comm.o build_tmp/timing.o build_tmp/mbox.o  -T src/_memmap.ld -o build_tmp/kernel8.elf


aarch64-none-elf-objdump -D build_tmp/kernel8.elf > build_tmp/kernel_disassemble.txt

aarch64-none-elf-objcopy -O binary build_tmp/kernel8.elf result_to_sd/kernel8.img


copy /Y .\result_to_sd\kernel8.img c:\TFTP-Root\c01111\kernel8.img

copy /Y .\result_to_sd\config.txt c:\TFTP-Root\c01111\config.txt