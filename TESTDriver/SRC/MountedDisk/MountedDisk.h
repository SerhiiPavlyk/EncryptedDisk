#pragma once
#include "main/pch.h"
#include "IRP/IRP.h"


MOUNTEDDISK MountDiskList[MAX_SIZE];

void InitMountDisk(PDRIVER_OBJECT DriverObject, UINT32 devId,	UINT32 totalLength, PMOUNTEDDISK disk);

void DesctructorMountDisk(PMOUNTEDDISK disk);

NTSTATUS MountedDiskDispatchIrp(PIRP irp, PMOUNTEDDISK disk);

void MountedDiskCompleteLastIrp(NTSTATUS status, ULONG information, PMOUNTEDDISK disk);

void MountedDiskRequestExchange(UINT32 lastType, UINT32 lastStatus, UINT32 lastSize, char* buf,
	UINT32* type, UINT32* length, UINT32* offset, PMOUNTEDDISK disk);