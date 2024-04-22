#include <iostream>
#include "Registry.h"


Registry::Registry()
{
    m_hRootKey = HKEY_LOCAL_MACHINE;
}

Registry::~Registry()
{
}

bool Registry::SetRootAndPath(HKEY hRootKey, const std::wstring& wsCurrentPath)
{
    return SetRootKey(hRootKey) && SetCurrentPath(wsCurrentPath);
}

bool Registry::SetRootKey(HKEY hRootKey)
{
    if (hRootKey != HKEY_CLASSES_ROOT &&
        hRootKey != HKEY_CURRENT_USER &&
        hRootKey != HKEY_LOCAL_MACHINE &&
        hRootKey != HKEY_USERS) {
        return false;
    }
    m_hRootKey = hRootKey;
    return true;
}

bool Registry::SetCurrentPath(const std::wstring& wsCurrentPath)
{
    if (wsCurrentPath.empty()) return false;
    m_wsCurrentPath = wsCurrentPath;
    return true;
}

bool Registry::KeyExists(HKEY hRootKey, const std::wstring& wsKey)
{
    HKEY hKey;
    bool bFound = false;

    if (::RegOpenKeyExW(hRootKey, wsKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        ::RegCloseKey(hKey);
        bFound = true;
    }
    return bFound;
}

bool Registry::ValueExists(HKEY hRootKey, const std::wstring& wsKey, const std::wstring& wsValue)
{
    HKEY hKey;
    bool bFound = false;

    if (::RegOpenKeyExW(hRootKey, wsKey.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (::RegQueryValueEx(hKey, wsValue.c_str(), NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            bFound = true;
        }
        ::RegCloseKey(hKey);
    }
    return bFound;
}

bool Registry::CreateKey(DWORD dwOption)
{
    HKEY hKey;
    // Create volatile registry key
    if (RegCreateKeyExW(
        m_hRootKey,
        m_wsCurrentPath.c_str(),
        0,
        NULL,
        dwOption,		// "REG_OPTION_VOLATILE" for temp
        KEY_ALL_ACCESS,
        NULL,
        &hKey,
        NULL
    ) != ERROR_SUCCESS)
    {
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

RegResult Registry::GetDword(const std::wstring& wsName)
{
    RegResult RegRes;
    DWORD dwData{};
    DWORD dwDataSize = sizeof(dwData);

    LSTATUS ret = ::RegGetValueW(
        m_hRootKey,
        m_wsCurrentPath.c_str(),
        wsName.c_str(),
        RRF_RT_REG_DWORD,
        NULL,
        &dwData,
        &dwDataSize);

    if (ret == ERROR_SUCCESS)
    {
        RegRes.bSuc = true;
        RegRes.dwValue = dwData;
    }
    return RegRes;
}

RegResult Registry::GetString(const std::wstring& wsName)
{
    RegResult RegRes;
    DWORD dwDataSize{};

    // Get string data size first
    LSTATUS ret = ::RegGetValueW(
        m_hRootKey,
        m_wsCurrentPath.c_str(),
        wsName.c_str(),
        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
        nullptr,
        nullptr,
        &dwDataSize);

    if (ret != ERROR_SUCCESS) return RegRes;

    std::wstring wsData;
    wsData.resize(dwDataSize / sizeof(wchar_t));

    // Get string data
    ret = ::RegGetValueW(
        m_hRootKey,
        m_wsCurrentPath.c_str(),
        wsName.c_str(),
        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
        nullptr,
        &wsData[0],
        &dwDataSize);

    if (ret == ERROR_SUCCESS)
    {
        // Exclude null-terminated added by the Win32 API
        DWORD stringLengthInWchars = dwDataSize / sizeof(wchar_t);
        stringLengthInWchars--;
        wsData.resize(stringLengthInWchars);

        RegRes.bSuc = true;
        RegRes.wsValue = wsData;
    }
    return RegRes;
}

// TODO: Add overwrite warning
//
bool Registry::SetDword(const std::wstring& wsName, const DWORD dwData)
{
    HKEY hKey;
    LSTATUS lResult = ::RegOpenKeyExW(m_hRootKey, m_wsCurrentPath.c_str(), 0, KEY_ALL_ACCESS, &hKey);
    if (lResult != ERROR_SUCCESS) return false;

    lResult = ::RegSetValueExW(
        hKey,
        wsName.c_str(),
        0,
        REG_DWORD,
        (LPBYTE)&dwData,
        sizeof(DWORD));

    ::RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS ? true : false;
}

// TODO: Add overwritten warning
//
bool Registry::SetString(const std::wstring& wsName, const std::wstring& wsData)
{
    HKEY hKey;
    LSTATUS lResult = ::RegOpenKeyExW(m_hRootKey, m_wsCurrentPath.c_str(), 0, KEY_ALL_ACCESS, &hKey);
    if (lResult != ERROR_SUCCESS) return false;

    lResult = ::RegSetValueExW(
        hKey,
        wsName.c_str(),
        0,
        REG_SZ,
        (LPBYTE)&wsData[0],
        (wsData.size() + 1) * sizeof(wchar_t));

    ::RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS ? true : false;
}

bool Registry::DeleteValue(const std::wstring& wsName)
{
    HKEY hKey;
    LSTATUS lResult = ::RegOpenKeyExW(m_hRootKey, m_wsCurrentPath.c_str(), 0, KEY_ALL_ACCESS, &hKey);
    if (lResult != ERROR_SUCCESS) return false;

    lResult = ::RegDeleteValueW(hKey, wsName.c_str());
    RegCloseKey(hKey);
    return lResult == ERROR_SUCCESS ? true : false;
}




//// DEBUG
//int main()
//{
//  using namespace AsrModules;
// 
//	Registry reg;
//	RegResult RegRes;
//
//	//std::cout << "Set Root key? " << reg.SetRootKey(HKEY_CURRENT_USER) << std::endl;
//	//std::cout << "Set Path? " << reg.SetCurrentPath(L"SOFTWARE\\Valve\\Steam") << std::endl;
//
//	std::cout << "Set Root and Path? " << reg.SetRootAndPath(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam") << std::endl;
//	std::cout << "Key Exists? " << reg.KeyExists(HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam") << std::endl;
//
//	RegRes = reg.GetDword(L"Restart");
//	if (RegRes.bSuc) std::cout << "Get Dword? " << RegRes.dwValue << std::endl;
//	else std::cout << "Get Dword fail!" << std::endl;
//
//	RegRes = reg.GetString(L"SteamExe");
//	if (RegRes.bSuc) std::wcout << L"Get String? " << RegRes.wsValue + L"___END" << std::endl;
//	else std::cout << "Get String fail!" << std::endl;
//
//	std::cout << "Set Dword? " << reg.SetDword(L"RunningAppID", 100) << std::endl;
//	std::cout << "Set String? " << reg.SetString(L"Rate", L"30000") << std::endl;
//	std::cout << "Delete Value? " << reg.DeleteValue(L"Skinnnnn") << std::endl;
//	std::cout << std::endl;
//
//	//std::cout << "-----------------  GetUserSidFromProfileList() Test  -----------------" << std::endl;
//	//std::wcout << "Result: " << reg.GetUserSidFromProfileList(L"systemprofile") + L"___END" << std::endl;
//
//	//std::cout << std::endl;
//
//	//std::cout << "-----------------  GetProfileImagePath() Test  -----------------" << std::endl;
//	//std::vector<std::wstring> testcase{ L"S-1-5-18", L"S-1-5-19", L"S-1-5-20", L"S-1-5-21-2096838975-2079322560-1945923086-1001" };
//	//for (auto ws : testcase)
//	//{
//	//	RegRes = reg.GetProfileImagePath(ws);
//	//	if (RegRes.bSuc)
//	//	{
//	//		std::wcout << "GetProfileImagePath: " << RegRes.wsValue + L"___END" << std::endl;
//	//	}
//	//	else
//	//	{
//	//		std::cout << "RegRes False" << std::endl;
//	//	}
//	//}
//	return 0;
//}