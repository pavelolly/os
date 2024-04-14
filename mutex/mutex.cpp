#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <ctime>


void doCalculations(int& n) {
	n++;
}

std::mutex n_mutex;
void doCalculationsMutex(int& n) {
	n_mutex.lock();

	n++;

	n_mutex.unlock();
}

void doCalculationsAtomic(std::atomic<int>& n) {
	n++;
}


#define CALCULATIONS_COUNT 100000
template<typename T>
void doCalculationsMany(void (*func)(T&), T& arg) {
	for (int i = 0; i < CALCULATIONS_COUNT; ++i) {
		func(arg);
	}
}

int main() {
	const int THREADS_COUNT = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(THREADS_COUNT);

	std::cout << "number of threads: " << THREADS_COUNT << "\n";
	std::cout << "number of calculations per thread: " << CALCULATIONS_COUNT << "\n";
	std::cout << "expected value: " << THREADS_COUNT * CALCULATIONS_COUNT << "\n";

	clock_t t1, t2;
	
	t1 = clock();
	int n = 0;
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i] = std::thread(doCalculationsMany<int>, doCalculations, std::ref(n));
	}
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i].join();
	}
	t2 = clock();

	std::cout << "No mutex, no atomic calculations result: " << n << "\n";
	std::cout << "Time: " << (float)(t2 - t1) / CLOCKS_PER_SEC << "s\n\n";

	t1 = clock();
	n = 0;
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i] = std::thread(doCalculationsMany<int>, doCalculationsMutex, std::ref(n));
	}
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i].join();
	}
	t2 = clock();

	std::cout << "Mutex calculations result: " << n << "\n";
	std::cout << "Time: " << (float)(t2 - t1) / CLOCKS_PER_SEC << "s\n\n";

	t1 = clock();
	std::atomic<int> an = 0;
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i] = std::thread(doCalculationsMany<std::atomic<int>>, doCalculationsAtomic, std::ref(an));
	}
	for (int i = 0; i < THREADS_COUNT; ++i) {
		threads[i].join();
	}
	t2 = clock();
	
	std::cout << "Atomic calculations result: " << an << "\n";
	std::cout << "Time: " << (float)(t2 - t1) / CLOCKS_PER_SEC << "s\n\n";

	return 0;
}