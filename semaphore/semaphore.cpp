#include <iostream>
#include <fstream>
#include <thread>
#include <assert.h>
#include <Windows.h>


HANDLE semaphore1;
HANDLE semaphore2;

void Write(int proccess_number) {
    char buffer[256];
    std::ofstream out;
    while (true) {
        std::cout << " >> ";
        std::cin.getline(buffer, 256);

        out.open("msg");
        out << buffer << std::endl;
        out.close();

        if (proccess_number == 1) {
            ReleaseSemaphore(semaphore1, 1, NULL);
        }
        else {
            ReleaseSemaphore(semaphore2, 1, NULL);
        }
    }
}

void Read(int proccess_number) {
    char buffer[256] = { 0 };
    std::ifstream in;
    while (true) {
        if (proccess_number == 1) {
            WaitForSingleObject(semaphore2, INFINITE);
        }
        else {
            WaitForSingleObject(semaphore1, INFINITE);
        }

        in.open("msg");
        in.getline(buffer, 256);
        std::cout << "\n > got message: " << buffer << "\n >> ";
        in.close();
    }
}

PROCESS_INFORMATION runUrself(const wchar_t *_cmd) {
    STARTUPINFO startupinfo;
    ZeroMemory(&startupinfo, sizeof(startupinfo));
    startupinfo.cb = sizeof(startupinfo);

    PROCESS_INFORMATION processinfo;
    ZeroMemory(&processinfo, sizeof(processinfo));

    wchar_t cmd[256];
    wcscpy_s(cmd, _cmd);

    bool success = CreateProcessW(L"C:\\Users\\km\\source\\repos\\pavelolly\\os\\semaphore\\x64\\Debug\\semaphore.exe", cmd,
        NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &startupinfo, &processinfo);

    assert(success);

    return processinfo;
}

int main(int argc, char *argv[]) {
    assert(argc <= 2);

    if (argc == 1) {
        PROCESS_INFORMATION process1 = runUrself(L"semaphore.exe 1");
        PROCESS_INFORMATION process2 = runUrself(L"semaphore.exe 2");

        WaitForSingleObject(process1.hProcess, INFINITE);
        WaitForSingleObject(process2.hProcess, INFINITE);
    }
    else {
        int proccess_number = atoi(argv[1]);
        assert(proccess_number == 1 || proccess_number == 2);

        semaphore1 = CreateSemaphore(NULL, 0, 1, L"S1");
        semaphore2 = CreateSemaphore(NULL, 0, 1, L"S2");

        std::thread WriteThread(Write, proccess_number);
        std::thread ReadThread(Read, proccess_number);

        WriteThread.join();
        ReadThread.join();
    }

    return 0;
}