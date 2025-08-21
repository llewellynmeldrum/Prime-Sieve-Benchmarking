
# working on right now:
- Sieves should return the result. Sieve Result should be an array of bits that have been set.
Or should they depend on the type of sieve?
Probably not.

# TODO 20-08-25
1. Make well defined requirements for the sieve programs. Make the first three:
 - naiveSievev1 -m=null
 - naiveSievev2 -m=Sqrt
 - naiveSievev3 -m=SqrtIgnoreComposite
 - naiveSievev4 -m=SqrtIgnoreCompositeIgnoreEven


2. Create the make file to build them all as objects.

3. How do I actually structure this project?
    - sieves.h folder than has a declaration of all sieves.
    - separate folders for each.
    - any prime solver is just a single c file. No headers allowed for simplicity.
    - The c file has the same name as the directory.
    - Registering a new sieve takes place in the main, tester.c file, where you call something like:
        RegisterSieve()
    - , which adds a new entry into the Sieves array. The sieve struct would have:
        - a function ptr
        - a name, maxlength relatively short for the tables maybe
        - a description, for verbose output, longer max length
    - so the call could look like:
**TEMPLATE FOR ALL SIEVE FUNCTIONS TO FOLLOW:**
``` c
SieveResult ExampleSieve(int limit);

```

```c
void RegisterAllSieves(){
    char* description = "This is the most basic of sieves, it simply divides all i<N && i>=2, if no factors are found, N is prime.";
    char* name = "NaiveSieveV1"; // should be the same as the filename, and the folder name. <-- write a script to verify this?
    FUNCPTR func_ptr = &NaiveSieve();       // null check this in the function
    RegisterSieve(func_ptr, name, description);
    
}

void RegisterSieve(){

}

void SieveExample(){

}

int main(int argc, char** argv){
    // 0. assign defaults
    // 1. check and parse args, applying values, setting flags
    // 2. depending on -v and -q, display some information, like current flag/arg states, system info
    //    even include detail on warmup run, taskset() calls, fixed frequency, warmup,
    // 2. call RegisterAllSieves() 
    // 3. call BenchAllSieves() --> records results in SieveTestResult
    // 4. Do any calculations needed for output stats
    // 5. Output results 
}

void BenchAllSieves(bool DEBUG_MODE){
    for (int i = 0; i<sieveCount; i++){
        BenchSieve();
    }
}

void BenchSieve
```

# Directory Structure:
├── Makefile
├── README.md
├── TODO.md
├── bin
├── build
├── exampleHelp
├── include
└── src
    ├── BenchmarkSieves.c
    ├── Sieve1
    │   └── Sieve1.c
    └── Sieve2
        └── Sieve2.c


Key thing to notice is the directory determines the executable name, the executable/lib will always be:
- "sieve-<DIR>" OR "sieve-<DIR>.a"

# TestSieve
1. Build:
`   $ make all                                                      ` <- builds all into .a static libs, links them into executable.

2. Run:
## Program Usage format:
<lf>
This program uses clock time. See BenchSieves.c for more details.
Usage:
    BenchSieves [-args]... | [-args=OPT]... 

Arguments:
    -l, --limit=UPPER_LIMIT                 Specify the max number to calculate to.                 DEFAULT=1000
    -r, --runcount=RUN_COUNT                Number of runs (not including single cold start run)    DEFAULT=5 
    -s, --seconds=SECONDS                   Specify the number of seconds to wait before stopping   DEFAULT=1
    -t, --threadcount=THREAD_COUNT          Number of worker threads each test should use.          DEFAULT=1
    -q, --quiet                             Prevents certain output and warnings.                   DEFAULT=FALSE
    -v, --verbose                           Increases output verbosity. Like a lot.                 DEFAULT=FALSE
    -p, --print                             Print the primes as they are found.                     DEFAULT=FALSE 
    -h, --help                              Print this message and exit.                            DEFAULT=FALSE

Warnings:
    1. Running with a THREAD_COUNT > LOGICAL_CORE_COUNT will trigger a warning.
    2. Running with a SECONDS > 120 might trigger a warning.
    3. Running with -p and a RUN_COUNT>1000, or a SECONDS>0.1

## output:
Should look something like:


Tested N sieves.
    Compiler flags: 
    Prebench Stuff:
    Threadcount:    # OR threadcount could be a column. 
                    # optionally also give some system info
    # sort in fastest to slowest total time order.

|   Sieve Name      |  Solved   |   Total time  |   Mean Time per solve  |      IQR     |  
|   Eratosthenes    |   P/N     |   hh:mm:ss:ms |   msms:us:ns           |              |
|   Faliure         |   N-1/N   |   FAILED      |   FAILED               |              | 

