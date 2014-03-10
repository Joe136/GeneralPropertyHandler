
#pragma once

//---------------------------Includes----------------------------------------------//
#include "StdAfx.h"
#include<propsys.h>
#include<propkey.h>
#include<Shlwapi.h>
#include<Propvarutil.h>



//---------------------------Defines-----------------------------------------------//
//class DECLSPEC_UUID("???????????????") ConcreteHandler;
#define H_ASSERT(HR)                    if (FAILED(HR) )  return HR;
#define H_ASSERT_(HR) { HRESULT HR_ = HR; if (FAILED(HR_) ) return HR_; }



//---------------------------Class IPropertyHandler--------------------------------//
template<class ConcreteHandler>
class IPropertyHandler : public IPropertyStore, public IInitializeWithStream
{
public:
   IPropertyHandler () : m_cRef(1), m_grfMode(0), m_pStream(NULL), m_pCache(NULL) {
      DllAddRef ();
   }//end Fct

   ~IPropertyHandler () {
      SafeRelease(&m_pStream);
      SafeRelease(&m_pCache);
      DllRelease ();
   }//end Fct


public:
   // HandlerFactory
   //------------------------Start CreateInstance----------------------------------//
    static STDMETHODIMP CreateInstance (REFIID riid, void **ppv) {
       HRESULT hr = E_OUTOFMEMORY;
       ConcreteHandler *handler = new (std::nothrow) ConcreteHandler();
       if (handler)
       {
          hr = handler->QueryInterface (riid, ppv);
          handler->Release ();
       }

       return hr;
    }//end Fct


public:
   // IUnknown
   //------------------------Start QueryInterface----------------------------------//
   IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
      static const QITAB qit[] =
      {
         QITABENT(typename ConcreteHandler, IPropertyStore),
         QITABENT(typename ConcreteHandler, IInitializeWithStream),
         {0, 0 },
      };

      return QISearch(this, qit, riid, ppv);
      //return NULL;
   }//end Fct



   //------------------------Start AddRef------------------------------------------//
   IFACEMETHODIMP_(ULONG) AddRef() {
      return InterlockedIncrement(&m_cRef);
   }//end Fct



   //------------------------Start Release-----------------------------------------//
   IFACEMETHODIMP_(ULONG) Release() {
      long cRef = InterlockedDecrement(&m_cRef);
      if (cRef == 0)
      {
         delete this;
      }
      return cRef;
   }//end Fct


public:
   // IInitializeWithStream
   //------------------------Start Initialize--------------------------------------//
   virtual IFACEMETHODIMP Initialize (IStream *pStream, DWORD grfMode) {
      //g_oLog << "IPropertyHandler::Initialize ( , " << grfMode <<")" << std::endl;

      if (m_pStream) return E_UNEXPECTED;

      STATSTG stat;
      H_ASSERT_ (pStream->Stat (&stat, STATFLAG_DEFAULT) );   // The filename and suffix is not important

      // empty stream => empty property store
      if (stat.cbSize.QuadPart == 0)
         return S_OK;

      try {
         // Get properties from file (exactly from a stream)
         H_ASSERT_ (readProperties (pStream) );
      } catch (...) {
          return E_UNEXPECTED;
      }

      // save a reference to the stream as well as the grfMode
      H_ASSERT_ (pStream->QueryInterface (&m_pStream) );

      m_grfMode = grfMode;

      return S_OK;
   }//end Fct


public:
   // IInitializeWithStream
   //virtual STDMETHODIMP Initialize (IStream *pStream, DWORD grfMode) = 0;
   virtual STDMETHODIMP readProperties (IStream *pStream) = 0;

   // IPropertyStore
   virtual STDMETHODIMP GetCount (DWORD *pcProps) = 0;
   virtual STDMETHODIMP GetAt    (DWORD iProp, PROPERTYKEY *pkey) = 0;
   virtual STDMETHODIMP GetValue (REFPROPERTYKEY key, PROPVARIANT *pPropVar) = 0;
   virtual STDMETHODIMP SetValue (REFPROPERTYKEY key, REFPROPVARIANT propVar) = 0;   // SetValue just updates the internal value cache
   virtual STDMETHODIMP Commit   () = 0;   // Commit writes the internal value cache back out to the stream passed to Initialize


protected:
   long                 m_cRef;
   DWORD                m_grfMode;
   IStream             *m_pStream;
   IPropertyStoreCache *m_pCache;  // internal value cache to abstract IPropertyStore operations from the DOM back-end

};
