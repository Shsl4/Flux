#pragma once

#include <Flux/UI/Reactive.h>
#include <Flux/UI/Color.h>
#include <Flux/UI/Transform.h>
#include <Flux/UI/Graphics.h>

namespace Flux {

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
        
        virtual void draw(Graphics const& graphics);

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
        Color renderColor = Color::white;
        Component* parentComponent = nullptr;
        MutableArray<Component*> childrenArray = {};
        bool bVisible = true;

    };

}
