#pragma once

#include "Engine/renderer/texture.h"

#include <filesystem>

class ContentBrowserPanel
{
private:
    std::filesystem::path m_CurrentDirectory;

    Ref<Texture2D> m_DirectoryIcon;
    Ref<Texture2D> m_FileIcon;
    Ref<Texture2D> m_NavigateIcon;

    void drawDirectoryTree(const std::filesystem::path& directory);
    void drawToolbar();

public:
    ContentBrowserPanel();

    void onImGuiRender();
};