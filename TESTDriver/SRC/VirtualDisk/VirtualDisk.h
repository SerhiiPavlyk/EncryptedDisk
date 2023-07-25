
#pragma once
#include "main/pch.h"

#define MAX_SIZE 25

FAST_MUTEX fastMtx;

//ВОПРОСЫ
//1. Когда мы добавляем инфу о членах структуры?
//2. Когда, соответственно, делаем проверку на доступность буквы тома?

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
	DeviceId devID;				//когда будет инициализироваться структура, мы не даем юзеру указать devID
	UNICODE_STRING password;
	//что-то типа мапы для хранения дисков. добавляются/удаляются диски при монтировании/демонтировании
} VIRTUALDISK, * PVIRTUALDISK;

VIRTUALDISK DiskList[MAX_SIZE];			//список з 25 доступних дисків (не 26, бо як мінімум один є на пк)

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

NTSTATUS PrintDisk(char Letter); //набросок поиска и вывода инфы