NAME = seamaster
# DEBUG = -DDEBUG
CPPFLAGS += -MD -MP -g $(DEBUG)
LIBS = -lc -lncurses -lportmidi -lsqlite3
LDFLAGS += $(LIBS) -macosx_version_min 10.10

SRC = $(wildcard src/*.c) $(wildcard src/curses/*.c)
OBJS = $(SRC:%.c=%.o)
TEST_SRC = $(wildcard test/*.c)
TEST_OBJS = $(TEST_SRC:%.c=%.o)
TEST_OBJ_FILTERS = src/seamaster.o

.PHONY: all
all: $(NAME)

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

-include $(SRC:%.c=%.d)

.PHONY: test
test: $(NAME)_test
	./$(NAME)_test

# Move "real" .o files out of the way so mocks with same name will be picked up.
$(NAME)_test:	$(OBJS) $(TEST_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(filter-out $(TEST_OBJ_FILTERS),$^)

.PHONY: clean
clean:
	rm -f $(NAME) $(NAME)_test src/*.o src/curses/*.o test/*.o

.PHONY: distclean
distclean: clean
	rm -f src/*.d src/curses/*.d test/*.d
