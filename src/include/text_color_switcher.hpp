/**
 * @file text_color_switcher.hpp
 * @brief
 * @author kypoyin, kypoyin@tencent.com
 * @date 2009-10-29
 */

#ifndef COMMON_UTIL_TEXT_COLOR_SWITCHER_H__
#define COMMON_UTIL_TEXT_COLOR_SWITCHER_H__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>

class TextColorSwitcher
{
public:
	TextColorSwitcher(WORD attr)
	{
		HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
		GetConsoleScreenBufferInfo(h, &csbi);
		m_old = csbi.wAttributes;
		SetConsoleTextAttribute(h, attr);
	}
	~TextColorSwitcher()
	{
		HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
		SetConsoleTextAttribute(h, m_old);
	}
private:
	WORD m_old;
};

#else

#include <stdio.h>

#define FOREGROUND_RED "1;31"
#define FOREGROUND_GREEN "1;32"
#define FOREGROUND_YELLOW "1;33"

class TextColorSwitcher
{
public:
	TextColorSwitcher(const char* color)
	{
		fprintf(stderr, "\033[%sm", color);
	}
	~TextColorSwitcher()
	{
		fprintf(stderr, "\033[m");
	}
};

#endif



#endif ///end define COMMON_UTIL_TEXT_COLOR_SWITCHER_H__

