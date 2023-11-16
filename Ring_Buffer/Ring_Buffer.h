//
// Created by 79616 on 14.11.2023.
//

#ifndef IS_LAB7_RING_BUFFER_H
#define IS_LAB7_RING_BUFFER_H


#include <iostream>

template <typename T>
class RingBuffer {
private:
    T* buffer;
    size_t size;
    size_t capacity;
    size_t head; // Index of the first element
    size_t tail; // Index of the next available slot after the last element

public:
    class Iterator {
    private:
        T* pointer;
    public:
        Iterator():pointer(nullptr){};
        Iterator(T*t):pointer(t){};
        Iterator(const Iterator &t):pointer(t.pointer) {};

        T& operator*() const{return *pointer;}
        T* operator ->() const{return pointer;}
        T& operator [](T difference) {return pointer[difference];}

        Iterator operator +(const T difference) {return Iterator(pointer + difference);}
        Iterator operator -(const T difference) {return Iterator(pointer + difference);}
        Iterator operator +=(const T difference) {pointer += difference; return *this;}
        Iterator operator -=(const T difference) {pointer -= difference; return *this;}
        Iterator operator ++(){pointer++; return *this;}
        Iterator operator --(){pointer--; return  *this;}

        bool operator ==(Iterator const &t) {return pointer == t.pointer;}
        bool operator !=(Iterator const &t) {return pointer != t.pointer;}
        bool operator >(Iterator const &t) {return pointer > t.pointer;}
        bool operator <(Iterator const &t) {return pointer < t.pointer;}
        bool operator >=(Iterator const &t) {return pointer >= t.pointer;}
        bool operator <=(Iterator const &t) {return pointer <= t.pointer;}
    };
    RingBuffer(size_t capacity) : size(0), capacity(capacity), head(0), tail(0) {
        buffer = new T[capacity];
    }

    ~RingBuffer() {
        delete[] buffer;
    }

    void push_back(const T& value) {
        if (size >= capacity) {
            resize(size*2);
        }
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        size++;
    }

    const T& pop_back() {
        if (size > 0) {
            T element = back();
            tail = (tail - 1 + capacity) % capacity;
            size--;
            return element;
        } else {
            std::cout << "Buffer is empty. Cannot pop_back." << std::endl;
        }
    }

    void push_front(const T& value) {
        if (size >= capacity) {
            resize(size*2);
        }
        head = (head - 1 + capacity) % capacity;
        buffer[head] = value;
        size++;
    }

    const T& pop_front() {
        if (size > 0) {
            T element = front();
            head = (head + 1) % capacity;
            size--;
            return element;
        } else {
            std::cout << "Buffer is empty. Cannot pop_front." << std::endl;
        }
    }

    void insert(size_t index, const T& value) {
        size++;
        if (size >= capacity) {
            resize(size*2);
        }
        T* tempArr = new T[capacity];
        for (size_t i = 0; i < index; i++) {
            tempArr[i] = buffer[(head + i) % capacity];
        }
        tempArr[index] = value;
        for(size_t i = index; i < size; i++) {
            tempArr[i + 1] = buffer[(head + i) % capacity];
        }
        buffer = tempArr;
    }
    void delete_element(size_t index) {
        if (size > 0) {
            size--;
            T* tempArr = new T[capacity];
            for (size_t i = 0; i < index; i++) {
                tempArr[i] = buffer[(head + i) % capacity];
            }
            for(size_t i = index; i < size; i++) {
                tempArr[i] = buffer[(head + i + 1) % capacity];
            }
            buffer = tempArr;
        } else {
            std::cout << "Buffer is empty. Cannot delete_element." << std::endl;
        }

    }

    const T& back() const {
        if (size > 0) {
            return buffer[(tail - 1 + capacity) % capacity];
        } else {
            std::cout << "Buffer is empty. No elements at the back." << std::endl;
            return buffer[0]; // Вернуть что-то, например, заглушку
        }
    }


    const T& front() const {
        if (size > 0) {
            return buffer[head];
        } else {
            std::cout << "Buffer is empty. No elements at the front." << std::endl;
            return buffer[0]; // Вернуть что-то, например, заглушку
        }
    }

    const T& operator[](size_t index) const {
        if (index < size) {
            return buffer[(head + index) % capacity];
        } else {
            std::cout << "Index out of range." << std::endl;
            return buffer[0]; // Вернуть что-то, например, заглушку
        }
    }

    void resize(size_t newCapacity) {
        if (newCapacity < size) {
            // Новая capacity меньше текущего размера, уменьшим размер буфера
            size = newCapacity;
        }

        T* newBuffer = new T[newCapacity];
        for (size_t i = 0; i < size; ++i) {
            newBuffer[i] = buffer[(head + i) % capacity];
        }

        delete[] buffer;
        buffer = newBuffer;
        head = 0;
        tail = size;
        capacity = newCapacity;
    }
    size_t get_size() {
        return size;
    }
};



#endif//IS_LAB7_RING_BUFFER_H
