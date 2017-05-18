/*==============================================================================
	NonTerminalSymbol

	An automaton corresponding to a non-terminal symbol.

	File			: non_terminal_symbol.h
	Author			: Paul Gainer
	Created			: 08/01/2015
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
#ifndef NON_TERMINAL_SYMBOL_H_
#define NON_TERMINAL_SYMBOL_H_

#include <list>
#include <string>

#include "intermediate_parser/automaton.h"

class NonTerminalSymbol: public Automaton
{
public:
	/*==========================================================================
			Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	nm				the name of the symbol
		@param	r				the number of times the automaton should repeat
	--------------------------------------------------------------------------*/
	NonTerminalSymbol(const std::string nm, const int r) :
		automata(std::list<Automaton*>()), has_reset(false)
	{
		name = nm;
		repeat = r;
		num_repeats = 0;
		input = "";
	}

	~NonTerminalSymbol()
	{
		for(Automaton* a : automata)
		{
			delete a;
		}
	}


	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		addAutomaton

		Adds an automaton to the non-terminal symbol.
	............................................................................
		@param	automaton		a pointer to the automaton to add to the list
								of sub-automata
	--------------------------------------------------------------------------*/
	void addAutomaton(Automaton* automaton);

	/*--------------------------------------------------------------------------
		feedLine

		Feeds a line, token by token, to the automaton.
	............................................................................
		@param	line			the line to feed, token-by-token, to the
								automaton
	--------------------------------------------------------------------------*/
	void feedLine(const std::string line);

	/*--------------------------------------------------------------------------
		getInputFromSubAutomaton

		Gets accepted input from the automaton at the automaton_num^th position
		in the list of Automata.
	............................................................................
		@param	automaton_num	the index of the automaton in the list
		@return					the input accepted by the automaton_num^th
								automaton in the list
	--------------------------------------------------------------------------*/
	const std::string getInputFromSubAutomaton(const unsigned int automaton_num)
		const;

	/*--------------------------------------------------------------------------
		overrides of pure virtual functions in class Automaton
	--------------------------------------------------------------------------*/
	const bool isAccepting() const;
	const bool isDead() const;
	void feedToken(const std::string token);
	const bool wouldBeDead(const std::string token);
	void resetAutomata();
	void resetSubAutomata();
	void resetSymbol();
	const bool isReset() const {return has_reset;}
	Automaton* getCopy();


private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The list of sub-automata.
	--------------------------------------------------------------------------*/
	std::list<Automaton*> automata;

	/*--------------------------------------------------------------------------
		An iterator pointing to the current sub-automata.
	--------------------------------------------------------------------------*/
	std::list<Automaton*>::iterator current_automaton_it;

	/*--------------------------------------------------------------------------
		True if the automaton has just been reset.
	--------------------------------------------------------------------------*/
	bool has_reset;
};

#endif
