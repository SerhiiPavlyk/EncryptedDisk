#pragma once
#include "test.h"



// IRP_MJ_READ handler
NTSTATUS handle_read_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


// IRP_MJ_WRITE handler
NTSTATUS handle_write_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);


// IRP_MJ_DEVICE_CONTROL handler (for IOCTL requests)
NTSTATUS handle_ioctl_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

// IRP_MJ_CLEANUP handler
NTSTATUS handle_cleanup_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

#define ROOT_DIR_NAME        L"\\??\\H:\\DISK"

// Dispatch routine to handle IRPs
NTSTATUS dispatch_irp(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

NTSTATUS read_from_virtual_disk(HANDLE fileName, char* buf, ULONG count, LARGE_INTEGER offset);

NTSTATUS write_request( const char* buf, ULONG count, LARGE_INTEGER offset);