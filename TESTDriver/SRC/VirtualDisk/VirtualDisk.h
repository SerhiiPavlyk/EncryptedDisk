#pragma once
#include "MountManager/MountManager.h"
#define MAX_SIZE 25

FAST_MUTEX fastMtx;

//�������
//1. ����� �� ��������� ���� � ������ ���������?
//2. �����, ��������������, ������ �������� �� ����������� ����� ����?

typedef struct DiskDevExt
{
	UINT32 deviceId;
}DeviceId;

typedef struct IrpHandlerStruct {
	UINT32 devId_;
	UINT64 totalLength_;
	PDEVICE_OBJECT  deviceObject_;
}IrpStruct;

NTSTATUS deleteDevice();

//����� ��� �� MountedDisk, � ���� ����  IrpHandler irpDispatcher_ � ���� ��� PIRP

IrpStruct irpStruct;

typedef struct Disk
{
	LARGE_INTEGER size;
	UNICODE_STRING FileName;
	PDEVICE_OBJECT obj;
	CHAR Letter;
	DeviceId devID;        //����� ����� ������������������ ���������, �� �� ���� ����� ������� devID
	PUNICODE_STRING password;
	//���-�� ���� ���� ��� �������� ������. �����������/��������� ����� ��� ������������/��������������
} VIRTUALDISK, * PVIRTUALDISK;

VIRTUALDISK DiskList[MAX_SIZE];      //������ � 25 ��������� ����� (�� 26, �� �� ����� ���� � �� ��)

NTSTATUS CreateVirtualDisk();

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