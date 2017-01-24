CC = clang
CPPFLAGS += -MD -MP -g
LIBS = -lc -lncurses -lportmidi -lsqlite3
LDFLAGS += $(LIBS) -macosx_version_min 10.10

SRC = $(wildcard src/*.c) $(wildcard src/curses/*.c)

seamaster: $(SRC:%.c=%.o)
	$(LD) $(LDFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

clean:
	rm -f seamaster src/*.o src/curses/*.o

distclean: clean
	rm -f src/*.d src/curses/*.d
