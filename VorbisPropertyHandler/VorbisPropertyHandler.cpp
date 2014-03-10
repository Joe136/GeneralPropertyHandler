

//http://bluishcoder.co.nz/2009/06/24/reading-ogg-files-using-libogg.html


//---------------------------Includes----------------------------------------------//
#include "VorbisPropertyHandler.h"
#include<cstdint>
#include<ogg/ogg.h>
#include<vorbis/codec.h>
#include<cassert>



//---------------------------Defines-----------------------------------------------//
#ifdef DEBUG
#define oLOG(TEXT) g_oLog << TEXT;
#else
#define oLOG(TEXT)
#endif

// Break, if COMMAND returns not null
#define V_ASSERT_(COMMAND)  { if ( (COMMAND) != 0) return E_FAIL; }
// Break, if COMMAND returns null
#define V_ASSERT_N(COMMAND) { if ( (COMMAND) == 0) return E_FAIL; }



//---------------------------Start Constructor-------------------------------------//
VorbisPropertyHandler::VorbisPropertyHandler(void) {
   // Initialize Vorbis structures
   vorbis_info_init    (&m_oInfo);
   vorbis_comment_init (&m_oComment);
}



//---------------------------Start Destructor--------------------------------------//
VorbisPropertyHandler::~VorbisPropertyHandler(void) {
   // Delete Vorbis structures
   vorbis_info_clear (&m_oInfo);
   vorbis_comment_clear (&m_oComment);
}



#ifdef DEBUG
//---------------------------Start print_header------------------------------------//
void print_header (ogg_page *og){
   int j;
   char buffer1[2048];
   char buffer2[2048];
   char buffer3[2048];

   sprintf(buffer1, "  capture: %c %c %c %c  version: %d  flags: %x\n",
      og->header[0],og->header[1],og->header[2],og->header[3],
      (int)og->header[4],(int)og->header[5]);

   sprintf(buffer2,"  granulepos: %d  serialno: %d  pageno: %ld\n",
      (og->header[9]<<24)|(og->header[8]<<16)|
      (og->header[7]<<8)|og->header[6],
      (og->header[17]<<24)|(og->header[16]<<16)|
      (og->header[15]<<8)|og->header[14],
      ((long)(og->header[21])<<24)|(og->header[20]<<16)|
      (og->header[19]<<8)|og->header[18]);

   sprintf(buffer3,"  checksum: %02x:%02x:%02x:%02x\n  segments: %d (",
      (int)og->header[22],(int)og->header[23],
      (int)og->header[24],(int)og->header[25],
      (int)og->header[26]);

   g_oLog << "HEADER:\n" << buffer1 << buffer2 << buffer3;

   for(j=27;j<og->header_len;j++) {
      sprintf(buffer1,"%d ",(int)og->header[j]);
      g_oLog << buffer1;
   }


   g_oLog << ")" << std::endl;
}
#endif



//---------------------------Start InitPropVariantFromCString----------------------//
inline HRESULT InitPropVariantFromCString (__in PCSTR psz, __out PROPVARIANT *ppropvar) {
      const size_t cSize = strlen (psz) + 1;
      std::wstring wc (cSize, L'#');
      mbstowcs (&wc[0], psz, cSize);

      return InitPropVariantFromString (wc.c_str (), ppropvar);
}



//---------------------------Start readProperties----------------------------------//
/*IFACEMETHODIMP VorbisPropertyHandler::readProperties (IStream *pStream) {
   // This is my own experiment to read properties
   ULONG length = 0L;
   struct header {
      uint8_t   type[4];
      uint32_t  length;
      uint8_t  *commentList;
      uint32_t  userLength;
      uint8_t  *userCommentList;

      header () : length (0), commentList (nullptr), userLength (0), userCommentList (nullptr) {}
      ~header () { if (commentList != nullptr) delete[] commentList; if (userCommentList != nullptr) delete[] userCommentList; }
   } header;

   // Read first 8 bites ("OggS" + length)
   H_ASSERT_ (pStream->Read (&header, 8, &length) );

   g_oLog << "VorbisPropertyHandler::Initialize: " << length << " " << header.length << " " << (char*)header.type << std::endl;

   // Read vorbis main comment list
   if (header.length) {
      header.commentList = new (std::nothrow) uint8_t [header.length];

      H_ASSERT_ (pStream->Read (header.commentList, header.length, &length) );

      g_oLog << "VorbisPropertyHandler::Initialize: " << length << std::endl;
   }

   // Read vorbis user comment list length
   H_ASSERT_ (pStream->Read (&header.userLength, 4, &length) );

   g_oLog << "VorbisPropertyHandler::Initialize: " << length << " " << header.userLength << std::endl;

   // Read vorbis user comment list
   if (header.userLength) {
      header.commentList = new (std::nothrow) uint8_t [header.userLength];

      H_ASSERT_ (pStream->Read (header.userCommentList, header.userLength, &length) );

      g_oLog << "VorbisPropertyHandler::Initialize: " << length << std::endl;
   }


   // Get entries from main common list
   do {

   } while (true);
}//end Fct
*/



