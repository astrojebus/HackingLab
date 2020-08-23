#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint8_t array[256*4096];
int temp;
char secret = 94;

#define CACHE_HIT_THRESHOLD (180) 	// cache hit time threshold 
#define DELTA 1024			//constant value to make the program consistent

void victim()
{
  temp = array[secret*4096 + DELTA];
}

void flushSideChannel()
{
  int i;
 
  for (i = 0; i < 256; i++) 
	array[i*4096 + DELTA] = 1;		 // Write to array to bring it to RAM to prevent Copy-on-write
  
  for (i = 0; i < 256; i++)
	 _mm_clflush(&array[i*4096 +DELTA]);	//flush the values of the array from cache
}

void reloadSideChannel()
{
  int access=0;
  register uint64_t timebefore, timeafter;
  volatile uint8_t *addr;
  int i;
  
  for(i = 1; i < 256; i++){
  
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

int main(int argc, const char **argv)
{
  flushSideChannel();
  victim();
  reloadSideChannel();
  return (0);
}
