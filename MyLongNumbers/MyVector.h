//
// Created by 79616 on 19.05.2023.
//

#ifndef LONGNUMBERLAB_MYVECTOR_H
#define LONGNUMBERLAB_MYVECTOR_H

#include <cstdint>
class MyVector
{
  private:
	uint32_t capacity;
	int size;
	uint32_t* memory;

  public:
	MyVector(uint32_t capacity);
	MyVector() : capacity(0), size(0), memory(nullptr) {}
	~MyVector();

	uint32_t& operator[](uint32_t index);
	const uint32_t& operator[](uint32_t index) const;
	MyVector& operator=(const MyVector& other);

	void resize(uint32_t newSize);

	void push_back(uint32_t value);
	int getSize() const;
	uint32_t getCapacity() const;
};

#endif	  // LONGNUMBERLAB_MYVECTOR_H
