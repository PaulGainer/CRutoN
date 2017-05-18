/*==============================================================================
	ActionPropValueAssignment

	An action that assigns a value to a propositional variable.

	File			: action_prop_value_assignment.h
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
#ifndef ACTION_PROP_VALUE_ASSIGNMENT_H_
#define ACTION_PROP_VALUE_ASSIGNMENT_H_

#include "enums/action_type.h"
#include "intermediate/action.h"
#include "intermediate/prop_variable.h"

#include <sstream>
#include <string>

class ActionPropValueAssignment : public Action
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	prop_var		a pointer to the propositional variable
		@param	truth_val		the boolean value to be assigned to the
								enumerated variable
		@param	order			an integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	ActionPropValueAssignment(PropVariable* prop_var, const bool truth_val,
			const int order) : prop_variable(prop_var), truth_value(truth_val)
	{
		action_type = PROP_VALUE_ASSIGNMENT;
		action_order = order;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getPropVariable

		Accessor for prop_variable.
	............................................................................
		@return					a pointer to the propositional variable
	--------------------------------------------------------------------------*/
	PropVariable* getPropVariable() {return prop_variable;}

	/*--------------------------------------------------------------------------
		getTruthValue

		Accessor for truth_value.
	............................................................................
		@return 				the boolean value to assign to the variable
	--------------------------------------------------------------------------*/
	const bool getTruthValue() {return truth_value;}

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
		const ActionPropValueAssignment& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		A pointer to the propositional variable to which the value is assigned.
	--------------------------------------------------------------------------*/
	PropVariable* prop_variable;

	/*--------------------------------------------------------------------------
		The value to assign to the propositional variable.
	--------------------------------------------------------------------------*/
	const bool truth_value;
};

#endif
