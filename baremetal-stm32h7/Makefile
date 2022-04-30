CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
BIN = arm-none-eabi-objcopy
STL = st-flash
CFLAGS = -mthumb -mcpu=cortex-m7

all: app.bin

startup_stm32h743xx.o: startup_stm32h743xx.c
	$(CC) $(CFLAGS) -c -o startup_stm32h743xx.o startup_stm32h743xx.c

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

app.elf: stm32h7_flash.ld startup_stm32h743xx.o main.o
	$(LD) -T stm32h7_flash.ld -o app.elf startup_stm32h743xx.o main.o

app.bin: app.elf
	$(BIN) -O binary app.elf app.bin

clean:
	rm -f *.o *.elf *.bin

flash: app.bin
	$(STL) write app.bin 0x8000000

erase:
	$(STL) erase
