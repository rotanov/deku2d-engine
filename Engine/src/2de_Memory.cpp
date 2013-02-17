#include "2de_Memory.h"

#if defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)

TAllocationList *allocationList = NULL;

void RemoveTrack(unsigned long addr)
{
	TAllocationList::iterator i;

	if (allocationList == NULL)
		return;

	i = allocationList->find(addr);
	if (i != allocationList->end())
		allocationList->erase(i);
};

void DumpUnfreed()
{
	FILE *fo = fopen("Memory.log", "w");
	TAllocationList::iterator i;
	unsigned long totalSize = 0;
	char buf[1024];

	if (allocationList == NULL)
		return;

	for(i = allocationList->begin(); i != allocationList->end(); i++)
	{
		sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n", i->second.path, i->second.line, i->first, i->second.size);
		fprintf(fo, "%s", buf);
		totalSize += i->second.size;
	}
	sprintf(buf, "-----------------------------------------------------------\n");
	fprintf(fo, "%s", buf);
	sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
	fprintf(fo, "%s", buf);
	fclose(fo);
};

#endif // defined(_DEBUG) && defined(_MSC_VER) && defined(DEKU2D_I_WANT_TO_LOOK_AFTER_MEMORY_LEAKS)