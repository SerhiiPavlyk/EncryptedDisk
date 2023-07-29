#include "main/pch.h"
#include "IRPQueue/IRPQueue.h"

void KernelCustomEventInit(BOOLEAN initState, KEVENT *EventData)
{
	KeInitializeEvent(&EventData, NotificationEvent, initState);
}

void set(KEVENT * EventData)
{
	KeSetEvent(EventData, 0, FALSE);
}

void reset(KEVENT* EventData)
{
	KeClearEvent(EventData);
}

BOOL readState(KEVENT* EventData)
{
	return KeReadStateEvent(EventData) != 0;
}

void InitSpinLock(SpinLock* spinLockData)
{
	KeInitializeSpinLock(&spinLockData->spinLock);
}

void enterSpinLock(SpinLock* spinLockData)
{
	KeAcquireSpinLock(&spinLockData->spinLock, &spinLockData->OldIrql);
}

void leaveSpinLock(SpinLock* spinLockData)
{
	KeReleaseSpinLock(&spinLockData->spinLock, spinLockData->OldIrql);
}
