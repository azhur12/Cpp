#include "return_codes.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

double product_scalar(double *vector1, double *vector2, size_t size);

void projection(double *b, double *a, size_t size, double *proj_b_a);

void sum_of_vectors(double *vector1, double *vector2, size_t size, double *result);

void sum_projections(double *projections, size_t num_projections, double *a, size_t size, double *summa);

void subtract_of_vectors(double *vector1, double *vector2, size_t size, double *result);

int Gram_Schmidt_process(double *A, size_t size_of_vector, double *Q);

void transpose_matrix(double *Q, size_t n, double *Q_transpose);

void multiplying_matrix(double *A, double *B, size_t n, double *result);

double length_of_vector(double *vector, size_t n);

void divide_vector(double *vector, size_t n, double coefficient, double *result_vector);

void display_matrix(double *matrix, size_t n);

double determinant_2_2(double *matrix, size_t step);

void eigen_values_print(double *matrix, size_t n, FILE *out);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "Wrong number of arguments");
		return ERROR_PARAMETER_INVALID;
	}
	FILE *file;
	file = fopen(argv[1], "rb");

	if (file == NULL)
	{
		fprintf(stderr, "Cannot open file");
		return ERROR_CANNOT_OPEN_FILE;
	}

	size_t n;
	if (fscanf(file, "%zd", &n) != 1)
	{
		fprintf(stderr, "Invalid size of matrix");
		return ERROR_DATA_INVALID;
	}

	double *A = malloc(sizeof(double) * n * n);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (fscanf(file, "%lf", &(A[i * n + j])) != 1)
			{
				fprintf(stderr, "Invalid data in [%d][%d] component of matrix", i, j);
				return ERROR_DATA_INVALID;
			}
		}
	}
	fclose(file);
	// display_matrix(A,n);
	double *Q = malloc(sizeof(double) * n * n);
	if (Q == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for matrix Q");
		free(A);
		return ERROR_OUT_OF_MEMORY;
	}
	double *Q_transpose = malloc(sizeof(double) * n * n);
	if (Q_transpose == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for matrix Q");
		free(Q);
		free(A);
		return ERROR_OUT_OF_MEMORY;
	}
	double *R = malloc(sizeof(double) * n * n);
	if (R == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for matrix R");
		free(Q);
		free(Q_transpose);
		free(A);
		return ERROR_OUT_OF_MEMORY;
	}
	int count_of_iteraitions = 2000;
	for (int i = 0; i < count_of_iteraitions; i++)
	{
		int return_code = Gram_Schmidt_process(A, n, Q_transpose);
		if (return_code != SUCCESS)
		{
			fprintf(stderr, "Error in Gram-Schmidt_process");
			free(A);
			free(Q);
			free(Q_transpose);
			free(R);
			return return_code;
		}
		// display_matrix(Q_transpose,n);
		multiplying_matrix(Q_transpose, A, n, R);
		// display_matrix(R,n);
		transpose_matrix(Q_transpose, n, Q);
		// display_matrix(Q, n);
		multiplying_matrix(R, Q, n, A);
		// display_matrix(A,n);
	}

	FILE *out = fopen(argv[2], "w");
	if (out == NULL)
	{
		free(Q_transpose);
		free(A);
		free(Q);
		free(R);
		fprintf(stderr, "Cannot open file");
		return ERROR_CANNOT_OPEN_FILE;
	}
	eigen_values_print(A, n, out);
	fclose(out);

	free(Q_transpose);
	free(A);
	free(Q);
	free(R);
}

void display_matrix(double *matrix, size_t n)
{	 // 32 yellow 33 green etc
	printf("{\n");
	for (int i = 0; i < n; i++)
	{
		printf("{");
		for (int j = 0; j < n; j++)
		{
			printf("%g ", matrix[i * n + j]);
		}
		printf("},\n");
	}
	printf("}\n");
}

void eigen_values_print(double *matrix, size_t n, FILE *out)
{
	double eps = 1e-10;
	bool isPrintedComplexValue = false;
	for (size_t i = 0; i + 1 < n; i++)
	{
		if (fabs(matrix[(i + 1) * n + i]) > eps)
		{
			double m = (matrix[i * n + i] + matrix[(i + 1) * n + (i + 1)]) / 2;
			double det = determinant_2_2(&matrix[i * n + i], n);
			double imaginary_part = m * m - det;
			if (imaginary_part < 0)
			{
				fprintf(out, "%g +%gi\n", m, sqrt(fabs(imaginary_part)));
				fprintf(out, "%g -%gi\n", m, sqrt(fabs(imaginary_part)));
				isPrintedComplexValue = true;
			}
			else
			{
				fprintf(out, "%g\n", m + sqrt(imaginary_part));
				fprintf(out, "%g\n", m - sqrt(imaginary_part));
				isPrintedComplexValue = false;
			}
			i++;
		}
		else
		{
			isPrintedComplexValue = false;
			fprintf(out, "%g\n", matrix[i * n + i]);
		}
	}
	if (isPrintedComplexValue == false)
	{
		fprintf(out, "%g\n", matrix[(n - 1) * n + (n - 1)]);
	}
}

