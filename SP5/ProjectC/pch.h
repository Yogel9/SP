// pch.h: это предварительно скомпилированный заголовочный файл.
// Перечисленные ниже файлы компилируются только один раз, что ускоряет последующие сборки.
// Это также влияет на работу IntelliSense, включая многие функции просмотра и завершения кода.
// Однако изменение любого из приведенных здесь файлов между операциями сборки приведет к повторной компиляции всех(!) этих файлов.
// Не добавляйте сюда файлы, которые планируете часто изменять, так как в этом случае выигрыша в производительности не будет.

#ifndef PCH_H
#define PCH_H

#define _AFXDLL 1 in stdafx.h
#pragma comment(lib, "D:\\Users\\SP5\\Debug\\netcoreapp3.1\\MFCLibrary.lib")

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

#include "synchapi.h"
#include "WinBase.h"

using namespace std;

#endif //PCH_H
