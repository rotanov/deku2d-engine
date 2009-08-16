#ifndef EVENT_H
#define EVENT_H
#include "coreutils.h"

/*
Event library
Deku team
*/
//Handle type for each event
typedef void* CHandle;
//Event
//each event must be implemented like this : void onClick(CHandle Sender){},
//where Sender - is the pointer to object, which call this event.
typedef void (*CEvent)(CHandle);
typedef void (*CKeyEvent)(CHandle, int);

//safe call for events X - Event, Sender - object, which calls this event

//////////////////////////////////////////////////////////////////////////
// !!!! BAD !!! __forceinline is microsoft specific
// делай как в mathutils.h
//////////////////////////////////////////////////////////////////////////

#define  SAFECALL(X, Sender) if (X != NULL) {(X)((CHandle)Sender);}
//	else
//	{
		//if (Sender!=NULL)
		//	Log("ERROR", "Undefined event for object %s!", Sender->name);
		//else
		//	Log("ERROR", "Undefined event for non existing object!");
//	}
//}
__forceinline  void  SAFEKEYCALL(CKeyEvent X, CObject *Sender, int Key){
	if (X != NULL && Sender != NULL) 
	{
		(X)((CHandle)Sender, Key);
	}
	else
	{
		if (Sender!=NULL)
			Log("ERROR", "Undefined event for object %s!", Sender->name);
		else
			Log("ERROR", "Undefined event for non existing object!");
	}
}


#endif
