#pragma once

#include "Engine/core/base.h"
#include "scene.h"

class SceneSerializer
{
private:
    Ref<Scene> m_Scene;

public:
    SceneSerializer(const Ref<Scene>& scene);

    void serialize(const std::string& filepath);
    void serializeRuntime(const std::string& filepath);

    bool deserialize(const std::string& filepath);
    bool deserializeRuntime(const std::string& filepath);
};