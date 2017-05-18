/*==============================================================================
	NonTerminalSymbol

	File			: non_terminal_symbol.cpp
	Author			: Paul Gainer
	Created			: 08/01/2015
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
#include <algorithm>
#include <iostream>
#include <list>
#include <string>

#include "define.h"
#include "utility.h"
#include "intermediate_parser/non_terminal_symbol.h"
#include "intermediate_parser/tokenize.h"

Automaton* NonTerminalSymbol::getCopy()
{
	NonTerminalSymbol* new_non_t = new NonTerminalSymbol(name,
		repeat);

	auto automata_it = automata.begin();
	auto end_it = automata.end();
	while(automata_it != end_it)
	{
		new_non_t->addAutomaton((*automata_it)->getCopy());
		automata_it++;
	}

	return new_non_t;
}

const bool NonTerminalSymbol::isAccepting() const
{
	if(has_reset)
	{
		// the automaton has just moved to an accepting state
		if(repeat == Automaton::REPEAT_NONE)
		{
			return num_repeats == 1;
		}
		else if(repeat == Automaton::REPEAT_ONE_OR_MORE)
		{
			return num_repeats > 0;
		}
		else
		{
			return num_repeats == repeat;
		}
	}

	Automaton* current_automaton = *current_automaton_it;
	auto next_automaton_it = current_automaton_it;
	next_automaton_it++;
	bool last_is_accepting = current_automaton->isAccepting()
		&& next_automaton_it == automata.end();
	// additional condition that the last automaton in the list
	// of automata is accepting
	if(repeat == Automaton::REPEAT_NONE)
	{
		return num_repeats == 1 && last_is_accepting;
	}
	else if(repeat == Automaton::REPEAT_ONE_OR_MORE)
	{
		return num_repeats > 0 && last_is_accepting;
	}
	else
	{
		return num_repeats == repeat && last_is_accepting;
	}
}

const bool NonTerminalSymbol::isDead() const
{
	// the automaton is dead if it has accepted
	// too many times
	switch(repeat)
	{
	case Automaton::REPEAT_NONE:
		if(num_repeats > 1)
		{
			return true;
		}
		break;

	case Automaton::REPEAT_ONE_OR_MORE:
		break;

	default:
		if(num_repeats > repeat)
		{
			return true;
		}
		break;
	}

	bool is_dead = false;

	// and it is also dead if any of its sub_automata
	// are dead
	std::for_each(automata.begin(), automata.end(),
		[&](Automaton* automaton)
		{
			is_dead = is_dead || automaton->isDead();
		});

	return is_dead;
}

/*------------------------------------------------------------------------------
	feedLine

	Feeds a line, token by token, to the automaton.
------------------------------------------------------------------------------*/
void NonTerminalSymbol::feedLine(const std::string line)
{
	std::string line_copy = line;
	// do this for each token in the line
	while(hasNextToken(line_copy))
	{
		std::string token = getNextToken(line_copy);
		Automaton* current_automaton = *current_automaton_it;
		auto next_it = current_automaton_it;
		next_it++;
		/*	if the current automaton is an accepting repeat one or more any_text
			automaton, the token is valid input for the next automaton in
			the list. and there is another token in the input */
		if(next_it != automata.end()
			&& current_automaton->getName() == "any_text"
			&& current_automaton->getRepeat() == REPEAT_ONE_OR_MORE
			&& current_automaton->isAccepting()
			&& (*next_it)->getName() == token
			&& hasNextToken(line_copy))
		{
			std::string line_copy_copy = line_copy;
			std::list<std::string> rest_of_line;
			// tokenize the rest of the input line
			while(hasNextToken(line_copy_copy))
			{
				rest_of_line.push_back(getNextToken(line_copy_copy));
			}
			int count = 0;
			for(std::string s : rest_of_line)
			{
				// count the number of times the token appears in the rest
				// of the line
				if(s == token)
				{
					count++;
				}
			}
			if(count > 0)
			{
				// prompt the user to choose the first occurrence of the token
				// that should not be accepted by +<any_text>
				displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2,
					"ambiguity accepting token in +<any_text> symbol");
				std::cout << "ambiguity in input:\n" << trim(line)
					<< std::endl;
				std::cout << "at positions:\n";
				int index_count = 1;
				std::cout << "[" << index_count++ << "]" << token << " ";
				for(std::string s : rest_of_line)
				{
					if(s == token)
					{
						std::cout << "[" << index_count++ << "]";
					}
					std::cout << s << " ";
				}
				std::cout << std::endl;
				std::string input;
				int input_int;
				bool valid = false;
				while(!valid)
				{
					std::cout << "enter the first position of \'"
						<< token << "\' not accepted by +<any_text> ["
						<< "1" << "-" << integerToString(count + 1) << "]: ";
					std::cin >> input;
					if(isInteger(input))
					{
						input_int = stringToInteger(input);
						if(input_int >= 1 && input_int <= count + 1)
						{
							valid = true;
						}
					}
				}
				rest_of_line.push_front(token);
				index_count = 1;
				auto it = rest_of_line.begin();
				auto end = rest_of_line.end();
				while(it != end)
				{
					std::string s = *it;
					if(s == token)
					{
						if(index_count == input_int)
						{
							// feed the rest of the tokens to the next
							// automaton
							while(it != end)
							{
								feedToken(*it);
								it++;
							}
							return;
						}
						else
						{
							// feed this token to this automaton and increment
							// the index
							index_count++;
							current_automaton->feedToken(s);
							it++;
						}
					}
					else
					{
						current_automaton->feedToken(s);
						it++;
					}
				}
			}
			else
			{
				feedToken(token);
			}
		}
		else
		{
			feedToken(token);
		}
	}
}

