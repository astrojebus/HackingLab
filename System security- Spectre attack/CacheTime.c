#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint8_t array[10*4096];

int main(int argc, const char **argv) {
  int access=0;
  register uint64_t timebefore, timeafter;
  volatile uint8_t *addr;
  int i;
  
  for(i=0; i<10; i++)
	 array[i*4096]=1 ;		// Initialize the array elements
  
  for(i=0; i<10; i++)
	 _mm_clflush(&array[i*4096]);	// Flush the array from the CPU cache
  
  array[4*4096] = 10;			// Access two random array elements. This will be cached.
  array[8*4096] = 20;		        //
  
  for(i=0; i<10; i++) {
    
    addr = &array[i*4096];
   
    timebefore = __rdtscp(&access);		 //Time Stamp Counter of CPU before memory read
    access = *addr;				 //Array element is accessed
    timeafter = __rdtscp(&access) - timebefore;  //Difference gives the time taken to access the array element

    printf("Access time for array[%d*4096]: %d CPU cycles\n",i, (int)timeafter);
  }
  return 0; 
}
