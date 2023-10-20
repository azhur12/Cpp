#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <iostream>
#include <stack>

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Wrong number of arguments"
				  << "\n";
		return ERROR_PARAMETER_INVALID;
	}

	std::ifstream inputFile(argv[1]);

	if (!inputFile)
	{
		std::cerr << "Failed to open input file." << std::endl;
		return ERROR_CANNOT_OPEN_FILE;
	}

	std::stack< LN > stack;
	std::string line;
	while (std::getline(inputFile, line))
	{
		if (line.empty())
		{
			continue;
		}

		char operation = line[0];
		LN result;

		switch (operation)
		{
		case '+':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			result = operand1 + operand2;
			break;
		}
		case '-':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			result = operand2 - operand1;
			break;
		}
		case '*':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			result = operand1 * operand2;
			break;
		}
		case '/':
		{
			std::cerr << "Unsupported operator"
					  << "\n";
			return ERROR_UNSUPPORTED;
		}
		case '%':
		{
			std::cerr << "Unsupported operator"
					  << "\n";
			return ERROR_UNSUPPORTED;
		}
		case '~':
		{
			std::cerr << "Unsupported operator"
					  << "\n";
			return ERROR_UNSUPPORTED;
		}
		case '_':
		{
			LN operand = stack.top();
			stack.pop();
			result = -operand;
			break;
		}
		case '<':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			if (line[1] == '=')
			{
				result = (operand2 <= operand1) ? LN(1) : LN();
			}
			else
			{
				result = (operand2 < operand1) ? LN(1) : LN();
			}
			break;
		}
		case '>':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			if (line[1] == '=')
			{
				result = (operand2 >= operand1 ? LN(1) : LN());
			}
			else
			{
				result = (operand2 > operand1 ? LN(1) : LN());
			}
			break;
		}
		case '=':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			result = (operand2 == operand1) ? LN(1) : LN();
			break;
		}
		case '!':
		{
			LN operand1 = stack.top();
			stack.pop();
			LN operand2 = stack.top();
			stack.pop();
			result = (operand2 != operand1) ? LN(1) : LN();
			break;
		}
		default:
		{
			LN operand = LN(line.c_str());
			result = operand;
			break;
		}
		}
		stack.push(result);
	}

	inputFile.close();

	std::ofstream outputFile(argv[2]);

	if (!outputFile)
	{
		std::cerr << "Failed to open output file." << std::endl;
		return ERROR_CANNOT_OPEN_FILE;
	}

	while (!stack.empty())
	{
		LN value = stack.top();
		stack.pop();
		const char* buffChar = value.toString();
		for (int i = 0; i <= value.getSizeVector(); i++)
		{
			outputFile << buffChar[i];
		}
		outputFile << std::endl;
	}

	outputFile.close();

	return 0;
}
