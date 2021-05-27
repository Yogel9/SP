// ProjectC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

extern "C"
{
	_declspec(dllexport) void _stdcall dllInit();

	_declspec(dllexport) bool _stdcall dllCreateServer();

	_declspec(dllexport) void _stdcall dllCloseServer();

	_declspec(dllexport) void _stdcall dllListenSocket();

	_declspec(dllexport) void _stdcall dllDenySocket();

	_declspec(dllexport) void _stdcall dllSendInt(int n);

	__declspec(dllimport) int __stdcall dllGetInt();

	__declspec(dllimport) char* __stdcall dllGetString();

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
	ofstream outFile("..\\Debug\\netcoreapp3.1\\" + fName + ".txt", ios_base::app);

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
		dllSendInt(CloseEvents.size());

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

		dllSendInt(CloseEvents.size());

		break;
	}
	case 1:
	{
		if (CloseEvents.size() > 0) {

			SetEvent(CloseEvents.back());
			CloseEvents.pop_back();
			MessageEvents.pop_back();
		}

		dllSendInt(CloseEvents.size());

		break;
	}
	case 2:
	{
		dllSendInt(CloseEvents.size());

		int recipient = dllGetInt();

		dllSendInt(CloseEvents.size());

		// текст сообщения
		char* mes = dllGetString();
		Message = string(mes);
		delete[] mes;

		dllSendInt(CloseEvents.size());

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

	vector <HANDLE> CloseEvents;

	vector <HANDLE> MessageEvents;

	cout << "Server is listening on port 3000 ..." << endl;

	while (true) {

		dllListenSocket();

		int newEvent = dllGetInt();

		//cout << "Новое событие: " << newEvent << endl;

		processEvent(newEvent, CloseEvents, MessageEvents);

		dllDenySocket();
	}

	dllCloseServer();


	for (HANDLE event : CloseEvents) {
		SetEvent(event);
		CloseHandle(event);
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");

	dllInit();

	dllCreateServer();

	start();

	dllCloseServer();
}
