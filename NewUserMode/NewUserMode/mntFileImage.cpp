#define _CRT_NON_CONFORMING_SWPRINTFS

#include "mntFileImage.h"
#include <stdexcept>

std::vector<char> LetterVector;


int DiskMount(ULONG32 DeviceNumber, PDISK_PARAMETERS  diskParam)
{
	wchar_t    VolumeName[] = L"\\\\.\\ :";
	wchar_t    DriveName[] = L" :\\";
	wchar_t    DeviceName[255];
	HANDLE  Device;
	DWORD   BytesReturned;

	VolumeName[4] = diskParam->Letter;
	DriveName[0] = diskParam->Letter;

	Device = CreateFile(
		VolumeName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL
	);

	if (Device != INVALID_HANDLE_VALUE)
	{
		CloseHandle(Device);
		SetLastError(ERROR_BUSY);
		throw std::exception("Device != INVALID_HANDLE_VALUE");
		return -1;
	}
	swprintf(DeviceName, DIRECT_DISK_PREFIX L"%u", DeviceNumber);
	if (!DefineDosDevice(
		DDD_RAW_TARGET_PATH,
		&VolumeName[4],
		DeviceName
	))
	{
		throw std::exception("!DefineDosDeviceA");
		return -1;
	}
	Device = CreateFile(
		VolumeName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL
	);

	if (Device == INVALID_HANDLE_VALUE)
	{
		DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
		throw std::exception("Device == INVALID_HANDLE_VALUE");
		return -1;
	}
	if (!DeviceIoControl(
		Device,
		IOCTL_FILE_DISK_OPEN_FILE,
		diskParam,
		sizeof(DiskParam) + diskParam->FileNameLength - 1,
		NULL,
		0,
		&BytesReturned,
		NULL
	))
	{
		DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
		CloseHandle(Device);
		throw std::exception("!DeviceIoControl(IOCTL_FILE_DISK_OPEN_FILE");
		return -1;
	}

	CloseHandle(Device);
	SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, DriveName, NULL);
	std::wcout << L"Disk " << &VolumeName[4] <<  L" mounted successfully" << std::endl;
	return 0;
}

int DiskUnmount(const wchar_t Letter)
{
	wchar_t    VolumeName[] = L"\\\\.\\ :";
	wchar_t    DriveName[] = L" :\\";
	HANDLE  Device;
	DWORD   BytesReturned;

	VolumeName[4] = Letter;
	DriveName[0] = Letter;


	
	//open disk
	Device = CreateFile(VolumeName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	if (INVALID_HANDLE_VALUE == Device)
	{
		throw std::exception("Cannot open disk.");
		return -1;
	}

	//1. lock access to the virtual disk file system
	if (!DeviceIoControl(Device,
		FSCTL_LOCK_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		CloseHandle(Device);
		throw std::exception("Error to lock access to the virtual disk file system.");
		return -1;
	}

	//2. close the file from which the virtual disk was created
	if (!DeviceIoControl(Device,
		IOCTL_FILE_DISK_CLOSE_FILE,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		CloseHandle(Device);
		throw std::exception("Error to close file in virtual disk.");
		return -1;
	}

	//3. dismount virtual disk
	if (!DeviceIoControl(Device,
		FSCTL_DISMOUNT_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		CloseHandle(Device);
		throw std::exception("Error to unmount virtual disk.");
		return -1;
	}

	//4. unlock access to the virtual disk file system
	if (!DeviceIoControl(Device,
		FSCTL_UNLOCK_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		CloseHandle(Device);
		throw std::exception("Error to unlock access to the virtual disk file system.");
		return -1;
	}

	CloseHandle(Device);

	if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL))
	{
		throw std::exception("Error to remove definition.");
	}

	//find and erase letter in LetterVector
	LetterVector.erase(std::find(LetterVector.begin(), LetterVector.end(), Letter));

	SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, DriveName, NULL);

	std::cout << "Disk " << Letter << ": successfully unmount!" << std::endl;

	return 0;
}

int PrintAllDisks(/*char DriveLetter*/)
{
	size_t size = NumDisks* (sizeof(Response) + MAX_PATH * sizeof(wchar_t));
	Response* response = new Response[NumDisks * (sizeof(Response) + MAX_PATH * sizeof(wchar_t))];
	HANDLE                  Device;
	DWORD                   BytesReturned;
	wchar_t    DriverName[] = L"\\\\.\\GLOBALROOT\\Device\\DEVICE_TEST_NAME";
	response[5].Letter = 't';
	Device = CreateFile(DriverName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		COPY_FILE_NO_BUFFERING,
		NULL);
	if (Device == INVALID_HANDLE_VALUE)
	{
		printf("Error opening device: %d\n", GetLastError());
		return -1;
	}
	// Step 2: Send the IRP_MJ_WRITE request to the driver
	if (!DeviceIoControl(Device, IOCTL_FILE_DISK_GET_ALL_DISK, NULL, 0, response,
		size, &BytesReturned, NULL))
	{
		printf("Error sending IOCTL: %d\n", GetLastError());
	}
	else
	{
		printf("IOCTL_FILE_DISK_CREATE_DISK request sent successfully to the driver.\n");
	}
	// Step 3: Close the device handle
	CloseHandle(Device);
	for (size_t i = 0; i < NumDisks; i++)
	{
		if(&response != NULL)
		{
			wprintf(L"Disk Parameters:\n");
			wprintf(L"  Size: %lld\n", response[i].Size.QuadPart);
			wprintf(L"  Letter: %C\n", response[i].Letter);
			wprintf(L"  FileNameLength: %hu\n", response[i].FileNameLength);
			wprintf(L"  FileName: %s\n", response[i].FileName);
			std::cout << "\n______________________\n";
		}
	}
	
	delete[]response;

	return 0;
}
