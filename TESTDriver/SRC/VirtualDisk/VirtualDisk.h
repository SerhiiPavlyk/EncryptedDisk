#pragma once
#include "main/pch.h"


typedef struct DiskDevExt
{
	int deviceId;
}DeviceId;

typedef struct DISK
{
	LARGE_INTEGER size;
	PUNICODE_STRING FileName;
	char Letter;
	DeviceId devID;
	PUNICODE_STRING password;
} VIRTUALDISK, * PVIRTUALDISK;


NTSTATUS CreateVirtualDisk(VIRTUALDISK virtualDisk);