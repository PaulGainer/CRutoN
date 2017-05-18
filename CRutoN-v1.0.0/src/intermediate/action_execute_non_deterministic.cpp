/*==============================================================================
	ActionExecuteNonDeterministic

	File			: action_execute_non_deterministic.cpp
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
#include <sstream>
#include <string>

#include "intermediate/action_execute_non_deterministic.h"

/*--------------------------------------------------------------------------
	addBehaviourValue

	Adds the name of a new behaviour.
--------------------------------------------------------------------------*/
void ActionExecuteNonDeterministic::addBehaviourValue(
	const std::string behaviour_value)
{
	bool found = false;
	std::for_each(behaviour_values.begin(), behaviour_values.end(),
		[&](std::string value)
		{
			if(value == behaviour_value)
			{
				// this value already exists
				found = true;
			}
		});
	if(!found)
	{
		behaviour_values.push_back(behaviour_value);
	}
}

/*------------------------------------------------------------------------------
	updateBehaviourName

	Updates the name of an executed behaviour.
------------------------------------------------------------------------------*/
void ActionExecuteNonDeterministic::updateBehaviourName(
	const std::string existing_name, const std::string new_name)
{
	auto it = behaviour_values.begin();
	auto end = behaviour_values.end();
	while(it != end)
	{
		if(*it == existing_name)
		{
			(*it) = new_name;
		}
		it++;
	}
}

/*------------------------------------------------------------------------------
	getActionDescription

	Returns a string describing the action.
------------------------------------------------------------------------------*/
std::string ActionExecuteNonDeterministic::getActionDescription() const
{
	std::stringstream stream;
	stream << *this;
	return stream.str();
}

/*------------------------------------------------------------------------------
	clone

	Returns a pointer to a copy of this.
------------------------------------------------------------------------------*/
Action* ActionExecuteNonDeterministic::clone() const
{
	ActionExecuteNonDeterministic* new_action =
		new ActionExecuteNonDeterministic(action_order);
	for(std::string value : behaviour_values)
	{
		new_action->addBehaviourValue(value);
	}
	return new_action;
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os,
	const ActionExecuteNonDeterministic& a)
{
	std::string behaviour_list;
	auto it = a.behaviour_values.begin();
	auto end = a.behaviour_values.end();
	while(it != end)
	{
		behaviour_list += (*it);

		if(++it != end)
		{
			behaviour_list += ", ";
		}
	}
	return os << "execute_behaviour_non_d: [order: " << a.action_order
		<< "][behaviours: " << behaviour_list << "]";
}
