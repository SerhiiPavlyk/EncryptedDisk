#pragma once

#include "MountedDisk/MountedDisk.h"
typedef struct MManager
{
	UINT32 gMountedDiskCount;   //счетчик дисков, глобальная переменная
	FAST_MUTEX diskMapLock_;
	BOOL isInitializied;
	PDRIVER_OBJECT DriverObject;


} MountManager, * PMountManager;


MountManager DataOfMountManager;

VOID MountManagerInit(PDRIVER_OBJECT DriverObject);

NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp);

int Mount(UINT32 totalLength);

VOID Unmount(UINT32 deviceId);

VOID MountManagerRequestExchange(UINT32 devID, UINT32 lastType, UINT32 lastStatus,
	UINT32 lastSize, char* buf, UINT32* type, UINT32* length, UINT32* offset);