//---------------------------Start readProperties----------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::readProperties (IStream *pStream) {
   oLOG ("VorbisPropertyHandler::readProperties ()" << std::endl);

   ogg_page         page;
   ogg_sync_state   syncstate;
   ogg_stream_state streamstate;
   ogg_packet       packet;

   int            packetCount = 0;

   // Initialize OGG structures
   V_ASSERT_ (ogg_sync_init (&syncstate) );
   V_ASSERT_ (ogg_stream_init(&streamstate, 0) );

   while (packetCount < 3) {
      H_ASSERT_ (readOGGPacket (pStream, page, syncstate, streamstate, packet) );

      // Check if it is a Vorbis header
      if (packetCount == 0 && vorbis_synthesis_idheader (&packet) != 1)
         break;

      V_ASSERT_ (vorbis_synthesis_headerin (&m_oInfo, &m_oComment, &packet) );

      // A packet is available, this is what we pass to the vorbis or theora libraries to decode.
      packetCount++;
   }

   //vorbis_dsp_state dspstate;
   //vorbis_block block;

   //V_ASSERT_ (vorbis_synthesis_init (&dspstate, &m_oInfo) );
   //V_ASSERT_ (vorbis_block_init (&dspstate, &block) );

#ifdef DEFINE
   g_oLog << "VorbisPropertyHandler::readProperties: m_oInfo = "
      << "\n" << m_oInfo.bitrate_lower
      << "\n" << m_oInfo.bitrate_nominal
      << "\n" << m_oInfo.bitrate_upper
      << "\n" << m_oInfo.bitrate_window
      << "\n" << m_oInfo.channels
      << "\n" << (char*)m_oInfo.codec_setup
      << "\n" << m_oInfo.rate
      << "\n" << m_oInfo.version
      << "\n" << m_oComment.vendor
      << "\n" << m_oComment.comments;

   char buffer[4096];

   for (int i = 0; i < m_oComment.comments; ++i) {
      memcpy (buffer, m_oComment.user_comments[i], m_oComment.comment_lengths[i]);
      buffer[m_oComment.comment_lengths[i] ] = 0;
      g_oLog << "\n" << buffer;
   }

   g_oLog << std::endl;
#endif

   // Delete OGG structures
   V_ASSERT_ (ogg_stream_clear (&streamstate) );
   V_ASSERT_ (ogg_sync_clear (&syncstate) );

   return S_OK;
}//end Fct



//---------------------------Start readOGGPacket-----------------------------------//
HRESULT VorbisPropertyHandler::readOGGPacket (IStream *pStream, ogg_page &page, ogg_sync_state &syncstate, ogg_stream_state &streamstate, ogg_packet &packet) {
   oLOG ("VorbisPropertyHandler::readOGGPacket ()" << std::endl);

   ULONG length = 0L;

   do {
      while (ogg_sync_pageout(&syncstate, &page) != 1) {
         char* buffer;
         V_ASSERT_N (buffer = ogg_sync_buffer (&syncstate, 4096) );

         H_ASSERT_ (pStream->Read (buffer, 4096, &length) );

         if (length == 0) {
            // End of file
            break;
         }

         V_ASSERT_ (ogg_sync_wrote (&syncstate, length) );
      }

      //oLOG ("VorbisPropertyHandler::readOGGPacket: " << page.header_len << std::endl);

      streamstate.serialno = ogg_page_serialno (&page);

      V_ASSERT_ (ogg_stream_pagein (&streamstate, &page) );

#ifdef DEBUG
      print_header (&page);

      g_oLog << "VorbisPropertyHandler::readOGGPacket: " << page.header_len << std::endl;
      g_oLog << "VorbisPropertyHandler::readOGGPacket: " << ogg_page_version (&page) << std::endl;
      g_oLog << "VorbisPropertyHandler::readOGGPacket: "
         << (char)page.header[0]
         << (char)page.header[1]
         << (char)page.header[2]
         << (char)page.header[3]
         << (int)page.header[4]
         << (int)page.header[5]
         << std::endl;
#endif

      int ret = ogg_stream_packetout (&streamstate, &packet);

      if (ret == 0)
         continue;           // Need more data to be able to complete the packet
      else
         return ret;
   } while (true);
}


