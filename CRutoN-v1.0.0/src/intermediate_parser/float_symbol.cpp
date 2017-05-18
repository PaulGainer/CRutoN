/*==============================================================================
	FloatrSymbol

	File			: float_symbol.cpp
	Author			: Paul Gainer
	Created			: 12/03/2015
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
#include <string>

#include "utility.h"
#include "intermediate_parser/automaton.h"
#include "intermediate_parser/float_symbol.h"

Automaton* FloatSymbol::getCopy()
{
	FloatSymbol* new_float_symbol = new FloatSymbol();
	new_float_symbol->setRepeat(repeat);
	return new_float_symbol;
}

const bool FloatSymbol::isAccepting() const
{
	if(repeat == Automaton::REPEAT_NONE)
	{
		return num_repeats == 1 && !isDead();
	}
	else if(repeat == Automaton::REPEAT_ONE_OR_MORE)
	{
		return num_repeats > 0 && !isDead();
	}
	else
	{
		return num_repeats == repeat && !isDead();
	}
}

void FloatSymbol::feedToken(const std::string token)
{
	if(wouldBeDead(token))
	{
		is_dead = true;
	}
	else
	{
		if(!input.empty())
		{
			input += ' ';
		}
		input += token;
		num_repeats++;
	}
}

const bool FloatSymbol::wouldBeDead(const std::string token)
{
	return ((!isFloat(token)) ||
		(repeat == Automaton::REPEAT_NONE && num_repeats == 1) ||
		(repeat != Automaton::REPEAT_ONE_OR_MORE && num_repeats == repeat));
}

void FloatSymbol::resetAutomata()
{
	num_repeats = 0;
}

void FloatSymbol::resetSymbol()
{
	is_dead = false;
	num_repeats = 0;
	input = "";
}



