#pragma once

#include <cstring>
#include <utility>

#include <Flux/Core/Memory/Memory.h>

namespace Flux {
 
    /*!
     *  \brief An object allowing to manage memory for objects of type T.
     *
     *  \tparam T Type of object to manage
     */
    template <typename T>
    class Allocator final {

    public:
     
        /*!
         * The default constructor.
         */
        Allocator() = delete;

        /*!
         *  Constructs an object of type T.
         *
         *  \tparam Args A variadic template.
         *  \param[in] args The arguments to pass to the constructor.
         *  \return The newly constructed object.
         */
        template <typename ... Args>
        static T* construct(Args&&... args);

        /*!
         *  Constructs an array of objects of type T.
         *
         *  \param size The number of object to create
         *  \return The newly constructed array.
         */
        static T* construct(size_t size);

        /*!
         *  Allocates a buffer containing \p size number of objects of type T.
         *  If T is an object class, objects are not initialized.
         *
         *  \warning If T is a class, it must have a public empty constructor.
         *
         *  \param[in] size The size of the buffer.
         *  \return The newly allocated buffer.
         */
        static T* alloc(size_t size);

        /*!
         *  Destroys a single object allocated with construct.
         *
         *  \param[in,out] pointer The object to release
         */
        static void destroy(T*& pointer);
     
        /*!
         *  Releases a buffer allocated with allocate.
         *
         *  \param[in,out] pointer The buffer to release.
         */
        static void release(T*& pointer);

        /*!
         *  Extends or shrinks a memory buffer.
         *
         *  \param[in,out] pointer The buffer to reallocate.
         *  \param[in] oldSize The old size of the buffer.
         *  \param[in] size The new size of the buffer.
         */
        static void realloc(T*& pointer, size_t oldSize, size_t size);

        /*!
         *  Moves memory forwards or backwards automatically.
         *
         *  \param[in] from Where to start reading data
         *  \param[in] to Where to stop reading data
         *  \param[out] dest Where move the data.
         */
        static void move(T* from, T* to, T* dest);

        /*!
         *  Copies memory forwards or backwards automatically.
         *
         *  \param[in] from Where to start reading data
         *  \param[in] to Where to stop reading data
         *  \param[out] dest Where copy the data.
         */
        static void copy(const T* from, const T* to, T* dest);

        /*!
         * Swaps the contents of the two pointers by moving (no copying).
         */
        static void swapMove(T* a, T* b);

    };
 
}

#define __ALLOC_INCL

#include "Allocator.inl"

#undef __ALLOC_INCL
