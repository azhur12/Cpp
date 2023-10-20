//
// Created by 79616 on 19.05.2023.
//

#include "LN.h"

#include <cmath>
#include <iostream>

LN::LN()
{
	isNaN = false;
	isNegative = false;
	limbs.push_back(0);
	size++;
}

LN::LN(const LN& other)
{
	isNaN = other.isNaN;
	size = other.size;
	isNegative = other.isNegative;
	limbs = other.limbs;
}

LN::LN(LN&& other) noexcept
{
	isNaN = other.isNaN;
	size = other.size;
	isNegative = other.isNegative;
	limbs = std::move(other.limbs);
	other.size = 0;
	other.isNegative = false;
	other.isNaN = true;
}

LN::LN(long long num) : limbs()
{
	isNaN = false;
	if (num >= 0)
	{
		isNegative = false;
		if (num == 0)
		{
			limbs.push_back(num);
		}
		else
		{
			while (num > 0)
			{
				limbs.push_back(num % 4294967296);
				num /= 4294967296;
				size++;
			}
		}
	}
	else
	{
		isNegative = true;
		uint64_t tempNum;
		if (num == -9223372036854775807)
		{
			tempNum = (uint64_t)9223372036854775807 + 1;
		}
		else
		{
			tempNum = std::abs(num);
		}
		while (tempNum > 0)
		{
			limbs.push_back(tempNum % 4294967296);
			tempNum /= 4294967296;
			size++;
		}
	}
}

LN::LN(const char* numberStr) : limbs()
{
	isNaN = false;
	isNegative = false;
	limbs.push_back(0);
	int index = 0;
	if (numberStr[index] == '-')
	{
		isNegative = true;
		index++;
	}
	while (numberStr[index] == '0')
	{
		index++;
	}
	uint64_t temp = 0;

	while (numberStr[index] != '\0')
	{
		multiplyLN(16);
		if ('0' <= numberStr[index] && numberStr[index] <= '9')
		{
			addLN(numberStr[index] - '0');
		}
		else if ('A' <= numberStr[index] && numberStr[index] <= 'F')
		{
			addLN(numberStr[index] - 'A' + 10);
		}
		else
		{
			isNaN = true;
			break;
		}
		index++;
	}
}

void LN::multiplyLN(uint64_t value)
{
	uint64_t tempValue = 0;
	uint64_t carry = 0;
	for (int i = 0; i < getSizeVector(); i++)
	{
		tempValue = limbs[i] * value + carry;
		limbs[i] = tempValue;
		carry = tempValue >> 32;
	}

	if (carry)
	{
		limbs.push_back(carry);
	}
}

void LN::addLN(uint64_t value)
{
	uint64_t tempValue = 0;
	uint64_t carry = value;
	for (int i = 0; i < getSizeVector(); i++)
	{
		tempValue = limbs[i] + carry;
		limbs[i] = tempValue;
		carry = tempValue >> 32;
	}

	if (carry)
	{
		limbs.push_back(carry);
	}
}

LN& LN::operator=(const LN& other)
{
	if (this != &other)
	{
		size = other.size;
		isNegative = other.isNegative;
		limbs = other.limbs;
	}
	return *this;
}

LN& LN::operator=(LN&& other) noexcept
{
	if (this != &other)
	{
		size = other.size;
		isNegative = other.isNegative;
		limbs = std::move(other.limbs);
		other.size = 0;
		other.isNegative = false;
	}
	return *this;
}

void LN::borrowNext(uint32_t index)
{
	uint32_t tempIndex = index;
	while (limbs[tempIndex] == 0)
	{
		limbs[tempIndex] = 4294967296 - 1;
		tempIndex++;
	}
	limbs[tempIndex] -= 1;
}

LN& LN::absSubtract(LN& bigOne, LN& smallOne)
{
	for (int i = 0; i < smallOne.limbs.getSize(); i++)
	{
		uint64_t tempRes = 0;
		if (bigOne[i] < smallOne[i])
		{
			bigOne.borrowNext(i + 1);
			tempRes += 4294967296;	  // 2^32;
		}
		tempRes += bigOne[i] - smallOne[i];
		bigOne.limbs[i] = tempRes;
	}
	*this = bigOne;
	return *this;
}

LN& LN::operator+=(const LN& other)
{
	if (isNegative == other.isNegative)
	{
		uint32_t carry = 0;
		uint32_t maxSize = std::max(limbs.getSize(), other.limbs.getSize());
		for (uint32_t i = 0; i < maxSize; ++i)
		{
			uint64_t sum = carry;
			if (i < limbs.getSize())
			{
				sum += limbs[i];
			}
			if (i < other.limbs.getSize())
			{
				sum += other.limbs[i];
			}

			limbs[i] = sum & 0xFFFFFFFF;	// Оставляем только первые 32 бита
			carry = sum >> 32;				// Получаем остальные биты в кэри
		}

		if (carry > 0)
			limbs.push_back(carry);	   // Добавляем кэри в новый разряд, если есть

		return *this;
	}
	else
	{
		LN positive = LN(1);
		LN negative = LN(1);
		if (isNegative)
		{
			negative = *this;
			positive = other;
		}
		else
		{
			positive = *this;
			negative = other;
		}

		if (positive.limbs.getSize() > negative.limbs.getSize())
		{
			return absSubtract(positive, negative);
		}
		else if (positive.limbs.getSize() < negative.limbs.getSize())
		{
			return absSubtract(negative, positive);
		}
		else
		{
			bool isPosBigger = false;
			for (int i = positive.limbs.getSize() - 1; i >= 0; i--)
			{
				if (positive[i] == negative[i])
				{
					continue;
				}
				else
				{
					isPosBigger = positive[i] > negative[i];
					break;
				}
			}
			if (isPosBigger)
			{
				return absSubtract(positive, negative);
			}
			else
			{
				return absSubtract(negative, positive);
			}
		}
	}
}

