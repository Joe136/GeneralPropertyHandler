
#pragma once
#include<windows.h>
#include "resource.h"
#include<fstream>

class DECLSPEC_UUID("3D8CA3B9-E2B8-42B8-A73D-F4CD3B275698") GeneralPropertyHandler;


#ifdef DEBUG
extern std::wofstream g_oLog;
#endif


extern long g_cRefModule;


void DllAddRef();
void DllRelease();

template <class T>
void SafeRelease(T **ppT);