// MFCLibrary.cpp: определяет процедуры инициализации для библиотеки DLL.



#include "pch.h"
#include "framework.h"
#include "MFCLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FILE_NAME "MemoryFile"

using namespace std;

struct Header
{
	int Size;
	int Address;
};

const BYTE SIZE_HEADER = sizeof(Header);


Header GetHeader()
{
	
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE_HEADER, FILE_NAME);
	BYTE* pBuf = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SIZE_HEADER);

	Header h;
	memcpy(&h, pBuf, SIZE_HEADER);

	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMap);
	return h;
}

// Мьютекс
HANDLE Mutex;

// Вектор всех событий
HANDLE vEvents[6];

// Вектор всех закрывающих событий
vector<HANDLE> CloseEvents;

// Вектор всех событий сообщений
vector<HANDLE> MessageEvents;



extern "C"
{
	// начало работы
	__declspec(dllexport)
		void __stdcall DllBegin()
	{
		vEvents[0] = CreateEvent(NULL, FALSE, FALSE, "event_start");
		vEvents[1] = CreateEvent(NULL, FALSE, FALSE, "event_stop");
		vEvents[2] = CreateEvent(NULL, FALSE, FALSE, "event_quit");
		vEvents[3] = CreateEvent(NULL, FALSE, FALSE, "event_message");
		vEvents[4] = CreateEvent(NULL, FALSE, FALSE, "event_confirm");
		vEvents[5] = CreateEvent(NULL, FALSE, FALSE, "message_confirm");

		Mutex = CreateMutex(NULL, FALSE, "mutex");
	}

	//Окончание работы программы закрытие всех handle'ов
	__declspec(dllexport)
		void __stdcall DllEnd()
	{
		for (HANDLE ev : vEvents) {
			CloseHandle(ev);
		}

		for (HANDLE ev : CloseEvents) {
			CloseHandle(ev);
		}

		for (HANDLE ev : MessageEvents) {
			CloseHandle(ev);
		}
	}

	// Установить событие "Старт"
	__declspec(dllexport)
		void __stdcall DllStart()
	{
		SetEvent(vEvents[0]);
	}

	// Установить событие "Стоп"
	__declspec(dllexport)
		void __stdcall DllStop()
	{

		SetEvent(vEvents[1]);
	}

	// Установить событие "Выход"
	__declspec(dllexport)
		void __stdcall DllQuit()
	{
		SetEvent(vEvents[2]);
	}

	__declspec(dllexport) void _stdcall DllMessage()
	{
		SetEvent(vEvents[3]);
	}

	// Установить событие "Выполнено"
	__declspec(dllexport) void __stdcall DllConfirm()
	{
		SetEvent(vEvents[4]);
	}

	// Установить событие "Выполнено"
	__declspec(dllexport) void __stdcall DllMessageConfirm()
	{
		SetEvent(vEvents[5]);
	}

	__declspec(dllexport) void _stdcall waitMutex()
	{
		WaitForSingleObject(Mutex, INFINITE);
	}

	__declspec(dllexport) void _stdcall releaseMutex()
	{
		ReleaseMutex(Mutex);
	}

	// ожидание "рабочих" событий
	__declspec(dllexport) int __stdcall DllWaitEvents()
	{
		int numEv = WaitForMultipleObjects(4, vEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		return numEv;
	}

	// ожидание события подтверждения
	__declspec(dllexport) bool __stdcall DllWaitConfirm()
	{
		WaitForSingleObject(vEvents[4], INFINITE);
		return true;
	}

	// ожидание события подтверждения
	__declspec(dllexport) bool __stdcall DllWaitMessageConfirm()
	{
		WaitForSingleObject(vEvents[5], INFINITE);
		return true;
	}



	// Чтение сообщения
	__declspec(dllexport)
		char* __stdcall DllAcceptMessage(int& receiving)
	{
		Header h = GetHeader();
		receiving = h.Address;

		HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE_HEADER + h.Size, FILE_NAME);
		BYTE* pByte = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SIZE_HEADER + h.Size);

		char* message = new char[h.Size];
		memcpy(message, (char*)pByte + SIZE_HEADER, h.Size);

		UnmapViewOfFile(pByte);

		DllMessageConfirm();
		CloseHandle(hFileMap);

		return message;
	}
	// Отправить сообщение
	__declspec(dllexport)
		void __stdcall DllSendMessage(char* message, int adress)
	{
		Header h;
		h.Size = strlen(message) + 1;
		h.Address = adress;

		HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE_HEADER + h.Size, FILE_NAME);
		BYTE* pByte = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SIZE_HEADER + h.Size);

		memcpy(pByte, &h, SIZE_HEADER);
		memcpy(pByte + SIZE_HEADER, message, h.Size);

		DllMessage();
		DllWaitMessageConfirm();


		UnmapViewOfFile(pByte);
		CloseHandle(hFileMap);
	}

	// добавляем событие закрытия для нового потока
	__declspec(dllexport) void _stdcall DllAddCloseEvent()
	{

		CloseEvents.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
	}

	// получаем событие закрытия
	__declspec(dllexport) HANDLE _stdcall DllGetCloseEvent(int threadNumber)
	{

		return CloseEvents[threadNumber];
	}

	__declspec(dllexport) int _stdcall DllGetCloseEventSize()
	{

		return CloseEvents.size();
	}


	// устанавливаем событие закрытия
	__declspec(dllexport) void _stdcall DllSetCloseEvent()
	{

		SetEvent(CloseEvents[DllGetCloseEventSize() - 1]);
	}

	// удаляем событие закрытия
	__declspec(dllexport) void _stdcall DllDeleteCloseEvent(int threadNumber)
	{

		CloseHandle(CloseEvents[threadNumber]);

		CloseEvents.pop_back();
	}

	// добавляем событие сообщения
	__declspec(dllexport) void _stdcall DllAddMessageEvent()
	{

		MessageEvents.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
	}

	// получаем текущее событие о сообщении
	__declspec(dllexport) HANDLE _stdcall DllGetMessageEvent(int threadNumber)
	{

		return MessageEvents[threadNumber];
	}
	// событие о сообщении для конкретного потока
	__declspec(dllexport) void _stdcall DllSetThreadEventMessage(int threadNumber)
	{
		SetEvent(MessageEvents[threadNumber]);
	}
	// событие о широковещательном потоке
	__declspec(dllexport) void _stdcall DllSetAllThreadEventMessage()
	{

		for (int i = 0; i < MessageEvents.size(); i++)
		{
			DllSetThreadEventMessage(i);
		}
	}
}
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CMFCLibraryApp

BEGIN_MESSAGE_MAP(CMFCLibraryApp, CWinApp)
END_MESSAGE_MAP()


// Создание CMFCLibraryApp

CMFCLibraryApp::CMFCLibraryApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CMFCLibraryApp

CMFCLibraryApp theApp;


// Инициализация CMFCLibraryApp

BOOL CMFCLibraryApp::InitInstance()
{
	CWinApp::InitInstance();

	setlocale(LC_ALL, "Russian");
	return TRUE;
}
