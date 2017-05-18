/*==============================================================================
	EnumVariable

	An enumerated variable.

	File			: enum_variable.h
	Author			: Paul Gainer
	Created			: 13/12/2014
	Last modified	: 18/04/2015
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
#ifndef ENUM_VARIABLE_H_
#define ENUM_VARIABLE_H_

#include <list>
#include <ostream>
#include <string>

#include "intermediate/named.h"

class EnumVariable : public Named
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	nm				a name for the enumerated variable
	--------------------------------------------------------------------------*/
	EnumVariable(const std::string& nm) : enum_resets(false),
		has_none_value(false), non_deterministic(false)
	{
		name = nm;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getValues

		Accessor for values.
	............................................................................
		@return					returns the list of enumeration values
	--------------------------------------------------------------------------*/
	std::list<std::string> getValues() const {return values;}

	/*--------------------------------------------------------------------------
		resets

		Accessor for enum_resets.
	............................................................................
		@return					returns true if the enumerated variable should
								reset, or false otherwise
	--------------------------------------------------------------------------*/
	const bool resets() const {return enum_resets;}

	/*--------------------------------------------------------------------------
		addValue

		Adds a new value to the set of values.
	............................................................................
		@param v				the new value to add to the list of values
	--------------------------------------------------------------------------*/
	std::string addValue(const std::string& v);

	/*--------------------------------------------------------------------------
		setResets

		Sets reset to true.
	--------------------------------------------------------------------------*/
	void setResets();

	/*--------------------------------------------------------------------------
		setHasNoneValue

		Sets has_none_value to true.
	--------------------------------------------------------------------------*/
	void setHasNoneValue();

	/*--------------------------------------------------------------------------
		isNonDeterministic

		Accessor for non_deterministic.
	............................................................................
		@return					true if the enumerated variable is
								non-deterministic, or false otherwise
	--------------------------------------------------------------------------*/
	const bool isNonDeterministic() const {return non_deterministic;}

	/*--------------------------------------------------------------------------
		setNonDeterministic

		Sets non_deterministic to true;
	--------------------------------------------------------------------------*/
	void setNonDeterministic() {non_deterministic = true;}

	friend std::ostream& operator<<(std::ostream& os, const EnumVariable& e);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The values assignable to the variable.
	--------------------------------------------------------------------------*/
	std::list<std::string> values;

	/*--------------------------------------------------------------------------
		Set to true if the variable should reset in the state following an
		assignment.
	--------------------------------------------------------------------------*/
	bool enum_resets;

	/*--------------------------------------------------------------------------
		Set to true if the variable should have an additional value
		'no_value'.
	--------------------------------------------------------------------------*/
	bool has_none_value;

	/*--------------------------------------------------------------------------
		Set to true if the variable's value should be determined
		non-deterministically.
	--------------------------------------------------------------------------*/
	bool non_deterministic;
};

#endif
