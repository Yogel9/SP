// MFCLibrary.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Header
{
	int Size;
	int Address;
};

const BYTE SIZE_HEADER = sizeof(Header);
const BYTE INT_SIZE = sizeof(int);

PROCESS_INFORMATION processInformation;

// Событие отправки команды
HANDLE commandWrite = GetStdHandle(STD_OUTPUT_HANDLE);
// Событие получения команды
HANDLE commandRead = GetStdHandle(STD_INPUT_HANDLE);
// Событие отправки команды
HANDLE confirmWrite = GetStdHandle(STD_ERROR_HANDLE);
// Событие получения команды
HANDLE confirmRead = GetStdHandle(STD_INPUT_HANDLE);
// Мьютекс
HANDLE Mutex = CreateMutex(NULL, FALSE, "mutex");;

// Вектор всех событий
HANDLE DialogEvents[5];

using namespace std;

// Вектор всех закрывающих событий
vector<HANDLE> CloseEvents;

// Вектор всех событий сообщений
vector<HANDLE> MessageEvents;

extern "C"
{
	__declspec(dllexport)
		void __stdcall dllSendCommand(int commandId)
	{
		DWORD dwWrite;
		WriteFile(commandWrite, &commandId, INT_SIZE, &dwWrite, nullptr);
	}

	__declspec(dllexport)
		void __stdcall dllSendConfirm()
	{
		DWORD dwWrite;
		int commandId = 200; // ОК
		WriteFile(confirmWrite, &commandId, INT_SIZE, &dwWrite, nullptr);
	}

	__declspec(dllexport)
		int __stdcall dllAcceptCommand()
	{

		DWORD dwRead;

		int commandId;

		while (true) {
			if (ReadFile(commandRead, &commandId, INT_SIZE, &dwRead, nullptr)) {

				break;
			}
		}

		return commandId;
	}

	__declspec(dllexport)
		int __stdcall dllAcceptConfirm()
	{

		DWORD dwRead;

		int confirmdId;

		while (true) {
			if (ReadFile(confirmRead, &confirmdId, INT_SIZE, &dwRead, nullptr)) {

				break;
			}
		}

		return confirmdId;
	}

	__declspec(dllexport)
		Header __stdcall dllGetHeader()
	{
		Header descript;
		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
		const int MAXLEN = SIZE_HEADER;
		char buff[MAXLEN];
		while (true) {
			DWORD dwRead;
			if (ReadFile(hIn, buff, MAXLEN, &dwRead, nullptr)) {
				break;
			}
		}
		memcpy(&descript, buff, SIZE_HEADER);
		return descript;
	}

	__declspec(dllexport)
		char* __stdcall dllAcceptMessage(int messageSize)
	{
		HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
		DWORD dwRead;
		char* buff = new char[messageSize];
		if (!ReadFile(input, buff, messageSize, &dwRead, nullptr) || !dwRead)
		{
			return NULL;
		}
		return buff;
	}

	_declspec(dllexport)
		void _stdcall dllSendMessage(char* pStr = " ", int threadNumber = -1)
	{
		DWORD dwWrite;
		Header descr;
		descr.Size = strlen(pStr) + 1;
		descr.Address = threadNumber;
		WriteFile(commandWrite, &descr, SIZE_HEADER, &dwWrite, nullptr);
		WriteFile(commandWrite, pStr, descr.Size, &dwWrite, nullptr);
	}

	_declspec(dllexport)
		void  _stdcall dllStartProcess()                                                                                                                                  //Init()
	{
		//механизм анонимных каналов
		SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
		CreatePipe(&commandRead, &commandWrite, &sa, 0);//создаёт 2 объекта которые возвращаются через handle
		CreatePipe(&confirmRead, &confirmWrite, &sa, 0);
		SetHandleInformation(commandWrite, HANDLE_FLAG_INHERIT, 0);
		
		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = commandRead;
		si.hStdError = confirmWrite;
		//создание консоли
		CreateProcess(NULL, (LPSTR)("ProjectC.exe"), &sa, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &processInformation);
	}

	_declspec(dllexport)
		bool  _stdcall isProcessActive()
	{
		DWORD dwExitCode;
		if (GetExitCodeProcess(processInformation.hProcess, &dwExitCode) != 0)
		{
			return dwExitCode == STILL_ACTIVE;
		}
	}

	__declspec(dllexport)
		void _stdcall dllWaitMutex()
	{

		WaitForSingleObject(Mutex, INFINITE);
	}

	__declspec(dllexport)
		void _stdcall dllReleaseMutex()
	{

		ReleaseMutex(Mutex);
	}

	// добавляем событие закрытия для нового потока
	__declspec(dllexport) void _stdcall dllAddCloseEvent()
	{

		CloseEvents.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
	}

	// получаем событие закрытия
	__declspec(dllexport) HANDLE _stdcall dllGetCloseEvent(int threadNumber)
	{

		return CloseEvents[threadNumber];
	}

	__declspec(dllexport) int _stdcall dllGetCloseEventSize()
	{

		return CloseEvents.size();
	}


	// устанавливаем событие закрытия
	__declspec(dllexport) void _stdcall dllSetCloseEvent()
	{

		SetEvent(CloseEvents[dllGetCloseEventSize() - 1]);
	}

	// удаляем событие закрытия
	__declspec(dllexport) void _stdcall dllDeleteCloseEvent(int threadNumber)
	{

		CloseHandle(CloseEvents[threadNumber]);

		CloseEvents.pop_back();
	}

	// добавляем событие сообщения
	__declspec(dllexport) void _stdcall dllAddMessageEvent()
	{

		MessageEvents.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
	}

	// получаем текущее событие о сообщении
	__declspec(dllexport) HANDLE _stdcall dllGetMessageEvent(int threadNumber)
	{

		return MessageEvents[threadNumber];
	}
	// событие о сообщении для конкретного потока
	__declspec(dllexport) void _stdcall dllSetThreadEventMessage(int threadNumber)
	{
		SetEvent(MessageEvents[threadNumber]);
	}
	// событие о широковещательном потоке
	__declspec(dllexport) void _stdcall dllSetAllThreadEventMessage()
	{

		for (int i = 0; i < MessageEvents.size(); i++)
		{
			dllSetThreadEventMessage(i);
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

	return TRUE;
}
