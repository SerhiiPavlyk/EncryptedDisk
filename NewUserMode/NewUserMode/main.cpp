#include "mntFileImage.h"



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


int main(int argc, char* argv[])
{
	unsigned int				choose = 0;
	bool						start = true;
	PDISK_PARAMETERS			diskParam = new DISK_PARAMETERS;
	wchar_t						DiskLetter;
	char						DiskSize[32];
	wchar_t						FilePath[MAX_PATH];
	int							DeviceNumber = 0;


	try
	{
		while (start)
		{
			PrintOptions();
			std::cin >> choose;
			switch (choose)
			{
			case(1):
				std::cout << "MOUNT DISK\n";

				std::cout << "Write disk letter: ";
				std::wcin >> DiskLetter;
				DiskLetter = std::tolower(DiskLetter);
				if (!IsLetter(DiskLetter))
				{
					std::wcout << L"The DiskLetter is not between 'a' and 'z' (lowercase)." << std::endl;
					return -1;
				}
				diskParam->Letter = DiskLetter;

				std::cout << "\nSyntax how to write disk size: \n"
					<< "size[g | m | k]\n"
					<< "where g - gigabytes, m - megabytes, k - kilobytes\n"
					<< "EXAMPLE: 20M\n"
					<< "Disk size: ";
				std::cin >> DiskSize;
				{
					wchar_t DiskSizeLower = std::tolower(DiskSize[strlen(DiskSize) - 1]);
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
				}
				std::cout << "\nSyntax how to write filepath: \n"
					<< "C:\\User\\file.txt\n\n";
				std::cout << "Write file path: ";
				std::wcin >> FilePath;

				wcscpy(diskParam->FileName, L"\\??\\");
				wcscat(diskParam->FileName, FilePath);

				diskParam->FileNameLength = (USHORT)wcslen(diskParam->FileName);

				DiskMount(DeviceNumber, diskParam);
				DeviceNumber++;
				break;

			case(2):
				std::cout << "UNMOUNT DISK\n";
				std::cout << "Write disk letter: ";

				std::wcin >> DiskLetter;
				DiskLetter = std::tolower(DiskLetter);
				if (!IsLetter(DiskLetter))
				{
					std::wcout << L"The DiskLetter is not between 'a' and 'z' (lowercase)." << std::endl;
					return -1;
				}

				DiskUnmount(DiskLetter);

				break;

			case(3):
				std::cout << "list of mounted disks\n";

				PrintAllDisks();

				break;

			case(0):
				start = false;

				system("pause");

				break;

			default:
				std::cout << "Incorrect command\n";

				break;
			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what();
		delete diskParam;
		start = false;
		return -1;
	}



	return 0;
}
