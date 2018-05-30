csrc = $(wildcard src/*.c)
ssrc = src/startup/arm9entry.s $(wildcard src/*.s)
Ssrc = $(wildcard src/startup/*.S) $(wildcard src/*.S)
obj = $(csrc:.c=.o) $(ssrc:.s=.o) $(Ssrc:.S=.o)

csrc-arm7 = $(wildcard src/arm7/*.c)
ssrc-arm7 = src/startup/arm7entry.s $(wildcard src/arm7/*.s)
obj-arm7 = $(csrc-arm7:.c=.o) $(ssrc-arm7:.s=.o)

name = nds-mtglife
bin = $(name).nds

ARCH = arm-none-eabi-
CPP = $(ARCH)cpp
CC = $(ARCH)gcc
AS = $(ARCH)as
OBJCOPY = $(ARCH)objcopy

EMU = desmume-cli

#opt = -O3 -fomit-frame-pointer -mcpu=arm946e-s -mtune=arm946e-s
opt = -O0 -fomit-frame-pointer -mcpu=arm946e-s -mtune=arm946e-s
dbg = -g

CFLAGS = -mthumb $(opt) $(dbg)
LDFLAGS = -nostartfiles -Wl,--gc-sections -lm

$(bin): arm9.elf arm7.elf data/icon.bmp
	ndstool -c $@ -9 arm9.elf -7 arm7.elf -b data/icon.bmp "$(name);mindlapse"

arm9.elf: $(obj)
	$(CC) -o $@ $(obj) -Wl,-T,ds_arm9.mem -Wl,-T,ds_arm9.ld $(LDFLAGS)

arm7.elf: $(obj-arm7)
	$(CC) -o $@ $(obj-arm7) -Wl,-T,ds_arm7.ld $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(obj-arm7) $(bin) arm9.elf arm7.elf $(dep)

.PHONY: simrun
simrun: $(bin)
	$(EMU) $(EMUFLAGS) $(bin)

.PHONY: debug
debug: $(bin)
	$(EMU) --arm9gdb=1234 $(bin) >/dev/null 2>/dev/null &
	$(ARCH)gdb ./arm9.elf

data/icon.bmp: data/icon.bmp.base64
	base64 -d $< >$@
