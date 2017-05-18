/*==============================================================================
	ActionEnumValueAssignment

	File			: action_enum_value_assignment.cpp
	Author			: Paul Gainer
	Created			: 14/04/2015
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
#include <ostream>
#include <sstream>
#include <string>

#include "intermediate/action_enum_value_assignment.h"

/*------------------------------------------------------------------------------
	getActionDescription

	Returns a string describing the action.
------------------------------------------------------------------------------*/
std::string ActionEnumValueAssignment::getActionDescription() const
{
	std::stringstream stream;
	stream << *this;
	return stream.str();
}

/*------------------------------------------------------------------------------
	clone

	Returns a pointer to a copy of this.
------------------------------------------------------------------------------*/
Action* ActionEnumValueAssignment::clone() const
{
	return new ActionEnumValueAssignment(enum_variable,
		enum_value, action_order);
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os,
	const ActionEnumValueAssignment& a)
{
	return os << "enum_value_assignment: [order: " << a.action_order
		<< "][variable: " << a.enum_variable->getName() << "][value: "
		<< a.enum_value << "]";
}
