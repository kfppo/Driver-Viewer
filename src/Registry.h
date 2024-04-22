#pragma once

#include <windows.h>
#include <string>

struct RegResult
{
    bool bSuc = false;
    DWORD dwValue = 0;
    std::wstring wsValue;
};

class Registry
{
public:
    Registry();
    ~Registry();

    bool SetRootAndPath(HKEY hRootKey, const std::wstring& wsCurrentPath);

    bool KeyExists(HKEY hRootKey, const std::wstring& wsKey);
    bool ValueExists(HKEY hRootKey, const std::wstring& wsKey, const std::wstring& wsValue);

    bool CreateKey(DWORD dwOption);

    // Data reading functions
    RegResult GetDword(const std::wstring& wsName);
    RegResult GetString(const std::wstring& wsName);

    // Data writing functions
    bool SetDword(const std::wstring& wsName, const DWORD dwData);
    bool SetString(const std::wstring& wsName, const std::wstring& wsData);

    bool DeleteValue(const std::wstring& wsName);

protected:
    bool SetRootKey(HKEY hRootKey);
    bool SetCurrentPath(const std::wstring& wsCurrentPath);
    HKEY m_hRootKey;
    std::wstring m_wsCurrentPath;
};

// Reference:
// Use Modern C++ to Access the Windows Registry
// https://docs.microsoft.com/zh-tw/archive/msdn-magazine/2017/may/c-use-modern-c-to-access-the-windows-registry