void NonTerminalSymbol::feedToken(const std::string token)
{
	has_reset = false;
	auto automata_end = automata.end();

	Automaton* current_automaton = *current_automaton_it;
	auto next_automaton_it = current_automaton_it;
	next_automaton_it++;

	// if this is the last automaton in the list
	if(next_automaton_it == automata_end)
	{
		// if this last automaton is accepting
		if(current_automaton->isAccepting())
		{
			Automaton* copy = nullptr;
			switch(current_automaton->getRepeat())
			{
			case Automaton::REPEAT_NONE:
				current_automaton->feedToken(token);

				break;

			case Automaton::REPEAT_ONE_OR_MORE:
				copy = current_automaton->getCopy();
				// if this token would not move the current automaton to a dead
				// state then reset the automaton and feed the token
				if(!copy->wouldBeDead(token) && !current_automaton->isReset())
				{
					current_automaton->resetSubAutomata();
				}
				current_automaton->feedToken(token);

				break;

			default:

				break;
			}
			delete copy;
			if(current_automaton->isAccepting())
			{
				has_reset = true;
			}
		}
		else
		{
			// this last automaton is not accepting
			current_automaton->feedToken(token);
			if(current_automaton->isAccepting())
			{
				int current_num_repeats = current_automaton->getNumRepeats();

				switch(current_automaton->getRepeat())
				{
				case Automaton::REPEAT_NONE:
					if(current_num_repeats == 1)
					{
						num_repeats++;
					}
					break;

				case Automaton::REPEAT_ONE_OR_MORE:
					if(current_num_repeats == 1)
					{
						num_repeats++;
					}
					break;

				default:
					if(current_num_repeats == current_automaton->getRepeat())
					{
						num_repeats++;
					}
					break;
				}

				// reset all the sub-automata, this non-terminal has just
				// accepted
				if(repeat == Automaton::REPEAT_ONE_OR_MORE ||
					(repeat > 0 && num_repeats < repeat))
				{
					resetSubAutomata();
				}
			}
		}
	}
	else
	{
		// this is not the last automaton in the list
		Automaton* next_automaton = *next_automaton_it;

		// if this automaton is accepting, and the next automaton would accept
		// the input, then feed it to the automaton
		if(current_automaton->isAccepting() &&
			!next_automaton->wouldBeDead(token))
		{
			next_automaton->feedToken(token);
			next_automaton_it++;
			if((*(++current_automaton_it))->isAccepting()
				&& next_automaton_it == automata.end())
			{
				num_repeats++;
				if(repeat == Automaton::REPEAT_ONE_OR_MORE ||
					(repeat > 1 && num_repeats < repeat))
				{
					resetSubAutomata();
				}
			}
		}
		else
		{
			// ...otherwise feed it to this automaton
			current_automaton->feedToken(token);
		}
	}

	if(!input.empty())
	{
		input += ' ';
	}
	input += token;
}

const bool NonTerminalSymbol::wouldBeDead(const std::string token)
{
	Automaton* copy = getCopy();
	copy->feedToken(token);
	bool copy_is_dead = copy->isDead();
	delete copy;
	return copy_is_dead;
}

void NonTerminalSymbol::resetAutomata()
{
	has_reset = true;
	num_repeats = 0;
	std::for_each(automata.begin(), automata.end(),
		[&](Automaton* automaton)
		{
			automaton->resetAutomata();
		});
	current_automaton_it = automata.begin();
}

void NonTerminalSymbol::resetSubAutomata()
{
	has_reset = true;
	std::for_each(automata.begin(), automata.end(),
		[&](Automaton* automaton)
		{
			automaton->resetAutomata();
		});
	current_automaton_it = automata.begin();
}

void NonTerminalSymbol::resetSymbol()
{
	std::for_each(automata.begin(), automata.end(),
		[&](Automaton* automaton)
		{
			automaton->resetSymbol();
		});
	current_automaton_it = automata.begin();
	num_repeats = 0;
	has_reset = false;
	input = "";
}

/*------------------------------------------------------------------------------
	addAutomaton

	Adds an automaton to the non-terminal symbol.
------------------------------------------------------------------------------*/
void NonTerminalSymbol::addAutomaton(Automaton* automaton)
{
	automata.push_back(automaton);
	current_automaton_it = automata.begin();
}

/*------------------------------------------------------------------------------
	getInputFromSubAutomaton

	Gets accepted input from the automaton at the automaton_num^th position
	in the list of Automata.
------------------------------------------------------------------------------*/
const std::string NonTerminalSymbol::getInputFromSubAutomaton(
	const unsigned int automaton_num) const
{
	if(automaton_num < 1 || automaton_num > automata.size())
	{
		return "";
	}

	auto automata_it = automata.begin();
	unsigned int i = 1;

	while(i++ < automaton_num)
	{
		automata_it++;
	}

	std::string automata_input = (*automata_it)->getInput();

	return automata_input;
}
