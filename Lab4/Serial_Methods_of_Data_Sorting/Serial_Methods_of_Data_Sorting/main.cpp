#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;
const double RandomDataMultiplier = 1000.0;

// Sorting by the standard library algorithm
void SerialStdSort(double* pData, int DataSize) {
	std::sort(pData, pData + DataSize);
}

// Function for simple setting the initial data
void DummyDataInitialization(double*& pData, int& DataSize) {
	for (int i = 0; i < DataSize; i++)
		pData[i] = DataSize - i;
}

// Function for allocating the memory and setting the initial values
void ProcessInitialization(double*& pData, int& DataSize) {
	pData = new double[DataSize];
	// Simple setting the data
	DummyDataInitialization(pData, DataSize);
	// Setting the data by the random generator
	//RandomDataInitialization(pData, DataSize);
}
// Function for computational process termination
void ProcessTermination(double* pData) {
	delete[]pData;
}
// Function for initializing the data by the random generator
void RandomDataInitialization(double*& pData, int& DataSize) {
	srand((unsigned)time(0));
	for (int i = 0; i < DataSize; i++)
		pData[i] = double(rand()) / RAND_MAX * RandomDataMultiplier;
}
// Function for the serial bubble sort algorithm
void SerialBubble(double* pData, int DataSize) {
	double Tmp;
	for (int i = 1; i < DataSize; i++)
		for (int j = 0; j < DataSize - i; j++)
			if (pData[j] > pData[j + 1]) {
				Tmp = pData[j];
				pData[j] = pData[j + 1];
				pData[j + 1] = Tmp;
			}
}

int main(int argc, char* argv[]) {
	std::vector<int> testData;

	testData.push_back(10000);
	testData.push_back(16);
	testData.push_back(104);
	testData.push_back(10000);
	testData.push_back(20000);
	testData.push_back(30000);
	testData.push_back(40000);
	testData.push_back(50000);

	for (auto i = testData.begin(); i != testData.end(); ++i)
	{
		double* pData = 0;
		int DataSize = *i;
		time_t start, finish;
		double duration = 0.0;
		// Process initialization
		ProcessInitialization(pData, DataSize);
		// Serial bubble sort
		start = clock();
		SerialStdSort(pData, DataSize);
		finish = clock();
		duration = (finish - start) / double(CLOCKS_PER_SEC);
		printf("Test data = %d; Time of execution = %f\n", DataSize, duration);
		// Process termination
		ProcessTermination(pData);
	}
	return 0;
}
