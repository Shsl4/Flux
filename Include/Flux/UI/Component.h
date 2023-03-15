#pragma once

#define SK_GL
#define SK_METAL

#include <skia/include/core/SkPathEffect.h>
#include <skia/include/core/SkCanvas.h>
#include <Flux/UI/Reactive.h>
#include <Flux/UI/Color.h>

namespace Flux::UI {

    struct Point {

        Point() = default;
        
        Point(const Float32 x, const Float32 y) : x(x), y(y) {}
        
        Point operator+(Point const& other) const {
            return { this->x + other.x, this->y + other.y };
        }
        
        Point operator-(Point const& other) const {
            return { this->x - other.x, this->y - other.y };
        }
        
        Point operator*(Point const& other) const {
            return { this->x * other.x, this->y * other.y };
        }

        Float32 x = 0.0f;
        Float32 y = 0.0f;
        
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

        class Factory final {

            friend Allocator<Factory>;

            Factory() = default;
            
            static inline Unique<Factory> factory = nullptr;
            
        public:
        
            template<class ComponentType, typename... Args> requires std::is_base_of_v<Component, ComponentType>
            static ComponentType* create(Args&&... args) {
                Console::log("Creating {}.\n", Type::name<ComponentType>());
                auto* component = Allocator<ComponentType>::construct(std::forward<Args>(args)...);
                instance()->components.add(component);
                component->initialize();
                return component;
            }

            ~Factory();

            static bool valid(Component* component);

            static void dispose(Component* component);

        private:
            
            NODISCARD static Factory* instance();

            SmartArray<Component> components;
            
        };

        Component() = default;
        
        Component(Point const& p, Point const& s);

        explicit Component(Transform const& t);

        Component(Component const& other) = delete;

        Component(Component&& other) noexcept = default;
        
        Component& operator=(Component const& other) = delete;
        
        Component& operator=(Component&& other) noexcept = default;
        
        void dispose();

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
        
        NODISCARD FORCEINLINE Transform transform() const { return this->localTransform; }
        
        NODISCARD FORCEINLINE Point position() const { return this->localTransform.position; }
        
        NODISCARD FORCEINLINE Point size() const { return this->localTransform.size; }
        
        NODISCARD FORCEINLINE Float32 rotation() const { return this->localTransform.rotation; }
        
        NODISCARD FORCEINLINE Component* parent() const { return this->parentComponent; }
        
        NODISCARD FORCEINLINE Color color() const { return this->renderColor; }
        
        NODISCARD FORCEINLINE bool visible() const { return this->bVisible; }

        NODISCARD FORCEINLINE MutableArray<Component*> const& children() const { return this->childrenArray; }

        virtual void draw(SkCanvas* canvas, Float64 deltaTime);

        virtual void initialize();
        
        ~Component() override;

    protected:
        
        virtual void modified() {}

        virtual void willDispose() {}

        virtual void parentLinked() {}

        virtual void childAdded(Component* component) {}
        
        virtual void childWillDispose(Component* component) {}
        
        virtual void childModified(Component* component);

    private:

        Transform localTransform = {};
        Color renderColor = Colors::white;
        Component* parentComponent = nullptr;
        MutableArray<Component*> childrenArray = {};
        bool bVisible = true;
    
    };

}
