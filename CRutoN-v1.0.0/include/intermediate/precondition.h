/*==============================================================================
	Precondition

	The superclass for the three types of precondition.

	File			: precondition.h
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
#ifndef PRECONDITION_H_
#define PRECONDITION_H_

#include "enums/precondition_type.h"
#include "enums/time_constraint_type.h"

class Precondition
{
public:
	virtual ~Precondition() {}

	/*==========================================================================
		Public Static Member Variables
	==========================================================================*/
	const static int NOT_DEFINED = -1;

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getPreconditionType

		Accessor for precondition_type.
	............................................................................
		@return					the type of precondition
	--------------------------------------------------------------------------*/
	const PreconditionType getPreconditionType() {return precondition_type;}

	/*--------------------------------------------------------------------------
		getTimeConstraintType

		Accessor for time_constraint_type.
	............................................................................
		@return					the type of time constraint
	--------------------------------------------------------------------------*/
	const TimeConstraintType getTimeConstraintType()
	{
		return time_constraint_type;
	}

	/*--------------------------------------------------------------------------
		getTimeConstraintSeconds

		Accessor for time_constraint_seconds.
	............................................................................
		@return					the number of seconds for the time constraint
	--------------------------------------------------------------------------*/
	const int getTimeConstraintSeconds() {return time_constraint_seconds;}

	/*--------------------------------------------------------------------------
		getPreconditionOrder

		Accessor for precondition_order.
	............................................................................
		@return					the integer used to determine the order in
								which preconditions are evaluated by a behaviour
	--------------------------------------------------------------------------*/
	const int getPreconditionOrder() {return precondition_order;}

	/*--------------------------------------------------------------------------
		getAndOrConnector

		Accessor for and_or_connector.
	............................................................................
		@return					returns 0 or 1 if this precondition and the next
								should form a conjunction, or 2 if this
								precondition and the next should form a
								disjunction
	--------------------------------------------------------------------------*/
	const int getAndOrConnector() {return and_or_connector;}

	/*--------------------------------------------------------------------------
		getNotConnector

		Accessor for not_or_connector.
	............................................................................
		@return					1 if this precondition should be negated, or 0
								otherwise
	--------------------------------------------------------------------------*/
	const int getNotConnector() {return not_connector;}

	/*--------------------------------------------------------------------------
		setAndOrConnector

		Mutator for and_or_connector.
	............................................................................
		@param	val				the new value for and_or_connector
	--------------------------------------------------------------------------*/
	void setAndOrConnector(const int val) {and_or_connector = val;}

	/*--------------------------------------------------------------------------
		setNotConnector

		Mutator for not_connector.
	............................................................................
		@param	val				the new value for not_connector
	--------------------------------------------------------------------------*/
	void setNotConnector(const int val) {not_connector = val;}

	/*--------------------------------------------------------------------------
		setPreconditionOrder

		Mutator for precondition_order.
	............................................................................
		@param	order			the new order value
	--------------------------------------------------------------------------*/
	void setPreconditionOrder(const int order) {precondition_order = order;}

	/*--------------------------------------------------------------------------
		setTimeConstraintType

		Mutator for time_constraint_type.
	............................................................................
		@param	t				the new time constraint type
	--------------------------------------------------------------------------*/
	void setTimeConstraintType(TimeConstraintType t) {time_constraint_type = t;}

	/*--------------------------------------------------------------------------
		setTimeConstraintSeconds

		Mutator for time_constraint_seconds.
	--------------------------------------------------------------------------*/
	void setTimeConstraintSeconds(const int s) {time_constraint_seconds = s;}

	/*--------------------------------------------------------------------------
		getPreconditionDescription

		Returns a string describing the precondition.
	............................................................................
		@return					a string describing the precondition
	--------------------------------------------------------------------------*/
	virtual std::string getPreconditionDescription() const = 0;

protected:
	/*==========================================================================
			Protected Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The type of precondition.
	--------------------------------------------------------------------------*/
	PreconditionType precondition_type;

	/*--------------------------------------------------------------------------
		The type of any applicable time constraint.
	--------------------------------------------------------------------------*/
	TimeConstraintType time_constraint_type;

	/*--------------------------------------------------------------------------
		The time constraint duration, if any.
	--------------------------------------------------------------------------*/
	int time_constraint_seconds;

	/*--------------------------------------------------------------------------
		An integer determining the order in which preconditions are evaluated.
	--------------------------------------------------------------------------*/
	int precondition_order;

	/*--------------------------------------------------------------------------
		0 or 1 if this precondition and the next should form a conjunction, or
		2 if this precondition and the next should form a disjunction.
	--------------------------------------------------------------------------*/
	int and_or_connector;

	/*--------------------------------------------------------------------------
		1 if the precondition should negated or 0 otherwise.
	--------------------------------------------------------------------------*/
	int not_connector;
};

#endif
