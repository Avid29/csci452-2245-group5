#
# Makefile fragment for the bootstrap component of the system.
# 
# THIS IS NOT A COMPLETE Makefile - run GNU make in the top-level
# directory, and this will be pulled in automatically.
#

SUBDIRS += boot

###################
#  FILES SECTION  #
###################

BOOT_SRC := boot/boot.S

BOOT_OBJ := $(BUILDDIR)/boot/boot.o

BLDFLAGS := -Ttext 0x0 -s --oformat binary -e begtext

###################
#  RULES SECTION  #
###################

bootstrap: $(BUILDDIR)/boot/boot

$(BUILDDIR)/boot/%.o:	boot/%.c $(BUILDDIR)/.vars.CFLAGS
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/boot/%.o:	boot/%.S $(BUILDDIR)/.vars.CFLAGS
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -o $(@D)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(@D)/$*.s -a=$(@D)/$*.asm
	$(RM) -f $(@D)/$*.s
	$(NM) -n $(@D)/$*.o > $(@D)/$*.sym

$(BUILDDIR)/boot/boot: $(BUILDDIR)/boot/boot.o
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $(BLDFLAGS) -o $@ $(BOOT_OBJ)
