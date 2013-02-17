#ifndef _2DE_MEMORY_H_
#define _2DE_MEMORY_H_

#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

#include <map>

//////////////////////////////////////////////////////////////////////////
// All memory related stuff in this file.

#define D2D_MEMORY_TRACK_MAX_PATH 16

struct CAllocationInfo
{	
	unsigned long size;
	unsigned long line;
	char path[D2D_MEMORY_TRACK_MAX_PATH];	
};

typedef std::map<unsigned long, CAllocationInfo> TAllocationList;

extern TAllocationList *allocationList;

inline void AddTrack(unsigned long addr,  unsigned long asize,  const char *fname, unsigned long lnum)
{
	static CAllocationInfo info;

	if (allocationList == NULL)
		allocationList = new(TAllocationList);

	memset(info.path, 0, D2D_MEMORY_TRACK_MAX_PATH);
	int l = strlen(fname);
	int i = l-1;
	int j = l > D2D_MEMORY_TRACK_MAX_PATH - 1 ? D2D_MEMORY_TRACK_MAX_PATH - 2 : l - 1;
	while ( i >= 0 && j >= 0 )
	{
		info.path[j] = fname[i];
		i--;
		j--;
	}

	info.line = lnum;
	info.size = asize;

	(*allocationList)[addr] = info;
};

void RemoveTrack(unsigned long addr);
void DumpUnfreed();

inline void * operator new(unsigned size, const char *file, int line)
{
	void *ptr = (void *)malloc(size);
	AddTrack((unsigned long)ptr, size, file, line);
	return(ptr);
};

inline void operator delete(void *p)
{
	RemoveTrack((unsigned long)p);
	free(p);
};

inline void operator delete[](void *p)
{
	RemoveTrack((unsigned long)p);
	free(p);
};

#ifdef _DEBUG
	#define DEBUG_NEW new(__FILE__, __LINE__)
#else
	#define DEBUG_NEW new
#endif
	#define new DEBUG_NEW

#endif // defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

#endif // _2DE_MEMORY_H_