LN operator+(const LN& lhs, const LN& rhs)
{
	LN result = lhs;
	result += rhs;
	return result;
}

LN& LN::operator*=(const LN& other)
{
	LN result;
	result.limbs.resize(limbs.getSize() + other.limbs.getSize());

	for (int i = 0; i < limbs.getSize(); i++)
	{
		uint64_t carry = 0;
		for (int j = 0; j < other.limbs.getSize() || carry != 0; j++)
		{
			uint64_t mul = limbs[i] * (j < other.limbs.getSize() ? other.limbs[j] : 0) + carry;
			uint64_t sum = result.limbs[i + j] + mul;

			result.limbs[i + j] = sum & 0xFFFFFFFF;
			carry = (sum >> 32) + (mul >> 32);
		}
	}

	result.isNegative = isNegative != other.isNegative;

	*this = result;

	return *this;
}

LN operator*(const LN& lhs, const LN& rhs)
{
	LN result = lhs;
	result *= rhs;
	return result;
}

uint32_t& LN::operator[](uint32_t index)
{
	return limbs[index];
}

LN LN::operator-() const
{
	LN result = *this;
	result.isNegative = !isNegative;
	return result;
}

LN& LN::operator-=(const LN& other)
{
	LN newNegative = -other;
	*this += newNegative;
	return *this;
}

LN operator-(const LN& lhs, const LN& rhs)
{
	LN result = lhs;
	result -= rhs;
	return result;
}

bool operator==(const LN& lhs, const LN& rhs)
{
	if (lhs.isNegative != rhs.isNegative || lhs.size != rhs.size)
	{
		return false;
	}

	for (int i = lhs.size - 1; i >= 0; --i)
	{
		if (lhs.limbs[i] != rhs.limbs[i])
		{
			return false;
		}
	}

	return true;
}

bool operator!=(const LN& lhs, const LN& rhs)
{
	return !(lhs == rhs);
}

bool operator<(const LN& lhs, const LN& rhs)
{
	if (lhs.isNegative && !rhs.isNegative)
	{
		return true;
	}
	if (!lhs.isNegative && rhs.isNegative)
	{
		return false;
	}

	if (lhs.size < rhs.size)
	{
		return !lhs.isNegative;
	}
	if (lhs.size > rhs.size)
	{
		return lhs.isNegative;
	}

	for (int i = lhs.size - 1; i >= 0; --i)
	{
		if (lhs.limbs[i] < rhs.limbs[i])
		{
			return !lhs.isNegative;
		}
		if (lhs.limbs[i] > rhs.limbs[i])
		{
			return lhs.isNegative;
		}
	}

	return false;
}

bool operator<=(const LN& lhs, const LN& rhs)
{
	return (lhs < rhs) || (lhs == rhs);
}

bool operator>(const LN& lhs, const LN& rhs)
{
	return !(lhs <= rhs);
}

bool operator>=(const LN& lhs, const LN& rhs)
{
	return !(lhs < rhs);
}

LN::operator long long() const
{
	if (limbs.getSize() == 0)
		return 0;

	if (limbs.getSize() > 2)
	{
		throw std::overflow_error("LN value does not fit into long long");
	}

	long long result = 0;
	uint64_t lowLimb = limbs[0];
	uint64_t highLimb = (limbs.getSize() == 2) ? limbs[1] : 0;

	if (limbs[1] >= std::pow(2, 31))
	{
		throw std::overflow_error("LN value does not fit into long long");
	}

	result = (static_cast< long long >(highLimb) << 32) | lowLimb;

	if (isNegative)
		result = -result;
	return result;
}

LN::operator bool() const
{
	return !(*this == LN());
}

bool LN::getIsNegative()
{
	return isNegative;
}

int LN::getSizeVector() const
{
	return limbs.getSize();
}

int LN::getCapacityVector()
{
	return limbs.getCapacity();
}

const char* LN::toString() const
{
	if (limbs[0] == 0 && getSizeVector() == 1)
	{
		return "0";
	}

	const int base = 1 << 32;
	const int digitsPerBlock = 8;

	int bufferSize = getSizeVector() * digitsPerBlock + 1;
	char* buffer = new char[bufferSize];
	char* bufferPtr = buffer;

	for (int i = getSizeVector() - 1; i >= 0; i--)
	{
		for (int j = digitsPerBlock - 1; j >= 0; j--)
		{
			int digitValue = (limbs[i] >> (j * 4)) & 0xF;
			*bufferPtr = digitValue < 10 ? '0' + digitValue : 'A' + digitValue - 10;
			bufferPtr++;
		}
	}

	while (*buffer == '0' && bufferPtr > buffer + 1)
	{
		buffer++;
	}

	*bufferPtr = '\0';

	return buffer;
}