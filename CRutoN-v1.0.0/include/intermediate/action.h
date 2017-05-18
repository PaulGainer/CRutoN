/*==============================================================================
	Action

	The superclass for the five types of action.

	File			: action.h
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
#ifndef ACTION_H_
#define ACTION_H_

#include <ostream>

#include "enums/action_type.h"

class Action
{
public:
	virtual ~Action() {}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getActionType

		Accessor for action_type.
	............................................................................
		@return					an enumerated value indicating the type of
								action
	--------------------------------------------------------------------------*/
	const ActionType getActionType() {return action_type;}

	/*--------------------------------------------------------------------------
		getActionOrder

		Accessor for action_order.
	............................................................................
		@return					the integer used to determine the order in
								which actions are executed by a behaviour
	--------------------------------------------------------------------------*/
	const int getActionOrder() {return action_order;}

	/*--------------------------------------------------------------------------
		setActionOrder

		Mutator for action_order.
	............................................................................
		@param	o				an integer to be assigned to order
	--------------------------------------------------------------------------*/
	void setActionOrder(const int o) {action_order = o;}

	/*--------------------------------------------------------------------------
		getActionDescription

		Returns a string describing the action.
	............................................................................
		@return					a string describing the action
	--------------------------------------------------------------------------*/
	virtual std::string getActionDescription() const = 0;

	/*--------------------------------------------------------------------------
		clone

		Returns a pointer to a copy of this.
	............................................................................
		@return					a pointer to a copy of this instance
	--------------------------------------------------------------------------*/
	virtual Action* clone() const = 0;

protected:
	/*==========================================================================
		Protected Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The type of action.
	--------------------------------------------------------------------------*/
	ActionType action_type;

	/*--------------------------------------------------------------------------
		An integer determining the order in which actions are performed.
	--------------------------------------------------------------------------*/
	int action_order;
};

#endif
