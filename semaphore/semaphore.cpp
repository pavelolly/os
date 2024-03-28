
#include <iostream>
#include <fstream>
#include <thread>
#include <semaphore>

std::binary_semaphore semaphore(0);

void readLoop() {
    while (true) {
        // std::cout << "[INFO] Entered read loop\n" << std::flush;
        semaphore.acquire();
        // std::cout << "[INFO] Acqiured from read loop\n" << std::flush;

        char buf[256] = { 0 };
        std::ifstream in("msg");

        in.read(buf, 256);
        in.close();

        std::cout << buf << std::endl;
    }
}

int main() {
    std::thread thread_read(readLoop);

    while (true) {
        // std::cout << "[INFO] Entered write loop\n" << std::flush;

        char buf[256] = { 0 };
        std::ofstream out("msg");

        std::cin >> buf;

        out << buf;
        out.close();

        semaphore.release();
        // std::cout << "[INFO] Released from write loop\n" << std::flush;
    }

    return 0;
}