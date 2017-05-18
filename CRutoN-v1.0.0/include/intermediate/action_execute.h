/*==============================================================================
	ActionExecute

	An action telling the robot to execute a given behaviour.

	File			: action_execute.h
	Author			: Paul Gainer
	Created			: 04/01/2015
	Last modified	: 18/04/2015
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
#ifndef ACTION_EXECUTE_H_
#define ACTION_EXECUTE_H_

#include <string>
#include <sstream>

#include "enums/action_type.h"
#include "intermediate/behaviour.h"
#include "intermediate/action.h"

class ActionExecute : public Action
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	behav			the name of the behaviour that is to be executed
		@param	order			an integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	ActionExecute(const std::string behav, const int order) : behaviour(behav)
	{
		action_type = EXECUTE;
		action_order = order;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getBehaviour

		Accessor for behaviour.
	............................................................................
		@return					the name of the behaviour to be executed
	--------------------------------------------------------------------------*/
	const std::string getBehaviour() const {return behaviour;}

	/*--------------------------------------------------------------------------
		setBehaviour

		Mutator for behaviour.
	............................................................................
		@param	b				the name of the behaviour to be executed
	--------------------------------------------------------------------------*/
	void setBehaviour(const std::string b) {behaviour = b;}

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
		const ActionExecute& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The behaviour to execute.
	--------------------------------------------------------------------------*/
	std::string behaviour;
};

#endif
