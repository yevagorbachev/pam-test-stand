board = arduino:avr:mega
port = /dev/ttyACM0
baud = 115200
app = HX
# NOT .cpp.extra_flags!
printf_flags_option = compiler.c.elf.extra_flags
printf_flags = -Wl,-u,vfprintf -lprintf_flt -lm
def_flags_option = compiler.cpp.extra_flags
def_flags = -DAPP_$(app)
libraries = $(foreach lib,$(wildcard ./src/*/),--library $(lib) )

all: 
	arduino-cli compile -b $(board) \
		--build-property $(printf_flags_option)="$(printf_flags)" \
		--build-property $(def_flags_option)="$(def_flags)" \
		$(libraries)
flash: all
	arduino-cli upload -p $(port) -b $(board) -v
	make monitor
monitor:
	# minicom -D $(port) -b $(baud)
	minicom arduino
