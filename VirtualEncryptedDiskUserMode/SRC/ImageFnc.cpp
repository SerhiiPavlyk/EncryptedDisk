#define _CRT_NON_CONFORMING_SWPRINTFS

#include "ImageFnc.h"
#include <stdexcept>


void DiskMount(ULONG32 DeviceNumber, PDISK_PARAMETERS  diskParam)
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
		throw std::exception("DISK is BUSY");
	}

	const wchar_t    DriverName[] = DriverName_;

	Device = CreateFile(DriverName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		COPY_FILE_NO_BUFFERING,
		NULL);

	if (Device == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Error opening device: %d\n", GetLastError());
	}
	std::unique_ptr< MountDisksAmount>response = std::make_unique <MountDisksAmount>();

	*response.get() = { 0 };
	if (!DeviceIoControl(Device, IOCTL_FILE_DISK_GET_FREE_ID, NULL, 0, (PVOID)response.get(),
		sizeof(MountDisksAmount), &BytesReturned, NULL))
	{
		CloseHandle(Device);
		throw std::exception("Error sending IOCTL: %d\n", GetLastError());
	}

	CloseHandle(Device);
	DeviceNumber = response.get()->amount;
	swprintf(DeviceName, DIRECT_DISK_PREFIX L"%u", DeviceNumber);
	if (!DefineDosDevice(DDD_RAW_TARGET_PATH, &VolumeName[4], DeviceName))
	{
		throw std::exception("Failed to register the disk");
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
		throw std::exception("Can't open disk!");
	}

	if (!DeviceIoControl(
		Device,
		IOCTL_FILE_DISK_OPEN_FILE,
		diskParam,
		sizeof(DiskParam) + diskParam->FileNameLength - 1,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
		CloseHandle(Device);
		throw std::exception("Mount disk UNSUCCESSFULL!");
	}
	CloseHandle(Device);
	SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, DriveName, NULL);
	std::wcout << L"Disk " << &VolumeName[4] << L" mounted successfully" << std::endl;

}

void DiskUnmount(const wchar_t Letter)
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
		throw std::exception("Cannot open disk for unmount.");
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
	}

	CloseHandle(Device);

	if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL))
	{
		throw std::exception("Error to remove definition.");
	}

	SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, DriveName, NULL);

	std::wcout << L"Disk " << Letter << L": successfully unmount!" << std::endl;

}

void PrintAllDisks()
{
	const ULONG32 size = sizeof(ULONG32) + NumDisks * (sizeof(Response) + MAX_PATH * sizeof(wchar_t));
	std::unique_ptr<char[]> response = std::make_unique<char[]>(size);
	HANDLE                  Device;
	DWORD                   BytesReturned = { 0 };
	const wchar_t    DriverName[] = DriverName_;

	Device = CreateFile(DriverName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		COPY_FILE_NO_BUFFERING,
		NULL);

	if (Device == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Error opening device: %d\n", GetLastError());
	}

	if (!DeviceIoControl(Device, IOCTL_FILE_DISK_GET_ALL_DISK, NULL, 0, (PVOID)response.get(),
		size, &BytesReturned, NULL))
	{
		throw std::exception("Error sending IOCTL: %d\n", GetLastError());
	}

	CloseHandle(Device);

	ULONG32 numDisk = *(ULONG32*)response.get();

	Response* data = reinterpret_cast<Response*>(response.get() + sizeof(ULONG32));
	for (ULONG32 i = 0; i < numDisk; ++i)
	{
		std::wstring tempName (data[i].FileName);
		tempName.erase(tempName.begin(), tempName.begin()+ wcslen(DiskFilePrefix));
		wprintf(L"Disk Parameters:\n");
		wprintf(L"  Size: %lld bytes | %.2f megabytes | %.2f gigabytes\n",
			data[i].Size.QuadPart,
			data[i].Size.QuadPart / (1024.0 * 1024.0),
			data[i].Size.QuadPart / (1024.0 * 1024.0 * 1024.0));
		wprintf(L"  Letter: %C\n", data[i].Letter);
		wprintf(L"  FileNameLength: %hu\n", data[i].FileNameLength);
		wprintf(L"  FileName: %s\n", tempName.c_str());
		std::cout << "\n______________________\n";

	}
}
