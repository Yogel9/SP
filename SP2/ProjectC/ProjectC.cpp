// ProjectC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
//#include "framework.h"


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// текст сообщения
string Message;

extern "C"
{

	__declspec(dllimport) void  __stdcall DllBegin();

	__declspec(dllimport) void  __stdcall DllEnd();

	__declspec(dllexport) void _stdcall waitMutex();

	__declspec(dllexport) void _stdcall releaseMutex();

	__declspec(dllimport) int   __stdcall DllWaitEvents();

	__declspec(dllimport) void __stdcall DllAddCloseEvent();

	__declspec(dllimport) HANDLE __stdcall DllGetCloseEvent(int threadNumber);

	__declspec(dllexport) void _stdcall DllSetCloseEvent();

	__declspec(dllexport) void _stdcall DllDeleteCloseEvent(int threadNumber);

	__declspec(dllexport) int _stdcall DllGetCloseEventSize();

	__declspec(dllexport) void _stdcall DllAddMessageEvent();

	__declspec(dllexport) HANDLE _stdcall DllGetMessageEvent(int threadNumber);

	__declspec(dllimport) void  __stdcall DllConfirm();

	__declspec(dllimport) char* __stdcall DllAcceptMessage(int& recipient);

	__declspec(dllexport) void _stdcall DllSetThreadEventMessage(int threadNumber);

	__declspec(dllexport) void _stdcall DllSetAllThreadEventMessage();
}

void writeMessageToConsole(string& Message) {
	cout << "Главный поток получил сообщение:" << endl;
	cout << Message << endl;
}

void writeToFile(string fName, string& message)
{
	ofstream outFile(fName + ".txt");

	outFile << message;

	outFile.close();
}


UINT myThread(LPVOID number) {
	bool active = true;

	int id = (int)number;

	waitMutex();
	cout << "Поток " << id  << " создан" << endl;
	DllConfirm();

	releaseMutex();

	while (active)
	{
		HANDLE ThreadEvents[2] = { DllGetMessageEvent(id), DllGetCloseEvent(id) };

		int evNum = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE) - WAIT_OBJECT_0;

		switch (evNum) {
		case 0:
		{
			waitMutex();
			//cout << "Поток " << id<< " записал файл." << endl;
			writeToFile(to_string(id), Message);

			releaseMutex();

			break;
		}
		case 1:
		{
			active = false;

			DllDeleteCloseEvent(id);

			break;
		}
		}

	}


	waitMutex();
	cout << "Поток " << (int)number<< " завершен" << endl;
	releaseMutex();

	DllConfirm();

	return 0;
}

void start() {
	cout << "Главный поток работает" << endl;

	DllConfirm();

	bool active = true;

	while (active) {

		// Ожидаем новое событие из C# приложения
		int newEvent = DllWaitEvents();

		switch (newEvent)
		{
		case 0:
		{
			DllAddCloseEvent();
			DllAddMessageEvent();

			AfxBeginThread(myThread, (LPVOID)(DllGetCloseEventSize() - 1));

			break;
		}
		case 1:
		{
			if (DllGetCloseEventSize() > 0) {

				DllSetCloseEvent();

				break;
			}
		}
		case 2:
		{
			cout << "Главный поток закрывается" << endl;

			active = false;

			break;
		}
		case 3: {

			// номер получателя сообщения
			int MessageRecipient = -2;

			// текст сообщения
			char* mes = DllAcceptMessage(MessageRecipient);
			Message = string(mes);
			delete[] mes;

			if (MessageRecipient == -1) {
				writeMessageToConsole(Message);
				DllSetAllThreadEventMessage();

			}
			else if (MessageRecipient == 0) {
				writeMessageToConsole(Message);
			}
			else if (MessageRecipient > 0) {
				DllSetThreadEventMessage(MessageRecipient - 1);
			}
			else {
				cout << "Неверно указан получатель!" << endl;
			}

			break;
		}
		default: {
			break;
		}
		}

		DllConfirm();
	}

	DllConfirm();

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

			DllBegin();

			start();

			DllEnd();

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
