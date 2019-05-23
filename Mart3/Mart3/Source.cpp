#include <Eigen/Dense>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <functional>
#include <fstream>
#include <iomanip> // ������������ �����/������

#define MAX_THREADS -1

using namespace std;

int main()
{
	unsigned int NI = 1000, NJ = 1000, NK = 1000, // ������� �������
		len_lag = 50; // ����� �����
	unsigned int M = NI * NJ * NK;

	int threads = 2, blank_value = -1;

	/*cout << "Threads: ";
	cin >> threads;*/

	if (threads == MAX_THREADS)
		threads = omp_get_max_threads();

#pragma region Create Sample

	Eigen::VectorXf cube(NI*NJ*NK); // �������
	float Etalon_SumMatr = 0; // ��������� ����� ��������� ����������� �������
	
	ofstream out_data("answer.txt");
	srand(time(NULL));
	for (size_t i = 0; i < NI; i++)
		for (size_t j = 0; j < NJ; j++)
			for (size_t k = 0; k < NK; k++)
			{
				// ���������
				cube(k + j*NK + i*NK*NJ) = i;

				// ������� �����
				Etalon_SumMatr += i;
			}

	out_data << fixed << setprecision(6) << "Etalon Sum: " << Etalon_SumMatr << endl;
	out_data << "Etalon Norm: " << Etalon_SumMatr / (NI*NJ*NK) << endl;
#pragma endregion
	
#pragma region Parallel 3 loop
	
	float Parall_SumMatr = 0, // ����� ��������� ����������� �������
		local = 0;

	// ������������ ������
	auto begin = std::chrono::steady_clock::now();

#pragma omp parallel shared(cube) num_threads(threads)
	{
#pragma omp for reduction(+:Parall_SumMatr)
		for (auto n = 0; n < NI * NJ * NK; ++n)
		{
			auto k = n % NK, j = (n / NK) % NJ, i = (n / NK) / NJ;
			// ������� �����
			Parall_SumMatr += cube[n];
		}

//#pragma omp barrier
//		cout << fixed << setprecision(6) << "lcal:" << threads / (2 * threads - 1) * Parall_SumMatr << endl;
	}

	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "The time: " << elapsed_ms.count() << " ms\n";

	out_data << fixed << setprecision(6) << "Parallel Sum: " << Parall_SumMatr << endl;
	out_data << "Parallel Norm: " << Parall_SumMatr / (NI*NJ*NK) << endl;

#pragma endregion

	out_data.close();
	cout << fixed << setprecision(6) << "Error: " << (Etalon_SumMatr - Parall_SumMatr) / Etalon_SumMatr << endl;
	cout << "\n_____End!_____" << endl;
	system("pause");

	return 0;
}