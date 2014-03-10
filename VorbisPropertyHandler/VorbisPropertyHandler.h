
#pragma once

//---------------------------Includes----------------------------------------------//
#include "../GeneralPropertyHandler/IPropertyHandler.h"
#include<ogg/ogg.h>
#include<vorbis/codec.h>



//---------------------------Defines-----------------------------------------------//
class DECLSPEC_UUID("6D285FD7-4175-4795-8559-AC73389F6FE4") VorbisPropertyHandler;



//---------------------------Class VorbisPropertyHandler---------------------------//
class VorbisPropertyHandler : public IPropertyHandler<VorbisPropertyHandler>
{
public:
   VorbisPropertyHandler(void);
   ~VorbisPropertyHandler(void);

   virtual IFACEMETHODIMP GetCount (DWORD *pcProps);
   virtual IFACEMETHODIMP GetAt    (DWORD iProp, PROPERTYKEY *pkey);
   virtual IFACEMETHODIMP GetValue (REFPROPERTYKEY key, PROPVARIANT *pPropVar);
   virtual IFACEMETHODIMP SetValue (REFPROPERTYKEY key, REFPROPVARIANT propVar);   // SetValue just updates the internal value cache
   virtual IFACEMETHODIMP Commit   ();   // Commit writes the internal value cache back out to the stream passed to Initialize


protected:
   IFACEMETHODIMP  readProperties        (IStream *pStream);
   HRESULT         readOGGPacket         (IStream *pStream, ogg_page &page, ogg_sync_state &state, ogg_stream_state &streamstate, ogg_packet &packet);
   HRESULT         getCommentValue       (char *key, __out PROPVARIANT *ppropvar, bool multiple);
   HRESULT         getCommentValueNumber (char *key, __out PROPVARIANT *ppropvar);


protected:
   vorbis_info      m_oInfo;
   vorbis_comment   m_oComment;

};

