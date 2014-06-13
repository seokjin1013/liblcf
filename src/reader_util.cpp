/*
 * Copyright (c) 2014 liblcf authors
 * This file is released under the MIT License
 * http://opensource.org/licenses/MIT
 */

#include "reader_options.h"
#ifdef LCF_SUPPORT_ICU
#  include "unicode/ucsdet.h"
#  include "unicode/ucnv.h"
#else
#  ifdef _WIN32
#    include <cstdio>
#    define WIN32_LEAN_AND_MEAN
#    ifndef NOMINMAX
#      define NOMINMAX
#    endif
#    include <windows.h>
#  else
#    include <locale>
#  endif
#endif

#include <cstdlib>
#include <sstream>
#include <vector>

#include "inireader.h"
#include "reader_util.h"

namespace ReaderUtil {
}

std::string ReaderUtil::CodepageToEncoding(int codepage) {
	if (codepage == 0)
		return std::string();
#ifdef _WIN32
	if (codepage > 0) {
		// Looks like a valid codepage
		return std::string(codepage);
	}
#else
	if (codepage == 932) {
#  ifdef LCF_SUPPORT_ICU
		return "cp943";
#  else
		return "SHIFT_JIS";
#  endif
	} else {
		std::ostringstream out;
#  ifdef LCF_SUPPORT_ICU
		out << "windows-" << codepage;
#  else
		out << "CP" << codepage;
#  endif
		// Check at first if the ini value is a codepage
		if (!out.str().empty()) {
			// Looks like a valid codepage
			return out.str();
		}
		return std::string();
	}
#endif
}

std::string ReaderUtil::DetectEncoding(const std::string& text) {
	const char* encoding = "";

#ifdef LCF_SUPPORT_ICU
	UErrorCode status = U_ZERO_ERROR;
	UCharsetDetector* detector = ucsdet_open(&status);

	ucsdet_setText(detector, text.data(), text.length(), &status);
	const UCharsetMatch* match = ucsdet_detect(detector, &status);
	if (match != NULL)
	{
		encoding = ucsdet_getName(match, &status);
	}
	ucsdet_close(detector);
#endif

	return std::string(encoding);
}

std::string ReaderUtil::GetEncoding(const std::string& ini_file) {
	INIReader ini(ini_file);
	if (ini.ParseError() != -1) {
		std::string encoding = ini.Get("EasyRPG", "Encoding", std::string());
		if (!encoding.empty()) {
			return CodepageToEncoding(atoi(encoding.c_str()));
		}
	}
	return std::string();
}

std::string ReaderUtil::GetLocaleEncoding() {
#ifdef _WIN32
	// On Windows, empty string encoding means current system locale.
	std::string encoding = "";
#else
	std::string encoding = "1252";

	std::locale loc = std::locale("");
	// Gets the language and culture part only
	std::string loc_full = loc.name().substr(0, loc.name().find_first_of("@."));
	// Gets the language part only
	std::string loc_lang = loc.name().substr(0, loc.name().find_first_of("_"));

	if      (loc_lang == "th")    encoding = "874";
	else if (loc_lang == "ja")    encoding = "932";
	else if (loc_full == "zh_CN" ||
	         loc_full == "zh_SG") encoding = "936";
	else if (loc_lang == "ko")    encoding = "949";
	else if (loc_full == "zh_TW" ||
	         loc_full == "zh_HK") encoding = "950";
	else if (loc_lang == "cs" ||
	         loc_lang == "hu" ||
	         loc_lang == "pl" ||
	         loc_lang == "ro" ||
	         loc_lang == "hr" ||
	         loc_lang == "sk" ||
	         loc_lang == "sl")    encoding = "1250";
	else if (loc_lang == "ru")    encoding = "1251";
	else if (loc_lang == "ca" ||
	         loc_lang == "da" ||
	         loc_lang == "de" ||
	         loc_lang == "en" ||
	         loc_lang == "es" ||
	         loc_lang == "fi" ||
	         loc_lang == "fr" ||
	         loc_lang == "it" ||
	         loc_lang == "nl" ||
	         loc_lang == "nb" ||
	         loc_lang == "pt" ||
	         loc_lang == "sv" ||
	         loc_lang == "eu")    encoding = "1252";
	else if (loc_lang == "el")    encoding = "1253";
	else if (loc_lang == "tr")    encoding = "1254";
	else if (loc_lang == "he")    encoding = "1255";
	else if (loc_lang == "ar")    encoding = "1256";
	else if (loc_lang == "et" ||
	         loc_lang == "lt" ||
	         loc_lang == "lv")    encoding = "1257";
	else if (loc_lang == "vi")    encoding = "1258";
#endif
	return encoding;
}

