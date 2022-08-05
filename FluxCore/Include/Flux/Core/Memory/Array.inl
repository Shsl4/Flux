#pragma once

#ifndef __ARRAY_INCL
#error This file sould only be included from Array.h
#endif

namespace Flux {
    
    template<typename T>
    Array<T>::Array(const size_t capacity) : size(0), capacity(capacity) {

        data = allocator.alloc(capacity);

    }

    template<typename T>
    Array<T>::Array(Array<T> const &other) {

        *this = other;

    }

    template<typename T>
    Array<T>::Array(Array<T> &&other) noexcept
    {

        *this = std::move(other);

    }

    template<typename T>
    Array<T>::Array(Array<T> const &other, size_t limit){
    
        size_t arrSize = other.size;
    
        // If the limit is above the array size,
        if(limit > arrSize){
        
            // Set the new size to be the array size.
            limit = arrSize;
        
        }
    
        // Set the size and capacity
        this->size = limit;
        this->capacity = limit;

        // Allocate our data.
        this->data = allocator.alloc(capacity);

        // Copy the data.
        allocator.copy(other.data, other.data + limit, this->data);
    
    }

    template<typename T>
    Array<T>::~Array() {

        allocator.release(data);
        size = 0;
        capacity = 0;

    }

    template<typename T>
    Array<T> &Array<T>::operator=(Array<T> const &other) {
    
        if (&other == this) { return *this; }
    
        // Deallocate the data in this array
        allocator.release(data);

        // If the other array is empty, set this array to empty.
        if (!other.data) {

            this->data = nullptr;
            this->size = 0;
            this->capacity = 0;

        } else {

            // Otherwise, allocate a new data buffer.
            this->data = allocator.alloc(other.capacity);

            // Copy the array contents.
            allocator.copy(other.data, other.data + other.size, this->data);

            // Store the new information.
            this->capacity = other.capacity;
            this->size = other.size;

        }

        return *this;

    }

    template<typename T>
    Array<T> &Array<T>::operator=(Array<T> &&other) noexcept
    {

        if (&other == this) { return *this; }
    
        allocator.release(data);

        this->data = other.data;
        this->capacity = other.capacity;
        this->size = other.size;

        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;

        return *this;

    }

    template<typename T>
    T& Array<T>::append(T const &elem) {

        // Extend the array if necessary.
        extend();

        // Store the new element.
        data[size] = std::move(elem);

        // Increase the number of held objects.
        ++size;

        return data[size];

    }


    template<typename T>
    void Array<T>::append(Array<T> const &arr) {
                
        if (arr.isEmpty()) {
            return;
        }

        if(isEmpty()) {
            *this = arr;
            return;
        }

        // Extend the array if necessary.
        extend(arr.getSize());

        // Store the new elements.
        allocator.copy(arr.data, arr.data + arr.size, data + getSize());

        // Increase the number of held objects.
        size += arr.getSize();

    }

    template<typename T>
    void Array<T>::removeAt(size_t index) {
    
        // If the index is out of bounds, return.
        if (index >= size) {
            return;
        }
    
        // Move back all the elements after the index.
        allocator.move(data + index + 1, data + size, data + index);

        // Reduce the size of the array.
        --size;

    }
    
    template<typename T>
    void OwnedArray<T>::removeAt(size_t index) {
        
        // If the index is out of bounds, return.
        if (index >= this->size) {
            return;
        }
    
        // Move back all the elements after the index.
        this->allocator.move(this->data + index + 1, this->data + this->size, this->data + index);
        
        // Reduce the size of the array.
        --this->size;

        this->data[this->size] = nullptr;
        
    }
    
    template<typename T>
    void WeakArray<T>::removeAt(size_t index) {
        
        // If the index is out of bounds, return.
        if (index >= this->size) {
            return;
        }
    
        // Move back all the elements after the index.
        this->allocator.move(this->data + index + 1, this->data + this->size, this->data + index);
        
        // Reduce the size of the array.
        --this->size;

        this->data[this->size] = nullptr;
        
    }

    template<typename T>
    void Array<T>::reverse() {

        for (size_t i = 0; i < getSize() / 2; ++i) {
            allocator.swapMove(data + i, data + getSize() - 1 - i);
        }

    }

