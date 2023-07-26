#pragma once
#include "MountManager/MountManager.h"

typedef struct IrpParametrs
{
	int type;
	UINT32 size;
	UINT64 offset;
	char* buffer;
}IrpParam;



NTSTATUS IrpHandlerInit(UINT32 devId,
	UINT64 totalLength,
	PDRIVER_OBJECT DriverObject,
	PMOUNTEDDISK Mdisk);

IrpStruct IrpData;

void IrpHandlergetIrpParam(PIRP irp, IrpParam* irpParam);
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

NTSTATUS DispatchMount(PVOID buffer,
	ULONG inputBufferLength,
	ULONG outputBufferLength);
NTSTATUS DispatchExchange(PVOID buffer,
	ULONG inputBufferLength,
	ULONG outputBufferLength);
NTSTATUS DispatchUnmount(PVOID buffer,
	ULONG inputBufferLength,
	ULONG outputBufferLength);

// Dispatch routine to handle IRPs
NTSTATUS dispatch_irp(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info);

NTSTATUS read_from_virtual_disk(char* buf, ULONG count, LARGE_INTEGER offset);

NTSTATUS write_request(const char* buf, ULONG count, LARGE_INTEGER offset);