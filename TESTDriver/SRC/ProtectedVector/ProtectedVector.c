#include "main/pch.h"
#include "ProtectedVector/ProtectedVector.h"

void initProtectedVector(Vector* vector, UINT32 capacity)
{
    ExInitializeFastMutex(&vector->mutex);
    vector->data = ExAllocatePoolWithTag(NonPagedPool, capacity * sizeof(PIRP), 'MYVC');
    vector->size = 0;
    vector->capacity = capacity;
    KernelCustomEventInit(FALSE, &vector->ownNotEmptyEvent_);
    vector->notEmptyEvent_ = &vector->ownNotEmptyEvent_;
}

void initProtectedVectorAddEvent(Vector* vector, UINT32 capacity, KEVENT* Kevent)
{
    ExInitializeFastMutex(&vector->mutex);
    vector->data = ExAllocatePoolWithTag(NonPagedPool, capacity * sizeof(PIRP), 'MYVC');
    vector->size = 0;
    vector->capacity = capacity;
    KernelCustomEventInit(FALSE, &vector->ownNotEmptyEvent_);
    vector->notEmptyEvent_ = Kevent;
}


void push_back(Vector* vector, PIRP value)
{
    ExAcquireFastMutex(&vector->mutex);
    if (vector->size >= vector->capacity)
    {
        vector->capacity *= 2;  // Double the capacity when it's full
        vector->data = (PIRP*)ExAllocatePoolWithTag(NonPagedPool, vector->capacity * sizeof(PIRP), "mntDisk");
    }
    vector->data[vector->size++] = value;
    //if (vector->size == 1)
        //set(vector->notEmptyEvent_);
    ExReleaseFastMutex(&vector->mutex);
}

void destroy(Vector* vector)
{
    ExFreePoolWithTag(vector->data, 'MYVC');
    vector->size = vector->capacity = 0;
}

int pop(Vector* vector)
{
    ExAcquireFastMutex(&vector->mutex);
    if (vector->size == 0)
    {
        ExReleaseFastMutex(&vector->mutex);
        return 0;  // Return 0 for an empty vector (you can choose a different error code if needed)
    }
    PIRP value = vector->data[--vector->size];
    //if (vector->size == 0)
        //set(vector->notEmptyEvent_);
    ExReleaseFastMutex(&vector->mutex);
    return 1;
}

BOOL IsEmpty(Vector* vector)
{
    ExAcquireFastMutex(&vector->mutex);
    BOOL empty = (vector->size == 0);
    ExReleaseFastMutex(&vector->mutex);
    return empty;
}

UINT32 size(Vector* vector)
{
    ExAcquireFastMutex(&vector->mutex);
    UINT32 size = vector->size;
    ExReleaseFastMutex(&vector->mutex);
    return size;
}
