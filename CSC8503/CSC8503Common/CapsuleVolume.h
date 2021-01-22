#pragma once
#include "CollisionVolume.h"
#include <string>
namespace NCL {
    class CapsuleVolume : public CollisionVolume
    {
    public:
        CapsuleVolume(float halfHeight, float radius) {
            this->halfHeight    = halfHeight;
            this->radius        = radius;
            this->type          = VolumeType::Capsule;
        };
        ~CapsuleVolume() {

        }
        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }

        std::string name;

    protected:
        float radius;
        float halfHeight;
    };
}

