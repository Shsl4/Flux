#pragma once

#define SK_GL

#ifdef __APPLE__
#define SK_METAL
#elif defined(_WIN64)
#define SK_DIRECT3D
#define SK_VULKAN
#else
#define SK_VULKAN
#endif

#include <skia/include/core/SkColorSpace.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkPathEffect.h>
#include <Flux/UI/Reactive.h>
#include <Flux/UI/Color.h>
#include <Flux/UI/EdgeInsets.h>

namespace Flux {

    struct Point {

        Point() = default;
        
        Point(const Float32 x, const Float32 y) : x(x), y(y) {}
        
        Point operator+(Point const& other) const {
            return { this->x + other.x, this->y + other.y };
        }

        Point operator-(Point const& other) const {
            return { this->x - other.x, this->y - other.y };
        }

        Point operator-() const {
            return { -this->x, -this->y };
        }
        
        Point operator*(Point const& other) const {
            return { this->x * other.x, this->y * other.y };
        }
        
        Point operator/(Point const& other) const {
            return { this->x / other.x, this->y / other.y };
        }

        Point& operator+=(Point const& other) {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Point& operator-=(Point const& other) {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        }

        Point operator/(Float32 s) const {
            return { x / s, y / s };
        }

        Point operator*(Float32 s) const {
            return { x * s, y * s };
        }

        Float32 x = 0.0f;
        Float32 y = 0.0f;

    public:

        const static Point zero;

    };

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

    class Component : public Reactive {
        
    public:

        Component() = default;
        
        Component(Point const& p, Point const& s);

        explicit Component(Transform const& t);

        Component(Component const& other) = delete;

        Component(Component&& other) noexcept = default;
        
        Component& operator=(Component const& other) = delete;
        
        Component& operator=(Component&& other) noexcept = default;
        
        void dispose();

        void disposeAllChildren();

        void setPosition(Point const& p);

        void setSize(Point const& s);

        void setRotation(Float32 r);

        void setTransform(Transform const& t);

        void setColor(Color const& c);

        void setVisible(bool state);
        
        Component* addChild(Component* component);
        
        NODISCARD UInt depth() const;

        NODISCARD bool inBounds(Point const& p) const;

        NODISCARD Transform globalTransform() const;
        
        NODISCARD FORCEINLINE Transform const& transform() const { return this->localTransform; }

        NODISCARD FORCEINLINE Float32 rotation() const { return this->localTransform.rotation; }

        NODISCARD FORCEINLINE Point const& position() const { return this->localTransform.position; }
        
        NODISCARD FORCEINLINE Point const& size() const { return this->localTransform.size; }

        NODISCARD FORCEINLINE Component* parent() const { return this->parentComponent; }

        NODISCARD FORCEINLINE Color const& color() const { return this->renderColor; }

        NODISCARD FORCEINLINE EdgeInsets& insets() { return this->edgeInsets; }

        NODISCARD FORCEINLINE EdgeInsets const& insets() const { return this->edgeInsets; }
        
        NODISCARD FORCEINLINE bool visible() const { return this->bVisible; }

        NODISCARD FORCEINLINE MutableArray<Component*> const& children() const { return this->childrenArray; }

        NODISCARD bool isChild(const Component* component) const;

        NODISCARD bool isNestedChild(const Component* component) const;

        NODISCARD class Window* window() const;

        NODISCARD virtual Component* componentAtPosition(Point const& p);

        template<typename T>
        NODISCARD T* firstComponentOf() const {

            for (auto const& child : childrenArray) {
                
                if(T* obj = dynamic_cast<T*>(child)) {
                    
                    return obj;
                    
                }
                
            }

            return nullptr;
            
        }
        
        virtual void draw(SkCanvas* canvas, Float64 deltaTime);

        virtual void initialize();

        ~Component() override;

    protected:
        
        virtual void modified() {}

        virtual void willDispose() {}

        virtual void parentLinked() {}

        virtual void colorChanged() {}

        virtual void parentModified() {}

        virtual void childAdded(Component* component) {}
        
        virtual void childWillDispose(Component* component) {}
        
        virtual void childModified(Component* component);

        NODISCARD virtual bool supportsChildren() const;

    private:

        Transform localTransform = {};
        Color renderColor = Colors::white;
        Component* parentComponent = nullptr;
        MutableArray<Component*> childrenArray = {};
        EdgeInsets edgeInsets = {};
        bool bVisible = true;

    };

}
