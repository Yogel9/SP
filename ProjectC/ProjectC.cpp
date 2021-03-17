// ProjectC.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "ProjectC.h"
#include "vector"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// Единственный объект приложения
CWinApp theApp;

using namespace std;

HANDLE hMutex;
//HANDLE hEventClose;
vector<HANDLE> vEventClose;

//потоковая функция
DWORD WINAPI MyThread(LPVOID lpParameter)
{
	int ID = (int)lpParameter;

	WaitForSingleObject(hMutex, INFINITE);
	cout << "Поток " << ID << " создан" << endl;
	ReleaseMutex(hMutex);

	WaitForSingleObject(vEventClose[(int)lpParameter], INFINITE);

	WaitForSingleObject(hMutex, INFINITE);
	cout << "Поток " << ID << " завершен" << endl;
	ReleaseMutex(hMutex);

	return 0;
}

void start()
{
	cout << "Главный поток работает" << endl;
	//Безопасность/Владение мьютексом/Имя
	hMutex = CreateMutex(NULL, FALSE, "MutexAL");
	HANDLE hEvents[4];
	//Безопасность/Автоматический сброс/Стартовоесостояние/Имя
	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, "EvStart");
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "EvStop");
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "EvExit");
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "EvConfirm");
	int i = 0;//ID потока
	bool exit = false;

	while (!exit)
	{
		// Ожидание события           /Кол-во ожидаемых объектов/Массив с объектами/FALSE-свободно/время - номер  освободившегося объекта
		switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
		{
		    case 0:
			{
			HANDLE hEventClose = CreateEvent(NULL, TRUE, FALSE, NULL);
			vEventClose.push_back(hEventClose);
			//Безопасность/Размер стека/Указатель на потоковую ф./параметры для ф./флаг создания/индикатор потока
			CreateThread(NULL, 0, MyThread, (LPVOID)i++, 0, NULL);
			break;
			}
			case 1:
			{
				if (!vEventClose.empty())// Проверка на существования потоков
				{
				PulseEvent(vEventClose[--i]);
				CloseHandle(vEventClose[i]);
				vEventClose.pop_back();//удаляем последний элемент в векторе 
				break;
				}
			}
			case 2:
			{
			cout << "Выход" << endl;
			exit = true;
			break;
			}
		} 
		SetEvent(hEvents[3]);
	}

	//закрытие всех объектов ядра
	for (auto event : vEventClose)
	{
		SetEvent(event);
		CloseHandle(event);
	}
	for (auto event : hEvents)
	{
		CloseHandle(event);
	}
	CloseHandle(hMutex);
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
            setlocale(LC_ALL, "Russian");
            start();
            // TODO: вставьте сюда код для приложения.
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
