#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <mpi.h>
#include <vector>

enum split_mode { KeepFirstHalf, KeepSecondHalf };

using namespace std;

const double RandomDataMultiplier = 1000.0;
int ProcNum = 0; // Number of available processes
int ProcRank = -1; // Rank of current process

// Function for copying the sorted data
void CopyData(double* pData, int DataSize, double* pDataCopy) {
	copy(pData, pData + DataSize, pDataCopy);
}
// Function for comparing the data
bool CompareData(double* pData1, double* pData2, int DataSize) {
	return equal(pData1, pData1 + DataSize, pData2);
}
// Serial bubble sort algorithm
void SerialBubbleSort(double* pData, int DataSize) {
	double Tmp;
	for (int i = 1; i < DataSize; i++)
		for (int j = 0; j < DataSize - i; j++)
			if (pData[j] > pData[j + 1]) {
				Tmp = pData[j];
				pData[j] = pData[j + 1];
				pData[j + 1] = Tmp;
			}
}
// Sorting by the standard library algorithm
void SerialStdSort(double* pData, int DataSize) {
	sort(pData, pData + DataSize);
}
// Function for simple setting the data to be sorted
void DummyDataInitialization(double*& pData, int& DataSize) {
	for (int i = 0; i < DataSize; i++)
		pData[i] = DataSize - i;
}
// Function for initializing the data by the random generator
void RandomDataInitialization(double*& pData, int& DataSize) {
	srand((unsigned)time(0));
	for (int i = 0; i < DataSize; i++)
		pData[i] = double(rand()) / RAND_MAX * RandomDataMultiplier;
}
// Function for data exchange between the neighboring processes
void ExchangeData(double* pProcData, int BlockSize, int DualRank,
	double* pDualData, int DualBlockSize) {
	MPI_Status status;
	MPI_Sendrecv(pProcData, BlockSize, MPI_DOUBLE, DualRank, 0,
		pDualData, DualBlockSize, MPI_DOUBLE, DualRank, 0,
		MPI_COMM_WORLD, &status);
}
// Function for allocating the memory and setting the initial values
void ProcessInitialization(double*& pData, int& DataSize, double
	*& pProcData, int& BlockSize) {
	setvbuf(stdout, 0, _IONBF, 0);
	// Broadcasting the data size
	MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int RestData = DataSize;
	for (int i = 0; i < ProcRank; i++)
		RestData -= RestData / (ProcNum - i);
	BlockSize = RestData / (ProcNum - ProcRank);
	pProcData = new double[BlockSize];
	if (ProcRank == 0) {
		pData = new double[DataSize];
		// Data initalization
		//RandomDataInitialization(pData, DataSize);
		DummyDataInitialization(pData, DataSize);
	}
}
// Function for computational process termination
void ProcessTermination(double* pData, double* pProcData) {
	if (ProcRank == 0)
		delete[]pData;
	delete[]pProcData;
}
// Data distribution among the processes
void DataDistribution(double* pData, int DataSize, double* pProcData, int
	BlockSize) {
	// Allocate memory for temporary objects
	int* pSendInd = new int[ProcNum];
	int* pSendNum = new int[ProcNum];
	int RestData = DataSize;
	int CurrentSize = DataSize / ProcNum;
	pSendNum[0] = CurrentSize;
	pSendInd[0] = 0;
	for (int i = 1; i < ProcNum; i++) {
		RestData -= CurrentSize;
		CurrentSize = RestData / (ProcNum - i);
		pSendNum[i] = CurrentSize;
		pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
	}
	MPI_Scatterv(pData, pSendNum, pSendInd, MPI_DOUBLE, pProcData,
		pSendNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// Free the memory
	delete[] pSendNum;
	delete[] pSendInd;
}
// Function for data collection
void DataCollection(double* pData, int DataSize, double* pProcData, int
	BlockSize) {
	// Allocate memory for temporary objects
	int* pReceiveNum = new int[ProcNum];
	int* pReceiveInd = new int[ProcNum];
	int RestData = DataSize;
	pReceiveInd[0] = 0;
	pReceiveNum[0] = DataSize / ProcNum;
	for (int i = 1; i < ProcNum; i++) {
		RestData -= pReceiveNum[i - 1];
		pReceiveNum[i] = RestData / (ProcNum - i);
		pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
	}
	MPI_Gatherv(pProcData, BlockSize, MPI_DOUBLE, pData,
		pReceiveNum, pReceiveInd, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// Free the memory
	delete[]pReceiveNum;
	delete[]pReceiveInd;
}
// Parallel bubble sort algorithm
void ParallelBubble(double* pProcData, int BlockSize) {
	// Local sorting the process data
	SerialBubbleSort(pProcData, BlockSize);
	int Offset;
	split_mode SplitMode;
	for (int i = 0; i < ProcNum; i++) {
		if ((i % 2) == 1) {
			if ((ProcRank % 2) == 1) {
				Offset = 1;
				SplitMode = KeepFirstHalf;
			}
			else {
				Offset = -1;
				SplitMode = KeepSecondHalf;
			}
		}
		else {
			if ((ProcRank % 2) == 1) {
				Offset = -1;
				SplitMode = KeepSecondHalf;
			}
			else {
				Offset = 1;
				SplitMode = KeepFirstHalf;
			}
		}
		// Check the first and last processes
		if ((ProcRank == ProcNum - 1) && (Offset == 1)) continue;
		if ((ProcRank == 0) && (Offset == -1)) continue;
		MPI_Status status;
		int DualBlockSize;
		MPI_Sendrecv(&BlockSize, 1, MPI_INT, ProcRank + Offset, 0,
			&DualBlockSize, 1, MPI_INT, ProcRank + Offset, 0,
			MPI_COMM_WORLD, &status);
		double* pDualData = new double[DualBlockSize];
		double* pMergedData = new double[BlockSize + DualBlockSize];
		// Data exchange
		ExchangeData(pProcData, BlockSize, ProcRank + Offset, pDualData,
			DualBlockSize);
		// Data merging
		merge(pProcData, pProcData + BlockSize, pDualData, pDualData +
			DualBlockSize, pMergedData);
		// Data splitting
		if (SplitMode == KeepFirstHalf)
			copy(pMergedData, pMergedData + BlockSize, pProcData);
		else
			copy(pMergedData + BlockSize, pMergedData + BlockSize +
				DualBlockSize, pProcData);
		delete[]pDualData;
		delete[]pMergedData;
	}
}

// Function for testing the result of parallel bubble sort
void TestResult(double* pData, double* pSerialData, int DataSize) {
	MPI_Barrier(MPI_COMM_WORLD);
	if (ProcRank == 0) {
		SerialBubbleSort(pSerialData, DataSize);
		//SerialStdSort(pSerialData, DataSize);
		if (!CompareData(pData, pSerialData, DataSize))
			printf("The results of serial and parallel algorithms are "
				"NOT identical. Check your code\n");
		else
			printf("The results of serial and parallel algorithms are "
				"identical\n");
	}
}

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	double* pData = 0;
	double* pProcData = 0;
	int DataSize = atoi(argv[1]);
	int BlockSize = 0;
	double* pSerialData = 0;
	double start, finish;
	double duration = 0.0;
	// Process initialization
	ProcessInitialization(pData, DataSize, pProcData, BlockSize);
	if (ProcRank == 0) {
		pSerialData = new double[DataSize];
		CopyData(pData, DataSize, pSerialData);
	}
	start = MPI_Wtime();
	// Distributing the initial data between processes
	DataDistribution(pData, DataSize, pProcData, BlockSize);
	// Parallel bubble sort
	ParallelBubble(pProcData, BlockSize);
	// Execution of data collection
	DataCollection(pData, DataSize, pProcData, BlockSize);
	TestResult(pData, pSerialData, DataSize);
	finish = MPI_Wtime();
	duration = finish - start;
	if (ProcRank == 0)
		printf("Test data = %d; Time of execution = %f\n", DataSize, duration);
	// Process termination
	ProcessTermination(pData, pProcData);
	if (ProcRank == 0)
		delete[]pSerialData;

	MPI_Finalize();

	return 0;
}