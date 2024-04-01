#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>


HANDLE semaphore1;
HANDLE semaphore2;

void Write() {
    char buffer[256];
    std::ofstream out;
    while (true) {
        std::cout << " >> ";
        std::cin >> buffer;

        out.open("msg");

        out << buffer << std::endl;

        out.close();

        ReleaseSemaphore(semaphore1, 1, NULL);
    }
}

void Read() {
    char buffer[256] = { 0 };
    std::ifstream in;
    while (true) {
        WaitForSingleObject(semaphore1, INFINITE);

        in.open("msg");

        in >> buffer;
        std::cout << "\n > got message: " << buffer << "\n >> ";

        in.close();
    }
}

int main() {
    semaphore1 = CreateSemaphore(NULL, 0, 1, L"Semaphore");
    std::thread WriteThread(Write);
    std::thread ReadThread(Read);

    WriteThread.join();
    ReadThread.join();

    return 0;
}