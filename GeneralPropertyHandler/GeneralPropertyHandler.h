
#pragma once

//---------------------------Includes----------------------------------------------//
#include "StdAfx.h"
#include<vector>



//---------------------------Defines-----------------------------------------------//
typedef HRESULT (*PFNCREATEINSTANCE)(REFIID riid, void **ppvObject);
struct CLASS_OBJECT_INIT
{
    const CLSID *pClsid;
    PFNCREATEINSTANCE pfnCreate;

    CLASS_OBJECT_INIT ();
    CLASS_OBJECT_INIT (const CLSID *pClsid, PFNCREATEINSTANCE pfnCreate) : pClsid (pClsid), pfnCreate (pfnCreate) {}
};



//---------------------------Class GeneralPropertyHandler--------------------------//
class GeneralPropertyHandler
{
public:
   GeneralPropertyHandler(void);
   ~GeneralPropertyHandler(void);

public:
   static HRESULT STDAPICALLTYPE DllGetClassObject   (REFCLSID clsid, REFIID riid, void **ppv);
   static HRESULT STDAPICALLTYPE DllCanUnloadNow     ();
   static HRESULT STDAPICALLTYPE DllRegisterServer   ();
   static HRESULT STDAPICALLTYPE DllUnregisterServer ();


public:
   void registerPropertyHandlers ();


protected:
   HRESULT getClassObject (REFCLSID clsid, REFIID riid, void **ppv);

   template<class HANDLER>
   static void Register () {
      m_oHandlerList.push_back (CLASS_OBJECT_INIT (&__uuidof(HANDLER), /*IPropertyHandler*/typename HANDLER::CreateInstance) );
   }//end Fct


public:
   static HINSTANCE               m_hInst;
   static GeneralPropertyHandler *m_pSelf;


protected:
   static std::vector<CLASS_OBJECT_INIT> m_oHandlerList;

};//end Class

