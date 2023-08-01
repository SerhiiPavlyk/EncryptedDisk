#pragma once
#include <windows.h>
#include <string>
#include <memory>

//typedef struct DriverControlStruct{
//	HANDLE coreDevice;
//	char driverName[];
//}DriverControl;
//
//void DriverControlInit();
//
//DriverControl drvCtrl;
//
//BOOL DeviceIoSet(DWORD ControlCode, PVOID InBuffer, DWORD BufferSize);
//BOOL DeviceIoGet(DWORD ControlCode, PVOID InBuffer, DWORD BufferSize,
//    PVOID OutBuffer, DWORD OutBufferSize, DWORD* BytesCount);
//
//void ConnectToDriver();
//void DisconnectFromDriver();
//BOOL IsDriverConnected();


namespace utils
{
    class DriverControl
    {
        HANDLE  coreDevice;
        std::wstring driverName_;

    public:
        DriverControl(const std::wstring& driverName);
        ~DriverControl();

        BOOL DeviceIoSet(DWORD ControlCode, PVOID InBuffer, DWORD BufferSize);
        BOOL DeviceIoGet(DWORD ControlCode, PVOID InBuffer, DWORD BufferSize,PVOID OutBuffer, DWORD OutBufferSize, DWORD* BytesCount);

        void ConnectToDriver();
        void DisconnectFromDriver();
        bool IsDriverConnected();
    };
}