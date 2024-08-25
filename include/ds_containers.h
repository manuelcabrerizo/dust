//
// Created by manue on 8/23/2024.
//

#ifndef DS_CONTAINERS_H
#define DS_CONTAINERS_H
#include <cstdlib>
#include <iostream>

namespace ds {

    /////////////////////////////////////////////////
    /// Static Array Declaration and Implementation
    /////////////////////////////////////////////////

    template <typename T, int SIZE_T>
    class Array {
    public:
        int Size();
        int ByteSize();
        void Zero();
        void Memset(const char fill);

        const T& operator[](int index) const;
        T& operator[](int index);

        const T* Ptr() const;
        T* Ptr();

    private:
        T ptr[SIZE_T];
    };

    template<typename T, int SIZE_T>
    int Array<T, SIZE_T>::Size() {
        return SIZE_T;
    }

    template<typename T, int SIZE_T>
    int Array<T, SIZE_T>::ByteSize() {
        return sizeof(ptr);
    }

    template<typename T, int SIZE_T>
    void Array<T, SIZE_T>::Zero() {
        memset(ptr, 0, sizeof(ptr));
    }

    template<typename T, int SIZE_T>
    void Array<T, SIZE_T>::Memset(const char fill) {
        memset(ptr, fill, sizeof(ptr)); // SIZE_T * sizeof(*ptr)
    }

    template<typename T, int SIZE_T>
    T &Array<T, SIZE_T>::operator[](int index) {
        return ptr[index];
    }

    template<typename T, int SIZE_T>
    const T &Array<T, SIZE_T>::operator[](int index) const {
        return ptr[index];
    }

    template<typename T, int SIZE_T>
    T *Array<T, SIZE_T>::Ptr() {
        return ptr;
    }

    template<typename T, int SIZE_T>
    const T *Array<T, SIZE_T>::Ptr() const {
        return ptr;
    }

    /////////////////////////////////////////////////
    /// Temp Array Declaration and Implementation
    /////////////////////////////////////////////////
    template<typename T>
    class TempArray {
    public:
        TempArray() = default;
        TempArray(TempArray<T>& other);
        TempArray(unsigned int num);
        ~TempArray();

        void Initialize(unsigned int num);

        int Size();
        int ByteSize();
        void Zero();
        void Memset(const char fill);

        const T& operator[](int index) const;
        T& operator[](int index);

        const T* Ptr() const;
        T* Ptr();

    private:
        T* buffer;
        unsigned int num;
    };

    template<typename T>
    TempArray<T>::TempArray(TempArray<T> &other) {
        num = other.num;
        buffer = other.buffer;
        other.num = 0;
        other.buffer = nullptr;
    }

    template<typename T>
    TempArray<T>::TempArray(unsigned int num) {
        Initialize(num);
    }

    template<typename T>
    TempArray<T>::~TempArray() {
        free(buffer);
    }

    template<typename T>
    void TempArray<T>::Initialize(unsigned int num_) {
        num = num_;
        buffer = (T *)malloc(sizeof(T) * num_); // use malloc or new (malloc and placement new)????
    }

    template<typename T>
    int TempArray<T>::Size() {
        return num;
    }

    template<typename T>
    int TempArray<T>::ByteSize() {
        return sizeof(T) * num;
    }

    template<typename T>
    void TempArray<T>::Zero() {
        memset(buffer, 0, ByteSize());
    }

    template<typename T>
    void TempArray<T>::Memset(const char fill) {
        memset(buffer, fill, ByteSize()); // SIZE_T * sizeof(*ptr)
    }

    template<typename T>
    T &TempArray<T>::operator[](int index) {
        return buffer[index];
    }

    template<typename T>
    const T &TempArray<T>::operator[](int index) const {
        return buffer[index];
    }

    template<typename T>
    T *TempArray<T>::Ptr() {
        return buffer;
    }

    template<typename T>
    const T *TempArray<T>::Ptr() const {
        return buffer;
    }
}

#endif //DS_CONTAINERS_H
