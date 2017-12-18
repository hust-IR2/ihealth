#include "StdAfx.h"
#include "RFCharSet.h"
#include <windows.h>


////////////////////////////////////////////////////////////////////////////////

/*
 * U-00000000 - U-0000007F: 0xxxxxxx
 * U-00000080 - U-000007FF: 110xxxxx 10xxxxxx
 * U-00000800 - U-0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 * U-00010000 - U-001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * U-00200000 - U-03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * U-04000000 - U-7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 */

char *TGUTF32ToUTF8(long code, char *s)
{
	int n;
	unsigned char mask, mark;

	if (code < 0) {
		// invalid?
		return s;
	} else if (code < 0x00000080) {
		*s = (char)code;
		return s+1;	
	} else if (code < 0x00000800) {
		n = 2;
		mask = 0x1F;
		mark = 0xC0;
	} else if (code < 0x00010000) {
		n = 3;
		mask = 0x0F;
		mark = 0xE0;
	} else if (code < 0x00200000) {
		n = 4;
		mask = 0x07;
		mark = 0xF0;
	} else if (code < 0x04000000) {
		n = 5;
		mask = 0x03;
		mark = 0xF8;
	} else {
		n = 6;	
		mask = 0x01;
		mark = 0xFC;
	}

	int i;
	for (i = n-1; i > 0; --i) {
		*(s+i) = (code&0x3F) | 0x80;
		code >>= 6;
	}
	*(s+i) = (code&mask) | mark;

	return s+n;
}

const char *TGUTF8ToUTF32(const char *s, long *code)
{
	long tc = 0x00000000;
	int n;
	unsigned char c, mask;
	*code = tc;

	c = *s;
	if ((c&0x80) == 0x00) {
		*code = c;
		return s+1;
	} else if ((c&0xE0) == 0xC0) {
		n = 2;
		mask = 0x1F;
	} else if ((c&0xF0) == 0xE0) {
		n = 3;
		mask = 0x0F;
	} else if ((c&0xF8) == 0xF0) {
		n = 4;
		mask = 0x07;
	} else if ((c&0xFC) == 0xF8) {
		n = 5;
		mask = 0x03;
	} else if ((c&0xFE) == 0xFC) {
		n = 6;
		mask = 0x01;
	} else {
		return s+1;
	}

	tc |= (c & mask);
	for (int i = 1; i < n; ++i) {
		tc <<= 6;
		c = *(s+i);
		if (c == '\0' || (c&0xC0) != 0x80) {
			return s+i;
		}
		tc |= (c & 0x3F);
	}

	*code = tc;
	return s+n;
}

std::wstring TGUTF8ToUTF16(const char *s, int size)
{
	std::wstring ws = L"";
	if (s != NULL && size != 0) {
		int n = MultiByteToWideChar(CP_UTF8, 0, s, size, NULL, 0);
		if (n > 0) {
			ws.resize(n);
			::MultiByteToWideChar(CP_UTF8, 0, s, size, (wchar_t *)&ws.at(0), n);
		}
	}

	return ws;
}


std::wstring TGUTF8ToUTF16(const std::string& s)
{
	std::wstring ws = L"";

	if (!s.empty()) {
		int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), NULL, 0);
		if (n > 0) {
			ws.resize(n);
			::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), (wchar_t *)&ws.at(0), n);
		}
	}

	return ws;
}

std::string TGUTF16ToUTF8(const std::wstring& ws)
{
	std::string s = "";

	if (!ws.empty()) {
		int n = ::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), ws.size(), NULL, 0, 0, 0);
		if (n > 0) {
			s.resize(n);
			::WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), ws.size(), (char *)&s.at(0), n, 0, 0);
		}
	}

	return s;
}

std::wstring TGGBKToUTF16(const char *s, int size)
{
	std::wstring ws;
	int n = MultiByteToWideChar(936, 0, s, size, NULL, 0);
	if (n > 0) {
		ws.resize(n);
		int m = ::MultiByteToWideChar(936, 0, s, size, (wchar_t *)&ws.at(0), n);
	}

	return ws.c_str();
}


std::wstring TGGBKToUTF16(const std::string& s)
{
	std::wstring ws;
	int n = MultiByteToWideChar(936, 0, s.c_str(), s.size(), NULL, 0);
	if (n > 0) {
		ws.resize(n);
		::MultiByteToWideChar(936, 0, s.c_str(), s.size(), (wchar_t *)&ws.at(0), n);
	}

	return ws;
}

std::string TGUTF16ToGBK(const std::wstring& ws)
{
	std::string s;
	int n = ::WideCharToMultiByte(936, 0, ws.c_str(), ws.size(), NULL, 0, 0, 0);
	if (n > 0) {
		s.resize(n);
		::WideCharToMultiByte(936, 0, ws.c_str(), ws.size(), (char *)&s.at(0), n, 0, 0);
	}

	return s;
}

// FIXME: a bug here
std::wstring TGUTF16BOMToUTF16(char *utf16_bom, int n)
{
	if (!utf16_bom || n < 2) {
		return std::wstring();
	}

	n = n - n%2;

	unsigned char *p = (unsigned char *)utf16_bom;

	int index = 0;

	// big-endian
	if (p[0] == 0xFE && p[1] == 0xFF) {
		index = 2;

		// to little-endian
		unsigned char t;
		for (int i = 2; i < n; i += 2) {
			t = p[i];
			p[i] = p[i+1];
			p[i+1] = t;
		}
	} else {
	// little-endian
		if (p[0] == 0xFF && p[1] == 0xFE) {
			index = 2;
		} else {
			index = 0;
		}
	}

	if (index == 2 && n == 2) {
		return std::wstring();
	}

	std::wstring ws;
	ws.resize((n-index)/2);

	memcpy((void *)ws.data(), p+index, n-index);

	return ws;
}

int TGGetInt(const char *p, int n)
{
	if (!p) {
		return 0;
	}

	std::string s(p, p+n);
	return atoi(s.c_str());
}

std::wstring TGUTF16ToUTF16BOM(const std::wstring& ws)
{
	wchar_t bom_head = 0xFEFF;
	return bom_head + ws;
}
