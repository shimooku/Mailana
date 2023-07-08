#pragma once

#include <vector>

enum class EXCEPT_ATTR
{
    EA_NOT_REGISTERED,
	EA_SIGNMARK,
    EA_EXCEPTION,
	EA_LESS_THAN_EXCEPTION,
};

class ExceptedMarks 
{
public:
	std::vector<std::pair<std::wstring, int>> marksPairArray;
	std::vector<std::wstring> marksArray;

	ExceptedMarks(char* filename);
	EXCEPT_ATTR isExcepted(wchar_t* pattern, int nRepeat, int* updatedMinRepeats);
};