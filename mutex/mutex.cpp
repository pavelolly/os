#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>


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


#define CALCULATIONS_COUNT 10000
template<typename T>
void doCalculationsMany(void (*func)(T&), T& arg) {
	for (int i = 0; i < CALCULATIONS_COUNT; ++i) {
		func(arg);
	}
}

int main() {
	const int threads_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(threads_count);

	std::cout << "number of threads: " << threads_count << "\n";
	std::cout << "expected value: " << threads_count * CALCULATIONS_COUNT << "\n";

	int n = 0;
	for (int i = 0; i < threads_count; ++i) {
		threads[i] = std::thread(doCalculationsMany<int>, doCalculations, std::ref(n));
	}
	for (int i = 0; i < threads_count; ++i) {
		threads[i].join();
	}

	std::cout << "No mutex, no atomic calculations result: " << n << "\n";

	n = 0;
	for (int i = 0; i < threads_count; ++i) {
		threads[i] = std::thread(doCalculationsMany<int>, doCalculationsMutex, std::ref(n));
	}
	for (int i = 0; i < threads_count; ++i) {
		threads[i].join();
	}

	std::cout << "Mutex calculations result: " << n << "\n";

	std::atomic<int> an = 0;
	for (int i = 0; i < threads_count; ++i) {
		threads[i] = std::thread(doCalculationsMany<std::atomic<int>>, doCalculationsAtomic, std::ref(an));
	}
	for (int i = 0; i < threads_count; ++i) {
		threads[i].join();
	}
	
	std::cout << "Atomic calculations result: " << an << "\n";

	return 0;
}