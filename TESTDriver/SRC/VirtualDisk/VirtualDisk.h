#pragma once
#include "IRP/IRP.h"
#define MAX_SIZE 25

FAST_MUTEX fastMtx;

//ВОПРОСЫ
//1. Когда мы добавляем инфу о членах структуры?
//2. Когда, соответственно, делаем проверку на доступность буквы тома?




NTSTATUS deleteDevice();

//пошел код из MountedDisk, у него есть  IrpHandler irpDispatcher_ и пока что PIRP

typedef struct Disk
{
	LARGE_INTEGER size;
	UNICODE_STRING FileName;
	PDEVICE_OBJECT obj;
	CHAR Letter;
	//DeviceId devID;        //когда будет инициализироваться структура, мы не даем юзеру указать devID
	PUNICODE_STRING password;
	//что-то типа мапы для хранения дисков. добавляются/удаляются диски при монтировании/демонтировании
} VIRTUALDISK, * PVIRTUALDISK;

VIRTUALDISK DiskList[MAX_SIZE];      //список з 25 доступних дисків (не 26, бо як мінімум один є на пк)

NTSTATUS CreateVirtualDisk();


NTSTATUS PrintDisk(char Letter); //набросок поиска и вывода инфы