std::string ReaderUtil::Recode(const std::string& str_to_encode, const std::string& source_encoding) {
#ifdef _WIN32
	return ReaderUtil::Recode(str_to_encode, source_encoding, "65001");
#else
	return ReaderUtil::Recode(str_to_encode, source_encoding, "UTF-8");
#endif
}

std::string ReaderUtil::Recode(const std::string& str_to_encode,
                               const std::string& src_enc,
                               const std::string& dst_enc) {
	if (src_enc.empty()) {
		return str_to_encode;
	}
#ifdef LCF_SUPPORT_ICU
	UErrorCode status = U_ZERO_ERROR;
	int size = str_to_encode.size() * 4;
	UChar unicode_str[size];
	UConverter *conv;
	int length;

	conv = ucnv_open(src_enc.c_str(), &status);
	length = ucnv_toUChars(conv, unicode_str, size, str_to_encode.c_str(), -1, &status);
	ucnv_close(conv);

	char result[length];

	conv = ucnv_open(dst_enc.c_str(), &status);
	ucnv_fromUChars(conv, result, length * 4, unicode_str, -1, &status);
	ucnv_close(conv);

	if (status != 0) return std::string();

	return std::string(&result[0]);
#else
#  ifdef _WIN32
	size_t strsize = str_to_encode.size();

	wchar_t* widechar = new wchar_t[strsize * 5 + 1];

	// To UTF-16
	// Default codepage is 0, so we dont need a check here
	int res = MultiByteToWideChar(atoi(src_enc.c_str()), 0, str_to_encode.c_str(), strsize, widechar, strsize * 5 + 1);
	if (res == 0) {
		// Invalid codepage
		delete [] widechar;
		return str_to_encode;
	}
	widechar[res] = '\0';

	// Back to UTF-8...
	char* utf8char = new char[strsize * 5 + 1];
	res = WideCharToMultiByte(atoi(dst_enc.c_str()), 0, widechar, res, utf8char, strsize * 5 + 1, NULL, NULL);
	utf8char[res] = '\0';

	// Result in str
	std::string str = std::string(utf8char, res);

	delete [] widechar;
	delete [] utf8char;

	return str;
#  else
	iconv_t cd = iconv_open(dst_enc.c_str(), src_enc.c_str());
	if (cd == (iconv_t)-1)
		return str_to_encode;
	char *src = const_cast<char *>(str_to_encode.c_str());
	size_t src_left = str_to_encode.size();
	size_t dst_size = str_to_encode.size() * 5 + 10;
	char *dst = new char[dst_size];
	size_t dst_left = dst_size;
#    ifdef ICONV_CONST
	char ICONV_CONST *p = src;
#    else
	char *p = src;
#    endif
	char *q = dst;
	size_t status = iconv(cd, &p, &src_left, &q, &dst_left);
	iconv_close(cd);
	if (status == (size_t) -1 || src_left > 0) {
	delete[] dst;
		return std::string();
	}
	*q++ = '\0';
	std::string result(dst);
	delete[] dst;
	return result;
#  endif
#endif
}