//---------------------------Start GetCount----------------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::GetCount (DWORD *pcProps) {
   oLOG ("VorbisPropertyHandler::GetCount ()" << std::endl);

   return NULL;
}



//---------------------------Start GetAt-------------------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::GetAt (DWORD iProp, PROPERTYKEY *pkey) {
   oLOG ("VorbisPropertyHandler::GetAt ()" << std::endl);

   return NULL;
}



//---------------------------Start GetValue----------------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::GetValue (REFPROPERTYKEY key, PROPVARIANT *pPropVar) {
#ifdef DEBUG
   OLECHAR* bstrClsid; StringFromCLSID (key.fmtid, &bstrClsid);
   g_oLog << "VorbisPropertyHandler::GetValue ( (" << bstrClsid << ", " << key.pid << "), 0x????????)" << std::endl;
   ::CoTaskMemFree(bstrClsid);
   g_oLog << "VorbisPropertyHandler::GetValue (";
#endif

   V_ASSERT_N (pPropVar);

   HRESULT hr = E_INVALIDARG;
   pPropVar->vt = VT_EMPTY;

   // Look at 'propkey.h'

   if (key == PKEY_Title) {
      oLOG ("PKEY_Title");
      hr = getCommentValue ("TITLE", pPropVar, false);
   }
   else if (key == PKEY_Music_Artist) {
      oLOG ("PKEY_Music_Artist");
      hr = getCommentValue ("ARTIST", pPropVar, true);
   }
   else if (key == PKEY_Music_AlbumArtist) {
      oLOG ("PKEY_Music_AlbumArtist");
      hr = getCommentValue ("ALBUMARTIST", pPropVar, true);
   }
   else if (key == PKEY_Music_AlbumTitle) {
      oLOG ("PKEY_Music_AlbumTitle");
      hr = getCommentValue ("ALBUM", pPropVar, false);
   }
   else if (key == PKEY_Music_Genre) {
      oLOG ("PKEY_Music_Genre");
      hr = getCommentValue ("GENRE", pPropVar, false);
   }
   else if (key == PKEY_Audio_ChannelCount) {
      oLOG ("PKEY_Audio_ChannelCount");
      pPropVar->uintVal = m_oInfo.channels;
      pPropVar->vt = VT_UI4;
      hr = S_OK;
   }
   else if (key == PKEY_Media_Duration) {
      oLOG ("PKEY_Media_Duration");
      //pPropVar->ulVal = ap->length()*10000000;
      //pPropVar->vt = VT_UI8;
      //hr = S_OK;
      hr = S_FALSE;
   }
   else if (key == PKEY_Audio_EncodingBitrate) {
      oLOG ("PKEY_Audio_EncodingBitrate");
      pPropVar->uintVal = m_oInfo.bitrate_nominal;
      pPropVar->vt = VT_UI4;
      hr = S_OK;
   }
   else if (key == PKEY_Audio_SampleRate) {
      oLOG ("PKEY_Audio_SampleRate");
      pPropVar->uintVal = m_oInfo.rate;
      pPropVar->vt = VT_UI4;
      hr = S_OK;
   }
   else if (key == PKEY_Music_TrackNumber) {
      oLOG ("PKEY_Music_TrackNumber");
      hr = getCommentValueNumber ("TRACKNUMBER", pPropVar);
   }
   else if (key == PKEY_Media_Year) {
      oLOG ("PKEY_Media_Year");
      hr = getCommentValueNumber ("YEAR", pPropVar);
   }
   else if (key == PKEY_Rating) {
      oLOG ("PKEY_Rating");
      hr = getCommentValueNumber ("RATING", pPropVar);
   }
   else if (key == PKEY_Media_SubTitle) {
      oLOG ("PKEY_Media_SubTitle");
      hr = getCommentValue ("SUBTITLE", pPropVar, false);
   }
   else if (key == PKEY_Comment) {
      oLOG ("PKEY_Comment");
      hr = getCommentValue ("COMMENT", pPropVar, false);
   }
   else if (key == PKEY_Media_Publisher) {
      oLOG ("PKEY_Media_Publisher");
      hr = getCommentValue ("PUBLISHER", pPropVar, false);
   }
   else if (key == PKEY_Media_EncodedBy) {
      oLOG ("PKEY_Media_EncodedBy");
      hr = InitPropVariantFromCString (m_oComment.vendor, pPropVar);
   }

