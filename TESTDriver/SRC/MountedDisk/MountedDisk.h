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
	//LARGE_INTEGER size;				//� IrpStruct irpDispatcher
	UNICODE_STRING FileName;
	//PDEVICE_OBJECT obj;					//� IrpStruct irpDispatcher
	CHAR Letter;
	//DeviceId devID;				//� IrpStruct irpDispatcher
	PUNICODE_STRING password;
	//���-�� ���� ���� ��� �������� ������. �����������/��������� ����� ��� ������������/��������������
} MOUNTEDDISK, * PMOUNTEDDISK;

MOUNTEDDISK MountDiskList[MAX_SIZE];

MOUNTEDDISK DataOfMountedDisk;
