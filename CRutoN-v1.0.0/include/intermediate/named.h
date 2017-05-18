/*==============================================================================
	Named

	An interface inherited by PropVariable, EnumVariable and Behaviour.

	Used by ActionRuleSet to parameterise the disambiguation of instance names.

	File			: named.h
	Author			: Paul Gainer
	Created			: 17/02/2015
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
#ifndef NAMED_H_
#define NAMED_H_

#include <string>

class Named
{
public:
	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getName

		Accessor for name.
	............................................................................
		@return					the name
	--------------------------------------------------------------------------*/
	const std::string getName() const {return name;}

	/*--------------------------------------------------------------------------
		setName

		Mutator for name.
	............................................................................
		@param	nm				the new value for name
	--------------------------------------------------------------------------*/
	void setName(std::string nm) {name = nm;}

protected:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The name of the instance
	--------------------------------------------------------------------------*/
	std::string name;
};

#endif
