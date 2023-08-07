#pragma once

#include <ntifs.h> 
#include <winsmcrd.h>
#include <handleapi.h>
#include <wdm.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <mountdev.h>
#include <ntdef.h> // Include the necessary header for OBJECT_ATTRIBUTES
#include <ntddvol.h>
#include <ntstrsafe.h>

#include <wdmsec.h>

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcessToken(
	IN HANDLE       ProcessHandle,
	IN ACCESS_MASK  DesiredAccess,
	OUT PHANDLE     TokenHandle
);

NTSYSAPI
NTSTATUS
NTAPI
ZwAdjustPrivilegesToken(
	IN HANDLE               TokenHandle,
	IN BOOLEAN              DisableAllPrivileges,
	IN PTOKEN_PRIVILEGES    NewState,
	IN ULONG                BufferLength,
	OUT PTOKEN_PRIVILEGES   PreviousState OPTIONAL,
	OUT PULONG              ReturnLength
);

#define _NO_CRT_STDIO_INLINE

#define DISK_TAG 'disk'

#define DIRECT_DISK_PREFIX L"\\Device\\Vdisk"

//#define MOUNTDEVCONTROLTYPE                 ((ULONG)'M')
#define IOCTL_MOUNTDEV_UNIQUE_ID_CHANGE_NOTIFY_READWRITE CTL_CODE(MOUNTDEVCONTROLTYPE, 1, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define CORE_MNT_DISPATCHER        0x8001
#define SECTOR_SIZE 512
#define CORE_MNT_MOUNT_IOCTL \
    CTL_CODE(CORE_MNT_DISPATCHER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CORE_MNT_EXCHANGE_IOCTL \
    CTL_CODE(CORE_MNT_DISPATCHER, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CORE_MNT_UNMOUNT_IOCTL \
    CTL_CODE(CORE_MNT_DISPATCHER, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)



#define IOCTL_MOUNTDEV_UNIQUE_ID_CHANGE_NOTIFY    CTL_CODE(MOUNTDEVCONTROLTYPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define 	TOC_DATA_TRACK   0x04

#define MAX_DISK_AMOUNT 20

typedef struct CORE_MNT_MOUNT_REQUEST
{
	ULONG64 totalLength;
	const wchar_t* FileName;
	WCHAR   mountPoint;
}CoreMNTMountRequest;

typedef struct CORE_MNT_MOUNT_RESPONSE
{
	ULONG32 deviceId;
}CoreMNTMountResponse;

typedef struct CORE_MNT_EXCHANGE_REQUEST
{
	ULONG32 deviceId;
	ULONG32 lastType;
	ULONG32 lastStatus;
	ULONG32 lastSize;
	char* data;
	ULONG32 dataSize;
}CoreMNTExchangeRequest;

typedef struct CORE_MNT_EXCHANGE_RESPONSE
{
	ULONG32 type;
	ULONG32 size;
	ULONG64 offset;
}CoreMNTExchangeResponse;

typedef struct CORE_MNT_UNMOUNT_REQUEST
{
	ULONG32 deviceId;
}CoreMNTUnmountRequest;


typedef enum _DiskOperationType
{
	directOperationSuccess = 0,
	directOperationEmpty = 0,
	directOperationRead,
	directOperationWrite,
	directOperationFail,
	directOperationMax = directOperationFail
}DiskOperationType;


#define IOCTL_FILE_DISK_OPEN_FILE \
   CTL_CODE(FILE_DEVICE_DISK, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_FILE_DISK_CLOSE_FILE \
	CTL_CODE(FILE_DEVICE_DISK, 0x805, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_FILE_DISK_QUERY_FILE  \
	CTL_CODE(FILE_DEVICE_DISK, 0x806, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_FILE_DISK_CREATE_DISK  \
	CTL_CODE(FILE_DEVICE_DISK, 0x807, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_FILE_DISK_GET_ALL_DISK  \
	CTL_CODE(FILE_DEVICE_DISK, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_FILE_DISK_GET_AMOUNT_OF_MOUNTED_DISKS  \
	CTL_CODE(FILE_DEVICE_DISK, 0x809, METHOD_BUFFERED, FILE_READ_ACCESS)

typedef struct _DEVICE_EXTENSION {
	UNICODE_STRING              device_name;
	HANDLE                      file_handle;
	UNICODE_STRING                 file_name;
	ULONG                       device_ID;
	LARGE_INTEGER               file_size;
	PSECURITY_CLIENT_CONTEXT    security_client_context;//////
	LIST_ENTRY                  list_head;
	BOOLEAN                     media_in_device;
	KSPIN_LOCK                  list_lock;
	KEVENT                      request_event;
	PVOID                       thread_pointer;
	BOOLEAN                     terminate_thread;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


typedef struct DiskParam {
	LARGE_INTEGER		Size;
	wchar_t				Letter;
	USHORT				FileNameLength;
	wchar_t				FileName[MAX_PATH];

} DISK_PARAMETERS, * PDISK_PARAMETERS;
typedef struct DiskParamR {
	LARGE_INTEGER		Size;
	wchar_t				Letter;
	USHORT				FileNameLength;
	wchar_t				FileName[MAX_PATH];
} Response, * PResponse;
