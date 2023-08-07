#pragma once
#include <main/pch.h>



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
	PLARGE_INTEGER size;
	PLARGE_INTEGER offset;
	char* buffer;
}IrpParam;

//typedef struct MountedDisk
//{
//	IrpStruct irpDispatcher;
//	PIRP pIrp;
//	UNICODE_STRING FileName;
//	UNICODE_STRING DeviceName;
//
//	HANDLE fileHandle;
//	LARGE_INTEGER               fileSize;
//	PUNICODE_STRING password;
//	KEVENT irpQueueNotEmpty_;
//	KEVENT stopEvent_;
//	Vector irpQueue_;
//} MOUNTEDDISK, * PMOUNTEDDISK;

IrpStruct IrpData;

//NTSTATUS IrpHandlerInit(UINT32 devId, UINT32 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk);
//
//void IrpHandlerGetIrpParam(PIRP irp, IrpParam* irpParam);
//
//NTSTATUS IrpHandlerdispatch(PIRP irp, PMOUNTEDDISK disk);
//
//NTSTATUS dispatchIoctl(PIRP irp, PDEVICE_OBJECT object);
//
//PVOID getIrpBuffer(PIRP irp);
//
//NTSTATUS deleteDevice(PMOUNTEDDISK disk);
//
//NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info);


VOID IOCTLHandle(IN PVOID Context);

NTSTATUS
FileDiskAdjustPrivilege(
	IN ULONG    Privilege,
	IN BOOLEAN  Enable
);

NTSTATUS
FileDiskReadWrite(
	IN PDEVICE_OBJECT   DeviceObject,
	IN PIRP             Irp
);

NTSTATUS FileDiskOpenFile(
	IN PDEVICE_OBJECT   DeviceObject,
	IN PIRP             Irp
);

NTSTATUS
FileDiskCloseFile(
	IN PDEVICE_OBJECT   DeviceObject,
	IN PIRP             Irp
);