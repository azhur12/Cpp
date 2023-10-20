#include "return_codes.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

struct PNG
{
	unsigned char signature[8];
	struct Chunk *chunks;
};

struct Chunk
{
	unsigned int length;
	unsigned char type[4];
	unsigned char *data;
	unsigned int crc;
};

unsigned int byteWiseReverse(unsigned int val)
{
	unsigned int result = 0;
	for (int i = 0; i < 4; i++)
	{
		result <<= 8;
		result |= (val & 0xFF);
		val >>= 8;
	}
	return result;
}

int readSignaturePNG(FILE *file, unsigned char *signature)
{
	unsigned char correct_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	fread(signature, 1, 8, file);
	bool correct = true;
	for (int i = 0; i < 8; i++)
	{
		if (signature[i] != correct_signature[i])
		{
			correct = false;
		}
	}
	if (!correct)
	{
		return ERROR_UNSUPPORTED;
	}
	return SUCCESS;
}

int readChunck(FILE *file, struct Chunk *chunk)
{
	fread(&(chunk->length), sizeof(unsigned int), 1, file);
	chunk->length = byteWiseReverse(chunk->length);

	fread(&(chunk->type), sizeof(unsigned char), 4, file);

	chunk->data = malloc(chunk->length * sizeof(unsigned char));	// *(chunk->data) ???
	if (chunk->data == NULL)
	{
		fprintf(stderr, "Cannot allocate memory");
		return ERROR_OUT_OF_MEMORY;
	}
	fread(chunk->data, sizeof(unsigned char), chunk->length, file);
	fread(&chunk->crc, sizeof(unsigned int), 1, file);
	chunk->crc = byteWiseReverse(chunk->crc);
	return SUCCESS;
}

bool compareType(unsigned char *type, unsigned char *needType, int size)
{
	bool result = true;
	for (int i = 0; i < size; i++)
	{
		if (type[i] != needType[i])
		{
			result = false;
		}
	}
	return result;
}

int processingIHDR(struct Chunk *headChunk)
{
	if (headChunk->length != 13)
	{
		fprintf(stderr, "Wrong data in IHDR");
		return ERROR_DATA_INVALID;
	}
	unsigned char NameIHDR[4] = { 'I', 'H', 'D', 'R' };
	if (!compareType(headChunk->type, NameIHDR, 4))
	{
		fprintf(stderr, "Wrong data in IHDR");
		return ERROR_DATA_INVALID;
	}

	if (headChunk->data[8] != 8)
	{
		fprintf(stderr, "Support only 8 bit in bit depth");
		return ERROR_UNSUPPORTED;
	}
	return SUCCESS;
}

int writeDataOfChunkInBuffer(unsigned char *buffer, unsigned char *data, unsigned int cursor, unsigned int lengthOfData)
{
	for (int i = 0; i < lengthOfData; i++)
	{
		buffer[cursor] = data[i];
		cursor++;
	}
	return SUCCESS;
}

unsigned int makeInteger(unsigned char *data)
{
	unsigned int result = 0;
	result = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
	return result;
}

