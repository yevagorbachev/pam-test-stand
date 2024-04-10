board = arduino:avr:mega
port = /dev/ttyACM0
baud = 115200
app = ADS
# NOT .cpp.extra_flags!
printf_flags_option = compiler.c.elf.extra_flags
printf_flags = -Wl,-u,vfprintf -lprintf_flt -lm
def_flags_option = compiler.cpp.extra_flags
def_flags = 

src = $(wildcard ./src/*/)
src_flag = $(foreach srcdir, $(src),--library $(srcdir) )
lib = HX711 ADS1220_WE
lib_flag = $(foreach libname, $(lib),--library $(libname))

all: 
	arduino-cli compile -b $(board) \
		--build-property $(printf_flags_option)="$(printf_flags)" \
		--build-property $(def_flags_option)="$(def_flags)" \
		$(src_flag)
flash: all
	arduino-cli upload -p $(port) -b $(board) -v
	make monitor
monitor:
	# minicom -D $(port) -b $(baud)
	minicom arduino
