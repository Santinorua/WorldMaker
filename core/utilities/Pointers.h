#pragma once

#include <iostream>
#include <memory>

template <typename T>
T* GetShared(std::weak_ptr<T> ptr) {
    if (ptr.expired())
    {
        std::cout << "Weak pointer of type " << typeid(T).name() << " is null\n";
        return nullptr;
    }
    return ptr.lock().get();
}

template <typename T>
T* GetShared(T* ptr) {
    if (ptr == nullptr)
    {
        std::cout << "Weak pointer of type " << typeid(T).name() << " is null\n";
        return nullptr;
    }
    return ptr;
}
