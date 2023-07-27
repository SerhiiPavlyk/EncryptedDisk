#pragma once
#include "main/pch.h"
#include "IRP/IRP.h"

//typedef struct MountedDiskStruct {
//	IrpStruct irpDispatcher;
//	PIRP pIrp;			//spizheno iz source proekta
//}MountedDiskStruct, * PMountedDiskStruct;



MOUNTEDDISK MountDiskList[MAX_SIZE];

void InitMountDisk(PDRIVER_OBJECT DriverObject,
	UINT32 devId,
	UINT32 totalLength,
	PMOUNTEDDISK disk);

void DesctructorMountDisk(PMOUNTEDDISK disk);
NTSTATUS MountedDiskDispatchIrp(PIRP irp, PMOUNTEDDISK disk);
void MountedDiskCompleteLastIrp(NTSTATUS status, ULONG information, PMOUNTEDDISK disk);
