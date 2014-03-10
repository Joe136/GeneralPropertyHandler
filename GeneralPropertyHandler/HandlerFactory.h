
#pragma once

#include<unknwn.h>
#include<shlwapi.h>
#include "StdAfx.h"



class HandlerFactory : public IClassFactory
{
public:
   HandlerFactory (PFNCREATEINSTANCE pfnCreate) : _cRef(1), _pfnCreate(pfnCreate) {
      DllAddRef();
   }

protected:
   ~HandlerFactory() {
      DllRelease();
   }


public:
   // IUnknown
   IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
      static const QITAB qit[] =
      {
         QITABENT(HandlerFactory, IClassFactory),
         { 0 }
      };

      return QISearch(this, qit, riid, ppv);
   }



   IFACEMETHODIMP_(ULONG) AddRef() {
      return InterlockedIncrement(&_cRef);
   }



   IFACEMETHODIMP_(ULONG) Release() {
      long cRef = InterlockedDecrement(&_cRef);
      if (cRef == 0)
      {
         delete this;
      }
      return cRef;
   }



   // IClassFactory
   IFACEMETHODIMP CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv) {
      return punkOuter ? CLASS_E_NOAGGREGATION : _pfnCreate(riid, ppv);
   }



   IFACEMETHODIMP LockServer(BOOL fLock) {
      if (fLock) {
         DllAddRef();
      } else {
         DllRelease();
      }

      return S_OK;
   }


private:
   long _cRef;
   PFNCREATEINSTANCE _pfnCreate;

};
