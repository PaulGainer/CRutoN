/*==============================================================================
	EnumVariable

	File			: enum_variable.cpp
	Author			: Paul Gainer
	Created			: 14/04/2015
	Last modified	: 14/04/2015
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
#include <list>
#include <ostream>
#include <string>

#include "utility.h"
#include "intermediate/enum_variable.h"

/*------------------------------------------------------------------------------
	addValue

	Adds a new value to the set of values.
------------------------------------------------------------------------------*/
std::string EnumVariable::addValue(const std::string& v)
{
	bool found = false;
	std::string return_string = v;
	std::for_each(values.begin(), values.end(),
		[&](std::string value)
		{
			if(toLower(value) == toLower(v))
			{
				// this value already exists
				return_string = value;
				found = true;
			}
		});

	if(!found)
	{
		values.push_back(v);
	}
	return return_string;
}

/*------------------------------------------------------------------------------
	setResets

	Sets reset to true.
------------------------------------------------------------------------------*/
void EnumVariable::setResets()
{
	enum_resets = true;
	addValue("none");
}

/*------------------------------------------------------------------------------
	setHasNoneValue

	Sets has_none_valuet to true.
------------------------------------------------------------------------------*/
void EnumVariable::setHasNoneValue()
{
	has_none_value = true;
	addValue("no_value");
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os, const EnumVariable& e)
{
	std::string values_string;
	auto it = e.values.begin();
	auto end = e.values.end();
	while(it != end)
	{
		values_string += *it;
		if(++it != end)
		{
			values_string += ", ";
		}
	}
	return os << e.getName() << " [does " << (e.resets() ? "" : "not ")
		<< "reset][values: " << values_string << "]["
		<< (e.non_deterministic ? "non-deterministic" : "deterministic")
		<< "]";
}
