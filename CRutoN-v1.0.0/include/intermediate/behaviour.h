/*==============================================================================
	Behaviour

	A set of preconditions, and actions that the robot should execute when the
	preconditions hold.

	File			: behaviour.h
	Author			: Paul Gainer
	Created			: 03/01/2015
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
#ifndef BEHAVIOUR_H_
#define BEHAVIOUR_H_

#include "intermediate/named.h"
#include "intermediate/action.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/precondition.h"

#include <algorithm>
#include <list>
#include <ostream>
#include <string>

class Behaviour : public Named
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	nm				the name of the behaviour
		@param	prty			the behaviour's priority
		@param	inter			a boolean that should be true if the behaviour
								is interruptible or false otherwise
		@param	sched			a boolean that should be true if the behaviour
								is schedulable or false otherwise
	--------------------------------------------------------------------------*/
	Behaviour(const std::string& nm, const int prty, const bool inter,
		const bool sched) :	priority(prty), interruptible(inter),
		schedulable(sched), logical_expression_tree(nullptr)
	{
		name = nm;
	}

	~Behaviour();

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getPriority

		Accessor for priority.
	............................................................................
		@return					the behaviour's priority
	--------------------------------------------------------------------------*/
	const int getPriority() {return priority;}

	/*--------------------------------------------------------------------------
		isInterruptible

		Returns true if the behaviour is interruptible or false otherwise.
	............................................................................
		@return					true if the behaviour is interruptible or
								false otherwise
	--------------------------------------------------------------------------*/
	const bool isInterruptible() {return interruptible;}

	/*--------------------------------------------------------------------------
		isSchedulable

		Returns true if the behaviour is schedulable or false otherwise.
	............................................................................
		@return					true if the behaviour is schedulable or
								false otherwise
	--------------------------------------------------------------------------*/
	const bool isSchedulable() {return schedulable;}

	/*--------------------------------------------------------------------------
		getPreconditions

		Accessor for preconditions.
	............................................................................
		@return					the list of preconditions
	--------------------------------------------------------------------------*/
	std::list<Precondition*> getPreconditions() {return preconditions;}

	/*--------------------------------------------------------------------------
		getActions

		Accessor for actions.
	............................................................................
		@return					the list of actions
	--------------------------------------------------------------------------*/
	std::list<Action*> getActions() {return actions;}

	/*--------------------------------------------------------------------------
		getLogicalExpressionTree

		Accessor for logical_expression_tree.
	............................................................................
		@return					the logical expression tree
	--------------------------------------------------------------------------*/
	LogicalExpressionTree* getLogicalExpressionTree()
	{
		return logical_expression_tree;
	}

	void setSchedulable(const bool s) {schedulable = s;}

	/*--------------------------------------------------------------------------
		addPrecondition

		Adds a new precondition to the list.
	............................................................................
		@param	precondition	a pointer to the precondition to add to the list
								of preconditions
	--------------------------------------------------------------------------*/
	void addPrecondition(Precondition* precondition);

	/*--------------------------------------------------------------------------
		addAction

		Adds a new action to the list.
	............................................................................
		@param	action			a pointer to the action to add to the list of
								actions
	--------------------------------------------------------------------------*/
	void addAction(Action* action);

	/*--------------------------------------------------------------------------
		buildLogicalExpressionTree

		Builds the logical expression tree using the preconditions.
	--------------------------------------------------------------------------*/
	void buildLogicalExpressionTree();

	friend std::ostream& operator<<(std::ostream& os, const Behaviour& b);

	friend class IntermediateForm;

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The behaviour's priority.
	--------------------------------------------------------------------------*/
	int priority;

	/*--------------------------------------------------------------------------
		Set to true if the behaviour can be interrupted.
	--------------------------------------------------------------------------*/
	bool interruptible;

	/*--------------------------------------------------------------------------
		Set to true if the behaviour can be scheduled;
	--------------------------------------------------------------------------*/
	bool schedulable;

	/*--------------------------------------------------------------------------
		The list of preconditions.
	--------------------------------------------------------------------------*/
	std::list<Precondition*> preconditions;

	/*--------------------------------------------------------------------------
		The list of actions.
	--------------------------------------------------------------------------*/
	std::list<Action*> actions;

	/*--------------------------------------------------------------------------
		The logical expression tree.
	--------------------------------------------------------------------------*/
	LogicalExpressionTree* logical_expression_tree;

	/*==========================================================================
		Private Member Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		buildNextNode

		Builds and returns a pointer to a new node storing the precondition
		pointed to by the iterator it.
	............................................................................
		@param	it				the reverse iterator pointing to the
								precondition to store at the node
		@return					a pointer to the new node
	--------------------------------------------------------------------------*/
	TreeNode* buildNextNode(std::list<Precondition*>::reverse_iterator it);
};

#endif
