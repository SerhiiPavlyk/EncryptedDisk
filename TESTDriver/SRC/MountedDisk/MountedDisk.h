#pragma once
#include "main/pch.h"
#include "IRP/IRP.h"

//typedef struct MountedDiskStruct {
//	IrpStruct irpDispatcher;
//	PIRP pIrp;			//spizheno iz source proekta
//}MountedDiskStruct, * PMountedDiskStruct;





MOUNTEDDISK MountDiskList[MAX_SIZE];

MOUNTEDDISK DataOfMountedDisk;

NTSTATUS MountedDiskDispatchIrp(PIRP irp);
