#pragma once

#include "entity.h"

class ScriptableEntity
{
private:
    Entity m_Entity;
    friend class Scene;

protected:
    virtual void onCreate() {}
    virtual void onDestroy() {}
    virtual void onUpdate(Timestep ts) {}

public:
    virtual ~ScriptableEntity() {}

    template<typename T>
    T& getComponent()
    {
        return m_Entity.getComponent<T>();
    }

};