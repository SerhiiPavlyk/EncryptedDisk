#include "ImageFnc.h"

void PrintOptions()
{
	std::cout << "--------------------------------------------\n";
	std::cout << "Select an action\n"
		<< "0. Close app\n"
		<< "1. Mount disk\n"
		<< "2. Unmount disk\n"
		<< "3. View list of mounted disks\n";
	std::cout << "--------------------------------------------\n";
}

inline bool IsLetter(const wchar_t letter)
{
	return (letter >= L'a' && letter <= L'z');
}

int main(void)
{
	unsigned int				choose = 0;
	bool						start = true;
	wchar_t						DiskLetter;
	char						DiskSize[32];
	wchar_t						FilePath[MAX_PATH];
	ULONG32						DeviceNumber = 0;
	wchar_t						password[100];
	HANDLE						Device;
	DWORD						BytesReturned;
	wchar_t						DriverName[] = DriverName_;
	setlocale(LC_ALL, "");
	SetConsoleCP(1251);
	std::unique_ptr<DISK_PARAMETERS>diskParam = std::make_unique<DISK_PARAMETERS>();

	Device = CreateFile(DriverName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		COPY_FILE_NO_BUFFERING,
		NULL);
	try
	{
	if (Device == INVALID_HANDLE_VALUE)
	{
		throw std::exception("Error opening device: %d\n", GetLastError());
	}

	std::unique_ptr< MountDisksAmount>response = std::make_unique <MountDisksAmount>();

	*response.get() = { 0 };

	if (!DeviceIoControl(Device, IOCTL_FILE_DISK_GET_AMOUNT_OF_MOUNTED_DISKS, NULL, 0, (PVOID)response.get(),
		sizeof(MountDisksAmount), &BytesReturned, NULL))
	{
		throw std::exception("Error sending IOCTL: %d\n", GetLastError());
	}

	CloseHandle(Device);

	DeviceNumber = response.get()->amount;


		while (start)
		{
			PrintOptions();
			std::cin >> choose;
			switch (choose)
			{
			case(1):
			{
				std::cout << "MOUNT DISK\n";

				std::cout << "Write disk letter: ";
				std::wcin >> DiskLetter;

				DiskLetter = static_cast<wchar_t>(std::tolower(DiskLetter));

				if (!IsLetter(DiskLetter))
				{
					std::wcout << L"The disk letter is not between 'a' and 'z' (lowercase)." << std::endl;
					break;
				}

				diskParam->Letter = DiskLetter;

				std::cout << "\nSyntax how to write disk size: \n"
					<< "size[g | m | k]\n"
					<< "where g - gigabytes, m - megabytes, k - kilobytes\n"
					<< "RECOMMENDTAIONS: SIZE DISK > 10 MB\n"
					<< "EXAMPLE: 20m\n"
					<< "Disk size: ";

				std::cin >> DiskSize;
				wchar_t DiskSizeLower = static_cast<wchar_t>(std::tolower(DiskSize[strlen(DiskSize) - 1]));

				if (DiskSizeLower == 'g')
				{
					diskParam->Size.QuadPart = _atoi64(DiskSize) * 1024 * 1024 * 1024;
				}
				else if (DiskSizeLower == 'm')
				{
					diskParam->Size.QuadPart = _atoi64(DiskSize) * 1024 * 1024;
				}
				else if (DiskSizeLower == 'k')
				{
					diskParam->Size.QuadPart = _atoi64(DiskSize) * 1024;
				}
				else
				{
					diskParam->Size.QuadPart = _atoi64(DiskSize);		//bytes
				}

				std::cout << "\nSyntax how to write filepath: \n"
					<< "C:\\User\\file.txt\n\n";

				std::cout << "Write file path: ";
				std::wcin >> FilePath;

				wcscpy(diskParam->FileName, DiskFilePrefix);
				wcscat(diskParam->FileName, FilePath);

				diskParam->FileNameLength = static_cast<USHORT>(wcslen(diskParam->FileName));

				std::cout << "Write password to the disk: ";
				std::wcin >> password;

				wcscpy(diskParam->password, password);

				diskParam->PasswordLength = static_cast<USHORT>(wcslen(diskParam->password));

				DiskMount(DeviceNumber, diskParam.get());
				DeviceNumber++;

				break;
			}
			case(2):
			{
				std::cout << "UNMOUNT DISK\n";
				std::cout << "Write disk letter: ";

				std::wcin >> DiskLetter;

				DiskLetter = static_cast<wchar_t>(std::tolower(DiskLetter));

				if (!IsLetter(DiskLetter))
				{
					std::wcout << L"The DiskLetter is not between 'a' and 'z' (lowercase)." << std::endl;
					break;
				}

				DiskUnmount(DiskLetter);

				break;
			}
			case(3):
			{
				std::cout << "List of mounted disks\n";

				PrintAllDisks();

				break;
			}
			case(0):
			{
				start = false;
				
				break;
			}
			default:
			{
				std::cout << "Incorrect command\n";

				break;
			}
			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what();
		start = false;
		return -1;
	}
	system("pause");
	return 0;
}
