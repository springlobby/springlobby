#include "curlhelper.h"

extern "C"
{
	size_t wxcurl_string_write(void* ptr, size_t size, size_t nmemb, void* pcharbuf)
	{
		size_t iRealSize = size * nmemb;
		wxCharBuffer* pStr = (wxCharBuffer*) pcharbuf;

		if(pStr)
		{
			wxString str = wxCURL_BUF2STRING(*pStr) + wxString((const char*)ptr, wxConvLibc);
			*pStr = wxCURL_STRING2BUF(str);
		}

		return iRealSize;
	}
	size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;

		wxOutputStream* pBuf = (wxOutputStream*)stream;

		if(pBuf)
		{
			pBuf->Write(ptr, iRealSize);

			return pBuf->LastWrite();
		}

		return 0;
	}
	/* reads from a stream */
	size_t wxcurl_stream_read(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;

		wxInputStream* pBuf = (wxInputStream*)stream;

		if(pBuf)
		{
			pBuf->Read(ptr, iRealSize);

			return pBuf->LastRead();
		}

		return 0;
	}
}//end extern "C"
