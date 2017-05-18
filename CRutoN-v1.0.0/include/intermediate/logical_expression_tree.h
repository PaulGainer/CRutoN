/*==============================================================================
	LogicalExpressionTree

	A logical expression tree of preconditions.

	File			: logical_expression_tree.h
	Author			: Paul Gainer
	Created			: 29/12/2014
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
#ifndef LOGICAL_EXPRESSION_TREE_H_
#define LOGICAL_EXPRESSION_TREE_H_

#include <ostream>
#include <sstream>

#include "enums/precondition_type.h"
#include "intermediate/precondition.h"
#include "intermediate/tree_node.h"

class LogicalExpressionTree
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	rt				the root of the tree
	--------------------------------------------------------------------------*/
	LogicalExpressionTree(TreeNode* rt) : root(rt) {}

	~LogicalExpressionTree()
	{
		delete root;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getRoot

		Accessor for root.
	............................................................................
		@return					the root of the tree
	--------------------------------------------------------------------------*/
	const TreeNode* getRoot() {return root;}

	/*--------------------------------------------------------------------------
		getLogicalExpressionTreeDescription

		Returns a string describing this tree.
	............................................................................
		@return					a string representation of this tree
	--------------------------------------------------------------------------*/
	std::string getLogicalExpressionTreeDescription() const;

	friend std::ostream& operator<<(std::ostream& os,
		const LogicalExpressionTree& t);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The root node of the tree.
	--------------------------------------------------------------------------*/
	TreeNode* root;
};

#endif
