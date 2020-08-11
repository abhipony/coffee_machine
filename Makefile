OBJ := $(shell /usr/bin/find . -path ./.git -prune -o -name '*.o' -print)
STATIC_FILES := $(shell /usr/bin/find . -path ./.git -prune -o -name '*.a' -print)
SRC := $(shell /usr/bin/find src -maxdepth 1 -name '*.cpp' -print)
SRC_SRC := $(shell /usr/bin/find src -maxdepth 2 -name 'main.cpp' -print)
EXE := $(patsubst %.cpp, % , $(subst src, binary, $(SRC)))
EXE += $(patsubst %/main, % , $(patsubst %.cpp, %, $(subst src, binary, $(SRC_SRC))))

TEST_EXE :=

DEBUG = -g
LFLAGS = -Wall $(DEBUG)
LDLIBS :=

LDLIBS += -lpthread
LDLIBS += -lstdc++fs

CPPFLAGS:=

CPPFLAGS += -I .

CXXFLAGS := -Wall -Werror -Wno-psabi

MY_COMPILE.cpp = $(CXX) $(CXXFLAGS) $(CPPFLAGS)
MY_OUTPUT_OPTION = -o $@
                                                                                 
%.o: %.cpp 
	$(MY_COMPILE.cpp) -c $(MY_OUTPUT_OPTION) $<

%.a: %.mk
	./ar_wrapper $@ $(filter-out %.mk, $^)

all: exe test_exes
all_then_test: all test_exes
	@+./run_tests

#Also include any files with a.mk extension
SUB_MAKES += $(shell /usr/bin/find . -name '*.mk' -print)

ifneq ($(SUB_MAKES),)
    include $(SUB_MAKES)
endif

exe: $(EXE)
test_exes: $(TEST_EXE)
report_test_exes:
	@for t in "$(TEST_EXE)"; do echo "$$t"; done 


$(shell mkdir -p binary)
