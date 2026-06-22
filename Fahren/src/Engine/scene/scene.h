#pragma once

#include "entt.hpp"

#include "Engine/core/timestep.h"

class Entity;

class Scene
{
private:
    friend class Entity;

    entt::registry m_Registry;

public:
    Scene();
    ~Scene();

    Entity createEntity(const std::string& name = std::string());

    void onUpdate(Timestep ts);
};