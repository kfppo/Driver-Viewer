#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <winver.h>
#include <memory>

#include "WMI.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Version.lib")


WmiInfo::WmiInfo()
{

}

WmiInfo::~WmiInfo()
{

}

std::vector<SystemDriverObj> WmiInfo::GetSystemDriverInfo(const std::wstring& wsWQL)
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x"
            << std::hex << hres << std::endl;
        return {};                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );


    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. Error code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return {};                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return {};                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices* pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x"
            << std::hex << hres << std::endl;
        pLoc->Release();
        CoUninitialize();
        return {};                // Program has failed.
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        std::cout << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return {};               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t(L"WQL"),
        //bstr_t("SELECT * FROM Win32_SystemDriver"), //bstr_t("SELECT * FROM Win32_OperatingSystem"),
        bstr_t(wsWQL.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cout << "Query for operating system name failed."
            << " Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return {};               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    std::vector<SystemDriverObj> vDrivers;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        // 
        // Get the value of the Name property
        //hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

        
        hr = pclsObj->Get(L"DisplayName", 0, &vtProp, 0, 0);

        SystemDriverObj DriverObj;

        if (hr == S_OK)
        {
            DriverObj.wsDisplayName = vtProp.bstrVal;
        }

        VariantClear(&vtProp);

        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

        if (hr == S_OK)
        {
            DriverObj.wsName = vtProp.bstrVal;
        }
        VariantClear(&vtProp);

        hr = pclsObj->Get(L"PathName", 0, &vtProp, 0, 0);

        if (hr == S_OK)
        {
            DriverObj.wsPathName = vtProp.bstrVal;
        }
        //VariantClear(&vtProp);

        // Get .sys fileversion
        DWORD verHandle = 0;
        DWORD verSize = GetFileVersionInfoSizeW(vtProp.bstrVal, &verHandle);
        //LPSTR verData = new char[verSize];

        // TEST THIS
        std::unique_ptr<BYTE[]> verData = std::make_unique<BYTE[]>(verSize);

        if (GetFileVersionInfoW(vtProp.bstrVal, verHandle, verSize, verData.get()))
        {
            BYTE* lpBuffer = nullptr;
            UINT size = 0;
            if (VerQueryValueW(verData.get(), L"\\", (LPVOID*)&lpBuffer, &size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    //if (verInfo->dwSignature == 0xfeef04bd)
                    {

                        // Doesn't matter if you are on 32 bit or 64 bit,
                        // DWORD is always 32 bits, so first two revision numbers
                        // come from dwFileVersionMS, last two come from dwFileVersionLS
                        std::wstring wsVer = std::to_wstring((verInfo->dwFileVersionMS >> 16) & 0xffff)
                            + L"." + std::to_wstring((verInfo->dwFileVersionMS >> 0) & 0xffff) 
                            + L"." + std::to_wstring((verInfo->dwFileVersionLS >> 16) & 0xffff) 
                            + L"." + std::to_wstring((verInfo->dwFileVersionLS >> 0) & 0xffff);
                        
                        DriverObj.wsFileVer = wsVer;
                    }
                }
            }
        }

        vDrivers.push_back(DriverObj);

        //delete verData;
        pclsObj->Release();
    }

    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return vDrivers;   // Program successfully completed.
}

//int main()
//{
//    WmiInfo wmi;
//    std::vector<SystemDriverObj> pack = wmi.GetSystemDriverInfo(L"select * from Win32_SystemDriver");
//
//    if (!pack.empty())
//    {
//        for (auto n : pack)
//        {
//            //std::wcout << "DisplayName: " << n.wsDisplayName << std::endl;
//            std::wcout << "PathName: " << n.wsPathName << std::endl;
//            std::wcout << "FileVer: " << n.wsFileVer << std::endl;
//            std::wcout << std::endl;
//        }
//    }
//    else
//    {
//        std::cout << "Obj is empty" << std::endl;
//    }
//    return 0;
//}