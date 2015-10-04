#
# Copyright (c) 2015 Sergi Granell (xerpi)
# based on Cirne's vita-toolchain test Makefile
#

TARGET = vitahelloshader
OBJS   = main.o 

LIBS = -lc -lSceKernel_stub -lSceDisplay_stub -lSceGxm_stub	\
	-lSceCtrl_stub -lSceTouch_stub -lSceShaccCg_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
READELF = $(PREFIX)-readelf
OBJDUMP = $(PREFIX)-objdump
CFLAGS  = -Wl,-q -Wall -O3 -I$(VITASDK)/include -L$(VITASDK)/lib -L. -std=gnu99
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)

all: $(TARGET).velf

%.velf: %.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@ $(VITASDK)/bin/db.json $(VITASDK)/bin/extra.json sceshaccgc.json

$(TARGET).elf: $(OBJS) libSceShaccCg_stub.a
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

libSceShaccCg_stub.a: 
	mkdir -p SceShaccCg
	vita-libs-gen sceshaccgc.json SceShaccCg
	make -C SceShaccCg ARCH=arm-vita-eabi 
	cp SceShaccCg/libSceShaccCg_stub.a .
clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS) libSceShaccCg_stub.a SceShaccCg/libSceShaccCg_stub.a

copy: $(TARGET).velf
	@cp $(TARGET).velf ~/shared/vitasample.elf
	@echo "Copied!"

run: $(TARGET).velf
	@sh run_homebrew_unity.sh $(TARGET).velf
