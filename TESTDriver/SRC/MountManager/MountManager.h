#pragma once
#include "main/pch.h"
#include "VirtualDisk/VirtualDisk.h"
FAST_MUTEX diskMapLock_;
BOOL isInitializied = FALSE;

UINT32 gDiskCount = 0;   //������� ������, ���������� ����������

NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp);
NTSTATUS MountDisk(VIRTUALDISK disk);
NTSTATUS UnmountDisk(UINT32 deviceId);			//����� ����, ��� ����� ���� ���������� � ����� ������� �������
VOID RequestExchange(UINT32 devID,
	UINT32 lastType,
	UINT32 lastStatus,
	UINT32 lastSize,
	char* buf,
	UINT32 bufSize,
	UINT32* type,
	UINT32* length,
	UINT64* offset);
