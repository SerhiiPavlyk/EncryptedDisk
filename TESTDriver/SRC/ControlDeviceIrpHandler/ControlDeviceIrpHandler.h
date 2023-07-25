#pragma once
#define CORE_MNT_DISPATCHER  0x8001		//значения менее 0x8000 зарезервированы microsoft

#define MOUNT_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)		//значения менее 0x800 зарезервированы microsoft
#define EXCHANGE_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNMOUNT_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

//сюда мы переходим в том случае, если переданный пользователем объект устройства совпадает с gDeviceObject

#include "pch.h"

// для передачи данных от юзера через GUI в драйвер для монтирования диска
struct Mount_Request 
{
	ULONG length;
	WCHAR mountPoint;	//точка монтирования
};


//для ответа юзеру на на запрос монтирования диска
struct Mount_Response 
{
	ULONG DeviceID;
};

struct Exchange_Request
{
	ULONG DeviceID;
	ULONG lastType;				//последний тип обмена данными
	ULONG lastStatus;
	ULONG lastSize;
	char* data;					// указатель на буфер данных, который мы передадим или получим
	ULONG dataSize;				// размер переданных/полученных данных
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

