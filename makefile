board = arduino:avr:mega
port = /dev/ttyACM0
baud = 115200

# NOT .cpp.extra_flags!
printf_flags_option = compiler.c.elf.extra_flags
printf_flags = -Wl,-u,vfprintf -lprintf_flt -lm

src = $(wildcard ./src/*/)
src_flag = $(foreach srcdir, $(src),--library $(srcdir) )

all: 
	arduino-cli compile -b $(board) \
		--build-property $(printf_flags_option)="$(printf_flags)" \
		--build-property $(def_flags_option)="$(def_flags)" \
		$(src_flag)

clean: 
	arduino-cli compile --clean

flash: all
	arduino-cli upload -p $(port) -b $(board) -v
	make monitor

sync:
	cp data/* ~/.projects/mckibben/Data -v

monitor:
	minicom arduino