    template <typename T>
    void Array<T>::reset() {

        size = 0;
        
    }

    template<typename T>
    Array<T> Array<T>::arrayWithRange(size_t from, size_t to){
    
        // Create a new array
        Array<T> array = Array<T>();

        // If the input parameter is invalid, return an empty array.
        if(from >= size || from >= to) { return array; }
    
        // If to is greater than the array size, set it to the array size.
        if(to > size) { to = size; }
    
        // Allocate data for the new array.
        array.data = allocator.alloc(to - from);
    
        // Copy the data to the new array.
        allocator.copy(data + from, data + to, array.data);
    
        // Set the size and capacity.
        array.size = to - from;
        array.capacity = to - from;
    
        // Return the array.
        return array;
    
    }

    template<typename T>
    Array<T> Array<T>::arrayMatching(std::function<bool(T const &)> condition) const{

        // Create a new array
        Array<T> array = Array<T>();

        // For each element
        for (auto &e: *this) {

            // If the condition is met
            if (condition(e)) {

                // Append the element to the new array
                array += e;

            }

        }

        // Return the array.
        return array;

    }

    template<typename T>
    void Array<T>::insert(T const &elem, size_t index) {

        // If the index to insert is greater to the array size,
        if (index > size) {

            // Simply append the element to the array.
            append(elem);
            return;

        }

        // Extend the array if necessary.
        extend();

        // Move all the elements forward.
        allocator.move(data + index, data + size, data + index + 1);

        // Store the new element.
        data[index] = std::move(elem);

        // Increase the size of the array
        ++size;

    }


    template<typename T>
    void Array<T>::insert(Array<T> const &arr, size_t index) {

        // If the index to insert is greater to the array size,
        if (index > size) {

            // Simply append the array to the array.
            append(arr);
            return;

        }

        // Extend the array if necessary.
        extend(arr.getSize());

        // Move all the elements forward.
        allocator.move(data + index, data + size, data + index + arr.getSize());

        // Store the new elements.
        allocator.copy(arr.data, arr.data + arr.getSize(), data + index);

        // Increase the size of the array
        size += arr.getSize();

    }

    template<typename T>
    void Array<T>::resize(size_t newCap) {

        // Resize the data buffer.
        allocator.realloc(data, capacity, newCap);

        // Set the new capacity
        this->capacity = newCap;

        // Set the size if necessary
        if (size > newCap) {size = newCap;}

    }

    template<typename T>
    void Array<T>::shrink() {

        // If the array can't be shrunk, return.
        if (size == capacity) {return;}

        if (size == 0) {
            clear();
            return;
        }

        // Shrink the array to the number of elements contained.
        allocator.realloc(data, capacity, size);
        this->capacity = size;

    }

    template<typename T>
    void Array<T>::clear() {

        // Clear the array.
        allocator.release(this->data);

        // Reset the variables.
        this->size = 0;
        this->capacity = 0;

    }

    template<typename T>
    T& Array<T>::getAt(size_t index) const {
    
        fassertf(index < size, "Index out of bounds");
        return data[index];
    
    }

    template<typename T>
    T& Array<T>::operator[](size_t index) const {
        return getAt(index);
    }

    template<typename T>
    void Array<T>::operator+=(T const &elem) {
        append(elem);
    }

    template<typename T>
    Array<T> Array<T>::operator+(Array<T> const& arr) const {
        
        Array<T> newArray = *this;
        newArray += arr;
        return newArray;
        
    }
    
    template<typename T>
    void Array<T>::operator+=(Array<T> const &arr) {
        append(arr);
    }

    template<typename T>
    T *Array<T>::begin() const {
        return this->data;
    }

    template<typename T>
    T *Array<T>::end() const {
        return this->data + size;
    }

    template<typename T>
    void Array<T>::extend(const size_t ext) {

        // If the array is full
        if (ext && ((size + ext) > capacity)) {

            size_t newSize = (capacity + ext) * 2;

            // Resize our data buffer.
            allocator.realloc(data, capacity, newSize);

            this->capacity = newSize;

        }

    }
    
}
