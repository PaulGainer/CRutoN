/*==============================================================================
	TimeSymbol

	File			: time_symbol.cpp
	Author			: Paul Gainer
	Created			: 05/02/2015
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
#include "intermediate_parser/time_symbol.h"

Automaton* TimeSymbol::getCopy()
{
	TimeSymbol* new_time_symbol = new TimeSymbol();
	new_time_symbol->setRepeat(repeat);
	return new_time_symbol;
}

const bool TimeSymbol::isAccepting() const
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

void TimeSymbol::feedToken(const std::string token)
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

const bool TimeSymbol::wouldBeDead(const std::string token)
{
	return ((!isValid(token)) ||
		(repeat == Automaton::REPEAT_NONE && num_repeats == 1) ||
		(repeat != Automaton::REPEAT_ONE_OR_MORE && num_repeats == repeat));
}

void TimeSymbol::resetAutomata()
{
	num_repeats = 0;
}

void TimeSymbol::resetSymbol()
{
	is_dead = false;
	num_repeats = 0;
	input = "";
}

const bool TimeSymbol::isValid(const std::string token) const
{
	// accepted time formats are HH:MM:SS, HH:MM, HH
	if(token.size() != 8)
	{
		if(token.size() == 5)
		{
			if(token.at(2) != ':')
			{
				return false;
			}
			else
			{
				std::string hours = token.substr(0,2);
				std::string minutes = token.substr(3,2);
				if(!isNaturalNumber(hours) || !isNaturalNumber(minutes))
				{
					return false;
				}
				else
				{
					int num_hours = stringToInteger(hours);
					int num_minutes = stringToInteger(minutes);

					return num_hours <= 23 && num_minutes <= 59;
				}
			}
		}
		else
		{
			if(token.size() == 2)
			{
				if(!isNaturalNumber(token))
				{
					return false;
				}
				else
				{
					int num_hours = stringToInteger(token);
					return num_hours <= 23;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		if(token.at(2) != ':' || token.at(5) != ':')
		{
			return false;
		}
		else
		{
			std::string hours = token.substr(0,2);
			std::string minutes = token.substr(3,2);
			std::string seconds = token.substr(6,2);
			if(!isNaturalNumber(hours) || !isNaturalNumber(minutes)
				|| !isNaturalNumber(seconds))
			{
				return false;
			}
			else
			{
				int num_hours = stringToInteger(hours);
				int num_minutes = stringToInteger(minutes);
				int num_seconds = stringToInteger(seconds);

				return num_hours <= 23 && num_minutes <= 59
					&& num_seconds <= 59;
			}
		}
	}
}



