#ifndef	UTIL_H
#define	UTIL_H

#undef MAX
#define MAX(a,b) ((a) > (b) ? a : b)
#undef MIN
#define MIN(a,b) ((a) < (b) ? a : b)

#define ROUND(x) ((x)<0 ? ((long)((x)-0.5)) : ((long)((x)+0.5)))

#define ERROR(msg)                              \
    if (e) {                                    \
        (*e) = ral_error_new(0, msg);           \
        goto fail;                              \
    } else                                      \
        fprintf(stderr, msg"\n")
    

#define ROUND_TEST(x,i,e) {                                     \
        long tmp = ROUND(x);                                    \
        if (tmp <= RAL_INTEGER_MIN || tmp >= RAL_INTEGER_MAX) { \
            ERROR("Integer out of bounds.");                    \
        } else                                                  \
            (i) = tmp;                                          \
    }

#define AND &&
#define OR ||

#define SQRT2 1.414213562

#define EPSILON 1.0E-6

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#define EVEN(a) ((a) % 2 == 0)
#undef ODD
#define ODD(n) ((n) & 1)

#define swap(a, b, temp) \
    {(temp) = (a);(a) = (b);(b) = (temp);}

#define swap_grids(grid, temp_grid)             \
    {                                           \
        ral_grid_t tmp;                         \
        swap(*(temp_grid), *(grid), tmp);       \
        ral_delete(temp_grid);                  \
    }

#define MSG_BUF_SIZE 1024

#define MALLOC(type) (type*)malloc(sizeof(type))
#define CALLOC(count, type) (type*)calloc((count), sizeof(type))
#define REALLOC(pointer, count, type) (type*)realloc((pointer), (count)*sizeof(type))
#define FREE(pointer) { if(pointer){ free(pointer); pointer = NULL; } }

/**
   Exceptions:

   TEST, FAIL_UNLESS Out of memory: print to stderr and return NULL or 0, upstream should fail silently.
   
   ASSERT Programmer mistake: print a message to stderr and return NULL or 0

   Warning (assuming something funny with the data): print a message to stderr but continue

   Give up: not an error but the requested operation is a no-op

   hard, soft
   User mistakes: returns ral_error
   check with message
   check if an exception has happened which has created a message already
*/
#define TEST(memory_allocation) { if (!(memory_allocation)) { fprintf(stderr, "Out of memory.\n"); goto fail; } }
#define FAIL_UNLESS(tested) { if (!(tested)) { goto fail; } }
#define ASSERT(condition, msg) { if (!(condition)) { fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, msg); goto fail; } }
#define WARN_UNLESS(condition, msg) { if (!(condition)) { fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, msg); } }
#define WARN(msg) { fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, msg); }

#define CHECK(test) { if (!(test)) goto fail; }

#define GIVE_UP_IF(test) { if ((test)) { goto fail; } }

void CPL_DLL CPL_STDCALL ral_cpl_error(CPLErr eclass, int code, const char *msg);

#endif
