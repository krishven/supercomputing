#include <iostream>
#include <omp.h>
using namespace std;

#define V 4
#define M 2
#define NA 100000

#define X11 xi, xj
#define X12 xi, xj + newlen
#define X21 xi + newlen, xj
#define X22 xi + newlen, xj + newlen

#define U11 ui, uj
#define U12 ui, uj + newlen
#define U21 ui + newlen, uj
#define U22 ui + newlen, uj + newlen

#define V11 vi, vj
#define V12 vi, vj + newlen
#define V21 vi + newlen, vj
#define V22 vi + newlen, vj + newlen

#define PARALLEL 

void iterFW (int X[][V], int length, int xi, int xj, int ui, int uj, int vi, int vj)
{
	for (int k = uj; k < uj + length; k++)
	{
		for (int i = xi; i < xi + length; i++)
		{
			for (int j = xj; j < xj + length; j++)
			{
				if (X[i][k] + X[k][j] < X[i][j])
					X[i][j] = X[i][k] + X[k][j];
			}
		}
	}
}

void AFW (int X[][V], int length, int xi, int xj, int ui, int uj, int vi, int vj) {
	if (length == M)
		iterFW(X, length, X11, U11, V11);
	else {
		int newlen = length / 2;
		AFW(X, newlen, X11, U11, V11);

#ifdef PARALLEL		
		#pragma omp parallel num_threads(2)
		{
			if (omp_get_thread_num()!=0)	
				AFW(X, newlen, X12, U11, V12);
			else
				AFW(X, newlen, X21, U21, V11);			
		}
		
		#pragma omp barrier

		AFW(X, newlen, X22, U21, V12);
		AFW(X, newlen, X22, U22, V22);
//		cilk_spawn AFW(X, newlen, X21, U22, V21);
		#pragma omp parallel num_threads(2)
		{
			if (omp_get_thread_num()!=0)	
				AFW(X, newlen, X21, U22, V21);
			else
				AFW(X, newlen, X12, U12, V22);
		}
		
		#pragma omp barrier
#else
		AFW(X, newlen, X12, U11, V12);
		AFW(X, newlen, X21, U21, V11);			
		AFW(X, newlen, X22, U21, V12);
		AFW(X, newlen, X22, U22, V22);
		AFW(X, newlen, X21, U22, V21);
		AFW(X, newlen, X12, U12, V22);
#endif

		AFW(X, newlen, X11, U12, V21);

	}
}

void printSolution(int X[][V])
{
	for (int i = 0; i < V; i++)
	{
		for (int j = 0; j < V; j++)
		{
			if (X[i][j] == NA)
				cout << "       " << "-";
			else
				cout << "       "   << X[i][j];
		}
		cout << "\n";
	}
}

int main(int argc, char* argv[])
{
	int X[V][V] = { {0,   5,  NA, 10},
		{NA, 0,   3, NA},
		{NA, NA, 0,   1},
		{NA, NA, NA, 0}
	};

	// Print the solution
	double start = omp_get_wtime();
	AFW(X, V, 0, 0, 0, 0, 0, 0);
	std::cout << "Time diff" << omp_get_wtime()-start <<"\n";

	printSolution(X);
	return 0;
}
