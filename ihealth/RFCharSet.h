#ifndef TG_CHAR_SET_H
#define TG_CHAR_SET_H


#include <string>

// UTF-8, UTF-16, UTF-32(Unicode), GBK
// ×Ö·û¼¯Ïà»¥×ª»»

 
// FIXME
char *TGUTF32ToUTF8(long code, char *s);
const char *TGUTF8ToUTF32(const char *s, long *code);



extern std::wstring TGUTF8ToUTF16(const char *s, int size);
extern std::wstring TGUTF8ToUTF16(const std::string& s);

extern std::string TGUTF16ToUTF8(const std::wstring& ws);

extern std::wstring TGGBKToUTF16(const char *s, int size);
extern std::wstring TGGBKToUTF16(const std::string& s);

extern std::string TGUTF16ToGBK(const std::wstring& ws);

extern std::wstring TGUTF16BOMToUTF16(char *utf16_bom, int n);
extern int TGGetInt(const char *p, int n);

extern std::wstring TGUTF16ToUTF16BOM(const std::wstring& ws);


#endif	// TG_CHAR_SET_H
