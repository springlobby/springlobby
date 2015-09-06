/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


#include "utils/conversion.h"
#include "log.h"
#include "downloader/lib/src/Downloader/Http/HttpDownloader.h"

// convert a string to IRI: https://en.wikipedia.org/wiki/Internationalized_resource_identifier
std::string ConvToIRI(const std::string& str)
{
	std::string utf8(wxString(str).mb_str(wxMBConvUTF8()));
	wxString escaped;
	for (unsigned i = 0; i < utf8.length(); i++) {
		const unsigned char c = utf8[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
			escaped.append(wxChar(c));
		} else { /* if (i+1<utf8.length())*/
			escaped.append(wxString::Format(_T("%%%02x"), wxChar(c)));
		}
		//FIXME: this function is incomplete! tested only with german umlauts
	}
	return STD_STRING(escaped);
}

//! @brief gets latest version from version.springlobby.info via HTTP
std::string GetHttpFile(const std::string& httpurl)
{
	std::string res;
	if (!CHttpDownloader::DownloadUrl(httpurl, res)) {
		wxLogWarning("Error downloading %s", httpurl.c_str());
	}
	return res;
}
