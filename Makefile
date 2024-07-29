# metadata
APP = kernel
CCVERSION = 12.2.1

# output files
EXE  = ${APP}.elf
MAP  = ${APP}.map
LIST = ${APP}.list
IMG  = ${APP}.img
HEX  = ${APP}.hex

# toolchain
PREFIX = arm-none-eabi-
CC = ${PREFIX}gcc
AS = ${PREFIX}as
LD = ${PREFIX}ld
OBJCPY = ${PREFIX}objcopy
OBJDMP = ${PREFIX}objdump

# build options
CFLAGS  = -Wall -march=armv7-a -mtune=cortex-a7 -g -O1
LDFLAGS = -L/usr/lib/gcc/arm-none-eabi/${CCVERSION}/ -lgcc

# input files
LDSCRIPT = linker.ld
SOURCES := $(wildcard *.c *.s)
OBJECTS := $(addsuffix .o,$(basename $(SOURCES)))

.PHONY: all
all: ${EXE} ${LIST} ${IMG} ${HEX}

.PHONY: rebuild
rebuild: clean all

${EXE}: ${OBJECTS}
	${LD} -T ${LDSCRIPT} -M=${MAP} ${OBJECTS} -o $@ ${LDFLAGS}

${LIST}: ${EXE}
	${OBJDMP} -std ${EXE} > ${LIST}

${IMG}: ${EXE}
	${OBJCPY} -O binary ${EXE} ${IMG}

${HEX}: ${EXE}
	${OBJCPY} -O ihex ${EXE} ${HEX}

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

.s.o:
	${AS} -g -o $@ $<

.PHONY: debug
debug: ${EXE}
	if ! pgrep openocd >/dev/null ; then \
		openocd -f rpi2.cfg & \
	fi
	gdb-multiarch -ex "target extended-remote:3333" \
                  -ex "load" \
                      ${EXE}

.PHONY: qemu
qemu:
	if ! pgrep qemu-system-arm >/dev/null ; then \
	    qemu-system-arm -s -S -M raspi2b \
                        -kernel ${EXE} & \
	fi
	gdb-multiarch -ex "set architecture arm" \
                  -ex "target extended-remote :1234" \
                  -ex "load" \
                      ${EXE}

.PHONY: clean
clean:
	rm -f *.o ${EXE} ${HEX} ${MAP} ${LIST} ${IMG}
	-killall openocd qemu-system-arm gdb-multiarch

