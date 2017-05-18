/*==============================================================================
	PreconditionTimingConstraint

	A precondition that states that the time must be before or after a given
	time, or during a given time period.

	File			: precondition_timing_constraint.h
	Author			: Paul Gainer
	Created			: 14/12/2014
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
#ifndef PRECONDITION_TIMING_CONSTRAINT_H_
#define PRECONDITION_TIMING_CONSTRAINT_H_

#include <ostream>
#include <sstream>
#include <string>

#include "enums/precondition_type.h"
#include "enums/time_constraint_type.h"
#include "utility.h"

class PreconditionTimingConstraint : public Precondition
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	start			the start time
		@param	end				the end time
		@param	time_constraint	the time constraint type
		@param	time_seconds	the number of seconds for the time constraint
	--------------------------------------------------------------------------*/
	PreconditionTimingConstraint(const std::string start, const std::string end,
		const TimeConstraintType time_constraint, const int time_seconds) :
			start_time(start), end_time(end)
	{
		precondition_type = TIMING_CONSTRAINT;
		time_constraint_type = time_constraint,
		time_constraint_seconds = time_seconds;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getStartTime

		Accessor for start_time.
	............................................................................
		@return					the start time
	--------------------------------------------------------------------------*/
	const std::string getStartTime() {return start_time;}

	/*--------------------------------------------------------------------------
		getEndTime

		Accessor for end_time
	............................................................................
		@return					the end time
	--------------------------------------------------------------------------*/
	const std::string getEndTime() {return end_time;}

	/*--------------------------------------------------------------------------
		getPreconditionDescription

		Returns a string describing this precondition.
	............................................................................
		@return					a string describing the precondition
	--------------------------------------------------------------------------*/
	std::string getPreconditionDescription() const
	{
		std::stringstream stream;
		stream << *this;
		return stream.str();
	}

	friend inline std::ostream& operator<<(std::ostream& os,
		const PreconditionTimingConstraint& p);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The start time of the interval during which this precondition holds.
	--------------------------------------------------------------------------*/
	const std::string start_time;

	/*--------------------------------------------------------------------------
		The end time of the interval during which this precondition holds.
	--------------------------------------------------------------------------*/
	const std::string end_time;
};

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& os,
		const PreconditionTimingConstraint& p)
{
	std::string time_constraint;
	switch(p.time_constraint_type)
	{
	case TimeConstraintType::NONE:
		break;

	case TimeConstraintType::WAS_IN_STATE_WITHIN:
		time_constraint = "(was_in_state_within: "
			+ integerToString(p.time_constraint_seconds) + ")";
		break;

	case TimeConstraintType::BEEN_IN_STATE_FOR:
		time_constraint = "(been_in_state_for: "
			+ integerToString(p.time_constraint_seconds) + ")";
		break;
	}

	return os << "timing_constraint: [start_time: " << p.start_time
		<< "][end time: " << p.end_time << "]" << time_constraint;
}

#endif
