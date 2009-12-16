/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision: 4909 $
 * $Id: base64.cpp 4909 2008-02-27 13:15:26Z mortenmacfly $
 * $HeadURL: svn://svn.berlios.de/codeblocks/tags/8.02/src/sdk/base64.cpp $
 */

//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*********************************************************************
//
// converted to wxWindows by Frank Bu?
//

#include "base64.h"
#include "defines.h"

#ifdef HAVE_WX29
    #include "utils/conversion.h"
#endif

const wxChar fillchar = '=';

                        // 00000000001111111111222222
                        // 01234567890123456789012345
static wxString     cvt = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ"

                        // 22223333333333444444444455
                        // 67890123456789012345678901
                          "abcdefghijklmnopqrstuvwxyz"

                        // 555555556666
                        // 234567890123
                          "0123456789+/");

wxString wxBase64::Encode(const wxString& data)
{
    return wxBase64::Encode((const wxUint8*)data.c_str(), data.Len());
}

wxString wxBase64::Encode(const wxUint8* pData, size_t len)
{
    size_t c;
    wxString ret;
    ret.Alloc(len * 4 / 3 + len * 2);

    for (size_t i = 0; i < len; ++i)
    {
        c = (pData[i] >> 2) & 0x3f;
        ret.Append(cvt[c], 1);
        c = (pData[i] << 4) & 0x3f;
        if (++i < len)
            c |= (pData[i] >> 4) & 0x0f;

        ret.Append(cvt[c], 1);
        if (i < len)
        {
            c = (pData[i] << 2) & 0x3f;
            if (++i < len)
                c |= (pData[i] >> 6) & 0x03;

            ret.Append(cvt[c], 1);
        }
        else
        {
            ++i;
            ret.Append(fillchar, 1);
        }

        if (i < len)
        {
            c = pData[i] & 0x3f;
            ret.Append(cvt[c], 1);
        }
        else
        {
            ret.Append(fillchar, 1);
        }
    }

    return ret;
}

std::string wxBase64::Decode(const wxString& data)
{
    int c;
    int c1;
    size_t len = data.Len();
    std::string ret;
    ret.reserve(data.length() * 3 / 4);

    for (size_t i = 0; i < len; ++i)
    {
        // TODO: check all Find results for -1 as result of wrong input data for release build
        c = cvt.find(data[i]);
        wxASSERT_MSG(c >= 0, _T("invalid base64 input"));
        ++i;
        c1 = cvt.find(data[i]);
        wxASSERT_MSG(c1 >= 0, _T("invalid base64 input"));
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret.push_back(c);
        if (++i < len)
        {
            c = data[i];
            if ((char)fillchar == c)
                break;
        #ifndef HAVE_WX29
            c = cvt.find(c);
        #else
            c = cvt.find( TowxString(c) );
        #endif
            wxASSERT_MSG(c >= 0, _T("invalid base64 input"));
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret.push_back(c1);
        }

        if (++i < len)
        {
            c1 = data[i];
            if ((char)fillchar == c1)
                break;

        #ifndef HAVE_WX29
            c1 = cvt.find(c1);
        #else
            c1 = cvt.find( TowxString(c1) );
        #endif
            wxASSERT_MSG(c1 >= 0, _T("invalid base64 input"));
            c = ((c << 6) & 0xc0) | c1;
            ret.push_back(c);
        }
    }

    return ret;
}
