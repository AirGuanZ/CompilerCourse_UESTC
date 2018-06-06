CC = clang++
CC_FLAGS = -std=c++11 -O2 -Wall -Werror

CPP_SRC_FILES = $(shell find . -name "*.cpp")
CPP_OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_SRC_FILES))
CPP_DPT_FILES = $(patsubst %.cpp, %.d, $(CPP_SRC_FILES))

DST = ./build/parser

$(DST) : $(CPP_OBJ_FILES)
	$(CC) $^ -o $(DST)

%.o : %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $@

%.d : %.cpp
	@set -e; \
	rm -f $@; \
	$(CC) -MM $< $(CC_INCLUDE_FLAGS) > $@.$$$$.dtmp; \
	sed 's,\(.*\)\.o\:,$*\.o $*\.d\:,g' < $@.$$$$.dtmp > $@; \
	rm -f $@.$$$$.dtmp

-include $(CPP_DPT_FILES)

clean :
	rm -f $(DST)
	rm -f $(CPP_OBJ_FILES)
	rm -f $(CPP_DPT_FILES)
	rm -f $(shell find . -name "*.dtmp")

	# 测试文件
	rm -f *.dyd
	rm -f *.varfil
	rm -f *.profil
	rm -f *.err

run :
	make
	$(DST) test.pas
