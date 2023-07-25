#pragma once
#include "VirtualDisk/VirtualDisk.h"


typedef struct MManager
{
	UINT32 gDiskCount;   //счетчик дисков, глобальная переменная
	FAST_MUTEX diskMapLock_;
	BOOL isInitializied;
	PDRIVER_OBJECT DriverObject;

} MountManager, * PMountManager;


MountManager DataOfMountManager;

NTSTATUS MountManagerInit(PDRIVER_OBJECT DriverObject);
NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp);
int Mount(UINT64 totalLength);
NTSTATUS MountDisk();
NTSTATUS UnmountDisk(UINT32 deviceId);			//ввиду того, что буква Тома выбирается в любом удобном порядке
VOID MountManagerRequestExchange(UINT32 devID,
	UINT32 lastType,
	UINT32 lastStatus,
	UINT32 lastSize,
	char* buf,
	UINT32 bufSize,
	UINT32* type,
	UINT32* length,
	UINT64* offset);
