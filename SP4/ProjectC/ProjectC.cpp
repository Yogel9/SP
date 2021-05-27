// ProjectC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

extern "C"
{
	__declspec(dllimport) inline void __stdcall dllDisconnectCommandPipe();

	__declspec(dllimport) inline void __stdcall dllDisconnectConfirmPipe();

	_declspec(dllexport) bool _stdcall dllConnectToCommandPipe();

	_declspec(dllexport) bool _stdcall dllConnectToConfirmPipe();

	__declspec(dllimport) void __stdcall dllInitNamedPipe();

	__declspec(dllimport) void __stdcall dllDestroyNamedPipe();

	__declspec(dllimport) inline void __stdcall dllSendInt(int n);

	__declspec(dllimport) inline int __stdcall dllGetInt();

	__declspec(dllimport) inline char* __stdcall dllGetString();

	__declspec(dllimport) inline void __stdcall dllSendConfirm(int n);
}

struct ThreadInfo {
	int number;
	HANDLE closeEvent;
	HANDLE messageEvent;
};

// Мьютекс
HANDLE Mutex = CreateMutex(NULL, FALSE, "mutex");
// Сообщение
string Message;

void writeMessageToConsole(string& Message) {
	cout << "Главный поток получил сообщение:" << endl;
	cout << Message << endl;
}

void writeToFile(string fName, string& message)
{
	ofstream outFile("..\\Debug\\net5.0-windows\\" + fName + ".txt", ios_base::app);

	outFile << message;

	outFile.close();
}


int myThread(ThreadInfo threadInfo) {

	bool active = true;

	WaitForSingleObject(Mutex, INFINITE);
	cout << "Поток " << threadInfo.number << " создан" << endl;
	ReleaseMutex(Mutex);

	while (active)
	{
		HANDLE ThreadEvents[2] = { threadInfo.messageEvent, threadInfo.closeEvent };

		int evNum = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE) - WAIT_OBJECT_0;

		switch (evNum) {
		case 0:
		{
			WaitForSingleObject(Mutex, INFINITE);

			writeToFile(to_string(threadInfo.number), Message);

			ReleaseMutex(Mutex);

			break;
		}
		case 1:
		{
			active = false;

			break;
		}
		default:
		{
			cout << "ERROR: Invalid thread event number" << endl;

			break;
		}
		}

	}

	WaitForSingleObject(Mutex, INFINITE);
	cout << "Поток " << (int)threadInfo.number << " завершен" << endl;
	ReleaseMutex(Mutex);

	return 0;
}

void processEvent(int& newEvent, vector <HANDLE>& CloseEvents, vector <HANDLE>& MessageEvents) {
	switch (newEvent)
	{
	case -1:
	{
		dllSendConfirm(CloseEvents.size());

		break;
	}
	case 0:
	{
		HANDLE handleCloseEvent = CreateEvent(NULL, false, false, NULL);
		CloseEvents.push_back(handleCloseEvent);

		HANDLE handleMessageEvent = CreateEvent(NULL, false, false, NULL);
		MessageEvents.push_back(handleMessageEvent);

		ThreadInfo threadNumber = { CloseEvents.size(), handleCloseEvent, handleMessageEvent };
		/*
		* Создание нового потока
		*/
		thread t(myThread, threadNumber);
		t.detach();

		dllSendConfirm(CloseEvents.size());

		break;
	}
	case 1:
	{
		if (CloseEvents.size() > 0) {

			SetEvent(CloseEvents.back());
			CloseEvents.pop_back();
			MessageEvents.pop_back();
		}

		dllSendConfirm(CloseEvents.size());

		break;
	}
	case 2:
	{
		dllSendConfirm(CloseEvents.size());

		int recipient = dllGetInt();

		dllSendConfirm(CloseEvents.size());

		// текст сообщения
		char* mes = dllGetString();
		Message = string(mes);
		delete[] mes;

		dllSendConfirm(CloseEvents.size());

		if (recipient == -1) {
			for (HANDLE ev : MessageEvents) {
				SetEvent(ev);
			}

			writeMessageToConsole(Message);
		}
		else if (recipient == 0) {
			writeMessageToConsole(Message);
		}
		else if (recipient > 0) {
			SetEvent(MessageEvents[recipient]);
		}
		else {
			cout << "ERROR: Некорректный получатель!" << endl;
		}

		break;
	}
	default: {
		break;
	}
	}
}

void start() {
	cout << "Консольное приложение запущено" << endl;

	vector <HANDLE> CloseEvents;

	vector <HANDLE> MessageEvents;

	while (true) {

		dllInitNamedPipe();

		while (true) {

			if (dllConnectToCommandPipe() != false) {
				int newEvent = dllGetInt();

				//cout << "Новое событие: " << newEvent << endl;

				processEvent(newEvent, CloseEvents, MessageEvents);

				break;
			}

		}

		dllDisconnectCommandPipe();
	}


	for (HANDLE event : CloseEvents) {
		SetEvent(event);
		CloseHandle(event);
	}
}


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
			/////////////////////////////////////////////
			setlocale(LC_ALL, "Russian");
			start();
			/////////////////////////////////////////////
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
