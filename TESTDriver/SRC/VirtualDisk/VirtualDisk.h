#pragma once
#include "IRP/IRP.h"
#define MAX_SIZE 25

FAST_MUTEX fastMtx;

//�������
//1. ����� �� ��������� ���� � ������ ���������?
//2. �����, ��������������, ������ �������� �� ����������� ����� ����?




NTSTATUS deleteDevice();

//����� ��� �� MountedDisk, � ���� ����  IrpHandler irpDispatcher_ � ���� ��� PIRP

typedef struct Disk
{
	LARGE_INTEGER size;
	UNICODE_STRING FileName;
	PDEVICE_OBJECT obj;
	CHAR Letter;
	//DeviceId devID;        //����� ����� ������������������ ���������, �� �� ���� ����� ������� devID
	PUNICODE_STRING password;
	//���-�� ���� ���� ��� �������� ������. �����������/��������� ����� ��� ������������/��������������
} VIRTUALDISK, * PVIRTUALDISK;

VIRTUALDISK DiskList[MAX_SIZE];      //������ � 25 ��������� ����� (�� 26, �� �� ����� ���� � �� ��)

NTSTATUS CreateVirtualDisk();


NTSTATUS PrintDisk(char Letter); //�������� ������ � ������ ����