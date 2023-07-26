#pragma once
#include "main/pch.h"


//typedef struct MountedDiskStruct {
//	IrpStruct irpDispatcher;
//	PIRP pIrp;			//spizheno iz source proekta
//}MountedDiskStruct, * PMountedDiskStruct;

typedef struct DiskDevExt
{
	UINT32 deviceId;
}DeviceId, * PDeviceId;

typedef struct IrpHandlerStruct {
	DeviceId devId_;
	UINT64 totalLength_;
	PDEVICE_OBJECT  deviceObject_;
}IrpStruct;


typedef struct MountedDisk
{
	IrpStruct irpDispatcher;
	PIRP pIrp;
	UINT32 gMountedDiskCount;
	//LARGE_INTEGER size;				//в IrpStruct irpDispatcher
	UNICODE_STRING FileName;
	//PDEVICE_OBJECT obj;					//в IrpStruct irpDispatcher
	CHAR Letter;
	//DeviceId devID;				//в IrpStruct irpDispatcher
	PUNICODE_STRING password;
	//что-то типа мапы для хранения дисков. добавляются/удаляются диски при монтировании/демонтировании
} MOUNTEDDISK, * PMOUNTEDDISK;

MOUNTEDDISK MountDiskList[MAX_SIZE];

MOUNTEDDISK DataOfMountedDisk;
