/*==============================================================================
	Automaton

	The base class of TerminalSymbol and NonTerminalSymbol.

	File			: automaton.h
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
#ifndef AUTOMATON_H_
#define AUTOMATON_H_

#include <ostream>
#include <string>

#include "intermediate/named.h"

class Automaton : public Named
{
public:
	/*==========================================================================
		Public Static Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Set repeat to this to only accept valid input once.
	--------------------------------------------------------------------------*/
	static const int REPEAT_NONE = -1;

	/*--------------------------------------------------------------------------
		Set repeat to this to accept valid input one or more times.
	--------------------------------------------------------------------------*/
	static const int REPEAT_ONE_OR_MORE = 0;

	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	virtual ~Automaton() {}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getInput

		Accessor for input.
	............................................................................
		@return					input accepted by the automaton so far.
	--------------------------------------------------------------------------*/
	std::string getInput() const {return input;}

	/*--------------------------------------------------------------------------
		getNumRepeats

		Accessor for num_repeats.
	............................................................................
		@return					the number of times the automaton has accepted
								valid input
	--------------------------------------------------------------------------*/
	int getNumRepeats() const {return num_repeats;}

	/*--------------------------------------------------------------------------
		getRepeat

		Accessor for repeat.
	............................................................................
		@return					the number of times the automaton should
								'repeat' until it reaches an accepting state.
	--------------------------------------------------------------------------*/
	int getRepeat() const {return repeat;}

	/*--------------------------------------------------------------------------
		setRepeat

		Mutator for repeat.
	............................................................................
		@param	r				the new value for repeat
	--------------------------------------------------------------------------*/
	void setRepeat(const int r) {repeat = r;}

	/*--------------------------------------------------------------------------
		isAccepting

		Returns true if the automaton has been fed valid input.
	............................................................................
		@return					true if the automaton is accepting or false
								otherwise
	--------------------------------------------------------------------------*/
	virtual const bool isAccepting() const = 0;

	/*--------------------------------------------------------------------------
		isDead

		Returns true if the automaton is dead.
	............................................................................
		@return					true if the automaton is dead
	--------------------------------------------------------------------------*/
	virtual const bool isDead() const = 0;

	/*--------------------------------------------------------------------------
		feedToken

		Feeds a token to the automaton.
	............................................................................
		@param	token			the token to be used as input for the automaton
	--------------------------------------------------------------------------*/
	virtual void feedToken(const std::string token) = 0;

	/*--------------------------------------------------------------------------
		wouldBeDead

		Returns true if feeding the token to the automaton would result in a
		dead state.
	............................................................................
		@return					true if feeding the token to the automaton would
								result in a dead state, or false otherwise
	--------------------------------------------------------------------------*/
	virtual const bool wouldBeDead(const std::string token) = 0;

	/*--------------------------------------------------------------------------
		resetAutomata

		Resets the automaton to its initial state and clears all accepted
		input.
	--------------------------------------------------------------------------*/
	virtual void resetSymbol() = 0;

	/*--------------------------------------------------------------------------
		resetAutomata

		Resets the automata to its initial state.
	--------------------------------------------------------------------------*/
	virtual void resetAutomata() = 0;

	/*--------------------------------------------------------------------------
		resetSubAutomata

		Returns the automaton's sub-automata to their initial states.
	--------------------------------------------------------------------------*/
	virtual void resetSubAutomata() = 0;

	/*--------------------------------------------------------------------------
		getCopy

		Returns a pointer to a copy of this automaton.
	............................................................................
		@return					a pointer to a copy of this automaton
	--------------------------------------------------------------------------*/
	virtual Automaton* getCopy() = 0;

	/*--------------------------------------------------------------------------
		isReset

		Returns true if the symbol has just been reset.
	--------------------------------------------------------------------------*/
	virtual const bool isReset() const = 0;

	friend std::ostream& operator<<(std::ostream& os, const Automaton& a);

protected:
	/*==========================================================================
		Protected Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Input accepted by the automaton so far.
	--------------------------------------------------------------------------*/
	std::string input;

	/*--------------------------------------------------------------------------
		The number of times the automaton should 'repeat' until it reaches an
		accepting state. Set to REPEAT_NONE for no repeats, and
		REPEAT_ONE_OR_MORE for one or more repeats.
	--------------------------------------------------------------------------*/
	int repeat;

	/*--------------------------------------------------------------------------
		The number of times the automaton has accepted valid input.
	--------------------------------------------------------------------------*/
	int num_repeats;
};

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& os, const Automaton& a)
{
	return os << "automaton: " << a.name << " [is "
			<< (a.isAccepting() ? "" : "not ") << "accepting][is "
			<< (a.isDead() ? "" : "not ") << "dead]"
			<< "[" << a.repeat << "][" << a.num_repeats << "]";
}

#endif
