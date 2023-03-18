#pragma once

#include <Nucleus/Nucleus.h>

namespace Flux{

    using namespace Nucleus;

    struct EdgeInsets {

    public:

        EdgeInsets() = default;

        EdgeInsets(Float32 top, Float32 bottom, Float32 left, Float32 right) :
            top(top), bottom(bottom), left(left), right(right) {

        }

        static EdgeInsets withHorizontal(Float32 value) { return { 0.0f, 0.0f, value, value }; }

        static EdgeInsets withVertical(Float32 value) { return { value, value, 0.0f, 0.0f }; }

        NODISCARD FORCEINLINE Float32 verticalInsets() const { return this->top + this->bottom; }

        NODISCARD FORCEINLINE Float32 horizontalInsets() const { return this->left + this->right; }

        FORCEINLINE void setVerticalInsets(Float32 value){
            this->top = value;
            this->bottom = value;
        }

        FORCEINLINE void setHorizontalInsets(Float32 value){
            this->left = value;
            this->right = value;
        }

        void reset() {

            this->top = 0.0f;
            this->bottom = 0.0f;
            this->left = 0.0f;
            this->right = 0.0f;

        }

        Float32 top = 0.0f;
        Float32 bottom = 0.0f;
        Float32 left = 0.0f;
        Float32 right = 0.0f;

    };

}