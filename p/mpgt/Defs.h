/** @file Defs.h
 *  @brief General preprocessor directives
 *
 *  @details This contains general preprocessor directives used for
 *  better readability of code and for keeping a standard of terms.
 */


//defines
#define no_argument 0
#define required_argument 1 
#define optional_argument 2

#define TRUE 1
#define FALSE 0

#define YES 1
#define NO 0

#define M_PIl          3.141592653589793238462643383279502884L /* pi */
#define INF 99999999.9

#define ON 1
#define OFF 0
#define TAG 42
#define CHANNELS 3

#define BACK 0
#define FRONT 1

#define SINGLE_PLAYER 0
#define MULTI_PLAYER 1
#define NON_PLAYER 3

#define VERSION1 1
#define VERSION2 2
#define VERSION3 3
#define VERSION4 4

//Rank mapping
#define MASTER 0
#define NODECOM 10
#define GRAPHCOM 100
#define MULTIGRAPHCOM 1000

//Clustering modes
#define KMEANS 1
#define KMEANSPP 2
#define AHCS 3
#define AHCC 4
#define AHCW 5

//Clustering distances
#define EUCLIDEAN_D 2

//KMEANS clustering init algorithm
#define KM 3
#define KMPP 1

//AHC clustering algorithm
#define COMPLETE_LINK 0
#define SINGLE_LINK 1
#define UNWEIGHT_AVG 2
#define WEIGHT_AVG 3
#define WARD 4

//Development debug macros
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif
#ifdef VDEBUG
#  define VD(x) x
#else
#  define VD(x)
#endif

//MPI error handling macro
#define MPI_CHECK(call) \
    if((call) != MPI_SUCCESS) { \
        cerr << "MPI error calling \""#call"\"\n"; \
        my_abort(-1); }

