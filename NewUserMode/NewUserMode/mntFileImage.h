#pragma once
#define _CRT_SECURE_NO_WARNINGS

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


//#include "mntImage.h"
#include "windows.h"
#include <shlobj.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

#define DIRECT_DISK_PREFIX L"\\Device\\Vdisk"
#define NumDisks 20
typedef struct DiskParam {
	LARGE_INTEGER		Size;
	wchar_t				Letter;
	USHORT				FileNameLength;
	wchar_t				FileName[MAX_PATH];
} DISK_PARAMETERS, * PDISK_PARAMETERS;
int DiskMount(ULONG32 DeviceNumber, PDISK_PARAMETERS  diskParam);

int DiskUnmount(const wchar_t Letter);

int PrintAllDisks();
typedef struct TypeMountDisks
{
	ULONG32 amount;
}MountDisksAmount, * PMountDisksAmount;

