// MFCLibrary.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CSocket* Socket;
CSocket Server;

extern "C"
{
	_declspec(dllexport)
		void _stdcall dllInit()
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
	}

	_declspec(dllexport)
		bool _stdcall dllCreateServer()
	{
		//TCP almost always uses SOCK_STREAM and UDP uses SOCK_DGRAM.
		return Server.Create(3000);
	}

	_declspec(dllexport)
		void _stdcall dllCloseServer()
	{
		Server.Close();
	}

	_declspec(dllexport)
		void _stdcall dllListenSocket()
	{
		Socket = new CSocket;
		Server.Listen();
		Server.Accept(*Socket);
	}

	_declspec(dllexport)
		void _stdcall dllDenySocket()
	{
		Socket->Close();
		delete Socket;
	}

	_declspec(dllexport)
		int _stdcall dllGetInt()
	{
		int n;
		Socket->Receive(&n, sizeof(int));
		return n;
	}

	_declspec(dllexport)
		void _stdcall dllSendInt(int n)
	{
		Socket->Send(&n, sizeof(int));
	}

	_declspec(dllexport)
		void _stdcall dllSendString(char* str)
	{
		int nLength = strlen(str) + 1;
		Socket->Send(&nLength, sizeof(int));
		Socket->Send(str, nLength);
	}

	_declspec(dllexport)
		char* _stdcall dllGetString()
	{
		int n;
		Socket->Receive(&n, sizeof(int));

		char* h = new char[n];
		Socket->Receive(h, n);
		return h;
	}

	_declspec(dllexport)
		bool _stdcall dllConnectToServer()
	{
		Socket = new CSocket;
		Socket->Create();
		return Socket->Connect("localhost", 3000);
	}

	_declspec(dllexport)
		void _stdcall dllDisconnectFromServer()
	{
		Socket->Close();
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
