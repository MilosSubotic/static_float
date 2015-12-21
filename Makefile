#
###############################################################################

CXX=clang++

SOURCES := static_float_test.cpp

CXXFLAGS += -std=c++11 -g -O0

# GLM
CPPFLAGS += -I.

# GMP
CPPFLAGS += -DHAVE_GMP=1
LIBS += -lgmpxx -lgmp

# clang/llvm 
CLANGXX=clang++
LLVM_DIS=llvm-dis

###############################################################################

.PHONY: all
all: static_float_test.elf

.PHONY: run
run: static_float_test.elf
	./$<

static_float_test.elf: ${SOURCES:%.cpp=%.o}
	${CXX} -o $@ $^ ${LDFLAGS} ${LIBS}

*.o: Makefile

static_float_test.o: *.h

########################################

.PHONY: ci
ci:
	ci ${CXXFLAGS} ${CPPFLAGS} static_float_test.cpp

########################################

.PHONY: dis
dis: static_float_test.S

static_float_test.S: static_float_test.elf
	objdump -dS $< > $@ 

########################################

.PHONY: ast
ast: ast.txt
ast.txt: ast_test.cpp Makefile
	${CLANGXX} -cc1 -ast-dump -ast-dump-filter "asdf" \
		-std=c++1y -fcxx-exceptions \
		-I/usr/include/c++/4.7 \
		-I/usr/include/c++/4.7/x86_64-linux-gnu \
		-I/usr/include/c++/4.7/backward \
		-I/usr/lib/gcc/x86_64-linux-gnu/4.7/include \
		-I/usr/local/include \
		-I/usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed \
		-I/usr/include/x86_64-linux-gnu \
		-I/usr/include \
		$< > $@

########################################

.PHONY: ll
ll: ast_test.ll
ast_test.ll: ast_test.bc
	${LLVM_DIS} -o $@ $<

ast_test.bc: ast_test.cpp Makefile
	${CLANGXX} -O3 -std=c++1y -emit-llvm -c -o $@ $<

########################################

.PHONY: clean
clean:
	rm -rf *.o *.elf *.bc *.ll *.S *.txt

###############################################################################
