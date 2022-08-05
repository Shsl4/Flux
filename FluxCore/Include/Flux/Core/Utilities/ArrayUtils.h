#pragma once

#include <Flux/Core/Memory/Array.h>

namespace Flux {
    
    class ArrayUtils {

    public:

        template<typename T>
        static void replaceAll(Array<T>& arr, T const& replace, T const &with) {

            for (auto& e: arr) {
            
                if (e == replace) {
                
                    e = with;
                
                }
            }
        
        }
    
        template<typename T>
        static bool contains(Array<T> const& arr, T const &elem) {

            for (auto const& e: arr) {
                if (e == elem) {
                    return true;
                }
            }

            return false;

        }

        template<typename T>
        static size_t firstIndexOf(Array<T> const& arr, T const &elem) {

            size_t counter = 0;

            for (const auto &e: arr) {
                if (e == elem) {
                    return counter;
                }
                ++counter;
            }

            throw std::logic_error("The array does not contain the input element.");

        }

        template<typename T>
        static size_t lastIndexOf(Array<T> const& arr, T const &elem) {

            for (Int64 i = arr.getSize() - 1; i >= 0; --i) {
                if (arr[i] == elem) {
                    return i;
                }
            }

            throw std::logic_error("The array does not contain the input element.");

        }

        template<typename T>
        static bool removeFirstOf(Array<T>& arr, T const &elem) {

            try {
                
                size_t index = ArrayUtils::firstIndexOf(arr, elem);
                arr.removeAt(index);
                return true;
                
            }
            catch(std::exception const&) {

                return false;
                
            }
            
        }

        template<typename T>
        static bool removeLastOf(Array<T> const& arr, T const &elem) {
        
            try {
                
                size_t index = ArrayUtils::lastIndexOf(arr, elem);
                arr.removeAt(index);
                return true;
                
            }
            catch(std::exception const&) {

                return false;
                
            }

        }

        template<typename T>
        static void sort(Array<T>& storage, std::function<bool(T, T)> predicate) {

            quickSort(storage, predicate, 0, static_cast<Int>(storage.getSize() - 1));
            
        }
                
        template<typename T, typename Y, typename ... Args>
        static void collect(OwnedArray<T>& storage, SharedPointer<Y>& elem) {
            storage += elem;
        }
        
        template<typename T, typename Y, typename ... Args>
        static void collect(OwnedArray<T>& storage, SharedPointer<Y>& elem, Args&& ... args) {

            storage += elem;
            collect(storage, args...);

        }


    private:

        template<typename T>
        static void quickSort(Array<T>& storage, std::function<bool(T, T)> const& predicate, Int from, Int to) {

            if (from < to) {
                
                const Int idx = partition(storage, predicate, from, to);

                quickSort(storage, predicate, from, idx - 1);
                quickSort(storage, predicate, idx + 1, to);

            }

        }

        template<typename T>
        static Int partition(Array<T>& storage, std::function<bool(T, T)> const& predicate, const Int from, const Int to) {

            Int idx = from - 1;
            
            T* data = storage.begin();
            
            for (Int i = from; i <= to - 1; ++i) {

                if (predicate(data[i], data[to])) {

                    ++idx;
                    Memory::swapMove(data + idx, data + i);

                }

            }

            Memory::swapMove(data + idx + 1, data + to);

            return idx + 1;

        }
        
    
    };
    
}
