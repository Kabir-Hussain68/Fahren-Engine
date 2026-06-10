#pragma once

#include <string>

#include "Engine/core/base.h"

class Texture
{
private:

public:
    virtual ~Texture() = default;

    virtual uint32_t getWidhth() const = 0;
    virtual uint32_t getHeight() const = 0;

    virtual void setData(void* data, uint32_t size) = 0;

    virtual void bind(uint32_t slot = 0) const = 0;

    virtual bool operator==(const Texture& other) const = 0;

};

class Texture2D : public Texture
{
private:

public:    
    static Ref<Texture2D> create(uint32_t width, uint32_t height);
    static Ref<Texture2D> create(const std::string& path);
};