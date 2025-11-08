/* $Id$ */
#ifndef MEM_INCLUDED
#define MEM_INCLUDED
#include "except.h"
extern const 	Except_T Mem_Failed;
/// @brief Allocate memory.
/// @param nbytes Number of bytes to allocate.
/// @param file Source file where allocation is requested.
/// @param line Line number in the source file.
/// @return Pointer to allocated memory or NULL if allocation fails.
extern void 	*Mem_alloc 				(long nbytes,
										const char *file, int line);
/// @brief Allocate zero-initialized memory.
/// @param count Number of elements to allocate.
/// @param nbytes Size of each element.
/// @param file Source file where allocation is requested.
/// @param line Line number in the source file.
/// @return Pointer to allocated memory or NULL if allocation fails.
extern void 	*Mem_calloc				(long count, long nbytes,
										const char *file, int line);
/// @brief Free allocated memory.
/// @param ptr Pointer to the memory to free.
/// @param file Source file where deallocation is requested.
/// @param line Line number in the source file.
extern void 	Mem_free				(void *ptr,
										const char *file, int line);
/// @brief Resize allocated memory.
/// @param ptr Pointer to the memory to resize.
/// @param nbytes New size in bytes.
/// @param file Source file where reallocation is requested.
/// @param line Line number in the source file.
/// @return Pointer to the reallocated memory or NULL if reallocation fails.
extern void 	*Mem_resize				(void *ptr, long nbytes,
										const char *file, int line);
/// @brief Allocate memory.
/// @param nbytes Number of bytes to allocate.
/// @param file Source file where allocation is requested.
/// @param line Line number in the source file.
/// @return Pointer to allocated memory or NULL if allocation fails.
#define 		ALLOC(nbytes) \
				Mem_alloc((nbytes), __FILE__, __LINE__)
/// @brief Allocate zero-initialized memory.
/// @param count Number of elements to allocate.
/// @param nbytes Size of each element.
/// @param file Source file where allocation is requested.
/// @param line Line number in the source file.
/// @return Pointer to allocated memory or NULL if allocation fails.
#define 		CALLOC(count, nbytes) \
				Mem_calloc				((count), (nbytes), __FILE__, __LINE__)
/// @brief Allocate a new object.
/// @param p Pointer to the object to allocate.
/// @return Pointer to the allocated object or NULL if allocation fails.
#define  		NEW(p) 					((p) = ALLOC((long)sizeof *(p)))
/// @brief Allocate a new zero-initialized object.
/// @param p Pointer to the object to allocate.
#define 		NEW0(p) 				((p) = CALLOC(1, (long)sizeof *(p)))
/// @brief Free allocated memory.
/// @param ptr Pointer to the memory to free.
#define 		FREE(ptr) 				((void)(Mem_free((ptr), \
										__FILE__, __LINE__), (ptr) = 0))
/// @brief Resize allocated memory.
/// @param ptr Pointer to the memory to resize.
/// @param nbytes New size in bytes.
/// @return Pointer to the reallocated memory or NULL if reallocation fails.
#define 		RESIZE(ptr, nbytes) 	((ptr) = Mem_resize((ptr), \
										(nbytes), __FILE__, __LINE__))

#endif