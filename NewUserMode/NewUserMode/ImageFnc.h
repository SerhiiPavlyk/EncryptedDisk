#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define IOCTL_FILE_DISK_OPEN_FILE \
   CTL_CODE(FILE_DEVICE_DISK, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_FILE_DISK_CLOSE_FILE \
	CTL_CODE(FILE_DEVICE_DISK, 0x805, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)


#define IOCTL_FILE_DISK_GET_ALL_DISK  \
  CTL_CODE(FILE_DEVICE_DISK, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_FILE_DISK_GET_AMOUNT_OF_MOUNTED_DISKS  \
  CTL_CODE(FILE_DEVICE_DISK, 0x809, METHOD_BUFFERED, FILE_READ_ACCESS)

#include "windows.h"
#include <shlobj.h>
#include <iostream>
#include <memory>

#define DIRECT_DISK_PREFIX L"\\Device\\Vdisk"
#define NumDisks 20

typedef struct DiskParam {
	LARGE_INTEGER		Size;
	wchar_t				Letter;
	USHORT				FileNameLength;
	wchar_t				FileName[MAX_PATH];
	wchar_t				password[100];
	USHORT				PasswordLength;
} DISK_PARAMETERS, * PDISK_PARAMETERS;

typedef struct DiskParamR {
	LARGE_INTEGER		Size;
	wchar_t				Letter;
	USHORT				FileNameLength;
	wchar_t				FileName[MAX_PATH];
} Response, * PResponse;

typedef struct TypeMountDisks {
	ULONG32 amount;
}MountDisksAmount, * PMountDisksAmount;

void DiskMount(ULONG32 DeviceNumber, PDISK_PARAMETERS  diskParam);

void DiskUnmount(const wchar_t Letter);

void PrintAllDisks();
