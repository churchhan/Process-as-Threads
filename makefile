SOURCE_DIR = .
INCLUDE_DIR = .

#SOURCE_DIR = source
#INCLUDE_DIR = include

#SRCS_IBS= $(SOURCE_DIR)/IBS/ibsop.c  
SRCS_IBS= 
SRCS =  $(SOURCE_DIR)/libmem.cpp \
	$(SOURCE_DIR)/libfuncs.cpp         \
	$(SRCS_IBS)  

INCS =  $(INCLUDE_DIR)/xdefines.h     \
	$(INCLUDE_DIR)/xmemory.h         \
	$(INCLUDE_DIR)/objectheader.h \
	$(INCLUDE_DIR)/xoneheap.h     \
	$(INCLUDE_DIR)/xpheap.h \
	$(INCLUDE_DIR)/privateheap.h \
	$(INCLUDE_DIR)/mm.h

INCLUDE_DIRS = -I. -I./heaplayers -I./heaplayers/util
DEPS = $(SRCS) $(INCS)

CXX = g++ -g -O2 -I$(INCLUDE_DIR) 

# Detection on 32bit
# CXX = g++ -DSSE_SUPPORT -m32 -DX86_32BIT -O3 -fno-omit-frame-pointer -DDETECT_FALSE_SHARING
# Detection on 64bit
#CXX = g++ -DSSE_SUPPORT -m64 -fno-omit-frame-pointer -DDETECT_FALSE_SHARING


# -march=core2 -msse3 -DSSE_SUPPORT 
#CFLAGS   = -Wall -msse3 -DSSE_SUPPORT -fno-omit-frame-pointer
#CFLAGS   = -fno-omit-frame-pointer -DPREDICTION
CFLAGS   = -Wno-unused-result -fno-omit-frame-pointer -DPREDICTION -DUSING_SIGUSR2
#CFLAGS   = -Wno-unused-result -fno-omit-frame-pointer -DUSING_IBS -DPREDICTION -DUSING_SIGUSR2
#-DOUTPUT_WORD_ACCESSES
#CFLAGS   = -fno-omit-frame-pointer -DPREDICTION -DOUTPUT_WORD_ACCESSES
#-DIMPROVE_PERF 
#-DOUTPUT_WORD_ACCESSES
#CFLAGS   = -fno-omit-frame-pointer 
CFLAGS32 = $(CFLAGS) -m32 -DX86_32BIT # -O3
CFLAGS64 = $(CFLAGS) #-m64 # -O3
#CFLAGS64 = $(CFLAGS) -m64 # -O3

#GET_CHARACTERISTICS

TARGETS = libmem64.so

all: $(TARGETS)

libmem64.so: $(DEPS)
	$(CXX) $(CFLAGS64) $(INCLUDE_DIRS) -shared -fPIC -D'CUSTOM_PREFIX(x)=mem_##x' $(SRCS) -o libmem64.so  -ldl -lpthread 

clean:
	rm -f $(TARGETS)

