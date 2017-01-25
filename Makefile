# DEBUG = -DDEBUG
CPPFLAGS += -MD -MP -g $(DEBUG)
LIBS = -lc -lncurses -lportmidi -lsqlite3
LDFLAGS += $(LIBS) -macosx_version_min 10.10

SRC = $(wildcard src/*.c) $(wildcard src/curses/*.c)

.PHONY: all
all: seamaster

seamaster: $(SRC:%.c=%.o)
	$(LD) $(LDFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

.PHONY: clean
clean:
	rm -f seamaster src/*.o src/curses/*.o

.PHONY: distclean
distclean: clean
	rm -f src/*.d src/curses/*.d
