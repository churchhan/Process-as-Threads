#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>
#include <syscall.h>
#include <dlfcn.h>
#include <fcntl.h>
#include "hglobal.h"
#include "libfuncs.h"


void initialize() __attribute__((constructor));
void finalize() __attribute__((destructor));

void initialize() {	
	
	hglobal::initialize();
	init_real_functions();

}

extern "C"{
//Redefine pthread_create to run thread in an individual storage block
int pthread_create (pthread_t * tid, const pthread_attr_t * attr, void *(*fn)(void *), void * arg) {
    int child;    
    child = syscall(SYS_clone, CLONE_FS | CLONE_FILES | SIGCHLD, (void*) 0);
                 
    *tid=(pthread_t) child;
    thread_list[thread_id].id = *tid;

    if (child){
	thread_list[thread_id].childnum = (int)child;
    //printf("Thread %ld 's child %d. \n",(long) thread_list[thread_id].id, thread_list[thread_id].childnum);
	}
    else {
	pid_t mypid = syscall(SYS_getpid);	
	void *result = (*fn)(arg); 
	//printf("clone() returned %d\n", mypid);
	exit(0);
	}
    
    thread_id++;
    return(1);

}

int pthread_join (pthread_t tid, void ** val) {
    
    int returnstatus, i;
    long inner_id;
    inner_id = (long)tid;
    //printf("Thread %ld is join with child. \n",inner_id);
    for (i=0;i<=xdefines::MAPPINGNUM;i++){
	if (inner_id==(long)thread_list[i].id){	
		//printf("child %d will join!\n",thread_list[i].childnum);
        waitpid(thread_list[i].childnum, &returnstatus, 0);
		return(0);
		}
	}        

}

//Redefine Malloc, realloc, free for heap management
void *malloc(size_t size){
 
    //start to asign memory block to malloc call
    pthread_mutex_lock(&heap_lock);

    //printf("redefined malloc started.\n");
    void * p = heap_pointer;

    // increas the heap_point for preparing the next malloc call
    heap_pointer += size;

    pthread_mutex_unlock(&heap_lock);
    //end of asignment
        	
    mapping_list[mappint_list_id].id=mappint_list_id;
    mapping_list[mappint_list_id].pointer=p;
    mapping_list[mappint_list_id].size=size;
    mappint_list_id++;
    //printf("Malloc function is successful.\n");
    return p;
    
  }

void free (void * pointer) {
    if(pointer == NULL) 
      return;

    void * innerPtr = pointer;
    
    int i;
    for (i=0;i<xdefines::MAPPINGNUM;i++){
	if (mapping_list[i].pointer==innerPtr){
		mapping_list[i].size=0;
		return;
		}
	}
  }

void *realloc (void * ptr, size_t size) {
    if(ptr == NULL) 
      return ptr;
    
    if(size==0){ free(ptr);}
    else{
    void * old_Ptr = ptr;
    void * p;

    int y;
    for(y=0;y<xdefines::MAPPINGNUM;y++){
	if (mapping_list[y].pointer==old_Ptr){
		//start to reasign memory block to realloc call
    		pthread_mutex_lock(&heap_lock);

    		//printf("redefined realloc started.\n");
    		p = heap_pointer;

    		// increas the heap_point for preparing the next malloc call
    		heap_pointer += size;

    		pthread_mutex_unlock(&heap_lock);
    		//end of asignment
        	
		//printf("oldsize : %ld new size : %ld.\n", mapping_list[y].size,size);
	
                if(mapping_list[y].size <= size){
                memcpy(p,old_Ptr,mapping_list[y].size);
		}
		else{
		memcpy(p,old_Ptr,size);}
                
    		mapping_list[mappint_list_id].id=mappint_list_id;
    		mapping_list[mappint_list_id].pointer=p;
    		mapping_list[mappint_list_id].size=size;
		
    		mappint_list_id++;
		}
	}
	
     return p;
    }
  }

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	pthread_mutexattr_t _mutexattr;

	if( attr == NULL) {		
		WRAP(pthread_mutexattr_init)(&_mutexattr);
		pthread_mutexattr_setpshared(&_mutexattr, PTHREAD_PROCESS_SHARED);
		WRAP(pthread_mutex_init)(mutex, &_mutexattr);
	}
	else {
		pthread_mutexattr_setpshared((pthread_mutexattr_t *)attr, PTHREAD_PROCESS_SHARED);
		WRAP(pthread_mutex_init)(mutex, &_mutexattr);
	}
  }

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	pthread_condattr_t _condattr;

	if( attr== NULL) {		
		WRAP(pthread_condattr_init)(&_condattr);
    		pthread_condattr_setpshared(&_condattr, PTHREAD_PROCESS_SHARED);
		WRAP(pthread_cond_init)(cond, &_condattr);
	}
	else {
		pthread_condattr_setpshared((pthread_condattr_t *)attr, PTHREAD_PROCESS_SHARED);
		WRAP(pthread_cond_init)(cond, &_condattr);
	}
  }

}

