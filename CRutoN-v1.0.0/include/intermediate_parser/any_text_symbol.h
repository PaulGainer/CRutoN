/*==============================================================================
	AnyTextSymbol

	File			: any_text_symbol.h
	Author			: Paul Gainer
	Created			: 06/02/2015
	Last modified	: 19/04/2015
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
#ifndef ANY_TEXT_SYMBOL_H_
#define ANY_TEXT_SYMBOL_H_

#include <string>

#include "intermediate_parser/automaton.h"

class AnyTextSymbol: public Automaton
{
public:
	/*==========================================================================
			Public Constructors/Deconstructor
	==========================================================================*/
	AnyTextSymbol()
	{
		name = "any_text";
		repeat = Automaton::REPEAT_NONE;
		num_repeats = 0;
		input = "";
		is_dead = false;
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		isDead

		Accessor for is_dead.
	............................................................................
		@return					true if the automaton is dead or false
								otherwise
	--------------------------------------------------------------------------*/
	const bool isDead() const {return is_dead;}

	/*--------------------------------------------------------------------------
		overrides of pure virtual functions in class Automaton
	--------------------------------------------------------------------------*/
	const bool isAccepting() const;
	void feedToken(const std::string token);
	const bool wouldBeDead(const std::string token);
	void resetAutomata() {num_repeats = 0;}
	void resetSubAutomata() {}
	void resetSymbol();
	const bool isReset() const {return false;}
	Automaton* getCopy();

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		True if the automaton is in a dead state.
	--------------------------------------------------------------------------*/
	bool is_dead;
};

#endif
