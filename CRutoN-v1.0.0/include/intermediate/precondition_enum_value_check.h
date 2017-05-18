/*==============================================================================
	PreconditionEnumValueCheck

	A precondition that ensures that an enumerated variable has a certain value.

	File			: precondition_enum_value_check.h
	Author			: Paul Gainer
	Created			: 14/12/2014
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
#ifndef PRECONDITION_ENUM_VALUE_CHECK_H_
#define PRECONDITION_ENUM_VALUE_CHECK_H_

#include <ostream>
#include <sstream>
#include <string>

#include "enums/precondition_type.h"
#include "enums/time_constraint_type.h"
#include "intermediate/enum_variable.h"
#include "utility.h"

class PreconditionEnumValueCheck : public Precondition
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	enum_var		a pointer to the enumerated variable
		@param	enum_val		the enumeration value
		@param	time_constraint	the time constraint type
		@param	time_seconds	the number of seconds for the time constraint
	--------------------------------------------------------------------------*/
	PreconditionEnumValueCheck(EnumVariable* enum_var,
		const std::string enum_val, const TimeConstraintType time_constraint,
		const int time_seconds) : enum_variable(enum_var), enum_value(enum_val)
	{
		precondition_type = ENUM_VALUE_CHECK,
		time_constraint_type = time_constraint,
		time_constraint_seconds = time_seconds;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getEnumVariable

		Accessor for enum_variable
	............................................................................
		@return					a pointer to the enumerated variable
	--------------------------------------------------------------------------*/
	EnumVariable* getEnumVariable() {return enum_variable;}

	/*--------------------------------------------------------------------------
		getEndValue

		Accessor for enum_value
	............................................................................
		@return					the enumerated value
	--------------------------------------------------------------------------*/
	const std::string getEnumValue() {return enum_value;}

	/*--------------------------------------------------------------------------
		getPreconditionDescription

		Returns a string describing this precondition.
	............................................................................
		@return					a string describing the precondition
	--------------------------------------------------------------------------*/
	std::string getPreconditionDescription() const
	{
		std::stringstream stream;
		stream << *this;
		return stream.str();
	}

	friend inline std::ostream& operator<<(std::ostream& os,
		const PreconditionEnumValueCheck& p);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		A pointer to the enumerated variable whose value is checked.
	--------------------------------------------------------------------------*/
	EnumVariable* enum_variable;

	/*--------------------------------------------------------------------------
		The enumerated variable should have this value.
	--------------------------------------------------------------------------*/
	const std::string enum_value;
};

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& os,
	const PreconditionEnumValueCheck& p)
{
	std::string time_constraint;
	switch(p.time_constraint_type)
	{
	case TimeConstraintType::NONE:
		break;

	case TimeConstraintType::WAS_IN_STATE_WITHIN:
		time_constraint = "(was_in_state_within: "
			+ integerToString(p.time_constraint_seconds) + ")";
		break;

	case TimeConstraintType::BEEN_IN_STATE_FOR:
		time_constraint = "(been_in_state_for: "
			+ integerToString(p.time_constraint_seconds) + ")";
		break;
	}

	return os << "enum_value_check: [variable: " << p.enum_variable->getName()
		<< "][value: " << p.enum_value << "]" << time_constraint;
}

#endif
