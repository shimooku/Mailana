#pragma once

#include <iostream>
#include <vector>

using namespace std;

enum class EXCEPTATTR
{
	NOT_REGISTERED,
	SIGNMARK,
	EXCEPTION,
	LESS_THAN_EXCEPTION,
};

class MarksExcepted
{
public:
	vector<pair<wstring, int>> marksPairArray;
	vector<wstring> marksArray;

	MarksExcepted(char* filename);
	EXCEPTATTR isExcepted(wchar_t* pattern, int nRepeat, int* updatedMinRepeats);
};

extern MarksExcepted* marksExcepted;

class SignMarkDetector
{
private:
	int minRepeats = 0;
	int maxUnitLetters = 0;

public:

	SignMarkDetector(int _minRepeats, int _maxUnitLetters)
	{
		minRepeats = _minRepeats;
		maxUnitLetters = _maxUnitLetters;
	}

	//! @note 行がサインマークか否かを判定
	bool isSignMark(wchar_t* text);
};

