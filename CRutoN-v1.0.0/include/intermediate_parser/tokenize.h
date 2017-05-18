/*==============================================================================
	A collection of functions that work with tokens in strings.

	File			: tokenize.h
	Author			: Paul Gainer
	Created			: 04/01/2015
	Last modified	: 19/04/2015
==============================================================================*/
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/==============================================================================\
|	Copyright (C) 2014 Paul Gainer, University of Liverpool					   |
|																			   |
|	This file is part of NAME HERE.											   |
|																			   |
|	NAME HERE is free software; you can redistribute it and/or modify it	   |
|	under the terms of the GNU General Public License as published by the Free |
|	Software Foundation, either version 3 of the License; or (at your option)  |
|	any later version.														   |
|																			   |
|	NAME HERE is distributed in the hope that it will be useful, but WITHOUT   |
|	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or	   |
|	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   |
|	more details.															   |
|																			   |
|	You should have received a copy of the GNU General Public License along	   |
|	with NAME HERE. If not, see <http:/www.gnu.org/licenses/>.				   |
|																			   |
\==============================================================================/
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#ifndef TOKENIZE_H_
#define TOKENIZE_H_

#include <string>

/*--------------------------------------------------------------------------
	hasNextToken

	Returns true if the string has a token available to read.
............................................................................
	@param	input_string	the input string
	@return					true if the input string has another token
--------------------------------------------------------------------------*/
const bool hasNextToken(std::string& input_string);

/*--------------------------------------------------------------------------
	getNextToken

	Removes and returns the next available token from the string.
............................................................................
	@param	input_string	the input string
	@return					the next available token from the string
--------------------------------------------------------------------------*/
const std::string getNextToken(std::string& input_string);

/*--------------------------------------------------------------------------
	peekNextToken

	Returns the next available token from the string.
............................................................................
	@param	input_string	the input string
	@return					the next available token from the string
--------------------------------------------------------------------------*/
const std::string peekNextToken(std::string& input_string);

/*--------------------------------------------------------------------------
	getNextLine

	Removes and returns the next available line from the string.
............................................................................
	@param	input_string	the input string
	@return					the next available line from the string
--------------------------------------------------------------------------*/
const std::string getNextLine(std::string& input_string);

/*--------------------------------------------------------------------------
	getShortestPrefixEndingWithChar

	Returns the shortest prefix of the string that terminates with the
	given character.
............................................................................
	@param	input_string	the input string
	@param	c				the terminating character
	@return					the shortes prefix of the string that terminates
							with c
--------------------------------------------------------------------------*/
const std::string getShortestPrefixEndingWithChar(
	std::string& input_string, const char c);

/*--------------------------------------------------------------------------
	trimString

	Returns the string with leading and ending whitespace removed
............................................................................
	@param	input_string	the input string
	@return					the input string with leading and ending
							whitespace removed
--------------------------------------------------------------------------*/
const std::string trim(const std::string& s);

#endif
