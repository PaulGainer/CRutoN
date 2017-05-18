/*==============================================================================
	LogicalExpressionTree

	File			: logical_expression_tree.cpp
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
#include <ostream>
#include <sstream>

#include "enums/precondition_type.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/precondition.h"
#include "intermediate/tree_node.h"

/*------------------------------------------------------------------------------
	getLogicalExpressionTreeDescription

	Returns a string describing this tree.
------------------------------------------------------------------------------*/
std::string LogicalExpressionTree::getLogicalExpressionTreeDescription() const
{
	std::stringstream stream;
	stream << *this;
	return stream.str();
}

/*------------------------------------------------------------------------------
	nextNode

	Given a TreeNode and a node depth, adds some information about the node
	to the given stream.
------------------------------------------------------------------------------*/
void nextNode(std::ostream& os, TreeNode* node, const int depth,
	const bool negated, const bool isLastNestedNode)
{
	switch(node->getNodeType())
	{
	case TreeNodeType::LEAF:
		for(int i = 0; i < depth; i++)
		{
			// tab across depth times
			if(i == depth - 1 && depth > 2 && !isLastNestedNode)
			{
				os << "|   ";
			}
			else
			{
				os << "    ";
			}
		}
		if(negated)
		{
			// this precondition is negated
			os << "!!---";
		}
		os << node->getPrecondition()->getPreconditionDescription() << "\n";
		break;

	case TreeNodeType::INTERNAL_AND:
		// recursively add information about the left child
		nextNode(os, node->getLeft(), depth + 1, false, false);
		for(int i = 0; i < depth; i++)
		{
			os << "    ";
		}
		// this is a conjunction
		os << "&&--+\n";
		// recursively add information about the right child
		nextNode(os, node->getRight(), depth + 1, false,
			node->getRight()->getNodeType() == TreeNodeType::LEAF);
		break;

	case TreeNodeType::INTERNAL_OR:
		// recursively add information about the left child
		nextNode(os, node->getLeft(), depth + 1, false, false);
		for(int i = 0; i < depth; i++)
		{
			os << "    ";
		}
		// this is a disjunction
		os << "||--+\n";
		// recursively add information about the right child
		nextNode(os, node->getRight(), depth + 1, false,
			node->getRight()->getNodeType() == TreeNodeType::LEAF);
		break;

	case TreeNodeType::INTERNAL_NOT:
		// this is a NOT node, call nextNode again with negated set to true
		nextNode(os, node->getLeft(), depth, true, false);
		break;
	}
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os,
	const LogicalExpressionTree& t)
{
	nextNode(os, t.root, 1, false, false);
	return os;
}
