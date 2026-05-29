#pragma once

#include <iostream>
#include <memory>

template <typename T>
std::shared_ptr<T> GetShared(std::weak_ptr<T> ptr) {
    if (ptr.expired())
    {
        std::cout << "Weak pointer of type " << typeid(T).name() << " is null\n";
        return nullptr;
    }
    return ptr.lock();
}

template <typename T>
T* GetShared(T* ptr) {
    if (ptr == nullptr)
    {
        std::cout << "Pointer of type " << typeid(T).name() << " is null\n";
        return nullptr;
    }
    return ptr;
}