int filtration(unsigned char *raw, unsigned char *filtered, unsigned int height, unsigned int width, int type_color)
{
	for (int i = 0; i < height; i++)
	{
		int filtrationType = raw[i * (width + 1)];
		if (filtrationType == 0x00)
		{
			for (int j = 0; j < width + 1; j++)
			{
				filtered[i * (width + 1) + j] = raw[i * (width + 1) + j];
			}
		}
		else if (filtrationType == 0x01)
		{
			for (int j = 1; j < width + 1; j++)
			{
				unsigned char recon;
				if (j < 4)
				{
					recon = 0;
				}
				else
				{
					recon = filtered[i * (width + 1) + j - type_color];
				}
				filtered[i * (width + 1) + j] = raw[i * (width + 1) + j] + recon;
			}
		}
		else if (filtrationType == 0x02)
		{
			for (int j = 1; j < width + 1; j++)
			{
				unsigned char recon;
				if (i == 0)
				{
					recon = 0;
				}
				else
				{
					recon = filtered[(i - 1) * (width + 1) + j];
				}
				filtered[i * (width + 1) + j] = raw[i * (width + 1) + j] + recon;
			}
		}
		else if (filtrationType == 0x03)
		{
			for (int j = 1; j < width + 1; j++)
			{
				unsigned char reconb;
				if (i == 0)
				{
					reconb = 0;
				}
				else
				{
					reconb = (filtered[(i - 1) * (width + 1) + j]);
				}
				unsigned char recona;
				if (j < 4)
				{
					recona = 0;
				}
				else
				{
					recona = filtered[(width + 1) * i + j - type_color];
				}
				filtered[i * (width + 1) + j] = raw[i * (width + 1) + j] + (reconb + recona) / 2;
			}
		}
		else if (filtrationType == 0x04)
		{
			for (int j = 1; j < width + 1; j++)
			{
				unsigned char a = filtered[i * (width + 1) + j - type_color];
				unsigned char b = filtered[(i - 1) * (width + 1) + j];
				unsigned char c = filtered[(i - 1) * (width + 1) + j - type_color];
				if (i == 0)
				{
					b = 0x00;
					c = 0x00;
				}
				if (j < 4)
				{
					c = 0x00;
					a = 0x00;
				}
				int p = a + b - c;
				int pa = abs(p - a);
				int pb = abs(p - b);
				int pc = abs(p - c);
				int Pr;
				if (pa <= pb && pa <= pc)
				{
					Pr = a;
				}
				else if (pb <= pc)
				{
					Pr = b;
				}
				else
				{
					Pr = c;
				}
				filtered[i * (width + 1) + j] = raw[i * (width + 1) + j] + Pr;
			}
		}
		else
		{
			fprintf(stderr, "unsupported filter");
			return ERROR_UNSUPPORTED;
		}
	}
	return SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Wrong number of arguments");
		return ERROR_PARAMETER_INVALID;
	}

	FILE *inputFile;
	inputFile = fopen(argv[1], "rb");

	if (inputFile == NULL)
	{
		fprintf(stderr, "Cannot open file");
		return ERROR_CANNOT_OPEN_FILE;
	}

	struct PNG png;

	bool return_code_signature = readSignaturePNG(inputFile, png.signature);
	if (return_code_signature)
	{
		fclose(inputFile);
		fprintf(stderr, "Wrong signature of PNG file");
		return return_code_signature;
	}

	struct Chunk IHDRChunk;

	int return_code_reading_IHDR = readChunck(inputFile, &IHDRChunk);
	if (return_code_reading_IHDR)
	{
		fclose(inputFile);
		fprintf(stderr, "Errors with allocate memory in Chunk");
		// memory is released in function readChunk
		return return_code_reading_IHDR;
	}

	int code_return_IHDR = processingIHDR(&IHDRChunk);
	if (code_return_IHDR)
	{
		fclose(inputFile);
		free(IHDRChunk.data);
		fprintf(stderr, "Errors with processing IHDR");
		return code_return_IHDR;
	}

	unsigned char NameIDAT[4] = { 'I', 'D', 'A', 'T' };
	unsigned char NameIEND[4] = { 'I', 'E', 'N', 'D' };
	unsigned char namePLTE[4] = { 'P', 'L', 'T', 'E' };
	bool isIENDExist = false;
	bool isPLTEExist = false;

	unsigned int sizeOfData = 0;
	unsigned char *buffer = malloc(sizeOfData * sizeof(unsigned char));
	struct Chunk PLTEChunk;
	struct Chunk newChunk;
	while (!feof(inputFile))
	{
		int return_code_reading_newChunk = readChunck(inputFile, &newChunk);
		if (return_code_reading_newChunk)
		{
			fclose(inputFile);
			free(newChunk.data);
			free(IHDRChunk.data);
			free(buffer);
			if (isPLTEExist)
			{
				free(PLTEChunk.data);
			}
			fprintf(stderr, "Error with reading Chunk");
			return ERROR_OUT_OF_MEMORY;
		}
		// printf("%c%c%c%c\n", newChunk.type[0], newChunk.type[1], newChunk.type[2], newChunk.type[3]);

		if (compareType(newChunk.type, NameIDAT, 4))
		{
			// processing of Data
			unsigned int cursor = sizeOfData;
			sizeOfData += newChunk.length;
			buffer = realloc(buffer, sizeof(unsigned char) * sizeOfData);
			writeDataOfChunkInBuffer(buffer, newChunk.data, cursor, newChunk.length);
		}

		if (compareType(newChunk.type, namePLTE, 4))
		{
			if (IHDRChunk.data[9] != 0x03)
			{
				fclose(inputFile);
				free(newChunk.data);
				free(IHDRChunk.data);
				free(buffer);
				fprintf(stderr, "Wrong color type");
				return ERROR_DATA_INVALID;
			}
			if (newChunk.length % 3 != 0)
			{
				fclose(inputFile);
				free(newChunk.data);
				free(IHDRChunk.data);
				free(buffer);
				fprintf(stderr, "Wrong data PLTE");
				return ERROR_DATA_INVALID;
			}

			PLTEChunk = newChunk;
			isPLTEExist = true;
		}

		// find IEND Chunk
		if (compareType(newChunk.type, NameIEND, 4))
		{
			isIENDExist = true;

			// checking on end of file;

			unsigned char controlChar;
			unsigned long long control;
			control = fread(&controlChar, sizeof(unsigned char), 1, inputFile);
			if (control != 0)
			{
				fclose(inputFile);
				free(IHDRChunk.data);
				free(newChunk.data);
				free(buffer);
				if (isPLTEExist)
				{
					free(PLTEChunk.data);
				}
				fprintf(stderr, "More data after IEND");
				return ERROR_DATA_INVALID;
			}
			break;
		}
	}
	if (!isIENDExist)
	{
		fclose(inputFile);
		free(IHDRChunk.data);
		free(newChunk.data);
		free(buffer);
		if (isPLTEExist)
		{
			free(PLTEChunk.data);
		}
		fprintf(stderr, "Invalid PNG file");
		return ERROR_DATA_INVALID;
	}
	// printf("%c%c%c%c\n", newChunk.type[0], newChunk.type[1], newChunk.type[2], newChunk.type[3]);
	// printf("%c%c%c%c\n", PLTEChunk.type[0], PLTEChunk.type[1], PLTEChunk.type[2], PLTEChunk.type[3]);
	free(newChunk.data);
	fclose(inputFile);

	unsigned int width = makeInteger(&IHDRChunk.data[0]);
	unsigned int height = makeInteger(&IHDRChunk.data[4]);

	unsigned char color_type[2];
	color_type[0] = 'P';
	if (IHDRChunk.data[9] == 0x00)
	{
		color_type[1] = '5';
	}
	else if (IHDRChunk.data[9] == 0x02 || IHDRChunk.data[9] == 0x03)
	{
		color_type[1] = '6';
	}

	unsigned long dst_size = width * height * 3;

	unsigned char *destination = malloc(dst_size);

	uncompress(destination, &dst_size, buffer, (unsigned long)sizeOfData);

	FILE *output_file;
	output_file = fopen(argv[2], "wb");

	free(buffer);

	if (output_file == NULL)
	{
		free(IHDRChunk.data);
		free(destination);
		if (isPLTEExist)
		{
			free(PLTEChunk.data);
		}
		fprintf(stderr, "Cannot open file");
		return ERROR_CANNOT_OPEN_FILE;
	}

	fprintf(output_file, "%c%c\n%u %u\n255\n", color_type[0], color_type[1], width, height);

	int type_color;
	if (IHDRChunk.data[9] == 0x00)
	{
		type_color = 1;
	}
	else
	{
		type_color = 3;
	}
	free(IHDRChunk.data);

	if (isPLTEExist)
	{
		for (unsigned int i = 1; i < width * height * 3; i++)
		{
			if (i % (width + 1) == 0)
			{
				continue;
			}
			unsigned int cursor = (destination[i]) * 3;
			fwrite(PLTEChunk.data + cursor, 1, 3, output_file);
		}
		free(PLTEChunk.data);
	}
	else
	{
		unsigned char *filteredBuffer = malloc(dst_size * type_color);
		int return_code_filtration = filtration(destination, filteredBuffer, height, width * type_color, type_color);
		if (return_code_filtration)
		{
			free(filteredBuffer);
			free(destination);
			fclose(output_file);
			fprintf(stderr, "Errors with filtration");
			return return_code_reading_IHDR;
		}
		for (int i = 0; i < height; ++i)
		{
			fwrite(filteredBuffer + i * (width * type_color + 1) + 1, 1, width * type_color, output_file);
		}
		free(filteredBuffer);
	}
	free(destination);
	fclose(output_file);
	return SUCCESS;
}