double determinant_2_2(double *matrix, size_t step)
{
	return matrix[0] * matrix[step + 1] - matrix[1] * matrix[step];
}

void multiplying_matrix(double *A, double *B, size_t n, double *result)
{
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			double component = 0.0;
			for (size_t k = 0; k < n; k++)
			{
				component += (A[i * n + k] * B[k * n + j]);
			}
			result[i * n + j] = component;
		}
	}
}

int Gram_Schmidt_process(double *A, size_t n, double *Q)
{
	for (size_t i = 0; i < n; i++)
	{
		double *vector_of_Q = Q + i * n;

		double *calculating = malloc(sizeof(double) * n * i);
		if (calculating == NULL)
		{
			fprintf(stderr, "Cannot allocate memory");
			return ERROR_OUT_OF_MEMORY;
		}
		double *vector_of_A_i = (A + i * n);
		double *projection_j = malloc(sizeof(double) * n);
		if (projection_j == NULL)
		{
			fprintf(stderr, "Cannot allocate memory");
			free(calculating);
			return ERROR_OUT_OF_MEMORY;
		}
		for (size_t j = 0; j < i; j++)
		{
			double *vector_of_Q_j = Q + j * n;
			projection(vector_of_Q_j, vector_of_A_i, n, projection_j);
			for (size_t k = 0; k < n; k++)
			{
				calculating[j * n + k] = projection_j[k];
			}
		}
		double *sum_of_projections = calloc(n, sizeof(double));
		if (sum_of_projections == NULL)
		{
			fprintf(stderr, "Cannot allocate memory");
			free(calculating);
			free(projection_j);
			return ERROR_OUT_OF_MEMORY;
		}
		sum_projections(calculating, i, vector_of_A_i, n, sum_of_projections);
		double *subtraction_of_vectors = malloc(sizeof(double) * n);
		if (subtraction_of_vectors == NULL)
		{
			fprintf(stderr, "Cannot allocate memory");
			free(calculating);
			free(projection_j);
			free(sum_of_projections);
			return ERROR_OUT_OF_MEMORY;
		}
		subtract_of_vectors(vector_of_A_i, sum_of_projections, n, subtraction_of_vectors);
		divide_vector(subtraction_of_vectors, n, length_of_vector(subtraction_of_vectors, n), vector_of_Q);
		free(calculating);
		free(sum_of_projections);
		free(subtraction_of_vectors);
		free(projection_j);
	}
	return SUCCESS;
}

void transpose_matrix(double *Q, size_t n, double *Q_transpose)
{
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < n; j++)
		{
			Q_transpose[j * n + i] = Q[i * n + j];
		}
	}
}

void sum_projections(double *projections, size_t num_projections, double *a, size_t size, double *summa)
{
	for (size_t i = 0; i < num_projections; i++)
	{
		double *proj = projections + i * size;
		sum_of_vectors(summa, proj, size, summa);
	}
}

void sum_of_vectors(double *vector1, double *vector2, size_t size, double *result)
{
	for (size_t i = 0; i < size; i++)
	{
		result[i] = vector1[i] + vector2[i];
	}
}

void subtract_of_vectors(double *vector1, double *vector2, size_t size, double *result)
{
	for (size_t i = 0; i < size; i++)
	{
		result[i] = vector1[i] - vector2[i];
	}
}
void display_vector(double *vector, size_t n)
{
	printf("{");
	for (int i = 0; i < n; i++)
	{
		printf("%g ", vector[i]);
	}
	printf("}\n");
}

void divide_vector(double *vector, size_t n, double coefficient, double *result_vector)
{
	if (fabs(coefficient) < 1e-10)
	{
		for (size_t i = 0; i < n; i++)
		{
			result_vector[i] = vector[i];
		}
	}
	else
	{
		for (size_t i = 0; i < n; i++)
		{
			result_vector[i] = vector[i] / coefficient;
		}
	}
}

void projection(double *b, double *a, size_t size, double *proj_b_a)
{
	if (fabs(product_scalar(b, b, size)) < 1e-10)
	{
		for (size_t i = 0; i < size; i++)
		{
			proj_b_a[i] = 0;
		}
		return;
	}
	double coefficient = product_scalar(a, b, size) / product_scalar(b, b, size);
	for (size_t i = 0; i < size; i++)
	{
		proj_b_a[i] = coefficient * b[i];
	}
}

double product_scalar(double *vector1, double *vector2, size_t size)
{
	double result = 0.0;
	for (size_t i = 0; i < size; i++)
	{
		result += vector1[i] * vector2[i];
	}
	return result;
}

double length_of_vector(double *vector, size_t n)
{
	return sqrt(product_scalar(vector, vector, n));
}
