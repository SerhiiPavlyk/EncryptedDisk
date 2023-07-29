#pragma once

void KernelCustomEventInit(BOOLEAN initState, KEVENT* EventData);

void set(KEVENT* EventData);

void reset(KEVENT* EventData);

BOOL readState(KEVENT* EventData);

typedef struct SpinLockStruct
{
    KIRQL OldIrql;
    KSPIN_LOCK spinLock;

}SpinLock;


void InitSpinLock(SpinLock* spinLockData);

void enterSpinLock(SpinLock* spinLockData);

void leaveSpinLock(SpinLock* spinLockData);

