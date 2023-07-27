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
	//LARGE_INTEGER size;				//â IrpStruct irpDispatcher
	UNICODE_STRING FileName;
	//PDEVICE_OBJECT obj;					//â IrpStruct irpDispatcher
	CHAR Letter;
	//DeviceId devID;				//â IrpStruct irpDispatcher
	PUNICODE_STRING password;
	KEVENT irpQueueNotEmpty_;
	KEVENT stopEvent_;
	Vector irpQueue_;
} MOUNTEDDISK, * PMOUNTEDDISK;


NTSTATUS IrpHandlerInit(UINT32 devId,
	UINT32 totalLength,
	PDRIVER_OBJECT DriverObject,
	PMOUNTEDDISK Mdisk);

IrpStruct IrpData;

void IrpHandlerGetIrpParam(PIRP irp, IrpParam* irpParam);
NTSTATUS IrpHandlerdispatch(PIRP irp);

void dispatchIoctl(PIRP irp);
PVOID getIrpBuffer(PIRP irp);

NTSTATUS deleteDevice();

// IRP_MJ_READ handler
NTSTATUS handle_read_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


// IRP_MJ_WRITE handler
NTSTATUS handle_write_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


// IRP_MJ_DEVICE_CONTROL handler (for IOCTL requests)
NTSTATUS handle_ioctl_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

// IRP_MJ_CLEANUP handler
NTSTATUS handle_cleanup_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info);

NTSTATUS read_from_virtual_disk(char* buf, ULONG count, LARGE_INTEGER offset);

NTSTATUS write_request(const char* buf, ULONG count, LARGE_INTEGER offset);