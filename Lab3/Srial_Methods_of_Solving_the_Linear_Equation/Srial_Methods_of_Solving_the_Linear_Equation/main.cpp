#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <vector>
int* pSerialPivotPos; // Number of pivot rows selected at the iterations
int* pSerialPivotIter; // Iterations, at which the rows were pivots
// Function for simple initialization of the matrix and the vector elements
void DummyDataInitialization(double* pMatrix, double* pVector, int Size) {
	int i, j; // Loop variables
	for (i = 0; i < Size; i++) {
		pVector[i] = i + 1;
		for (j = 0; j < Size; j++) {
			if (j <= i)
				pMatrix[i * Size + j] = 1;
			else
				pMatrix[i * Size + j] = 0;
		}
	}
}
// Function for random initialization of the matrix and the vector elements
void RandomDataInitialization(double* pMatrix, double* pVector, int Size) {
	int i, j; // Loop variables
	srand(unsigned(clock()));
	for (i = 0; i < Size; i++) {
		pVector[i] = rand() / double(1000);
		for (j = 0; j < Size; j++) {
			if (j <= i)
				pMatrix[i * Size + j] = rand() / double(1000);
			else
				pMatrix[i * Size + j] = 0;
		}
	}
}
// Function for memory allocation and data initialization
void ProcessInitialization(double*& pMatrix, double*& pVector,
	double*& pResult, int& Size) {
	// Memory allocation
	pMatrix = new double[Size * Size];
	pVector = new double[Size];
	pResult = new double[Size];
	// Initialization of the matrix and the vector elements
	DummyDataInitialization(pMatrix, pVector, Size);
	//RandomDataInitialization(pMatrix, pVector, Size);
}
// Function for finding the pivot row
int FindPivotRow(double* pMatrix, int Size, int Iter) {
	int PivotRow = -1; // Index of the pivot row
	int MaxValue = 0; // Value of the pivot element
	int i; // Loop variable
	// Choose the row, that stores the maximum element
	for (i = 0; i < Size; i++) {
		if ((pSerialPivotIter[i] == -1) &&
			(fabs(pMatrix[i * Size + Iter]) > MaxValue)) {
			PivotRow = i;
			MaxValue = fabs(pMatrix[i * Size + Iter]);
		}
	}
	return PivotRow;
}
// Function for the column elimination
void SerialColumnElimination(double* pMatrix, double* pVector, int Pivot,
	int Iter, int Size) {
	double PivotValue, PivotFactor;
	PivotValue = pMatrix[Pivot * Size + Iter];
	for (int i = 0; i < Size; i++) {
		if (pSerialPivotIter[i] == -1) {
			PivotFactor = pMatrix[i * Size + Iter] / PivotValue;
			for (int j = Iter; j < Size; j++) {
				pMatrix[i * Size + j] -= PivotFactor * pMatrix[Pivot * Size + j];
			}
			pVector[i] -= PivotFactor * pVector[Pivot];
		}
	}
}
// Function for the Gaussian elimination
void SerialGaussianElimination(double* pMatrix, double* pVector, int Size) {
	int Iter; // Number of the iteration of the Gaussian elimination
	int PivotRow; // Number of the current pivot row
	for (Iter = 0; Iter < Size; Iter++) {
		// Finding the pivot row
		PivotRow = FindPivotRow(pMatrix, Size, Iter);
		pSerialPivotPos[Iter] = PivotRow;
		pSerialPivotIter[PivotRow] = Iter;
		SerialColumnElimination(pMatrix, pVector, PivotRow, Iter, Size);
	}
}
// Function for the back substution
void SerialBackSubstitution(double* pMatrix, double* pVector,
	double* pResult, int Size) {
	int RowIndex, Row;
	for (int i = Size - 1; i >= 0; i--) {
		RowIndex = pSerialPivotPos[i];
		pResult[i] = pVector[RowIndex] / pMatrix[Size * RowIndex + i];
		for (int j = 0; j < i; j++) {
			Row = pSerialPivotPos[j];
			pVector[j] -= pMatrix[Row * Size + i] * pResult[i];
			pMatrix[Row * Size + i] = 0;
		}
	}
}
// Function for the execution of the Gauss algorithm
void SerialResultCalculation(double* pMatrix, double* pVector,
	double* pResult, int Size) {
	// Memory allocation
	pSerialPivotPos = new int[Size];
	pSerialPivotIter = new int[Size];
	for (int i = 0; i < Size; i++) {
		pSerialPivotIter[i] = -1;
	}
	// Gaussian elimination
	SerialGaussianElimination(pMatrix, pVector, Size);
	// Back substitution
	SerialBackSubstitution(pMatrix, pVector, pResult, Size);
	// Memory deallocation
	delete[] pSerialPivotPos;
	delete[] pSerialPivotIter;
}
// Function for computational process termination
void ProcessTermination(double* pMatrix, double* pVector, double* pResult) {
	delete[] pMatrix;
	delete[] pVector;
	delete[] pResult;
}
void main() {

	std::vector<int> testData;

	testData.push_back(1000);
	testData.push_back(10);
	testData.push_back(100);
	testData.push_back(500);
	testData.push_back(1000);
	testData.push_back(1500);
	testData.push_back(2000);
	testData.push_back(2500);
	testData.push_back(3000);

	for (auto i = testData.begin(); i != testData.end(); ++i)
	{
		double* pMatrix; // Matrix of the linear system
		double* pVector; // Right parts of the linear system
		double* pResult; // Result vector
		int Size = *i; // Sizes of the initial matrix and the vector
		time_t start, finish;
		double duration;
		// Memory allocation and definition of objects' elements
		ProcessInitialization(pMatrix, pVector, pResult, Size);
		// Execution of the Gauss algorithm
		start = clock();
		SerialResultCalculation(pMatrix, pVector, pResult, Size);
		finish = clock();
		duration = (finish - start) / double(CLOCKS_PER_SEC);
		// Printing the result vector
		// Printing the execution time of the Gauss method
		printf("Test data = %d; Time of execution = %f\n", Size, duration);
		// Computational process termination
		ProcessTermination(pMatrix, pVector, pResult);
	}
}