#include <stdio.h>
#include "SievesShared.h"
#include "SieveFunctions.h" // dont think its necessary


#define SETBIT64(x, k)  (x)|=(1ULL<<k) 
#define CLEARBIT64(x, k)  (x)&=~(1ULL<<k) 

#define SET_RESULT(bit_index) SETBIT64(result[bit_index/BITSPERBYTE], bit_index%BITSPERBYTE)
#define CLR_RESULT(bit_index) CLEARBIT64(result[bit_index/BITSPERBYTE], bit_index%BITSPERBYTE)

#ifndef IGNORE_EVEN
	#define MARK_PRIME(n)	SET_RESULT((n - 3ULL))
	#define CLEAR_PRIME(n)	CLR_RESULT((n - 3ULL))
#endif 

#ifdef IGNORE_EVEN
	#define MARK_PRIME(n)	SET_RESULT((n - 1)/2)
	#define CLEAR_PRIME(n)	CLR_RESULT((n - 1)/2)
#endif 


/* expects a zalloced array. Doesnt ignore evens in results.*/
/* expect N>=3 */
void NaiveSieve(int N, uint64_t* result, bool printPrimes){
	uint64_t count = 1;
	for (int i = 3; i<=N; i++){
		bool prime = true;
		for (int j = i-1; j>=2 ; j--){
			if(i%j==0){
				prime = false;
				break;
			}
		}
		if (prime){
			MARK_PRIME(i);
			count++;
		}
	}
	printf("correct count: %llu\n",count);
}
