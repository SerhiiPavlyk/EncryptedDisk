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
#define _NO_CRT_STDIO_INLINE

#define ROOT_DIR_NAME        L"\\??\\H:\\DISK"

#define DIRECT_DISK_PREFIX ROOT_DIR_NAME L"\\disk"

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

#define MAX_SIZE 25

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