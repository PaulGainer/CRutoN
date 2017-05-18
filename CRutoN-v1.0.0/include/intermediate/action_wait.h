/*==============================================================================
	ActionWait

	An action telling the robot to wait for a given number of seconds.

	File			: action_wait.h
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
#ifndef ACTION_WAIT_H_
#define ACTION_WAIT_H_

#include <sstream>
#include <string>

#include "enums/action_type.h"
#include "intermediate/action.h"

class ActionWait : public Action
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	seconds			the number of seconds the robot should wait
		@param	order			an integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	ActionWait(const float seconds, const int order) : wait_seconds(seconds)
	{
		action_type = WAIT;
		action_order = order;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getWaitSeconds

		Accessor for wait_seconds.
	............................................................................
		@return					the number of seconds the robot should wait
	--------------------------------------------------------------------------*/
	const float getWaitSeconds() {return wait_seconds;}

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
		const ActionWait& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The number of seconds to wait.
	--------------------------------------------------------------------------*/
	const float wait_seconds;
};

#endif
