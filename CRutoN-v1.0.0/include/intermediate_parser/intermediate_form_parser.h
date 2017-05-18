/*==============================================================================
	IntermediateFormParser

	Has functions to parse a set of grammar definitions, a set of control rules
	and a set of data extraction rules, and to construct an intermediate form
	from these rules.

	File			: intermediate_form_parser.h
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
#ifndef INTERMEDIATE_FORM_PARSER_H_
#define INTERMEDIATE_FORM_PARSER_H_

#define NUM_TYPES 10
#define RULE_TYPE_LOWER_BOUND 1
#define RULE_TYPE_UPPER_BOUND 9

#include <string>
#include <list>
#include <map>
#include <vector>

#include <intermediate/intermediate_form.h>
#include "intermediate/enum_variable.h"
#include "intermediate/prop_variable.h"
#include "intermediate/action.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/tree_node.h"
#include "intermediate_parser/any_text_symbol.h"
#include "intermediate_parser/float_symbol.h"
#include "intermediate_parser/integer_symbol.h"
#include "intermediate_parser/non_terminal_symbol.h"
#include "intermediate_parser/relational_operator_symbol.h"
#include "intermediate_parser/terminal_symbol.h"
#include "intermediate_parser/time_symbol.h"

class IntermediateFormParser
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	IntermediateFormParser() : intermediate_form(nullptr)
	{
		buildPredefinedAutomatonTypeMap();
		buildPredefinedAutomatonMap();
		buildDataExtractionRuleDefinitionMap();
	}

	~IntermediateFormParser()
	{
		// delete each predefined automaton in the map
		for(auto pair : predefined_automaton_map)
		{
			delete pair.second;
		}
		// and delete all non-terminal symbols
		for(NonTerminalSymbol* non_terminal : non_terminal_symbols)
		{
			delete non_terminal;
		}
		if(intermediate_form != nullptr)
		{
			delete intermediate_form;
		}
	}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		parse

		Parses the given grammar file, data extraction rule file, and control
		rule file. Returns true if successful or false otherwise.
	............................................................................
		@param	grammar_rule_file	the name of the file containing the
									grammar definitions
		@param data_rule_file		the name of the file containing the
									data extraction definitions
		@param control_rule_file	the name of the file containing the
									control rule definitions
		@return						true if parsing was sucessful or false
									otherwise
	--------------------------------------------------------------------------*/
	bool parse(const std::string grammar_rule_file,
		const std::string data_rule_file, const std::string control_rule_file);

	/*--------------------------------------------------------------------------
		parseGrammarFile

		Parses the given grammar file. Throws an error if parsing was
		unsuccessful.
	............................................................................
		@param	filename		the name of the file containing the grammar
								definitions
		@throw					an error message if parsing failed
	--------------------------------------------------------------------------*/
	void parseGrammarFile(const std::string filename)
		throw (std::string);

	/*--------------------------------------------------------------------------
		parseDataExtractionFile

		Parses the data extraction rule file. Throws an error if parsing was
		unsuccessful.
	............................................................................
		@param	filename		the name of the file containing the data
								extraction definitions
		@throw					an error message if parsing failed
	--------------------------------------------------------------------------*/
	void parseDataExtractionFile(const std::string filename)
		throw (std::string);

	/*--------------------------------------------------------------------------
		parseControlRuleFile

		Parses the control rule rule file. Throws an error if parsing was
		unsuccessful.
	............................................................................
		@param	filename		the name of the file containing the control rule
								definitions
		@throw					an error message if parsing failed
	--------------------------------------------------------------------------*/
	void parseControlRuleFile(const std::string filename) throw (std::string);

	/*--------------------------------------------------------------------------
		getIntermediateForm

		Returns a pointer to the intermediate form constructed after the
		successful parsing of the input files.
	............................................................................
		@return					a pointer to the intermediate form
	--------------------------------------------------------------------------*/
	IntermediateForm* getIntermediateForm() {return intermediate_form;}

