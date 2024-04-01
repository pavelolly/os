#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>


HANDLE semaphore;

void Write() {
    char buffer[256];
    while (true) {
        std::cin >> buffer;
        std::cout << " > got input: " << buffer << std::endl;
        ReleaseSemaphore(semaphore, 1, NULL);
    }
}

void Read() {
    while (true) {
        WaitForSingleObject(semaphore, INFINITE);
        std::cout << " > fetched data from file" << std::endl;
    }
}

int main() {
    semaphore = CreateSemaphore(NULL, 0, 1, L"Semaphore");
    std::thread WriteThread(Write);
    std::thread ReadThread(Read);

    WriteThread.join();
    ReadThread.join();

    return 0;
}