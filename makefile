INC_DIR = include
SRC_DIR = src

SRCS = $(SRC_DIR)/libmythread.cpp $(SRC_DIR)/libfuncs.cpp 
DEPS = $(SRCS) $(INC_DIR)/hglobal.h $(INC_DIR)/libfuncs.h $(INC_DIR)/xdefines.h

INCLUDE_DIRS = -I$(INC_DIR)

TARGETS = libmythread.so

all:	$(TARGETS)

CXX = g++

CFLAGS64 = -m64 -msse2 -O3 -DNDEBUG -shared -fPIC -DLAZY_COMMIT -DLOCK_OWNERSHIP -DDETERM_MEMORY_ALLOC -D'CUSTOM_PREFIX(x)=grace\#\#x'

LIBS = -ldl -lpthread

libmythread.so:	$(SRCS) 
	$(CXX) $(CFLAGS64) $(INCLUDE_DIRS) $(SRCS) -o $@ $(LIBS)


clean:
	rm -f $(TARGETS)

