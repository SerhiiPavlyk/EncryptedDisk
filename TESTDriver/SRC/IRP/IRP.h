#pragma once
#include "ProtectedVector/ProtectedVector.h"



typedef struct DiskDevExt
{
	UINT32 deviceId;
}DeviceId, * PDeviceId;

typedef struct IrpHandlerStruct {
	DeviceId devId_;
	UINT32 totalLength_;
	PDEVICE_OBJECT  deviceObject_;
}IrpStruct;

typedef struct IrpParametrs
{
	int type;
	UINT32 size;
	UINT32 offset;
	char* buffer;
}IrpParam;

typedef struct MountedDisk
{
	IrpStruct irpDispatcher;
	PIRP pIrp;
	UNICODE_STRING FileName;
	PUNICODE_STRING password;
	KEVENT irpQueueNotEmpty_;
	KEVENT stopEvent_;
	Vector irpQueue_;
} MOUNTEDDISK, * PMOUNTEDDISK;

IrpStruct IrpData;

NTSTATUS IrpHandlerInit(UINT32 devId, UINT32 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk);

void IrpHandlerGetIrpParam(PIRP irp, IrpParam* irpParam);

NTSTATUS IrpHandlerdispatch(PIRP irp);

void dispatchIoctl(PIRP irp);

PVOID getIrpBuffer(PIRP irp);

NTSTATUS deleteDevice(PMOUNTEDDISK disk);

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info);
