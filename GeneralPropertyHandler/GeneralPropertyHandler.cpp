

//

//---------------------------Includes----------------------------------------------//
#include "GeneralPropertyHandler.h"
#include "HandlerFactory.h"
#include<GuidDef.h>



//---------------------------Defines-----------------------------------------------//
HINSTANCE               GeneralPropertyHandler::m_hInst      = NULL;
GeneralPropertyHandler *GeneralPropertyHandler::m_pSelf      = nullptr;
std::vector<CLASS_OBJECT_INIT> GeneralPropertyHandler::m_oHandlerList;



//---------------------------Start DllMain-----------------------------------------//
/** Standard DLL functions */
STDAPI_(BOOL) DllMain (HINSTANCE hInstance, DWORD dwReason, void *) {
   if (dwReason == DLL_PROCESS_ATTACH)
   {
#ifdef DEBUG
      g_oLog = std::wofstream (L"R:\propertyhandler.log", std::ios_base::out);
#endif
      GeneralPropertyHandler::m_hInst = hInstance;
      DisableThreadLibraryCalls(hInstance);
      GeneralPropertyHandler::m_pSelf = new GeneralPropertyHandler ();
      GeneralPropertyHandler::m_pSelf->registerPropertyHandlers ();
   }
   else if (dwReason == DLL_PROCESS_DETACH) {
      if (GeneralPropertyHandler::m_pSelf)
         delete GeneralPropertyHandler::m_pSelf;
   }

    return TRUE;
}



//---------------------------Start Constructor-------------------------------------//
GeneralPropertyHandler::GeneralPropertyHandler (void) {
   //DllAddRef();
}//end Fct



//---------------------------Start Destructor--------------------------------------//
GeneralPropertyHandler::~GeneralPropertyHandler (void) {
   //DllRelease();
}//end Fct



//---------------------------Start DllGetClassObject-------------------------------//
HRESULT STDAPICALLTYPE GeneralPropertyHandler::DllGetClassObject (REFCLSID clsid, REFIID riid, void **ppv) {
   OLECHAR* bstrClsid; StringFromCLSID (clsid, &bstrClsid);
   OLECHAR* bstrRiid;  StringFromCLSID (clsid, &bstrRiid);
#ifdef DEBUG
   g_oLog << "GeneralPropertyHandler::DllGetClassObject (" << bstrClsid << ", " << bstrRiid << ", 0x" << ppv << ")" << std::endl;
#endif
   ::CoTaskMemFree(bstrClsid); ::CoTaskMemFree(bstrRiid);

   return m_pSelf->getClassObject (clsid, riid, ppv);
}



//---------------------------Start getClassObject----------------------------------//
HRESULT GeneralPropertyHandler::getClassObject (REFCLSID clsid, REFIID riid, void **ppv) {
   //g_oLog << "GeneralPropertyHandler::getClassObject ()" << std::endl;

   *ppv = NULL;
   HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

   for (std::vector<CLASS_OBJECT_INIT>::iterator i = m_oHandlerList.begin (); i != m_oHandlerList.end (); ++i) {
      if (IsEqualCLSID (clsid, *i->pClsid) )
      {
         // Create a factory and return the factory, not the PropertyHandler
         HandlerFactory *pHandlerFactory = new (std::nothrow) HandlerFactory (i->pfnCreate);

         hr = pHandlerFactory ? S_OK : E_OUTOFMEMORY;

         if (SUCCEEDED(hr) )
         {
            hr = pHandlerFactory->QueryInterface (riid, ppv);
            pHandlerFactory->Release ();
         }

         break; // match found
      }
   }

   return hr;
}//end Fct



//---------------------------Start DllCanUnloadNow---------------------------------//
HRESULT STDAPICALLTYPE GeneralPropertyHandler::DllCanUnloadNow () {
#ifdef DEBUG
   g_oLog << "GeneralPropertyHandler::DllCanUnloadNow () => " << g_cRefModule << std::endl;
#endif

    // Only allow the DLL to be unloaded after all outstanding references have been released
    return (g_cRefModule == 0) ? S_OK : S_FALSE;
}//end Fct



//---------------------------Start DllRegisterServer-------------------------------//
HRESULT STDAPICALLTYPE GeneralPropertyHandler::DllRegisterServer () {
#ifdef DEBUG
   g_oLog << "GeneralPropertyHandler::DllRegisterServer ()" << std::endl;
#endif

   //HRESULT hr = RegisterDocFile();
   //if (SUCCEEDED(hr))
   //{
   //    hr = RegisterOpenMetadata();
   //}
   //return hr;

   return E_UNEXPECTED;
}//end Fct



//---------------------------Start DllUnregisterServer-----------------------------//
HRESULT STDAPICALLTYPE GeneralPropertyHandler::DllUnregisterServer () {
#ifdef DEBUG
   g_oLog << "GeneralPropertyHandler::DllUnregisterServer ()" << std::endl;
#endif

   //HRESULT hr = UnregisterDocFile();
   //if (SUCCEEDED(hr))
   //{
   //    hr = UnregisterOpenMetadata();
   //}
   //return hr;

   return E_UNEXPECTED;
}//end Fct



#include "config"
