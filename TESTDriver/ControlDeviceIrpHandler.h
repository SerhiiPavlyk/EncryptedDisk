#pragma once
#define CORE_MNT_DISPATCHER  0x8001		//�������� ����� 0x8000 ��������������� microsoft

#define MOUNT_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)		//�������� ����� 0x800 ��������������� microsoft
#define EXCHANGE_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNMOUNT_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//���� �� ��������� � ��� ������, ���� ���������� ������������� ������ ���������� ��������� � gDeviceObject

#include "pch.h"

// ��� �������� ������ �� ����� ����� GUI � ������� ��� ������������ �����
struct Mount_Request 
{
	ULONG length;
	WCHAR mountPoint;	//����� ������������
};


//��� ������ ����� �� �� ������ ������������ �����
struct Mount_Response 
{
	ULONG DeviceID;
};

struct Exchange_Request
{
	ULONG DeviceID;
	ULONG lastType;				//��������� ��� ������ �������
	ULONG lastStatus;
	ULONG lastSize;
	char* data;					// ��������� �� ����� ������, ������� �� ��������� ��� �������
	ULONG dataSize;				// ������ ����������/���������� ������
};

struct Exchange_Response
{
	ULONG type;
	ULONG size;
	ULONG offset;
};

struct Unmount_Request
{
	ULONG deviceId;
};

NTSTATUS Mount_Disk(PVOID buff, ULONG input_buff_length, ULONG output_buff_length);

NTSTATUS Unmount_Disk(PVOID buff, ULONG input_buff_length, ULONG output_buff_length);

NTSTATUS Data_Exchange_User_Driver(PVOID buff, ULONG input_buff_length, ULONG output_buff_length);

NTSTATUS ControlDeviceIrpHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp);

