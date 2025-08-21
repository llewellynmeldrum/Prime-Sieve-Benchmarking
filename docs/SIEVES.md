# Benchmarking 

## Relevant questions:
What benchmark results are absolutely necessary?       --> LOG_MANDATORY()  | stdout |
What benchmark results are decently important?         --> LOG_DEFAULT()    | stdout |
What benchmark results are good for a verbose mode?    --> LOG_VERBOSE()    | stderr |
What benchmark results are good for a debug mode?      --> LOG_DEBUG()      | stderr | 

How do I verify primes are correct? <-- run the slightly slower but guaranteed correct algorithm at the end to check them all. 
^ OR: pack known primes up to a certain point in binary file, read the file into memory.


# Sieves 

A Sieves characteristics are defined through this struct. 
```c
typedef struct Sieve{
	void (*func_ptr)(int N, SieveResult results);       // sieve function.
	char name[128];                                         // Used when printing results (or debug/verbose mode)
	char description[256];                                  // Used in debug/verbose mode.
        bool ignoreEven;                                        // See ## BITARRAY for details.
//    bool threadSafe;                                        // Implement later
        bool zalloc;                                            // whether the sieve expects a 0 allocated or 1 allocaeted array.
}Sieve;
```
### Sieve Function IO: 
- `int N`:  
The maximum number up to which a sieve should check for primes. 
Sieves should follow this no matter what, as the results array is malloced based on this amount.

`BitArray results`: 
- A bitwise array/bitset/bitvector/bitfield of results.
- It is implemented as a heap allocated array of `uint64_t`. <-- *would 128 be faster(?) (cache line size on my machine)*
- The *ith* bit of results represents:
    primality of the *ith* integer >=3, 
    OR 
    Primality of the *ith* **EVEN** integer >=3 
- Depending on whether ignoreEven is set.
- Examples of the bitwise state of the array: 
// NOTE: returns NULL if N<=2
    **if (ignoreEven == FALSE)**
        BitArray:   1  0  1  0  1  0  0  0  <-- i.e 3, 5 and 7  are prime.
        Meaning:    03,04,05,06,07,08,09,10     
        *BitIndex:  00,01,02,03,04,05,06,07*

    **if (ignoreEven == TRUE)** (uses half the memory)
        BitArray:   1  1  1  0  1  1  0  1  <-- i.e 3, 5, 7, 11, 13 and 17 are prime.
        Meaning:    03,05,07,09,11,13,15,17
        *BitIndex:  00,01,02,03,04,05,06,07*

- Storage Required For N digits:
`           IgnoreEvens=TRUE      IgnoreEvens=FALSE`
bits        (N-1)                ((N-1)/2) 
bytes       ceil(bits/8)         ceil(bits/8)
uint64_t's  ceil(bytes/8)        ceil(bytes/8)

*see STORAGE_REQ.md for details*.



## What options should be avaliable to differentiate sieve implementations?
- Need to keep it somewhat limited, so that processing them isnt too different.
- Ideally, they would all be black boxes with a known input and output.


## BitArray results
