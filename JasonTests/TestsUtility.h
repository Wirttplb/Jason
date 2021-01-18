#pragma once
#include <windows.h>
#include <debugapi.h>
#include <string>
#include <array>

static void PopAssert()
{
	std::array<TCHAR, 1024> txt;
	//_scwprintf(txt.data(), txt.size() - 1, "UNITTEST FAILED line %d in file %s (%s).\n", line, file, msg);
	//txt[txt.size() - 1] = '\0';
	txt[0] = '\0';
	std::wstring msg = L"Test failed!";
	MessageBox(NULL, txt.data(), msg.c_str(), MB_OK);
	DebugBreak();
};

#define ASSERT( test ) { if (!(test)) { PopAssert(); } };

