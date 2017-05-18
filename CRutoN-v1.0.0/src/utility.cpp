/*==============================================================================
	Some useful functions

	File			: utility.cpp
	Author			: Paul Gainer
	Created			: 06/02/2015
	Last modified	: 15/04/2015
==============================================================================*/
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/==============================================================================\
|   Copyright (C) 2015 Paul Gainer, University of Liverpool                    |
|                                                                              |
|   This file is part of CRuToN.                                               |
|                                                                              |
|   CRuToN is free software; you can redistribute it and/or modify it          |
|   under the terms of the GNU General Public License as published by the Free |
|   Software Foundation, either version 3 of the License; or (at your option)  |
|   any later version.                                                         |
|                                                                              |
|   CruToN is distributed in the hope that it will be useful, but WITHOUT      |
|   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      |
|   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   |
|   more details.                                                              |
|                                                                              |
|   You should have received a copy of the GNU General Public License along    |
|   with CRuToN. If not, see <http:/www.gnu.org/licenses/>.                    |
|																			   |
\==============================================================================/
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#include <algorithm>
#include <cmath>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include "define.h"
#include "global.h"
#include "utility.h"

#define LINE_WIDTH 80.0

/*------------------------------------------------------------------------------
	stringToInteger

	Converts a string to an integer. Does not include validation.
------------------------------------------------------------------------------*/
int stringToInteger(const std::string& s)
{
	std::locale loc;
	std::istringstream stream(s);
	int parsed_value;
	stream >> parsed_value;

	return parsed_value;
}

/*------------------------------------------------------------------------------
	stringToFloat

	Converts a string to a float.
------------------------------------------------------------------------------*/
int stringToFloat(const std::string& s)
{
	std::locale loc;
	std::istringstream stream(s);
	float parsed_value;
	stream >> parsed_value;

	return parsed_value;
}

/*------------------------------------------------------------------------------
	integerToString

	Converts an integer to a string.
------------------------------------------------------------------------------*/
const std::string integerToString(const int n)
{
	std::stringstream stream;
	stream << n;
	return stream.str();
}

/*------------------------------------------------------------------------------
	isFloat

	Returns true if the given string represents a valid float.
------------------------------------------------------------------------------*/
const bool isFloat(const std::string& s)
{
	if(s.empty())
	{
		return false;
	}
	std::istringstream stream(s);
	float parsed_value;
	stream >> parsed_value;
	return stream.eof();
}

/*------------------------------------------------------------------------------
	isInteger

	Returns true if the given string represents a valid integer.
------------------------------------------------------------------------------*/
const bool isInteger(const std::string& s)
{
	if(s.empty())
	{
		return false;
	}
	std::istringstream stream(s);
	int parsed_value;
	stream >> parsed_value;
	return stream.eof();
}

/*------------------------------------------------------------------------------
	isNaturalNumber

	Returns true if the given string represents a valid natural number,
------------------------------------------------------------------------------*/
const bool isNaturalNumber(const std::string& s)
{
	std::istringstream stream(s);
	int parsed_value;
	stream >> parsed_value;

	return stream.eof() && parsed_value >= 0;
}

/*------------------------------------------------------------------------------
	toLower

	Converts a string to lowercase.
------------------------------------------------------------------------------*/
const std::string toLower(const std::string& s)
{
	std::string s_primed;
	for(unsigned int index = 0; index < s.size(); index++)
	{
		char c = s.at(index);
		if(c >= 'A' && c <= 'Z')
		{
			s_primed += (char)(c + 32);
		}
		else
		{
			s_primed += c;
		}
	}
	return s_primed;
}

/*------------------------------------------------------------------------------
	toUpper

	Converts an string to uppercase.
------------------------------------------------------------------------------*/
const std::string toUpper(const std::string& s)
{
	std::string s_primed;
	for(unsigned int index = 0; index < s.size(); index++)
	{
		char c = s.at(index);
		if(c >= 'a' && c <= 'z')
		{
			s_primed += (char)(c - 32);
		}
		else
		{
			s_primed += c;
		}
	}
	return s_primed;
}

