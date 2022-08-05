#pragma once

#include <Flux/Core/Memory/Allocator.h>
#include <Flux/Core/Memory/RefCounter.h>

namespace Flux {
    template <typename T>
    class WeakPointer;

    /**
     * \brief A smart shared pointer class.
     * 
     * \tparam T Type of the object to manage.
     */
    template <typename T>
    class SharedPointer final {

        template<typename Y>
        friend class SharedPointer;
    
        friend WeakPointer<T>;
    
    public:

        template <typename ... Args>
        static SharedPointer<T> make(Args&&... args);

        explicit SharedPointer(T* object);

        SharedPointer() = default;

        ~SharedPointer();

        explicit SharedPointer(const WeakPointer<T>& weak);

        SharedPointer(const SharedPointer& other);
    
        SharedPointer(nullptr_t) noexcept;

        template<typename Y>
        SharedPointer(SharedPointer<Y> const& other);

        SharedPointer(SharedPointer&& other) noexcept;

        SharedPointer& operator=(const SharedPointer& other);

        SharedPointer& operator=(SharedPointer&& other) noexcept;

        SharedPointer& operator=(nullptr_t);
    
        template <typename Y>
        bool operator==(SharedPointer<Y> const& other) const noexcept;

        template <typename Y>
        bool operator<(SharedPointer<Y> const& other) const noexcept;

        operator T*();
        
        NODISCARD WeakPointer<T> weak() const;
    
        T* operator->() const;

        NODISCARD T* raw() const;
    
        NODISCARD bool valid() const;

    private:

        void discard();

        T* pointer = nullptr;
        RefCounter* counter = nullptr;
        Allocator<RefCounter> refAllocator {};
        Allocator<T> allocator {};
    
    };

}

#define __SHARED_PTR_INCL
#include "SharedPointer.inl"
#undef __SHARED_PTR_INCL

