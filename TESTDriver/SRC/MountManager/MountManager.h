#pragma once
#include "main/pch.h"
#include "VirtualDisk/VirtualDisk.h"
FAST_MUTEX diskMapLock_;
BOOL isInitializied = FALSE;
size_t AmountOfDisks;
const PVIRTUALDISK DiskList;
void Init();
NTSTATUS DispatchIrp(int devId, PIRP irp);