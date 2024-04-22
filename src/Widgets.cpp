#include "Imgui/imgui.h"
#include "Imgui/imgui_stdlib.h"

#include <windows.h>

#include "Widgets.h"
#include "Path.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip> // std::setw

namespace CustomWidget
{
    void DriverTable(
        const std::string& strLabel,
        const std::string& strTitle,
        const ImVec2& childSize,
        const ImVec2& tableSize,
        const std::wstring& wsFileName,
        const std::vector<std::vector<std::string>>& vContainer_)
    {
        static std::vector<std::vector<std::string>> vContainer;

        static ImGuiTextFilter filter;
        ImGui::Text("Filter");
        ImGui::SameLine();
        filter.Draw("");
        ImGui::SameLine();

        for (int i = 0; i < vContainer_.size(); i++)
        {
            if (filter.PassFilter(vContainer_[i][0].c_str()))
            {
                vContainer.push_back(vContainer_[i]);
            }
        }

        if (ImGui::Button("Export", ImVec2(65, 0)))
        {
            HANDLE hFile = CreateFileW(std::wstring(GetAppPath() + L"\\" + wsFileName).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                std::cout << GetLastError() << std::endl;
                return;
            }
            //std::string str;
            std::stringstream ss;

            for (int i = 0; i < vContainer.size(); i++)
            {
                //str += vSysDrvObjANSI[i][0] + "\t\t\t\t\t" + vSysDrvObjANSI[i][1] + "\n";
                //str += "[" + vContainer[i][1] + "]" + "\t\t\t" + vContainer[i][0] + "\n";
                //ss << std::setw(60) << std::setfill(' ') << std::left << vContainer[i][0] + " " << " [" + vContainer[i][1] + "]" << std::endl;
                ss << std::setw(40) << std::setfill(' ') << std::left << "[" + vContainer[i][1] + "]" << vContainer[i][0] << std::endl;
            }
            std::string str = ss.str();
            DWORD dwWritten{ 0 };
            WriteFile(hFile, &str[0], str.size(), &dwWritten, NULL);
            CloseHandle(hFile);
        }

        ImGui::Separator();

        ImGui::Text(strTitle.c_str());
        ImGui::BeginChild(std::string(strLabel + "list").c_str(), childSize);

        if (ImGui::BeginTable(strLabel.c_str(), 2, ImGuiTableColumnFlags_WidthStretch | ImGuiTableFlags_Borders, tableSize))
        {
            ImGuiListClipper clipper;
            clipper.Begin(vContainer.size());

            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(vContainer[row_n][0].c_str());
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), vContainer[row_n][2].c_str());
                        ImGui::EndTooltip();
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(vContainer[row_n][1].c_str());
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        vContainer.clear();
    }
}