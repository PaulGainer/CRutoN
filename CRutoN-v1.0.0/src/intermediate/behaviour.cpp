/*==============================================================================
	Behaviour

	File			: behaviour.cpp
	Author			: Paul Gainer
	Created			: 29/12/2014
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
#include <algorithm>
#include <list>
#include <sstream>

#include "utility.h"
#include "enums/action_type.h"
#include "enums/tree_node_type.h"
#include "intermediate/action.h"
#include "intermediate/action_execute.h"
#include "intermediate/action_execute_non_deterministic.h"
#include "intermediate/behaviour.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/tree_node.h"

Behaviour::~Behaviour()
{
	for(Action* a : actions)
	{
		delete a;
	}
	for(Precondition* p : preconditions)
	{
		delete p;
	}
	if(logical_expression_tree != nullptr)
	{
		delete logical_expression_tree;
	}
}

/*------------------------------------------------------------------------------
	addPrecondition

	Adds a new logical expression tree to the list.
------------------------------------------------------------------------------*/
void Behaviour::addPrecondition(Precondition* precondition)
{
	// add the new precondition then sort by precondition order, lowest first
	preconditions.push_back(precondition);
	preconditions.sort(
		[&](Precondition* this_precondition,
			Precondition* that_precondition)
		{
			return this_precondition->getPreconditionOrder()
				< that_precondition->getPreconditionOrder();
		});
}

/*------------------------------------------------------------------------------
	addAction

	Adds a new action to the list.
------------------------------------------------------------------------------*/
void Behaviour::addAction(Action* action)
{
	// add the new action then sort by action order, lowest first
	actions.push_back(action);
	actions.sort(
		[&](Action* this_action, Action* that_action)
		{
			return this_action->getActionOrder() <
				that_action->getActionOrder();
		});
}

/*------------------------------------------------------------------------------
	buildLogicalExpressionTree

	Builds the logical expression tree using the preconditions.
------------------------------------------------------------------------------*/
void Behaviour::buildLogicalExpressionTree()
{
	auto it = preconditions.rbegin();
	if(it == preconditions.rend())
	{
		return;
	}
	logical_expression_tree = new LogicalExpressionTree(buildNextNode(it));
}

/*------------------------------------------------------------------------------
	buildNextNode

	Builds and returns a pointer to a new treenode storing the precondition
	pointed to by the iterator it.
------------------------------------------------------------------------------*/
TreeNode* Behaviour::buildNextNode(
	std::list<Precondition*>::reverse_iterator it)
{
	auto prev_it = it;
	prev_it++;
	TreeNode* node;
	Precondition* precondition = *it;
	int not_connector = precondition->getNotConnector();
	if(prev_it == preconditions.rend())
	{
		// this is the last precondition in the list, create a new leaf node
		node = new TreeNode(nullptr, nullptr, precondition,
			TreeNodeType::LEAF);
		if(not_connector == 1)
		{
			// this precondition is negated so create a new
			// NOT node with node as its child
			node = new TreeNode(node, nullptr, nullptr,
				TreeNodeType::INTERNAL_NOT);
		}
	}
	else
	{
		Precondition* previous_precondition = *prev_it;
		int and_or_connector = previous_precondition->getAndOrConnector();
		// construct a new leaf node for the left child
		TreeNode* left = new TreeNode(nullptr, nullptr, precondition,
			TreeNodeType::LEAF);
		if(not_connector == 1)
		{
			// this is negated so create a new NOT node with the
			// old left node as a child
			left = new TreeNode(left, nullptr, nullptr,
				TreeNodeType::INTERNAL_NOT);
		}
		// recursively construct the right child
		TreeNode* right = buildNextNode(prev_it);
		if(and_or_connector < 2)
		{
			// create a new AND node
			node = new TreeNode(left, right, nullptr,
				TreeNodeType::INTERNAL_AND);
		}
		else
		{
			// create a new OR node
			node = new TreeNode(left, right, nullptr,
				TreeNodeType::INTERNAL_OR);
		}
	}
	return node;
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os, const Behaviour& b)
{
	std::string actions = "";
	for(Action* a : b.actions)
	{
		actions += ("\t" + a->getActionDescription() + "\n");
	}
	std::string preconditions = "";
	if(!b.preconditions.empty())
	{
		std::stringstream ss;
		ss << *(b.logical_expression_tree);
		preconditions += ss.str();
	}
	return os << b.name << " [" << (b.interruptible ? "" : "not ")
		<< "interruptible][" << (b.schedulable ? "" : "not ")
		<< "schedulable][priority: " << b.priority << "]\n"
		<< (preconditions.empty() ? "" : "\t[preconditions]\n")
		<< preconditions << (actions.empty() ? "" : "\t[actions]\n")
		<< actions;
}

