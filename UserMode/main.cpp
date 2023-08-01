#include <iostream>
#include <conio.h>          //for getch()
#include "mntImage.h"
#include "mntFileImage.h"
#include "mntSyncMountManager.h"
#include <string>
#include <limits> // Include the <limits> header for std::numeric_limits

#include <mutex>
#include <thread>

void PrintOptions()
{
	std::cout << "--------------------------------------------\n";
	std::cout << "Select an action\n"
		<< "0. Close app\n"
		<< "1. Mount disk\n"
		<< "2. Unmount disk\n";
	std::cout << "--------------------------------------------\n";
}

std::mutex mux;
void Mount(SyncMountManager &mountManager)
{
	std::string fileName;
	std::cout << "Enter file name: \n";
	std::cin.ignore(100, '\n');
	std::getline(std::cin, fileName);
	std::wstring wfileNam = std::wstring(fileName.begin(), fileName.end());
	std::cout << "---------------------\n";
	std::wcout << wfileNam << std::endl;
	std::cout << "---------------------\n";
	std::auto_ptr<IImage> img(new FileImage(wfileNam.c_str()));

	wchar_t letter;
	do
	{
		std::cout << "Enter Letter of virtual disk:\t";
		std::wcin >> letter;
		std::cout << "---------------------\n";
		std::wcout << letter << std::endl;
		std::cout << "---------------------\n";
	} while (!std::isalpha(letter));

	{
		std::lock_guard<std::mutex> lock(mux);
		int devId = mountManager.AsyncMountImage(img, letter, (wfileNam.c_str()));
		std::cout << "Image was mounted. Device ID = " << devId << std::endl;
	}

}
void Print(SyncMountManager& mountManager)
{
	
}

void UnMount(SyncMountManager& mountManager)
{
	int devID;
	std::cout << "Enter Device ID for unmount:\t";
	std::cin >> devID;
	mountManager.UnmountImage(devID);
	std::cout << "Image was unmounted.\n";
}
int main(int argc, char* argv[])
{
	unsigned int choose = 0;
	bool start = true;
	SyncMountManager mountManager;
	try
	{
		while (start)
		{
			PrintOptions();
			std::cin >> choose;
			switch (choose)
			{
			case(1):
				Mount(mountManager);
				break;
			case(2):
				UnMount(mountManager);
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
	}
	return 0;
}


