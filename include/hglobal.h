#include <sys/types.h>
#include <unistd.h>
#include <set>
#include <sys/types.h>
#include <pthread.h>
#include "xdefines.h" //defines popular constant


extern "C" {          //defines global variables for process data sharing

#if !defined(__APPLE__)
extern int __data_start;
extern int _end;

int thread_id =1;
struct threadmapping{
       pthread_t id;
       int childnum;
};
struct threadmapping thread_list[xdefines::MAPPINGNUM];

void *heap_pointer;
int mappint_list_id =1;
struct heapmapping{
       int id;
       void *pointer;
       size_t size;
};
struct heapmapping mapping_list[xdefines::MAPPINGNUM];

pthread_mutex_t heap_lock=PTHREAD_MUTEX_INITIALIZER;

#endif

}

// Macros to align to the nearest page down and up, respectively.
#define PAGE_ALIGN_DOWN(x) (((size_t) (x)) & ~xdefines::PAGE_SIZE_MASK)
#define PAGE_ALIGN_UP(x) ((((size_t) (x)) + xdefines::PAGE_SIZE_MASK) & ~xdefines::PAGE_SIZE_MASK)

// Macros that define the start and end addresses of program-wide globals.


#define GLOBALS_START  PAGE_ALIGN_DOWN((size_t) &__data_start)
#define GLOBALS_END    PAGE_ALIGN_UP(((size_t) &_end - 1))


#define GLOBALS_SIZE   (GLOBALS_END - GLOBALS_START)

class hglobal {
private:
    hglobal(){}
public:
//=========main initialize function to setup global Variables and heap area would be accessed by multiplethread===========================
static void initialize() {
  	
	globals_initialize(); 
	heap_initialize();     
	//_heap.initialize(USER_HEAP_SIZE);

  }
//============end of main initialize function==========================

  void finalize(void) {
  }

//=====================================start of globalinitialize()===================  
static  void globals_initialize(){
	char *tempMem;
	char *threaduseMem;

	char _persistFname[L_tmpnam];
	sprintf(_persistFname,"NameXXXXXX");
	int _persistfd = mkstemp(_persistFname);
	
        int temp2 = ftruncate(_persistfd,GLOBALS_SIZE);
        
	tempMem = (char*)mmap(NULL, GLOBALS_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	if(tempMem == MAP_FAILED){
		printf("tempMem failed.\n");
	}
	
	printf("GLOBALS_START: %10p.\n", (void *)GLOBALS_START);
		
	memcpy( tempMem, (void *)GLOBALS_START, GLOBALS_SIZE);

	threaduseMem = (char*)mmap((void *)GLOBALS_START, GLOBALS_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_FIXED , _persistfd, 0);
	if(threaduseMem == MAP_FAILED){
		printf("threaduseMem failed.\n");
	}

	memcpy( threaduseMem, tempMem, GLOBALS_SIZE);
  }
//===================================end of globalinitialize()============

//===================================start of heapinitialize()============
static void heap_initialize(){
	
	char *heapMem;
	heapMem= (char *) mmap(NULL, xdefines::USER_HEAP_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	//printf("heapMem address: %10p \n", heapMem);

	if(heapMem == MAP_FAILED){
		printf("heapMem failed.\n");
	}
	
	heap_pointer= (void *) (((size_t)heapMem + xdefines::PAGE_SIZE_MASK) & ~xdefines::PAGE_SIZE_MASK);
	printf("heap_pointer address: %10p \n", heap_pointer);
	
}
//===================================end of heapinitialize()==============

};



