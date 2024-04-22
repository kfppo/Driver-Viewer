#pragma once

#include <string>
#include <vector>

struct SystemDriverObj
{
    std::wstring wsDisplayName;
    std::wstring wsPathName;
    std::wstring wsFileVer;
    std::wstring wsName;
};

// TODO: make this class more reusable
class WmiInfo
{
public:
    WmiInfo();
    ~WmiInfo();

    std::vector<SystemDriverObj> GetSystemDriverInfo(const std::wstring& wsWQL);

    WmiInfo(const WmiInfo&) = delete;
    WmiInfo& operator =(const WmiInfo&) = delete;

private:
    //bool QueryInfo(const std::wstring& wsWQL);
};