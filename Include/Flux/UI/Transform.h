#pragma once

#include <UI/Point.h>

namespace Flux {

    struct Transform {

        Transform() = default;

        Transform(const Point& position, const Point& size, const Float32 rotation) : position(position), size(size),
                                                                                      rotation(rotation) {}

        NODISCARD Transform mix(Transform const& other) const {

            return { this->position + other.position, this->size, this->rotation + other.rotation};

        }

        NODISCARD FORCEINLINE Point centeredPosition() const {

            return { this->position.x + this->size.x / 2.0f, this->position.y + this->size.y / 2.0f };

        }

        Point position = {};
        Point size = {};
        Float32 rotation = 0.0f;

    };

}