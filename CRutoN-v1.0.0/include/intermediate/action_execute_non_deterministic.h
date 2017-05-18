/*==============================================================================
	ActionExecuteNonDeterministic

	An action that non deterministically executes one of a list of behaviour.

	File			: action_execute_non_deterministic.h
	Author			: Paul Gainer
	Created			: 07/02/2015
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
#ifndef ACTION_EXECUTE_NON_DETERMINISTIC_H
#define ACTION_EXECUTE_NON_DETERMINISTIC_H

#include <list>
#include <sstream>
#include <string>

#include "enums/action_type.h"
#include "intermediate/action.h"
#include "intermediate/enum_variable.h"

class ActionExecuteNonDeterministic : public Action
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	order			an integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	ActionExecuteNonDeterministic(const int order)
	{
		action_type = EXECUTE_NON_D;
		action_order = order;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getBehaviourValues

		Accessor for behaviour_values.
	............................................................................
		@return					the list of behaviour names
	--------------------------------------------------------------------------*/
	std::list<std::string> getBehaviourValues() {return behaviour_values;}

	/*--------------------------------------------------------------------------
		addBehaviourValue

		Adds the name of a new behaviour.
	............................................................................
		@param	behaviour_value	the name of a behaviour to be added to the
								list of behaviours that can be executed
								non-deterministically
	--------------------------------------------------------------------------*/
	void addBehaviourValue(const std::string behaviour_value);

	/*--------------------------------------------------------------------------
		updateBehaviourName

		Updates the name of an executed behaviour.
	............................................................................
		@param	existing_name	the name of an existing behaviour in the list
								of behaviours
		@param	new_name		a replacement name for the existing behaviour
								name
	--------------------------------------------------------------------------*/
	void updateBehaviourName(const std::string existing_name,
		const std::string new_name);

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
		const ActionExecuteNonDeterministic& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The names of the behaviours to be executed non-deterministically.
	--------------------------------------------------------------------------*/
	std::list<std::string> behaviour_values;
};

#endif