#ifdef DEBUG
   g_oLog << ")" << std::endl;
#endif

   return hr;
}//end Fct



//---------------------------Start getCommentValue---------------------------------//
HRESULT VorbisPropertyHandler::getCommentValue (char *key, __out PROPVARIANT *ppropvar, bool multiple) {
   int          keylen = strlen (key);
   int          len;
   HRESULT      hr     = S_FALSE;
   std::wstring mbuffer;
   bool         first  = true;

   for (int i = 0; i < m_oComment.comments; ++i) {
      len   = min (m_oComment.comment_lengths[i], 30);
      int n = 0;

      // Search the enter key
      for (; n < len; ++n)
         if (m_oComment.user_comments[i][n] == '=')
            break;

      // Check if comment label too long
      if (n >= len)
         continue;

      // Check if this is the correct comment
      if ( (n == keylen) && !strncmp (key, m_oComment.user_comments[i], keylen) ) {
         len          = m_oComment.comment_lengths[i] - n;
         char *buffer = new (std::nothrow) char[len];

         memcpy (buffer, &(m_oComment.user_comments[i][n + 1]), len - 1);
         buffer[len - 1] = 0;

         if (!multiple) {
            hr = InitPropVariantFromCString (buffer, ppropvar);
            delete[] buffer;
            break;
         } else {
            std::wstring wc (len - 1, L'#');
            mbstowcs (&wc[0], buffer, len - 1);

            if (!first)
               mbuffer.push_back (';');
            mbuffer.append (wc);

            delete[] buffer;
            first = false;

            hr = S_OK;
         }
      }
   }//end for

   if (multiple && hr == S_OK) {
      ppropvar->bstrVal = SysAllocString (mbuffer.c_str () );
      ppropvar->vt      = VT_BSTR;
   }

   return hr;
}//end Fct



//---------------------------Start getCommentValueNumber---------------------------//
HRESULT VorbisPropertyHandler::getCommentValueNumber (char *key, __out PROPVARIANT *ppropvar) {
   char buffer[30];
   int  keylen = strlen (key);
   int  len;

   for (int i = 0; i < m_oComment.comments; ++i) {
      len   = min (m_oComment.comment_lengths[i], 30);
      int n = 0;

      for (; n < len; ++n)
         if (m_oComment.user_comments[i][n] == '=')
            break;

      if (n >= len)
         continue;           // Comment label too long

      if ( (n == keylen) && !strncmp (key, m_oComment.user_comments[i], keylen) ) {
         memcpy (buffer, &(m_oComment.user_comments[i][n + 1]), m_oComment.comment_lengths[i] - n - 1);
         buffer[m_oComment.comment_lengths[i] - n - 1] = 0;

         ppropvar->uintVal = atoi (buffer);
         ppropvar->vt = VT_UI4;

         return S_OK;
      }
   }//end for

   return S_FALSE;
}//end Fct



//---------------------------Start SetValue----------------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::SetValue (REFPROPERTYKEY key, REFPROPVARIANT propVar) {
   oLOG ("VorbisPropertyHandler::SetValue ()" << std::endl);

#ifdef DEBUG
   if (propVar.vt == VT_UI4)
      g_oLog << "VorbisPropertyHandler::SetValue: " << propVar.uintVal << std::endl;
   else if (propVar.vt == VT_UI8)
      g_oLog << "VorbisPropertyHandler::SetValue: " << propVar.ulVal << std::endl;
   else if (propVar.vt == VT_LPWSTR)
      g_oLog << "VorbisPropertyHandler::SetValue: " << propVar.pwszVal << std::endl;
   else if (propVar.vt == VT_LPSTR)
      g_oLog << "VorbisPropertyHandler::SetValue: " << propVar.pszVal << std::endl;
   else if (propVar.vt == VT_BSTR)
      g_oLog << "VorbisPropertyHandler::SetValue: " << propVar.bstrVal << std::endl;
#endif

   return NULL;
}



//---------------------------Start Commit------------------------------------------//
IFACEMETHODIMP VorbisPropertyHandler::Commit () {
   oLOG ("VorbisPropertyHandler::Commit ()" << std::endl);

   return NULL;
}
