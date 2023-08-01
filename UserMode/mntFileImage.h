#pragma once

#include "mntImage.h"
#include "windows.h"
class FileImage : public IImage
{
    HANDLE m_hFile;
public:
    FileImage(const wchar_t* fileName);
    ~FileImage();
    void Read(char* buf, UINT64 offset, UINT32 bytesCount);
    void Write(const char* buf, UINT64 offset, UINT32 bytesCount);
    UINT64 Size();
};