//
// Created by 79616 on 19.05.2023.
//

#include "MyVector.h"

#include <cstring>
#include <iostream>

MyVector::MyVector(uint32_t userCapacity)
{
	size = 0;
	capacity = userCapacity;
	try
	{
		memory = new uint32_t[capacity];
	} catch (std::bad_alloc& e)
	{
		std::cerr << "Failed to allocate memory: " << e.what() << std::endl;
		memory = nullptr;
		capacity = 0;
		throw;
	}
}

MyVector::~MyVector()
{
	delete[] memory;
}
uint32_t& MyVector::operator[](uint32_t index)
{
	return memory[index];
}

const uint32_t& MyVector::operator[](uint32_t index) const
{
	return memory[index];
}

MyVector& MyVector::operator=(const MyVector& other)
{
	if (this != &other)
	{
		delete[] memory;
		try
		{
			capacity = other.capacity;
			size = other.size;
			memory = new uint32_t[capacity];
			std::memcpy(memory, other.memory, size * sizeof(uint32_t));
		} catch (std::bad_alloc& e)
		{
			std::cerr << "Failed to allocate memory: " << e.what() << std::endl;
			memory = nullptr;
			capacity = 0;
			size = 0;
			throw;
		}
	}
	return *this;
}

void MyVector::push_back(uint32_t value)
{
	if (size == capacity)
	{
		if (capacity == 0)
		{
			capacity = 1;
		}
		else
		{
			capacity *= 2;
		}
		uint32_t* newMemory = nullptr;

		try
		{
			newMemory = new uint32_t[capacity];
		} catch (std::bad_alloc& e)
		{
			std::cerr << "Failed to allocate memory: " << e.what() << std::endl;
			throw;
		}

		std::memcpy(newMemory, memory, size * sizeof(uint32_t));

		delete[] memory;
		memory = newMemory;
	}

	memory[size] = value;
	size++;
}

void MyVector::resize(uint32_t newSize)
{
	if (newSize > capacity)
	{
		capacity = newSize;

		uint32_t* newMemory = nullptr;

		try
		{
			newMemory = new uint32_t[capacity];
		} catch (std::bad_alloc& e)
		{
			std::cerr << "Failed to allocate memory: " << e.what() << std::endl;
			throw;	  // Переброс исключения для дальнейшей обработки
		}

		// Копирование элементов в новый массив
		std::memcpy(newMemory, memory, size * sizeof(uint32_t));

		delete[] memory;
		memory = newMemory;
	}

	size = newSize;
}

int MyVector::getSize() const
{
	return size;
}

uint32_t MyVector::getCapacity() const
{
	return capacity;
}
