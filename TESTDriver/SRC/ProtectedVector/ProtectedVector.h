#pragma once
#include "main/pch.h"
#include "IRPQueue/IRPQueue.h"

typedef struct {
    PIRP* data;
    UINT32 size;
    UINT32 capacity;
    SpinLock mutex;
    KEVENT ownNotEmptyEvent_;
    KEVENT* notEmptyEvent_;

} Vector;

void initProtectedVector(Vector* vector, UINT32 capacity);

void initProtectedVectorAddEvent(Vector* vector, UINT32 capacity, KEVENT* Kevent);

void push_back(Vector* vector, PIRP value);


void destroy(Vector* vector);


int pop(Vector* vector, PIRP value);

void PVon_pop();

BOOL IsEmpty(Vector* vector);

UINT32 size(Vector* vector);
