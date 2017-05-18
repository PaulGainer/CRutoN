/*==============================================================================
	PropVariable

	A propositional variable.

	File			: prop_variable.h
	Author			: Paul Gainer
	Created			: 13/12/2014
	Last modified	: 18/04/2014
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
#ifndef PROP_VARIABLE_H_
#define PROP_VARIABLE_H_

#include <ostream>
#include <string>

#include "intermediate/named.h"

class PropVariable : public Named
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	nm				the name of the propositional variable
		@param	non_d			true if the propositional variables value
								should be determined non-deterministically
	--------------------------------------------------------------------------*/
	PropVariable(const std::string& nm, const bool non_d) :
		non_deterministic(non_d)
	{
		name = nm;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		isNonDeterministic

		Accessor for non_deterministic.
	............................................................................
		@return					true if the variable is non-deterministic
	--------------------------------------------------------------------------*/
	const bool isNonDeterministic() const {return non_deterministic;}

	/*--------------------------------------------------------------------------
		setNonDeterministic

		Sets non_deterministic to true;
	--------------------------------------------------------------------------*/
	void setNonDeterministic() {non_deterministic = true;}

	friend std::ostream& operator<<(std::ostream& os, const PropVariable& p);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Set to true if the variable's value should be determined
		non-deterministically.
	--------------------------------------------------------------------------*/
	bool non_deterministic;
};

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& os, const PropVariable& p)
{
	return os << p.name << " ["
		<< (p.non_deterministic ? "non-deterministic" : "deterministic")
		<< "]";
}

#endif
