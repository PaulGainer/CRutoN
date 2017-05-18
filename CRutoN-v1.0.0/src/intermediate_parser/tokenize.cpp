/*==============================================================================
	File			: tokenize.cpp
	Author			: Paul Gainer
	Created			: 04/01/2015
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
#include <cstdio>
#include <sstream>
#include <string>

#include "define.h"
#include "intermediate_parser/tokenize.h"

/*--------------------------------------------------------------------------
	hasNextToken

	Returns true if the string has a token available to read.
--------------------------------------------------------------------------*/
const bool hasNextToken(std::string& input_string)
{
	int pos = 0;
	std::locale loc;
	int string_length = input_string.size();
	while(pos < string_length)
	{
		if(!std::isspace(input_string.at(pos), loc))
		{
			return true;
		}
		pos++;
	}

	return false;
}

/*--------------------------------------------------------------------------
	getNextToken

	Removes and returns the next available token from the string.
--------------------------------------------------------------------------*/
const std::string getNextToken(std::string& input_string)
{
	std::string token = "";
	std::locale loc;
	int pos = 0;
	int string_length = input_string.size();

	while(pos < string_length && std::isspace(input_string.at(pos), loc))
	{
		pos++;
	}

	while(pos < string_length && !std::isspace(input_string.at(pos), loc))
	{
		token += input_string.at(pos++);
	}

	if(token.size() > 0)
	{
		input_string = input_string.substr(pos);
	}

	return token;
}

/*--------------------------------------------------------------------------
	peekNextToken

	Returns the next available token from the string.
--------------------------------------------------------------------------*/
const std::string peekNextToken(std::string& input_string)
{
	std::string token = "";
	char input_char;
	std::locale loc;
	int pos = 0;
	int string_length = input_string.size();

	while(pos < string_length && std::isspace(input_string.at(pos), loc))
	{
		pos++;
	}

	while(pos < string_length &&
		!std::isspace((input_char = input_string.at(pos)), loc))
	{
		token += input_string.at(pos++);
	}

	return token;
}

/*--------------------------------------------------------------------------
	getNextLine

	Removes and returns the next available line from the string.
--------------------------------------------------------------------------*/
const std::string getNextLine(std::string& input_string)
{
	std::string line;
	char input_char;
	int pos = 0;
	int string_length = input_string.size();

	while(pos < string_length &&
		((input_char = input_string.at(pos)) != '\n')
		&& input_char != EOF)
	{
		line += input_char;
		pos++;
	}

	if(input_char == '\n' || input_char == EOF)
	{
		pos++;
	}

	input_string = input_string.substr(pos);

	return line;
}

/*--------------------------------------------------------------------------
	getShortestPrefixEndingWithChar

	Returns the shortest prefix of the string that terminates with the
	given character.
--------------------------------------------------------------------------*/
const std::string getShortestPrefixEndingWithChar(
	std::string& input_string, const char c)
{
	if(input_string.size() == 0)
	{
		return "";
	}
	else if(input_string.size() == 1)
	{
		std::string prefix = input_string;
		input_string = "";
		return prefix;
	}

	std::string prefix = "";
	std::locale loc;
	int pos = 0;
	char input_c;
	int string_length = input_string.size();

	while(pos < string_length && std::isspace(input_string.at(pos), loc))
	{
		pos++;
	}

	do
	{
		input_c = input_string.at(pos);
		if(!std::isspace(input_c, loc))
		{
			prefix += input_c;
		}
		pos++;
	} while(pos < string_length && input_c != c);

	if(pos < string_length)
	{
		input_string = input_string.substr(pos);
	}
	else
	{
		input_string = "";
	}

	return prefix;
}

/*--------------------------------------------------------------------------
	trimString

	Returns the string with leading and ending whitespace removed
--------------------------------------------------------------------------*/
const std::string trim(const std::string& s)
{
	std::locale loc;
	int s_size = s.size();
	int start_index = 0;
	while(start_index < s_size && std::isspace(s.at(start_index), loc))
	{
		start_index++;
	}
	int end_index = s_size - 1;
	while(end_index >= 0 && std::isspace(s.at(end_index), loc))
	{
		end_index--;
	}
	if(start_index >= end_index)
	{
		return "";
	}
	else
	{
		return s.substr(start_index, end_index - start_index + 1);
	}
}

