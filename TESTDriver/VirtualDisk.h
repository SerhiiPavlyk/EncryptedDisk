#pragma once
#include "pch.h"


typedef struct DiskDevExt
{
	int deviceId;
}DeviceId;

typedef struct DISK
{
	LARGE_INTEGER size;
	UNICODE_STRING FileName;
	char Letter;
	ULONG devID;
	UNICODE_STRING password;
} VIRTUALDISK, * PVIRTUALDISK;


NTSTATUS CreateVirtualDisk(VIRTUALDISK virtualDisk);