# STORAGE REQUIREMENTS
- Bits required to store N digits: 
// NOTE: if N<=2, return NULL.
    **if (ignoreEven == FALSE)**
        - Index i=0 refers to 3, (since we skip 1 and 2. By definition, 1 is nonprime, 2 is prime)
        - **Therefore the bits required to store primality of integers up to N = N-2**
        - Example: Primality of (0,1,2,3,4,5,6,7,8) = 101010 (6 bits for N=8)
                    (3=prime,4=notprime,5=prime,6=notprime,7=prime,8=notprime)

    **if (ignoreEven == TRUE)** (uses half the memory)
        - Index i=0 still refers to 3, BUT, we choose to only store ODD integers >=3, 
        - since (N%2==0 && N!=2 && isPrime(N)) is a contradiction.
        - **Therefore the bits required to store primality of integers up to N = N/2** (IGNORE_EVEN)
        - Example:      Primality of (N=8) =  1110 = (4 bits for N=8)

- Bytes required to store N digits:
    **if (ignoreEven == FALSE)**
        bits = N-2
        bytes = ceil( (N - 2) )

    **if (ignoreEven == TRUE)**
        bits = N/2
        bytes = ceil( ((N-1) / 2) / 8 ) 

- uint64_t's required to store N digits:
    **if (ignoreEven == FALSE)**
        bytes = ceil( (N - 2)/8 )
        uint64_t's = ceil( ceil( (N-2)/8 ) / 8 )

    **if (ignoreEven == TRUE)**
        bytes = ceil( (N / 2) / 8 ) 
        uint64_t's = ceil( ceil( (N/2)/8 ) / 8 )
