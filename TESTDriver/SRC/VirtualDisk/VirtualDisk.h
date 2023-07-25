
#pragma once
#include "main/pch.h"

#define MAX_SIZE 25

FAST_MUTEX fastMtx;

//�������
//1. ����� �� ��������� ���� � ������ ���������?
//2. �����, ��������������, ������ �������� �� ����������� ����� ����?

typedef struct DiskDevExt
{
	int deviceId;
}DeviceId;



typedef struct Disk
{
	LARGE_INTEGER size;
	UNICODE_STRING FileName;
	PDRIVER_OBJECT obj;
	CHAR Letter;
	DeviceId devID;				//����� ����� ������������������ ���������, �� �� ���� ����� ������� devID
	UNICODE_STRING password;
	//���-�� ���� ���� ��� �������� ������. �����������/��������� ����� ��� ������������/��������������
} VIRTUALDISK, * PVIRTUALDISK;

VIRTUALDISK DiskList[MAX_SIZE];			//������ � 25 ��������� ����� (�� 26, �� �� ����� ���� � �� ��)

NTSTATUS CreateVirtualDisk(/*struct Disk*/VIRTUALDISK disk);

NTSTATUS MountDisk(VIRTUALDISK disk);

NTSTATUS UnmountDisk(UINT32 deviceId);

NTSTATUS DispatchIrp(UINT32 devId, PIRP irp);

VOID RequestExchange(UINT32 devID,
	/*VIRTUALDISK disk,*/
	UINT32 lastType,
	UINT32 lastStatus,
	UINT32 lastSize,
	char* buf,
	UINT32 bufSize,
	UINT32* type,
	UINT32* length,
	UINT64* offset);

NTSTATUS PrintDisk(char Letter); //�������� ������ � ������ ����