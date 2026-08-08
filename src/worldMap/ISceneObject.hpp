#pragma once

#include "Idrawable.hpp"
#include "Icontrollable.hpp"

class ISceneObject : public Icontrollable, public Idrawable {
public:
    virtual void update() = 0;
    virtual ~ISceneObject() = default;
};