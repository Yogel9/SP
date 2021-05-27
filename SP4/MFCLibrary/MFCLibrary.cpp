// MFCLibrary.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


HANDLE CommandPipe;
HANDLE ConfirmPipe;

extern "C"
{
	_declspec(dllexport)
		bool _stdcall dllConnectToCommandPipe() {
		return ConnectNamedPipe(CommandPipe, NULL);
	}

	_declspec(dllexport)
		bool _stdcall dllConnectToConfirmPipe() {
		return ConnectNamedPipe(ConfirmPipe, NULL);
	}

	_declspec(dllexport)
		void _stdcall dllDisconnectCommandPipe()
	{
		DisconnectNamedPipe(CommandPipe);
	}

	_declspec(dllexport)
		void _stdcall dllDisconnectConfirmPipe()
	{
		DisconnectNamedPipe(CommandPipe);
	}

	_declspec(dllexport)
		inline void _stdcall dllConnectToServer() {
		if (WaitNamedPipe("\\\\.\\pipe\\CommandPipe", 5000) && WaitNamedPipe("\\\\.\\pipe\\ConfirmPipe", 5000))
		{
			CommandPipe = CreateFile("\\\\.\\pipe\\CommandPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			ConfirmPipe = CreateFile("\\\\.\\pipe\\ConfirmPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		}
	}

	_declspec(dllexport)
		inline void _stdcall dllDisconnectFromServer() {
		CloseHandle(CommandPipe);
		CloseHandle(ConfirmPipe);
	}

	_declspec(dllexport)
		inline void _stdcall dllSendInt(int n) {

		DWORD dwDone;
		WriteFile(CommandPipe, &n, sizeof(int), &dwDone, NULL);
		FlushFileBuffers(CommandPipe);
	}

	_declspec(dllexport)
		inline int _stdcall dllGetInt() {
		DWORD dwDone;
		int n;

		ReadFile(CommandPipe, &n, sizeof(int), &dwDone, NULL);

		return n;
	}

	_declspec(dllexport)
		inline void _stdcall dllSendString(char* str) {
		DWORD dwDone;
		int n = strlen(str) + 1;
		WriteFile(CommandPipe, &n, sizeof(n), &dwDone, NULL);
		WriteFile(CommandPipe, str, n, &dwDone, NULL);
		FlushFileBuffers(CommandPipe);
	}

	_declspec(dllexport)
		inline char* _stdcall dllGetString() {
		DWORD dwDone;
		int n;

		ReadFile(CommandPipe, &n, sizeof(int), &dwDone, NULL);

		char* h = new char[n];
		ReadFile(CommandPipe, h, n, &dwDone, NULL);
		return h;
	}

	__declspec(dllexport)
		inline void _stdcall dllSendConfirm(int n) {
		DWORD dwDone;

		WriteFile(ConfirmPipe, &n, sizeof(int), &dwDone, NULL);
		FlushFileBuffers(ConfirmPipe);
	}

	_declspec(dllexport)
		inline int _stdcall dllWaitConfirm() {
		DWORD dwDone;
		int n;

		ReadFile(ConfirmPipe, &n, sizeof(int), &dwDone, NULL);
		return n;
	}

	_declspec(dllexport)
		void _stdcall dllInitNamedPipe() {
		CommandPipe = CreateNamedPipe("\\\\.\\pipe\\CommandPipe",
			PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
			1024, 1024, 0, NULL);

		ConfirmPipe = CreateNamedPipe("\\\\.\\pipe\\ConfirmPipe",
			PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
			1024, 1024, 0, NULL);
	}

	_declspec(dllexport)
		void _stdcall dllDestroyNamedPipe() {
		CloseHandle(CommandPipe);
		CloseHandle(ConfirmPipe);
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
