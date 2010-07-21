#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	DSOrganize
export TOPDIR		:=	$(CURDIR)


.PHONY: arm7/$(TARGET).arm7 arm9/$(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: bin $(TARGET).ds.gba

bin:
	[ -d $@ ] || mkdir -p $@

$(TARGET).ds.gba	: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	arm7/$(TARGET).arm7 arm9/$(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9 -o banner.bmp -b logo.bmp "DSOrganize;Shaun Taylor;Version 3.2" 
	cp $(TARGET).nds _BOOT_MP.nds
	cp $(TARGET).nds DSOrganize-gnm.nds
	cp $(TARGET).nds DSOrganize-mk2.nds
	cp $(TARGET).nds moonshell.nds
	dlditool gmtf.dldi DSOrganize-gnm.nds
	dlditool nmmc.dldi DSOrganize-mk2.nds
	dlditool CycloEvo.dldi moonshell.nds

#---------------------------------------------------------------------------------
arm7/$(TARGET).arm7	: arm7/$(TARGET).elf
arm9/$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9