# Unix
fntdump: fntdump.c
	cc fntdump.c -o fntdump

# TURBO C 3.0
fntdump.exe: fntdump.c
	tc fntdump.c -o fntdump.exe

clean:
	-rm fntdump fntdump.o fntdump.obj fntdump.exe
