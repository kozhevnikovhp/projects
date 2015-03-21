// ----------------------------------------------------------------------

#define SI_ALLOC(size) SI_sp_alloc((unsigned int)size)
#define SI_ALLOC_STRUCT2(t,p) (p = (t *)SI_ALLOC(sizeof(t)))

#define SI_MALLOC(size)                memset(SI_sp_malloc((size_t)size), (char)0, ((size_t)size))
#define SI_FREE(x, typecast)           if (x) {  free((void *)x); x = (typecast) 0; }
#define SI_MALLOC_PRIMS(prim, n)       (prim *) SI_MALLOC((sizeof(prim) * (n)))

#define SI_MAXLINE	512
#define SI_SMLINE	256

#define PIx2  6.28318530718
#define SQRT2 1.414213562

#define FLOAT_TOL 0.00001
#define FLOAT_TOLD 0.00000001

#define SI_POS 0
#define SI_NEG 1

#define SI_ABS(x)	(((x) < 0) ? (-(x)) : (x))

// ----------------------------------------------------------------------

// Heapsort

#define SI_HS_INDEX 0
#define SI_HS_VALUE1 1
#define SI_HS_NET1 2
#define SI_HS_NET2 3
#define SI_HS_LAYER1 4
#define SI_HS_PIN1 5
#define SI_HS_COMP1 6

// This structure will be sorted. The original database will be untouched.
// Initialise this structure : 0, 1, 2 ... N
typedef struct HS_Sort
{
   int iIndex;
} HS_SORT;
					       
// ----------------------------------------------------------------------

