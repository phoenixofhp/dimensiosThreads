#include<iostream>
#include<random>
#include<vector>
#include<chrono>
#include<thread>
#include<mutex>

#define THREAD_LOOP
//#define DEBUG_ON
std::mutex mutex;

constexpr int THREADS = 2;
constexpr int DIMENSIONS = 15;
constexpr int AMOUNT = 10000;
constexpr double RADIUS = 1.0;
constexpr double EPSILON = 0.1;
constexpr double RADIUS_SQR = RADIUS * RADIUS;
constexpr double RADIUS_EPSILON_SQR = (RADIUS - EPSILON) * (RADIUS - EPSILON);


double rng(int seed) {

	static std::default_random_engine rng(seed);
	std::uniform_real_distribution<double> dist(-1.0, 1.0);
	return dist(rng);
}

bool isSphere(std::vector<double>& point, int n) {

	double summ = 0.0;
	for (int i = 0; i < (n + 1); i++) {
		summ += (point[i] * point[i]);
		if (summ > (RADIUS_SQR)) return false;
	}

	return true;
}

bool isInLesserSphere(std::vector<double>& point) {

	double summ = 0.0;
	for (int i = 0; i < DIMENSIONS; i++) {
		summ += (point[i] * point[i]);
		if (summ > RADIUS_EPSILON_SQR) return false;
	}

	return true;
}

void run( std::vector<std::vector<double>>* points,int seed) {
	std::vector<double> point(DIMENSIONS, 0);
	while (points->size() < AMOUNT) {
		
			
			for (int j = 0; j < DIMENSIONS; j++) {
				point[j] = rng(seed);
				if (isSphere(point, j)) continue;
				else j--;
			}
			mutex.lock();
			points->push_back(point);

#ifdef DEBUG_ON
			std::cout << 100.0 * (double)points->size() / (double)AMOUNT << "% òî÷åê íàéäåíî" << std::endl;
#endif 
			mutex.unlock();
	
	}
}

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	std::setlocale(LC_ALL, "ru_RU.UTF-8");

	std::thread threads[THREADS];

	std::vector<std::vector<double>>* points;
	points = new std::vector<std::vector<double>>;
	points->reserve(AMOUNT);

	#ifdef THREAD_LOOP
		for (int i = 0; i < THREADS; i++)
		{
			(threads[i])= std::thread(run, points,i);
		}
	#endif

	#ifndef THREAD_LOOP
		std::thread th1(run, points,0);
		std::thread th2(run, points,1);
		std::thread th3(run, points,2);
		std::thread th4(run, points,3);
	#endif 
	do
	{
		bool end = false;

		#ifdef THREAD_LOOP
			while(!end)
			{
				if (points->size() >= AMOUNT)
				{
					for (int i = 0; i < THREADS; i++)
					{
						(threads[i]).join();
					}
					end = true;
				}
			}
		#endif

		#ifndef THREAD_LOOP
			while (!end) 
			{
				if (points->size() >= AMOUNT)
				{
					th1.join();
					th2.join();
					th3.join();
					th4.join();
					end = true;
				}
			}
		#endif
	} while (false);

	int outside = points->size();
	for (size_t i = 0; i < points->size(); i++) {

		if (isInLesserSphere((*points)[i])) outside--;

	}

	std::cout << "Áåðåì " << DIMENSIONS << " ìåðíóþ ñôåðó" << std::endl;
	std::cout << "Áåðåì " << points->size() << " òî÷åê âíóòðè ñôåðû" << std::endl;
	std::cout << "Â ïîâåðõíîñòíîì ñëîå " << 100.0 * (double)outside / (double)points->size() << "% òî÷åê" << std::endl;

	delete points;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Ïðîøëî " << elapsed.count() << " ìñ, íà "<< THREADS << " ïîòîêàõ\n";
	std::cin.get();
	std::cin.get();
	return 0;
}
