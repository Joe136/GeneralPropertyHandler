
#include "StdAfx.h"
#include<propsys.h>



long g_cRefModule = 0L;


#ifdef DEBUG
std::wofstream g_oLog;
#endif


void DllAddRef()
{
    InterlockedIncrement(&g_cRefModule);
}



void DllRelease()
{
    InterlockedDecrement(&g_cRefModule);
}



template <class T>
void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

template void SafeRelease<IStream> (IStream**);
template void SafeRelease<IPropertyStoreCache> (IPropertyStoreCache**);
