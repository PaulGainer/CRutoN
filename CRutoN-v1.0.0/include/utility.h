/*==============================================================================
	Some useful functions

	File			: utility.h
	Author			: Paul Gainer
	Created			: 05/02/2015
	Last modified	: 19/04/2015
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
#ifndef UTILITY_H_
#define UTILITY_H_

#include <sstream>
#include <string>
#include <vector>

/*------------------------------------------------------------------------------
	stringToInteger

	Converts a string to an integer. Does not include validation.
................................................................................
	@param	s				the string to convert
	@return					the integer represented by the string
------------------------------------------------------------------------------*/
int stringToInteger(const std::string& s);

/*------------------------------------------------------------------------------
	stringToFloat

	Converts a string to a float.
................................................................................
	@param	s				the string to convert
	@return					the float represented by the string
------------------------------------------------------------------------------*/
int stringToFloat(const std::string& s);

/*------------------------------------------------------------------------------
	integerToString

	Converts an integer to a string.
................................................................................
	@param	n				the integer to convert
	@return					the string representation of the integer
------------------------------------------------------------------------------*/
const std::string integerToString(const int n);

/*------------------------------------------------------------------------------
	isInteger

	Returns true if the given string represents a valid integer.
................................................................................
	@param	s				the string representing an integer
	@return					true if the string represents a valid integer or
							false otherwise
------------------------------------------------------------------------------*/
const bool isInteger(const std::string& s);

/*------------------------------------------------------------------------------
	isFloat

	Returns true if the given string represents a valid float.
................................................................................
	@param	s				the string representing a float
	@return					true if the string represents a valid float or
							false otherwise
------------------------------------------------------------------------------*/
const bool isFloat(const std::string& s);

/*------------------------------------------------------------------------------
	isNaturalNumber

	Returns true if the given string represents a valid natural number,
................................................................................
	@param	s				the string representing a natural number
	@return					true if the string represents a valid natural number
							or false otherwise
------------------------------------------------------------------------------*/
const bool isNaturalNumber(const std::string& s);

/*------------------------------------------------------------------------------
	toLower

	Converts a string to lowercase.
................................................................................
	@param	s				the string to convert
	@return					the string converted to lowercase
------------------------------------------------------------------------------*/
const std::string toLower(const std::string& s);

/*------------------------------------------------------------------------------
	toUpper

	Converts an string to uppercase.
	................................................................................
	@param	s				the string to convert
	@return					the string converted to uppercase
------------------------------------------------------------------------------*/
const std::string toUpper(const std::string& s);

/*------------------------------------------------------------------------------
	caseInsensitiveCompare

	Returns true if there is a case-insensitive match between s_! and s_2.
................................................................................
	@param	s_1				the first string
	@param	s_2				the second string
	@return					true if there is a case-insensitive match between
							s_1 and s_2, or false otherwise
------------------------------------------------------------------------------*/
const bool caseInsensitiveCompare(const std::string& s_1,
	const std::string& s_2);

/*------------------------------------------------------------------------------
	compareStrings

	Calculates the LCS of the two strings, and returns the LCS and the
	similarity (as a percentage) or the two strings.
................................................................................
	@param	this_string		the first string
	@param	that_string		the second string
	@return					a pair where the first element is the LCS of the
							two strings, and the second element is an integer
							corresponding to the similarity (as a percentage)
							of the two strings
------------------------------------------------------------------------------*/
const std::pair<const std::string, const int> compareStrings(
	const std::string& this_string, const std::string& that_string);

/*------------------------------------------------------------------------------
	displayTitle

	Displays the given string in between two dividers.
................................................................................
	@param	divider_first	the divider to draw before the title
	@param	divider_second	the divider to draw after the title
	@param	title			the title text
------------------------------------------------------------------------------*/
void displayTitle(const std::string& divider_first,
	const std::string& divider_second, const std::string& title);

/*------------------------------------------------------------------------------
	displayError

	Displays the given error string.
................................................................................
	@param	error			the error message
------------------------------------------------------------------------------*/
void displayError(const std::string& error);

/*------------------------------------------------------------------------------
	displayWarning

	Displays the given warning string.
................................................................................
	@param	warning			the warning message
------------------------------------------------------------------------------*/
void displayWarning(const std::string& warning);

#endif
