# metadata
APP = pifork
CCVERSION = 12.2.1

# output
OBJDIR = obj
OUTDIR = rpi-firmware
EXE  = ${APP}.elf
MAP  = ${APP}.map
LIST = ${APP}.list
HEX  = ${APP}.hex
IMG  = ${OUTDIR}/${APP}.img

# toolchain
PREFIX = arm-none-eabi-
CC = ${PREFIX}gcc
AS = ${PREFIX}as
LD = ${PREFIX}ld
OBJCPY = ${PREFIX}objcopy
OBJDMP = ${PREFIX}objdump

# build options
CFLAGS  = -Wall -march=armv7-a -mtune=cortex-a7 -g -O1
LDFLAGS = -L/usr/lib/gcc/arm-none-eabi/${CCVERSION}/ -lgcc -L/lib/arm-none-eabi/newlib/ -lc

# input files
SRC = src
EXAMPLE = wait
LDSCRIPT = linker.ld

SOURCES := $(wildcard ${SRC}/**/*.c ${SRC}/*.c ${SRC}/**/*.s ${SRC}/*.s) examples/${EXAMPLE}.c
OBJECTS := $(addprefix ${OBJDIR}/,$(addsuffix .o,$(basename ${SOURCES})))

.PHONY: all
all: ${EXE} ${LIST} ${IMG} ${HEX}

.PHONY: rebuild
rebuild: clean all

${OBJDIR}:
	@mkdir -p ${OBJDIR}

${EXE}: ${OBJECTS}
	${LD} -T ${LDSCRIPT} -M=${MAP} ${OBJECTS} -o $@ ${LDFLAGS}

${LIST}: ${EXE}
	${OBJDMP} -std ${EXE} > ${LIST}

${IMG}: ${EXE}
	${OBJCPY} -O binary ${EXE} ${IMG}

${HEX}: ${EXE}
	${OBJCPY} -O ihex ${EXE} ${HEX}

${OBJDIR}/%.o: %.c | ${OBJDIR}
	@mkdir -p $(dir $@)
	${CC} ${CFLAGS} -c -o $@ $<

${OBJDIR}/%.o: %.s | ${OBJDIR}
	@mkdir -p $(dir $@)
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
qemu: ${EXE}
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
	rm -rf ${OBJDIR} ${EXE} ${HEX} ${MAP} ${LIST} ${IMG}
	-killall openocd qemu-system-arm gdb-multiarch

