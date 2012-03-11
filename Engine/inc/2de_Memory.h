#ifndef _2DE_MEMORY_H_
#define _2DE_MEMORY_H_

#include <list>
#define MAX_PATH 260

//namespace Deku2D
//{
	//////////////////////////////////////////////////////////////////////////
	// All memory related stuff in this file.
	/**
	*	Memory leaks catching.
	*	Some issues are fixed but there's still a room for improvement.
	*	Taken from http://www.flipcode.com/archives/How_To_Find_Memory_Leaks.shtml by Dion Picco (23 May 2000)
	*	@todo: Implement new[] and delete []
	*	@todo: Remove inline from AddTrack and RemoveTrack and others.
	*/

	#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

	struct ALLOC_INFO
	{
		unsigned long address;
		unsigned long size;
		char file[MAX_PATH];	//bad but anyway... probably needs replacement with string type
		unsigned long line;
	};

	typedef std::list<ALLOC_INFO*> AllocList;

	extern AllocList *allocList;

	inline void AddTrack(unsigned long addr,  unsigned long asize,  const char *fname, unsigned long lnum)
	{
		ALLOC_INFO *info;

		if (allocList == NULL)
			allocList = new(AllocList);

		info = new(ALLOC_INFO);
		info->address = addr;
		strncpy(info->file, fname, MAX_PATH);
		info->file[MAX_PATH] = 0;
		info->line = lnum;
		info->size = asize;
		allocList->insert(allocList->begin(), info);
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

//} // namespace Deku2D

#endif // _2DE_MEMORY_H_