/*------------------------------------------------------------------------------
	caseInsensitiveCompare

	Returns true if there is a case-insensitive match between s_! and s_2.
------------------------------------------------------------------------------*/
const bool caseInsensitiveCompare(const std::string& s_1,
	const std::string& s_2)
{
	if(s_1.size() != s_2.size())
	{
		return false;
	}
	else
	{
		for(unsigned int i = 0; i < s_1.size(); i++)
		{
			if(std::tolower(s_1.at(i)) != std::tolower(s_2.at(i)))
			{
				return false;
			}
		}
	}
	return true;
}

/*------------------------------------------------------------------------------
	compareStrings

	Calculates the LCS of the two strings, and returns the LCS and the
	similarity (as a percentage) or the two strings.
------------------------------------------------------------------------------*/
const std::pair<const std::string, const int> compareStrings(
	const std::string& this_string, const std::string& that_string)
{
	int m = this_string.size();
	int n = that_string.size();

	if(n == 0 || m == 0)
	{
		return std::pair<const std::string, const int>(
			"", 0.0f);
	}
	else
	{
		int lcs[m + 1][n + 1];
		for(int i = 1; i <= m; i++)
		{
			lcs[i][0] = 0;
		}
		for(int j = 0; j <= n; j++)
		{
			lcs[0][j] = 0;
		}
		for(int i = 1; i <= m; i++)
		{
			for(int j = 1; j <= n; j++)
			{
				if(this_string.at(i - 1) == that_string.at(j - 1))
				{
					lcs[i][j] = lcs[i - 1][j - 1] + 1;
				}
				else if(lcs[i - 1][j] >= lcs[i][j - 1])
				{
					lcs[i][j] = lcs[i - 1][j];
				}
				else
				{
					lcs[i][j] = lcs[i][j - 1];
				}
			}
		}

		std::string subsequence = "";
		int i = m;
		int j = n;
		while(i > 0 && j > 0)
		{
			if(lcs[i][j] == lcs[i - 1][j - 1] + 1)
			{
				subsequence = this_string.at(i - 1) + subsequence;
				i--;
				j--;
			}

			else if(lcs[i][j] == lcs[i - 1][j])
			{
				i--;
			}
			else
			{
				j--;
			}
		}

		int max_length = m > n ? m : n;
		int similarity = (float)lcs[m][n] / (float)max_length * 100.0f;
		return std::pair<const std::string, const int>(subsequence, similarity);
	}
}

/*------------------------------------------------------------------------------
	displayTitle

	Displays the given string in between two dividers.
------------------------------------------------------------------------------*/
void displayTitle(const std::string& divider_first,
	const std::string& divider_second, const std::string& title)
{
	std::cout << divider_first;
	std::cout << title << std::endl;
	std::cout << divider_second;
}

/*------------------------------------------------------------------------------
	displayError

	Displays the given error string.
------------------------------------------------------------------------------*/
void displayError(const std::string& error)
{
	std::string title = EXEC_NAME + " " + EXEC_VERSION + " Error";
	int num_leading = floor((LINE_WIDTH - (float)title.size()) / 2.0);
	int num_following = ceil((LINE_WIDTH - (float)title.size()) / 2.0);
	title = "] " + title + " [";
	for(int i = 0; i < num_leading - 2; i++)
	{
		title = "-" + title;
	}
	for(int i = 0; i < num_following - 2; i++)
	{
		title = title + "-";
	}
	title += "\n";
	displayTitle(title, DIVIDER_2, error);
}

/*------------------------------------------------------------------------------
	displayWarning

	Displays the given warning string.
------------------------------------------------------------------------------*/
void displayWarning(const std::string& warning)
{
	std::string title = EXEC_NAME + " " + EXEC_VERSION + " Warning";
	int num_leading = floor((LINE_WIDTH - (float)title.size()) / 2.0);
	int num_following = ceil((LINE_WIDTH - (float)title.size()) / 2.0);
	title = "] " + title + " [";
	for(int i = 0; i < num_leading - 2; i++)
	{
		title = "-" + title;
	}
	for(int i = 0; i < num_following - 2; i++)
	{
		title = title + "-";
	}
	title += "\n";
	displayTitle(title, DIVIDER_2, warning);
}

