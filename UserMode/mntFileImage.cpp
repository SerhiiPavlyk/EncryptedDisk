
#include "mntFileImage.h"
#include <stdexcept>

FileImage::FileImage(const wchar_t* fileName)
{
    const wchar_t* fileName_ = L"H:\\filedisk.txt";
   m_hFile = ::CreateFileW(fileName_,
                          FILE_READ_ACCESS | FILE_WRITE_ACCESS,
                          0,
                          NULL,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL, 
                          NULL);

    if (INVALID_HANDLE_VALUE == m_hFile) {
        throw std::exception("Can't open file.");
    }
}

FileImage::~FileImage()
{
    ::CloseHandle(m_hFile);
}

void FileImage::Read(char* buf, UINT64 offset, UINT32 bytesCount)
{
    DWORD bytesWritten = 0;
    LARGE_INTEGER tmpLi; 
    tmpLi.QuadPart = offset;
    if (!::SetFilePointerEx(m_hFile, tmpLi, &tmpLi, FILE_BEGIN)) {
        throw std::exception("SetFilePointerEx failed.");
    }
    
    if (!::ReadFile(m_hFile, buf, bytesCount, &bytesWritten, 0)) {
        throw std::exception("ReadFile failed.");
    }
}
void FileImage::Write(const char* buf, UINT64 offset, UINT32 bytesCount)
{
    DWORD bytesWritten = 0;
    LARGE_INTEGER tmpLi; tmpLi.QuadPart = offset;
    if (!::SetFilePointerEx(m_hFile, tmpLi, &tmpLi, FILE_BEGIN)) {
        throw std::exception("SetFilePointerEx failed.");
    }
    
    if (!::WriteFile(m_hFile, buf, bytesCount, &bytesWritten, 0)) {
        throw std::exception("WriteFile failed.");
    }
}
UINT64 FileImage::Size()
{
    LARGE_INTEGER fileSize = {0,0};
    if (!::GetFileSizeEx(m_hFile, &fileSize)) {
        throw std::exception("GetFileSizeEx failed.");
    }
    CloseHandle(m_hFile);
    return fileSize.QuadPart;
}