private:
	/*==========================================================================
		Private Static Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Integer constants corresponding to the different tytpes of action,
		precondition and state_n_seconds terms.
	--------------------------------------------------------------------------*/
	static const int NO_TYPE;
	static const int PRECONDITION_PROP_VALUE_CHECK;
	static const int PRECONDITION_ENUM_VALUE_CHECK;
	static const int PRECONDITION_TIME_CONSTRAINT;
	static const int ACTION_PROP_VALUE_ASSIGNMENT;
	static const int ACTION_ENUM_VALUE_ASSIGNMENT;
	static const int ACTION_EXECUTE_BEHAVIOUR_NON_D;
	static const int ACTION_EXECUTE_BEHAVIOUR;
	static const int ACTION_DELAY;
	static const int STATE_N_SECONDS;

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The intermediate form.
	--------------------------------------------------------------------------*/
	IntermediateForm* intermediate_form;

	/*--------------------------------------------------------------------------
		The list of all parsed non-terminal symbols.
	--------------------------------------------------------------------------*/
	std::list<NonTerminalSymbol*> non_terminal_symbols;

	/*--------------------------------------------------------------------------
		A map from automaton names (parsed in the grammar file) to a constant
		corresponding to the type of the automaton.
	--------------------------------------------------------------------------*/
	std::map<const std::string, const int> automaton_type_map;

	/*--------------------------------------------------------------------------
		A map from predefined automaton names (parsed in the grammar file) to
		pointers to instances of the corresponding predefined automaton.
	--------------------------------------------------------------------------*/
	std::map<const std::string, Automaton*> predefined_automaton_map;

	/*--------------------------------------------------------------------------
		A map from constants representing the types of data extraction rule to
		a list of strings corresponding to the expected list of parameters for
		that type.
	--------------------------------------------------------------------------*/
	std::map<const int, std::list<std::string>>
		data_extraction_rule_definition_map;

	/*--------------------------------------------------------------------------
		A map from data extraction rule names (parsed in the data extraction
		rule file) to the list of parameters parsed for that rule.
	--------------------------------------------------------------------------*/
	std::map<const std::string, std::vector<std::string>>
			data_extraction_rule_values_map;

	/*--------------------------------------------------------------------------
		A vector of lists of pointer to non-terminal symbols, index by type of
		non-terminal symbol.
	--------------------------------------------------------------------------*/
	std::vector<std::list<NonTerminalSymbol*>> non_terminal_symbols_by_type;

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		isTerminalSymbol

		Returns true if the given token is a valid terminal symbol.
	............................................................................
		@param	token			the input token
		@return					true if the given token is a valid terminal
								symbol, or false otherwise
	--------------------------------------------------------------------------*/
	const bool isTerminalSymbol(const std::string token) const;

	/*--------------------------------------------------------------------------
		isNonTerminalSymbol

		Returns true if the given token is a valid non-terminal symbol.
	............................................................................
		@param	token			the input token
		@return					true if the given token is a valid non-terminal
								symbol, or false otherwise
	--------------------------------------------------------------------------*/
	const bool isNonTerminalSymbol(const std::string token) const;

	/*--------------------------------------------------------------------------
		getSymbolPrefix

		Returns the prefix (empty, an integer, or + for one or more) of the
		given symbol. Does not check the symbol for validity first.
	............................................................................
		@param	symbol			the string corresponding to a symbol
		@return					the symbol prefix, if any
	--------------------------------------------------------------------------*/
	const std::string getSymbolPrefix(const std::string symbol) const;

	/*--------------------------------------------------------------------------
		getSymbolText

		Returns the text of the given symbol, so for +'Hello' this would return
		Hello.
	............................................................................
		@param	symbol			the string corresponding to a symbol
		@return					the symbol's text
	--------------------------------------------------------------------------*/
	const std::string getSymbolText(const std::string symbol) const;

	/*--------------------------------------------------------------------------
		isPredefinedAutomaton

		Returns true if the given symbol name corresponds to an existing
		predefined automaton.
	............................................................................
		@param	name			the name of a predefined automaton
		@return					true if there is a predefined automaton with
								the given name
	--------------------------------------------------------------------------*/
	const bool isPredefinedAutomaton(const std::string name) const;

	/*--------------------------------------------------------------------------
		getPredefinedAutomatonByName

		Given the name of a predefined automaton, returns a pointer to an
		instance of that automaton.
	............................................................................
		@param	name			the name of a predefined automaton
		@return					a pointer to an instance of the predefined
								automaton having the given name
	--------------------------------------------------------------------------*/
	Automaton* getPredefinedAutomatonByName(const std::string name) const;

	/*--------------------------------------------------------------------------
		validateDataExtractionRuleValues

		Checks the validity of data extraction rule definitions. Throws an
		error if a rule is not valid.
	............................................................................
		@param	values			the values parsed for a data extraction rule
		@param	type			the type of data extraction rule
		@throw					an error message if the rule was not valid.
	--------------------------------------------------------------------------*/
	void validateDataExtractionRuleValues(
		std::vector<std::string>* values, const int type) throw(std::string);

	/*--------------------------------------------------------------------------
		getDataExtractionRuleDefinitionByType

		Given a constant corresponding to a type of action, precondition or
		state-n-seconds term, returns the list of expected parameters for a data
		extraction rule of this type.
	............................................................................
		@param	type			the type of data extraction rule
		@return					a list of expected parameters for the given
								type of rule
	--------------------------------------------------------------------------*/
	std::list<std::string> getDataExtractionRuleDefinitionByType(
		const int type) const;

	/*--------------------------------------------------------------------------
		getDataExtractionRuleValuesByName

		Given the name of a data extraction rule, returns a list of strings
		corresponding to the parameters parsed for that data extraction rule.
	............................................................................
		@param	name			the name of a data extraction rule
		@return					the list of parameters parsed for that rule
	--------------------------------------------------------------------------*/
	std::vector<std::string> getDataExtractionRuleValuesByName(
			const std::string name) const;

	/*--------------------------------------------------------------------------
		getAutomatonType

		Given the name of an automaton, returns the type of automaton
		corresponding to that name.
	............................................................................
		@param	name			the name of an automaton
		@return					a constant indicating the type of that
								automaton
	--------------------------------------------------------------------------*/
	const int getAutomatonType(const std::string name) const;

	/*--------------------------------------------------------------------------
		parseAction

		Given a string and an integer corresponding to the order of the action
		in the corresponding behaviour, constructs and returns a new Action, or
		throws an error if parsing was unsuccessful.
	............................................................................
		@param	s				the text extracted from the control rule file
								representing the action
		@order					the integer indicating the order in which the
								action should be executed by the behaviour
		@return					a pointer to the parsed Action
		@throw					an error message if parsing was unsuccessful
	--------------------------------------------------------------------------*/
	Action* parseAction(const std::string s, const int order)
		throw(std::string);

	/*--------------------------------------------------------------------------
		parsePrecondition

		Given a string corresponding to a precondition, possibly with a
		state-n-seconds term, constructs and returns a new Precondition, or
		throws an error if parsing was unsuccessful.
	............................................................................
		@param	s				the text extracted from the control rule file
								representing the precondition
		@return					a pointer to the parsed Precondition
		@throw					an error message if parsing was unsuccessful
	--------------------------------------------------------------------------*/
	Precondition* parsePrecondition(const std::string s) throw(std::string);

	/*--------------------------------------------------------------------------
		parsePreconditionNoStateNSeconds

		Given a string corresponding to a precondition, with no state-n-seconds
		term, constructs and returns a new Precondition, or	throws an error if
		parsing was unsuccessful.
	............................................................................
		@param	s				the text extracted from the control rule file
								representing the precondition
		@return					a pointer to the parsed Precondition
		@throw					an error message if parsing was unsuccessful
	--------------------------------------------------------------------------*/
	Precondition* parsePreconditionNoStateNSeconds(const std::string s)
		throw(std::string);

	/*--------------------------------------------------------------------------
		parseStateNSeconds

		Given a string corresponding to a state-n-seconds term, returns a pair
		where the first element is an integer corresponding to the number of
		seconds, and the second element is a pair of booleans, where the first
		element	is true and the second element is false if the term is a
		been_in_state_for term, or conversely the first element is false and the
		second element is true if the term is a was_in_state_within term.
		Throws an error if parsing was unsuccessful.
	............................................................................
		@param	s				the text extracted from the control rule file
								representing the state_n_seconds term
		@return					a pair, where the first element is an integer
								corresponding to the number of seconds, and the
								second element is apair, where the first element
								is true and the second element is false if the
								term is a been_in_state_for term, or conversely
								the first element is false and the second
								element is true if the term is a was_in_state
								_within term
		@throw					an error message if parsing was unsuccessful
	--------------------------------------------------------------------------*/
	std::pair<const int, const std::pair<const bool, const bool>>*
		parseStateNSeconds(const std::string s) throw (std::string);

	/*--------------------------------------------------------------------------
		getAcceptingGrammarRules

		Given a string and a list of possible types, returns a list of pairs
		where for each pair the	first element is an integer corresponding to
		the type of rule, and the second element is a pointer to the accepting
		non-terminal symbol.
	............................................................................
		@param	s				text extracted from the control rule file
		@param	types			the list of possible types
		@return					a vector of pointers to pairs, where for each
								pair the first element is an integer
								corresponding to the type of rule, and the
								second element is a pointer to the accepting
								non-terminal symbol
	--------------------------------------------------------------------------*/
	const std::vector<std::pair<int, NonTerminalSymbol*>*>
		getAcceptingGrammarRules(const std::string s, std::vector<int> types)
			const;

	/*--------------------------------------------------------------------------
		validateAcceptingGrammarRules

		Conducts a number of additional checks to validate parsed grammar rules.
		Throws an error if a rule is not valid.
	............................................................................
		@param	candidates		a pointer to a vector of pointers to pairs,
								where for each pair the first element is an
								integer corresponding to the type of rule, and
								the second element is a pointer to the accepting
								non-terminal symbol
		@throw					an error message if validation fails for one of
								the candidates
	--------------------------------------------------------------------------*/
	void validateAcceptingGrammarRules(
		std::vector<std::pair<int, NonTerminalSymbol*>*>* candidates)
			throw(std::string);

	/*--------------------------------------------------------------------------
		disambiguateCandidates

		Prompts the user to select a matching grammar rule for a parsed term
		if more than one match is found. A pair corresponding to the type
		and accepting non-terminal symbol for the selected grammar rule is then
		returned. Throws an error if no match is found.
	............................................................................
		@param	candidates		a pointer to a vector of pointers to pairs,
								where for each pair the first element is an
								integer corresponding to the type of rule, and
								the second element is a pointer to the accepting
								non-terminal symbol
		@param	s				the original parsed text, used to prompt the
								user to disambiguate candidates
		@throw					an error message if disambiguation fails
	--------------------------------------------------------------------------*/
	std::pair<int, NonTerminalSymbol*>* disambiguateCandidates(
		std::vector<std::pair<int,NonTerminalSymbol*>*>* candidates,
		const std::string s) throw(std::string);

	/*--------------------------------------------------------------------------
		getLastAndConnective

		Returns the position of the last occurrence of 'and' or 'AND' in the
		given string, or std::string::npos if none was found.
	............................................................................
		@param	s				the input text
		@return					the position of the last occurence of 'and' or
								'AND' in the input text, or std::string::npos
								if none was found
	--------------------------------------------------------------------------*/
	const std::size_t getLastAndConnective(const std::string s) const;

	/*--------------------------------------------------------------------------
		buildPredefinedAutomatonTypeMap

		Builds the map from predefined automaton names (parsed in the grammar
		file) to a constant corresponding to the type of the predefined
		automaton.
	--------------------------------------------------------------------------*/
	void buildPredefinedAutomatonTypeMap();

	/*--------------------------------------------------------------------------
		buildPredefinedAutomatonMap

		Builds the map from predefined automaton names (parsed in the grammar
		file) to pointers to instances of the corresponding predefined
		automaton.
	--------------------------------------------------------------------------*/
	void buildPredefinedAutomatonMap();

	/*--------------------------------------------------------------------------
		buildDataExtractionRuleDefinitionMap

		Builds the map from constants representing the types of data extraction
		rule to	a list of strings corresponding to the expected list of
		parameters for that type.
	--------------------------------------------------------------------------*/
	void buildDataExtractionRuleDefinitionMap();

	/*--------------------------------------------------------------------------
		getDataExtractionRuleValues

		Given a string corresponding to the rule name, and an integer
		corresponding to the rule type, returns the list of parsed parameters
		for the rule. Throws an error is the required parameters were not found.
	............................................................................
		@param	rule			the name of the data extraction rule
		@return					the list of parameters parsed for the data
								extraction rule
		@throw					an error message if the required parameters
								were not found
	--------------------------------------------------------------------------*/
	std::vector<std::string>* getDataExtractionRuleValues(std::string rule,
		const int type) throw(std::string);

	/*--------------------------------------------------------------------------
		validateDataExtractionRuleIdentifier

		Validates a data extraction rule identifier definition and throws an
		error if the identifier is invalid.
	............................................................................
		@param	identifier		a data extraction rule identifier
		@throw					an error message if the identifier is not valid
	--------------------------------------------------------------------------*/
	void validateDataExtractionRuleIdentifier(
		const std::string identifier) throw(std::string);

	/*--------------------------------------------------------------------------
		constructIdentifier

		Given an accepting non-terminal symbol, and an identifier definition,
		constructs and returns an identifier.
	............................................................................
		@param	identifier		a data extraction rule identifier
		@param	can_be_null		true if "null" is a valid value for this
								identifier
		@param	non_t			a pointer to the accepting non-terminal symbol
		@return					the constructed identifier
	--------------------------------------------------------------------------*/
	const std::string constructIdentifier(const std::string identifier,
		const bool can_be_null,	NonTerminalSymbol* non_t);

	/*--------------------------------------------------------------------------
		validateDataExtractionRuleString

		Validates a data extraction rule string definition and throws an
		error if the string is invalid.
	............................................................................
		@param	string			a data extraction rule string
		@throw					an error message if the string is not valid
	--------------------------------------------------------------------------*/
	void validateDataExtractionRuleString(
		const std::string string) throw(std::string);

	/*--------------------------------------------------------------------------
		constructString

		Given a parsed data extraction rule string returns a string without
		enclosing quotes.
	............................................................................
		@param	string			a data extraction rule string
		@param	can_be_null		true if "null" is a valid value for this
								string
		@return					the constructed string
	--------------------------------------------------------------------------*/
	const std::string constructString(const std::string string,
		const bool can_be_null);

	/*--------------------------------------------------------------------------
		validateDataExtractionRuleBoolean

		Validates a data extraction rule boolean term and throws an	error if the
		boolean term is invalid.
	............................................................................
		@param	boolean			a data extraction rule boolean value
		@throw					an error message if the boolean is not valid
	--------------------------------------------------------------------------*/
	void validateDataExtractionRuleBoolean(
		const std::string boolean) throw(std::string);

	/*--------------------------------------------------------------------------
		constructBehaviourList

		Splits the list using the given split character, ignoring the entries
		defined in ignore_entries, and returns the parsed list.
	............................................................................
		@param	list			the string corresponding to the list of
								behaviours
		@param	split_char		the character with which to split the list
		@param	ignore_entries	a list of values, where for each index i in the
								list the i^th element of the list is ignored,
								can be null. For instance [1,3,4] would ignore
								the 1st, 3rd and 4th entries in the list
		@return					the list of behaviours
	--------------------------------------------------------------------------*/
	std::list<std::string> constructBehaviourList(
		const std::string list, const std::string split_char,
		const std::string ignore_entries);
};

#endif
