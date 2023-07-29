#include "main/pch.h"
#include "ProtectedVector/ProtectedVector.h"

void initProtectedVector(Vector* vector, UINT32 capacity)
{
//    ExInitializeFastMutex(&vector->mutex);
    InitSpinLock(&vector->mutex);
    vector->data = ExAllocatePoolWithTag(NonPagedPool, capacity * sizeof(PIRP), 'MYVC');
    vector->size = 0;
    vector->capacity = capacity;
    KernelCustomEventInit(FALSE, &vector->ownNotEmptyEvent_);
    vector->notEmptyEvent_ = &vector->ownNotEmptyEvent_;
}

void initProtectedVectorAddEvent(Vector* vector, UINT32 capacity, KEVENT* Kevent)
{
   // DbgBreakPoint();
 //   ExInitializeFastMutex(&vector->mutex);
    InitSpinLock(&vector->mutex);
    vector->data = ExAllocatePoolWithTag(NonPagedPool, capacity * sizeof(PIRP), 'MYVC');
    vector->size = 0;
    vector->capacity = capacity;
    KernelCustomEventInit(FALSE, &vector->ownNotEmptyEvent_);
    vector->notEmptyEvent_ = Kevent;
}


void push_back(Vector* vector, PIRP value)
{
  //  ExAcquireFastMutex(&vector->mutex);
    enterSpinLock(&vector->mutex);
    if (vector->size >= vector->capacity)
    {
        vector->capacity *= 2;  // Double the capacity when it's full
        vector->data = (PIRP*)ExAllocatePoolWithTag(NonPagedPool, vector->capacity * sizeof(PIRP), "mntDisk");
    }
    vector->data[vector->size++] = value;
    if (vector->size == 1 && KeReadStateEvent(vector->notEmptyEvent_))
        set(vector->notEmptyEvent_);
    leaveSpinLock(&vector->mutex);
   // ExReleaseFastMutex(&vector->mutex);
}

void destroy(Vector* vector)
{
    ExFreePoolWithTag(vector->data, 'MYVC');
}

int pop(Vector* vector, PIRP value)
{
    //ExAcquireFastMutex(&vector->mutex);
    enterSpinLock(&vector->mutex);
    if (vector->size == 0)
    {
        leaveSpinLock(&vector->mutex);
        return 0;  // Return 0 for an empty vector (you can choose a different error code if needed)
    }
    value = vector->data[--vector->size];
    if (vector->size == 0 && !KeReadStateEvent(vector->notEmptyEvent_))
        reset(vector->notEmptyEvent_);
    PVon_pop();
    leaveSpinLock(&vector->mutex);
   // ExReleaseFastMutex(&vector->mutex);
    return 1;
}

void PVon_pop()
{

}

BOOL IsEmpty(Vector* vector)
{
 //   ExAcquireFastMutex(&vector->mutex);
    enterSpinLock(&vector->mutex);
    BOOL empty = (vector->size == 0);
    leaveSpinLock(&vector->mutex);
 //   ExReleaseFastMutex(&vector->mutex);
    return empty;
}

UINT32 size(Vector* vector)
{
//    ExAcquireFastMutex(&vector->mutex);
    enterSpinLock(&vector->mutex);
    UINT32 size = vector->size;
    leaveSpinLock(&vector->mutex);
//    ExReleaseFastMutex(&vector->mutex);
    return size;
}
