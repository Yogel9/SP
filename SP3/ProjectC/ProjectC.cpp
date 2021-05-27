// ProjectC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

struct Header
{
	int Size;
	int Address;
};


extern "C"
{
	__declspec(dllexport) void __stdcall dllSendConfirm();

	__declspec(dllimport) int __stdcall dllAcceptCommand();

	__declspec(dllexport) void _stdcall dllWaitMutex();

	__declspec(dllexport) void _stdcall dllReleaseMutex();

	__declspec(dllimport) Header __stdcall dllGetHeader();

	__declspec(dllimport) char* __stdcall dllAcceptMessage(int messageSize);

	

	__declspec(dllimport) void __stdcall dllAddCloseEvent();

	__declspec(dllimport) HANDLE __stdcall dllGetCloseEvent(int threadNumber);

	__declspec(dllexport) void _stdcall dllSetCloseEvent();

	__declspec(dllexport) void _stdcall dllDeleteCloseEvent(int threadNumber);

	__declspec(dllexport) int _stdcall dllGetCloseEventSize();

	__declspec(dllexport) void _stdcall dllAddMessageEvent();

	__declspec(dllexport) HANDLE _stdcall dllGetMessageEvent(int threadNumber);

	__declspec(dllexport) void _stdcall dllSetThreadEventMessage(int threadNumber);

	__declspec(dllexport) void _stdcall dllSetAllThreadEventMessage();

}

int MessageRecipient = -2;
string Message;

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

	dllWaitMutex();
	cout << "Поток " << id  << " создан" << endl;
	dllSendConfirm();

	dllReleaseMutex();

	while (active)
	{
		HANDLE ThreadEvents[2] = { dllGetMessageEvent(id), dllGetCloseEvent(id) };

		int evNum = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE) - WAIT_OBJECT_0;

		switch (evNum) {
		case 0:
		{
			dllWaitMutex();
			//cout << "Поток " << id<< " записал файл." << endl;
			writeToFile(to_string(id), Message);

			dllReleaseMutex();

			dllSendConfirm();

			break;
		}
		case 1:
		{
			active = false;

			dllDeleteCloseEvent(id);

			break;
		}
		default:
		{
			cout << "ERROR" << endl;

			break;
		}
		}

	}


	dllWaitMutex();
	cout << "Поток " << (int)number<< " завершен" << endl;
	dllReleaseMutex();

	dllSendConfirm();

	return 0;
}

void start() {
	cout << "Главный поток работает" << endl;

	dllSendConfirm();

	bool active = true;

	while (active) {

		int newEvent = dllAcceptCommand();
		

		switch (newEvent)
		{
		case 0:
		{
			dllAddCloseEvent();
			dllAddMessageEvent();

			thread t(myThread, (LPVOID)(dllGetCloseEventSize() - 1));
			t.detach();

			dllSendConfirm();
			break;
		}
		case 1:
		{
			if (dllGetCloseEventSize() > 0) {

				dllSetCloseEvent();

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
			const Header descript = dllGetHeader();

			// номер получателя сообщения
			MessageRecipient = descript.Address;

			// текст сообщения
			char* mes = dllAcceptMessage(descript.Size);
			Message = string(mes);
			delete[] mes;

			if (MessageRecipient == -1) {
				writeMessageToConsole(Message);
				dllSetAllThreadEventMessage();

			}
			else if (MessageRecipient == 0) {
				writeMessageToConsole(Message);
			}
			else if (MessageRecipient > 0) {
				dllSetThreadEventMessage(MessageRecipient - 1);
			}
			else {
				cout << "ERROR: Неверно указан получатель!" << endl;
			}
			
			dllSendConfirm();
			break;
		}
		default: {
			break;
		}
		}
	}
	dllSendConfirm();
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
