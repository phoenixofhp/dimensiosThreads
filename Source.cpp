#include<iostream>
#include<random>
#include<vector>
#include<chrono>
#include<thread>
#include<mutex>

#define THREAD_LOOP //Всегда включена в актуальной версии
//#define DEBUG_ON //Вывод в консоль информации о том, сколько % точек уже найдено. Сильно замедляет работу, если размерность пространства <20-25
std::mutex mutex; //Объяслен в глобальной области, правильно ли так делать?

constexpr int THREADS = 4; //Кол-во потоков
constexpr int DIMENSIONS = 15; //Размерность пространства
constexpr int AMOUNT = 10000; //Кол-во точек, которых мы генерируем
constexpr double RADIUS = 1.0; //Радиус сферы
constexpr double EPSILON = 0.1; //Толщина внешнего слоя сферы
constexpr double RADIUS_SQR = RADIUS * RADIUS;
constexpr double RADIUS_EPSILON_SQR = (RADIUS - EPSILON) * (RADIUS - EPSILON);

double rng(int seed) {

	static std::default_random_engine rng(seed);
	std::uniform_real_distribution<double> dist(-1.0 * RADIUS, 1.0 * RADIUS);
	return dist(rng);
} //Возвращает случайную точку из квадрата со стороной RADIUS

bool isSphere(std::vector<double>& point, int n) {

	double summ = 0.0;
	for (int i = 0; i < (n + 1); i++) {
		summ += (point[i] * point[i]);
		if (summ > (RADIUS_SQR)) return false;
	}

	return true;
} //Проверяет, лежит ли точка внутри сферы

bool isInLesserSphere(std::vector<double>& point) {

	double summ = 0.0;
	for (int i = 0; i < DIMENSIONS; i++) {
		summ += (point[i] * point[i]);
		if (summ > RADIUS_EPSILON_SQR) return false;
	}

	return true;
} //Проверяет, лежит ли точка во внутреннем слое сферы

void run(std::vector<std::vector<double>>* points,int seed) {

	while (points->size() < AMOUNT) {
		std::vector<std::vector<double>> point_n;
		int j = -1;

		do 
		{	
			j++;
			point_n.push_back(std::vector<double>(DIMENSIONS, 0)); //Инициализируем точку в пространстве нулями
			for (int i = 0; i < DIMENSIONS; i++)
			{
				(point_n[j])[i] = (rng(seed));
				if (isSphere(point_n[j], i)) continue;
				else i--;
			}
		} 
		while (!mutex.try_lock());

		for (size_t i = 0; i < point_n.size(); i++)
		{
			points->push_back(point_n[i]);
		}

		#ifdef DEBUG_ON
		std::cout << 100.0 * (double)points->size() / (double)AMOUNT << "% точек найдено" << std::endl;
		#endif 

		mutex.unlock();
	}
}

int main() {

	auto start = std::chrono::high_resolution_clock::now(); //Измеряю время работы программы при помощи методов из <chrono>

	setlocale(LC_CTYPE, "rus");

	std::thread threads[THREADS];

	std::vector<std::vector<double>>* points; //Вектор точек в куче, так как у него может быть очень большой объем
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

	} while (false); //Одноразовый цикл, созданный для того, чтобы сделать переменную end локальной для данного блока кода

	int outside = points->size();
	for (size_t i = 0; i < points->size(); i++)
	{
		if (isInLesserSphere((*points)[i])) outside--;

	}

	std::cout << "Берем " << DIMENSIONS << " мерную сферу" << std::endl;
	std::cout << "Берем " << points->size() << " точек внутри сферы" << std::endl;
	std::cout << "В поверхностном слое " << 100.0 * (double)outside / (double)points->size() << "% точек" << std::endl;

	delete points;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Прошло " << elapsed.count() << " мс, на "<< THREADS << " потоках\n";
	std::cin.get();
	std::cin.get();
	return 0;
}