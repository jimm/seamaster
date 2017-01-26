# DEBUG = -DDEBUG
CPPFLAGS += -MD -MP -g $(DEBUG)
LIBS = -lc -lncurses -lportmidi -lsqlite3
LDFLAGS += $(LIBS) -macosx_version_min 10.10

SRC = $(wildcard src/*.c) $(wildcard src/curses/*.c)
TEST_SRC = $(filter-out src/seamaster.c src/input.c src/output.c,$(SRC)) $(wildcard test/*.c)

.PHONY: all
all: seamaster

seamaster: $(SRC:%.c=%.o)
	$(LD) $(LDFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

.PHONY: test
test: seamaster_test
	./seamaster_test

seamaster_test:	$(TEST_SRC:%c=%o)
	if [ -f src/input.o ] ; then mv src/input.o /tmp ; fi
	if [ -f src/output.o ] ; then mv src/output.o /tmp ; fi
	$(LD) $(LDFLAGS) -o $@ $^
	if [ -f /tmp/input.o ] ; then mv /tmp/input.o src ; fi
	if [ -f /tmp/output.o ] ; then mv /tmp/output.o src ; fi

.PHONY: clean
clean:
	rm -f seamaster src/*.o src/curses/*.o test/*.o

.PHONY: distclean
distclean: clean
	rm -f src/*.d src/curses/*.d test/*.d
