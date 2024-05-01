#include <iostream>
#include <fstream>
#include <thread>
#include <assert.h>
#include <Windows.h>


HANDLE semaphore1;
HANDLE semaphore2;

namespace task1 {

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

}

namespace task2 {

HANDLE hFile;
HANDLE hMapFile;
char* map_view;

void Write(int proccess_number) {
	char buffer[256];

	while (true) {
		std::cout << " >> ";
		std::cin.getline(buffer, 256);

		strcpy_s(map_view, 256, buffer);

		if (proccess_number == 1) {
			ReleaseSemaphore(semaphore1, 1, NULL);
		}
		else {
			ReleaseSemaphore(semaphore2, 1, NULL);
		}
	}
}

void Read(int proccess_number) {
	char buffer[256];

	while (true) {
		if (proccess_number == 1) {
			WaitForSingleObject(semaphore2, INFINITE);
		}
		else {
			WaitForSingleObject(semaphore1, INFINITE);
		}

		std::cout << "\n > got message: " << map_view << "\n >> ";
	}
}

}

namespace task3 {

char* shared_memory;

void Write(int proccess_number) {
	char buffer[256];

	while (true) {
		std::cout << " >> ";
		std::cin.getline(buffer, 256);

		strcpy_s(shared_memory, 256, buffer);

		if (proccess_number == 1) {
			ReleaseSemaphore(semaphore1, 1, NULL);
		}
		else {
			ReleaseSemaphore(semaphore2, 1, NULL);
		}
	}
}

void Read(int proccess_number) {
	char buffer[256];

	while (true) {
		if (proccess_number == 1) {
			WaitForSingleObject(semaphore2, INFINITE);
		}
		else {
			WaitForSingleObject(semaphore1, INFINITE);
		}

		std::cout << "\n > got message: " << shared_memory << "\n >> ";
	}
}

}

PROCESS_INFORMATION runUrself(const wchar_t* _cmd) {
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

int main(int argc, char* argv[]) {
	assert(argc <= 2);

	if (argc == 1) {
		PROCESS_INFORMATION process1 = runUrself(L"semaphore.exe 1");
		PROCESS_INFORMATION process2 = runUrself(L"semaphore.exe 2");

		WaitForSingleObject(process1.hProcess, INFINITE);
		WaitForSingleObject(process2.hProcess, INFINITE);

		CloseHandle(process1.hProcess);
		CloseHandle(process1.hThread);

		CloseHandle(process2.hProcess);
		CloseHandle(process2.hThread);
	}
	else {
		int proccess_number = atoi(argv[1]);
		assert(proccess_number == 1 || proccess_number == 2);

		std::cout << "PROCESS: " << proccess_number << std::endl;

		// task 1, 2, 3
		semaphore1 = CreateSemaphore(NULL, 0, 1, L"S1");
		semaphore2 = CreateSemaphore(NULL, 0, 1, L"S2");

		// task 2
		task2::hFile = CreateFileW(L"msg", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		assert(task2::hFile != INVALID_HANDLE_VALUE);

		task2::hMapFile = CreateFileMappingW(task2::hFile, NULL, PAGE_READWRITE, 0, 256, L"Map");
		assert(task2::hMapFile);

		task2::map_view = (char*)MapViewOfFile(task2::hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		assert(task2::map_view);

		// task3
		HANDLE shared_memory_handle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 256, L"Shared memmory");
		assert(shared_memory_handle);

		task3::shared_memory = (char*)MapViewOfFile(shared_memory_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		assert(task3::shared_memory);

		// выбор задания 1, 2, 3 через namespace
		using namespace task3;

		std::thread WriteThread(Write, proccess_number);
		std::thread ReadThread(Read, proccess_number);

		WriteThread.join();
		ReadThread.join();
	}

	return 0;
}