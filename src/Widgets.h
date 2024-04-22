#pragma once

#include <vector>

namespace CustomWidget
{
    void DriverTable(
        const std::string& strLabel,
        const std::string& strTitle,
        const ImVec2& childSize,
        const ImVec2& tableSize,
        const std::wstring& wsFileName,
        const std::vector<std::vector<std::string>>& vContainer);
}