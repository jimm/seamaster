NAME = seamaster
# DEBUG = -DDEBUG -DDEBUG_STDERR

WXFLAGS := $(shell wx-config --cxxflags)
WXLIBS := $(shell wx-config --libs)

CPP = $(shell wx-config --cxx)
CPPFLAGS += -std=c++11 -MD -MP -g $(DEBUG) $(WXFLAGS)

LD = $(shell wx-config --ld)
LIBS = -lc -lc++ -lportmidi $(WXLIBS)
LDFLAGS += $(LIBS) $(WXLIBS)

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

SRC = $(wildcard src/*.cpp) $(wildcard src/wx/*.cpp)
OBJS = $(SRC:%.cpp=%.o)
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJS = $(TEST_SRC:%.cpp=%.o)
TEST_OBJ_FILTERS = src/wx/app_main.o

.PHONY: all test tags clean distclean

all: $(NAME)

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

-include $(SRC:%.cpp=%.d)
-include $(TEST_SRC:%.cpp=%.d)

test: $(NAME)_test
	./$(NAME)_test

$(NAME)_test:	$(OBJS) $(TEST_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(filter-out $(TEST_OBJ_FILTERS),$^)

install:	$(bindir)/$(NAME)

$(bindir)/$(NAME):	$(NAME)
	cp ./$(NAME) $(bindir)
	chmod 755 $(bindir)/$(NAME)

tags:	TAGS

TAGS:	$(SRC)
	etags $(SRC)

clean:
	rm -f $(NAME) $(NAME)_test src/*.o src/wx/*.o test/*.o

distclean: clean
	rm -f src/*.d src/wx/*.d test/*.d
