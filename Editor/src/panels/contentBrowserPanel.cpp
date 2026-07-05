#include "fhpch.h"
#include "contentBrowserPanel.h"

#include <imgui.h>

extern const std::filesystem::path g_AssetPath = "assets";

ContentBrowserPanel::ContentBrowserPanel()
{
    m_CurrentDirectory = g_AssetPath;
    m_DirectoryIcon = Texture2D::create("resources/icons/contentBrowser/DirectoryIcon.png");
    m_FileIcon = Texture2D::create("resources/icons/contentBrowser/FileIcon.png");
    m_NavigateIcon = Texture2D::create("resources/icons/contentBrowser/navigateButton.png");
}

void ContentBrowserPanel::drawDirectoryTree(const std::filesystem::path& directory)
{
    for (auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_directory())
            continue; // tree only shows folders, not files

        const auto& path = entry.path();
        std::string name = path.filename().string();

        bool hasSubDirs = false;
        for (auto& sub : std::filesystem::directory_iterator(path))
        {
            if (sub.is_directory())
            {
                hasSubDirs = true;
                break;
            }
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (path == m_CurrentDirectory)
            flags |= ImGuiTreeNodeFlags_Selected;
        if (!hasSubDirs)
            flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::PushID(path.string().c_str());
        bool opened = ImGui::TreeNodeEx(name.c_str(), flags);
        bool toggledOpen = ImGui::IsItemToggledOpen();

        if (ImGui::IsItemClicked() && !toggledOpen)
            m_CurrentDirectory = path;

        if (opened)
        {
            if (hasSubDirs)
                drawDirectoryTree(path);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void ContentBrowserPanel::drawToolbar()
{
    float buttonSize = 20.0f;

    // Back button
    if (ImGui::ImageButton("##back", (ImTextureID)m_NavigateIcon->getRendererID(),
        { buttonSize, buttonSize }, { 0, 1 }, { 1, 0 }))
    {
        if (m_CurrentDirectory != g_AssetPath)
            m_CurrentDirectory = m_CurrentDirectory.parent_path();
    }

    ImGui::SameLine();

    // Showing the current directory name
    std::string dirName = m_CurrentDirectory.filename().string();
    float textWidth = ImGui::CalcTextSize(dirName.c_str()).x;
    float availableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SameLine(ImGui::GetCursorPosX() + availableWidth - textWidth);
    ImGui::Text("%s", dirName.c_str());
}

void ContentBrowserPanel::onImGuiRender()
{
    ImGui::Begin("Content Browser");

    // Left panel folder tree
    ImGui::BeginChild("DirectoryTree", ImVec2(200, 0), true);

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
    drawDirectoryTree(g_AssetPath);
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel toolbar and thumbnail grid
    ImGui::BeginChild("DirectoryContents", ImVec2(0, 0), true);

    drawToolbar();
    ImGui::Separator();

    static float thumbnailSize = 100.0f;
    static float padding = 6.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
    {
        const auto& path = entry.path();
        auto relativePath = std::filesystem::relative(path, g_AssetPath);
        std::string filenameString = relativePath.filename().string();

        Ref<Texture2D> icon = entry.is_directory() ? m_DirectoryIcon : m_FileIcon;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushID(filenameString.c_str());
        ImGui::ImageButton("##thumbnail", (ImTextureID)icon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

        if (ImGui::BeginDragDropSource())
        {
            std::string pathString = relativePath.string();
            const char* itemPath = pathString.c_str();
            ImGui::SetDragDropPayload("Content_Browser_Item", itemPath, pathString.size() + 1, ImGuiCond_Once);
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (entry.is_directory())
                m_CurrentDirectory /= path.filename();
        }
        ImGui::PopID();

        ImGui::TextWrapped("%s", filenameString.c_str());
        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::EndChild();

    ImGui::End();
}