#include "StdAfx.h"
#include "MemoryStrategy.h"
#include <stdlib.h>
#include <string.h>

void* CMemoryStrategy::OnMalloc(size_t size)
{
  return (size > 0) ? malloc(size) : 0;
}

void* CMemoryStrategy::OnCalloc(size_t n, size_t size)
{
  return (n > 0 && size > 0) ? calloc(n, size) : 0;
}

void CMemoryStrategy::OnFree(void *memblock)
{
  if (memblock)
    free(memblock);
}

void* CMemoryStrategy::OnRealloc(void *memblock, size_t size)
{
	if ( 0 == memblock )
	{
		return OnMalloc(size);
	}

	if ( 0 == size )
	{
		OnFree(memblock);
		return 0;
	}

#if defined(ON_REALLOC_BROKEN)
	/* use malloc() and memcpy() instead of buggy realloc() */
	void* p;
	const size_t memblocksz = _msize(memblock);
	if ( size <= memblocksz ) {
		/* shrink */
		if ( memblocksz <= 28 || 8*size >= 7*memblocksz ) 
		{
			/* don't bother reallocating */
			p = memblock;
		}
		else {
			/* allocate smaller block */
			p = malloc(size);
			if ( p ) 
			{
				memcpy( p, memblock, size );
				free(memblock);
			}
		}
	}
	else if ( size > memblocksz ) {
		/* grow */
		p = malloc(size);
		if ( p ) {
			memcpy( p, memblock, memblocksz );
			free(memblock);
		}
	}
	return p;
#else
	return realloc( memblock, size );
#endif
}