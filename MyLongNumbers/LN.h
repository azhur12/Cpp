//
// Created by 79616 on 19.05.2023.
//

#ifndef LONGNUMBERLAB_LN_H
#define LONGNUMBERLAB_LN_H

#include "MyVector.h"
class LN
{
  private:
	int size;
	bool isNaN;
	bool isNegative;
	MyVector limbs;

  public:
	LN();
	LN(const LN& other);
	LN(LN&& other) noexcept;
	LN(long long num);
	explicit LN(const char* numberStr);
	void multiplyLN(uint64_t value);
	void addLN(uint64_t value);

	LN& operator+=(const LN& other);
	uint32_t& operator[](uint32_t index);
	friend LN operator+(const LN& lhs, const LN& rhs);

	LN& operator*=(const LN& other);
	friend LN operator*(const LN& lhs, const LN& rhs);

	LN& operator=(const LN& other);
	LN& operator=(LN&& other) noexcept;

	LN operator-() const;
	LN& operator-=(const LN& other);
	friend LN operator-(const LN& lhs, const LN& rhs);

	friend bool operator==(const LN& lhs, const LN& rhs);	 // Перегрузка оператора ==
	friend bool operator!=(const LN& lhs, const LN& rhs);	 // Перегрузка оператора !=
	friend bool operator<(const LN& lhs, const LN& rhs);	 // Перегрузка оператора <
	friend bool operator<=(const LN& lhs, const LN& rhs);	 // Перегрузка оператора <=
	friend bool operator>(const LN& lhs, const LN& rhs);	 // Перегрузка оператора >
	friend bool operator>=(const LN& lhs, const LN& rhs);	 // Перегрузка оператора >=
	operator long long() const;
	operator bool() const;

	bool getIsNegative();
	int getSizeVector() const;
	int getCapacityVector();

	const char* toString() const;

	void borrowNext(uint32_t index);

	void shiftLeft(uint32_t index)
	{
		for (int i = 0; i < index; i++)
		{
			limbs.push_back(0);
		}
	}

	LN& absSubtract(LN& bigOne, LN& smallOne);
};

LN operator+(const LN& lhs, const LN& rhs);
LN operator-(const LN& lhs, const LN& rhs);
LN operator*(const LN& lhs, const LN& rhs);

#endif	  // LONGNUMBERLAB_LN_H