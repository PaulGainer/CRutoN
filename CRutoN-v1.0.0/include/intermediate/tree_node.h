/*==============================================================================
	TreeNode

	A node in a logical expression tree of preconditions.

	File			: tree_node.h
	Author			: Paul Gainer
	Created			: 29/12/2014
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
#ifndef TREE_NODE_H_
#define TREE_NODE_H_

#include <ostream>

#include "enums/tree_node_type.h"
#include "intermediate/precondition.h"

class TreeNode
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	l				a pointer to the left child node
		@param	r				a pointer to the right child node
		@param	pre				a pointer to the precondition to be stored at
								this node
		@param	type			one of LEAF, INTERNAL_AND, INTERNAL_OR or
								INTERNAL_NOT
	--------------------------------------------------------------------------*/

	TreeNode(TreeNode* l, TreeNode* r, Precondition* pre,
		const TreeNodeType type) : left(l), right(r), precondition(pre),
			node_type(type) {}

	~TreeNode()
	{
		if(left != nullptr)
		{
			delete left;
		}
		if(right != nullptr)
		{
			delete right;
		}
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getPrecondition

		Accessor for the precondition.
	............................................................................
		@return					a pointer to the precondition
	--------------------------------------------------------------------------*/
	Precondition* getPrecondition() const {return precondition;}

	/*--------------------------------------------------------------------------
		getLeft

		Accessor for left.
	............................................................................
		@return					a pointer to the left child node
	--------------------------------------------------------------------------*/
	TreeNode* getLeft() const {return left;}

	/*--------------------------------------------------------------------------
		getRight

		Accessor for right.
	............................................................................
		@return					a pointer to the right child node
	--------------------------------------------------------------------------*/
	TreeNode* getRight() const {return right;}

	/*--------------------------------------------------------------------------
		getNodeType

		Accessor for node_type.
	--------------------------------------------------------------------------*/
	TreeNodeType getNodeType() const {return node_type;}

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The left child
	--------------------------------------------------------------------------*/
	TreeNode* left;

	/*--------------------------------------------------------------------------
		The right child
	--------------------------------------------------------------------------*/
	TreeNode* right;

	/*--------------------------------------------------------------------------
		A pointer to the precondition stored at this node.
	--------------------------------------------------------------------------*/
	Precondition* precondition;

	/*--------------------------------------------------------------------------
		The node type.
	--------------------------------------------------------------------------*/
	const TreeNodeType node_type;
};

#endif
