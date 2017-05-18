/*==============================================================================
	ActionEnumValueAssignment

	An action that assigns a value to an enumerated variable.

	File			: action_enum_value_assignment.h
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
#ifndef ACTION_ENUM_VALUE_ASSIGNMENT_H_
#define ACTION_ENUM_VALUE_ASSIGNMENT_H_

#include <ostream>
#include <string>

#include "enums/action_type.h"
#include "intermediate/action.h"
#include "intermediate/enum_variable.h"

class ActionEnumValueAssignment : public Action
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	enum_var		a pointer to the enumerated variable
		@param	enum_val		the value to be assigned to the enumerated
								variable
		@param	order			an integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	ActionEnumValueAssignment(EnumVariable* enum_var,
			const std::string enum_val, const int order) :
			enum_variable(enum_var), enum_value(enum_val)
	{
		action_type = ENUM_VALUE_ASSIGNMENT;
		action_order = order;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getEnumVariable

		Accessor for enum_variable.
	............................................................................
		@return					a pointer to the enumerated variable
	--------------------------------------------------------------------------*/
	EnumVariable* getEnumVariable() {return enum_variable;}

	/*--------------------------------------------------------------------------
		getEnumValue

		Accessor for enum_value.
	............................................................................
		@return					the value to assign to the variable
	--------------------------------------------------------------------------*/
	const std::string getEnumValue() {return enum_value;}

	/*--------------------------------------------------------------------------
		getActionDescription

		Returns a string describing the action.
	............................................................................
		@return					a string describing the action
	--------------------------------------------------------------------------*/
	std::string getActionDescription() const;

	/*--------------------------------------------------------------------------
		clone

		Returns a pointer to a copy of this.
	............................................................................
		@return					a pointer to a copy of this instance
	--------------------------------------------------------------------------*/
	Action* clone() const;

	friend std::ostream& operator<<(std::ostream& os,
		const ActionEnumValueAssignment& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		A pointer to the enumerated variable to which the value is assigned.
	--------------------------------------------------------------------------*/
	EnumVariable* enum_variable;

	/*--------------------------------------------------------------------------
		The value to assign to the enumerated variable.
	--------------------------------------------------------------------------*/
	const std::string enum_value;
};

#endif
