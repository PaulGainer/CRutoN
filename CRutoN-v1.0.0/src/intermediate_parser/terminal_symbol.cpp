/*==============================================================================
	TerminalSymbol

	File			: terminal_symbol.cpp
	Author			: Paul Gainer
	Created			: 10/01/2015
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

#include "intermediate_parser/automaton.h"
#include "intermediate_parser/terminal_symbol.h"

Automaton* TerminalSymbol::getCopy()
{
	return new TerminalSymbol(name, repeat);
}

const bool TerminalSymbol::isAccepting() const
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

void TerminalSymbol::feedToken(const std::string token)
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

const bool TerminalSymbol::wouldBeDead(const std::string token)
{
	return ((token != name) ||
		(repeat == Automaton::REPEAT_NONE && num_repeats == 1) ||
		(repeat != Automaton::REPEAT_ONE_OR_MORE && num_repeats == repeat));
}

void TerminalSymbol::resetAutomata()
{
	num_repeats = 0;
}

void TerminalSymbol::resetSymbol()
{
	is_dead = false;
	num_repeats = 0;
	input = "";
}
