/*==============================================================================
	IntermediateFormParser

	File			: intermediate_form_parser.cpp
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
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "utility.h"
#include "enums/time_constraint_type.h"
#include "enums/tree_node_type.h"
#include "intermediate/action_wait.h"
#include "intermediate/action_execute.h"
#include "intermediate/action_execute_non_deterministic.h"
#include "intermediate/action_enum_value_assignment.h"
#include "intermediate/action_prop_value_assignment.h"
#include "intermediate/precondition_prop_value_check.h"
#include "intermediate/precondition_enum_value_check.h"
#include "intermediate/precondition_timing_constraint.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/tree_node.h"
#include "intermediate_parser/intermediate_form_parser.h"
#include "intermediate_parser/automaton.h"
#include "intermediate_parser/non_terminal_symbol.h"
#include "intermediate_parser/terminal_symbol.h"
#include "intermediate_parser/tokenize.h"

const int IntermediateFormParser::NO_TYPE = 0;
const int IntermediateFormParser::PRECONDITION_PROP_VALUE_CHECK = 1;
const int IntermediateFormParser::PRECONDITION_ENUM_VALUE_CHECK = 2;
const int IntermediateFormParser::PRECONDITION_TIME_CONSTRAINT = 3;
const int IntermediateFormParser::ACTION_PROP_VALUE_ASSIGNMENT = 4;
const int IntermediateFormParser::ACTION_ENUM_VALUE_ASSIGNMENT = 5;
const int IntermediateFormParser::ACTION_EXECUTE_BEHAVIOUR_NON_D = 6;
const int IntermediateFormParser::ACTION_EXECUTE_BEHAVIOUR = 7;
const int IntermediateFormParser::ACTION_DELAY = 8;
const int IntermediateFormParser::STATE_N_SECONDS = 9;

/*------------------------------------------------------------------------------
	parse

	Parses the given grammar file, data extraction rule file, and control
	rule file. Returns true if successful or false otherwise.
------------------------------------------------------------------------------*/
bool IntermediateFormParser::parse(const std::string grammar_rule_file,
	const std::string data_rule_file, const std::string control_rule_file)
{
	try
	{
		parseGrammarFile(grammar_rule_file);
		parseDataExtractionFile(data_rule_file);
		parseControlRuleFile(control_rule_file);
	}
	catch(std::string& error)
	{
		displayError("[parse]->\ncould not parse\n"
			+ error);
		return false;
	}
	return true;
}

/*------------------------------------------------------------------------------
	parseGrammarFile

	Parses the given grammar file. Throws an error if parsing was
	unsuccessful.
------------------------------------------------------------------------------*/
void IntermediateFormParser::parseGrammarFile(
	const std::string filename) throw (std::string)
{
	std::ifstream ifstream(filename);

	try
	{
		if(!ifstream)
		{
			throw("error loading file \'" + filename + "\'\n");
		}

		// read the file in as a string
		std::string input_file_string = std::string(
			std::istreambuf_iterator<char>(ifstream),
			std::istreambuf_iterator<char>());

		// do this while there is more to read
		while(hasNextToken(input_file_string))
		{
			std::string line = getNextLine(input_file_string);
			// get the left hand side non-terminal symbol
			std::string non_t = getNextToken(line);

			NonTerminalSymbol* new_non_terminal;
			if(!isNonTerminalSymbol(non_t))
			{
				throw("\'" + non_t + "\' bad non_terminal name on left\n");
			}
			if(getNextToken(line) != "::=")
			{
				throw("expected '::=' in file \'" + filename + "\'\n");
			}
			// create a new empty non-terminal symbol
			new_non_terminal =
				new NonTerminalSymbol(getSymbolText(non_t),
					Automaton::REPEAT_NONE);
			while(hasNextToken(line))
			{
				std::string token = getNextToken(line);
				bool is_non_terminal = false;
				bool is_terminal = false;
				bool is_predefined = false;

				if(isPredefinedAutomaton(token))
				{
					is_predefined = true;
				}
				else if(isNonTerminalSymbol(token))
				{
					is_non_terminal = true;
				}
				else if(isTerminalSymbol(token))
				{
					is_terminal = true;
				}

				if(!is_non_terminal && !is_terminal && !is_predefined)
				{
					// this is an invalid token
					delete new_non_terminal;
					throw("\'" + token
						+ "\' is not a terminal or non-terminal\n");
				}

				int repeat;
				std::string prefix = getSymbolPrefix(token);
				if(prefix == "+")
				{
					repeat = Automaton::REPEAT_ONE_OR_MORE;
				}
				else if(isInteger(prefix))
				{
					std::istringstream stream(prefix);
					stream >> repeat;
				}
				else
				{
					repeat = Automaton::REPEAT_NONE;
				}

				std::string symbol_name = getSymbolText(token);

				if(is_terminal)
				{
					// create a new terminal symbol...
					TerminalSymbol* new_terminal =
						new TerminalSymbol(symbol_name,
								repeat);
					// ...and add it to the list of automata in the non-terminal
					new_non_terminal->addAutomaton(new_terminal);
				}
				else if(is_predefined)
				{
					// get a copy of the predefined automaton
					Automaton* predefined_symbol =
						getPredefinedAutomatonByName(symbol_name);
					predefined_symbol->setRepeat(repeat);
					// ...and add it to the list of automata in the non-terminal
					new_non_terminal->addAutomaton(predefined_symbol);
				}
				else
				{
					// has this non-terminal symbol already been defined?
					auto it = std::find_if(
						non_terminal_symbols.begin(),
						non_terminal_symbols.end(),
						[&](NonTerminalSymbol* n)
						{
							return symbol_name == n->getName();
						});
					if(it == non_terminal_symbols.end())
					{
						// it was not defined, throw an error
						delete new_non_terminal;
						throw("non-terminal symbol " + symbol_name
							+ " was not already defined\n");
					}
					// it was already defined, make a copy of it
					NonTerminalSymbol* n = (NonTerminalSymbol*)(*it)->getCopy();
					n->setRepeat(repeat);
					// ...and add it to the list of automata in the non-terminal
					new_non_terminal->addAutomaton(n);
				}
			}
			// check to see if the new non-terminal has already been defined
			auto it = std::find_if(
				non_terminal_symbols.begin(),
				non_terminal_symbols.end(),
				[&](NonTerminalSymbol* n)
				{
					return getSymbolText(non_t) == n->getName();
				});
			if(it != non_terminal_symbols.end())
			{
				// it was already defined, throw an error
				delete new_non_terminal;
				throw("non_terminal " + non_t + " is already defined\n");
			}
			int type;
			if((type = getAutomatonType(getSymbolText(non_t)))
				!= NO_TYPE)
			{
				// this matched a type, add the automaton to the corresponding
				// list
				non_terminal_symbols_by_type[type - 1].push_back(
					new_non_terminal);
			}
			// add the new non-terminal to the list of non-terminals
			non_terminal_symbols.push_back(new_non_terminal);
		}
	}
	catch(std::string& error)
	{
		throw(std::string("[parseGrammarDefinitions]->\n")
			+ "error in file \'" + filename + "\'\n"
			+ error);
	}
	ifstream.close();
}

/*------------------------------------------------------------------------------
	isTerminalSymbol

	Returns true if the given token is a valid terminal symbol.
------------------------------------------------------------------------------*/
const bool IntermediateFormParser::isTerminalSymbol(
	const std::string token) const
{
	int token_length = token.size();

	if(token_length < 3 || token.at(token_length - 1) != '\'')
	{
		return false;
	}

	int pos = 0;

	if(token.at(0) == '+')
	{
		if(token.at(1) != '\'')
		{
			return false;
		}
		pos += 2;
	}
	else
	{
		char c;
		while((c = token.at(pos)) >= '0' && c <= '9')
		{
			pos++;
		}

		if(token.at(pos) != '\'')
		{
			return false;
		}

		pos++;
	}

	return true;
}

/*------------------------------------------------------------------------------
	isNonTerminalSymbol

	Returns true if the given token is a valid non-terminal symbol.
------------------------------------------------------------------------------*/
const bool IntermediateFormParser::isNonTerminalSymbol(
	const std::string token) const
{
	int token_length = token.size();

	if(token_length < 3 || token.at(token_length - 1) != '>')
	{
		return false;
	}

	int pos = 0;

	if(token.at(0) == '+')
	{
		if(token.at(1) != '<')
		{
			return false;
		}
		pos += 2;
	}
	else
	{
		char c;
		while((c = token.at(pos)) >= '0' && c <= '9')
		{
			pos++;
		}

		if(token.at(pos) != '<')
		{
			return false;
		}

		pos++;
	}

	return true;
}

/*------------------------------------------------------------------------------
	getSymbolPrefix

	Returns the prefix (empty, an integer, or + for one or more) of the
	given symbol. Does not check the symbol for validity first.
------------------------------------------------------------------------------*/
const std::string IntermediateFormParser::getSymbolPrefix(
	const std::string symbol) const
{
	std::string prefix = "";
	char c;
	int pos = 0;

	while((c = symbol.at(pos++)) != '\'' && c != '<')
	{
		prefix += c;
	}

	return prefix;
}

/*------------------------------------------------------------------------------
	getSymbolText

	Returns the text of the given symbol, so for +'Hello' this would return
	Hello.
------------------------------------------------------------------------------*/
const std::string IntermediateFormParser::getSymbolText(
	const std::string symbol) const
{
	std::string text = "";
	int symbol_length = symbol.size();
	char c;
	int pos = 0;

	while((c = symbol.at(pos)) == '+' || (c >= '0' && c <= '9'))
	{
		pos++;
	}

	pos++;

	while((pos < symbol_length - 1) && ((c = symbol.at(pos++)) != '\''
		|| c != '>'))
	{
		text += c;
	}

	return text;
}

/*------------------------------------------------------------------------------
	isPredefinedAutomaton

	Returns true if the given symbol name corresponds to an existing
	predefined automaton.
------------------------------------------------------------------------------*/
const bool IntermediateFormParser::isPredefinedAutomaton(
	const std::string symbol) const
{
	if(isNonTerminalSymbol(symbol))
	{
		auto it = predefined_automaton_map.find(getSymbolText(symbol));
		return it != predefined_automaton_map.end();
	}
	return false;
}

/*------------------------------------------------------------------------------
	getPredefinedAutomatonByName

	Given the name of a predefined automaton, returns a pointer to an
	instance of that automaton.
------------------------------------------------------------------------------*/
Automaton* IntermediateFormParser::getPredefinedAutomatonByName(
	const std::string name) const
{
	return predefined_automaton_map.find(name)->second->getCopy();
}

/*------------------------------------------------------------------------------
	parseDataExtractionFile

	Parses the data extraction rule file. Throws an error if parsing was
	unsuccessful.
------------------------------------------------------------------------------*/
void IntermediateFormParser::parseDataExtractionFile(
	const std::string filename) throw (std::string)
{
	std::ifstream ifstream(filename);
	std::vector<std::string>* values = nullptr;

	try
	{
		if(!ifstream)
		{
			throw("error loading file \'" + filename + "\'\n");
		}
		// read the file in as a string
		std::string input_file_string(
			(std::istreambuf_iterator<char>(ifstream)),
			std::istreambuf_iterator<char>());

		for(int type = RULE_TYPE_LOWER_BOUND;
			type <= RULE_TYPE_UPPER_BOUND; type++)
		{
			// get the list of non-terminal symbols of this type
			std::list<NonTerminalSymbol*> non_ts =
				non_terminal_symbols_by_type[type - 1];
			auto it = non_ts.begin();
			auto end = non_ts.end();
			while(it != end)
			{
				values = nullptr;
				NonTerminalSymbol* symbol = *it;
				std::string symbol_name = symbol->getName();
				// find the corresponding data extraction rule in the input
				// from the file
				std::size_t pos = input_file_string.find(symbol_name);
				if(pos == std::string::npos)
				{
					throw(std::string("found no corresponding data extraction")
						+ " rule for grammar rule: " + symbol_name + "\n");
				}
				else
				{
					std::string file_from_pos =
						input_file_string.substr(pos + symbol_name.size());
					// get the line defining the grammar rule
					std::string grammar_rule =
						getNextLine(file_from_pos);
					try
					{
						// extract the data extraction rule parameters
						values = getDataExtractionRuleValues(
							grammar_rule, type);
						// and validate them
						validateDataExtractionRuleValues(values, type);
					}
					catch(std::string& error)
					{
						throw("error in data extraction rule \'" + symbol_name
							+ "\'\n" + error);
					}
					// add the parameter values to the map
					data_extraction_rule_values_map.insert(
						std::pair<const std::string, std::vector<std::string>>(
							symbol_name, std::vector<std::string>(*values)));
					delete values;
				}
				it++;
			}
		}
	}
	catch(std::string& error)
	{
		if(values != nullptr)
		{
			delete values;
		}
		throw(std::string("[parseRuleDefinitions]->\n")
			+ "error in file \'" + filename + "\'\n"
			+ error);
	}

	ifstream.close();
}

/*------------------------------------------------------------------------------
	getDataExtractionRuleValues

	Given a string corresponding to the rule name, and an integer
	corresponding to the rule type, returns the list of parsed parameters
	for the rule. Throws an error is the required parameters were not found.
------------------------------------------------------------------------------*/
std::vector<std::string>* IntermediateFormParser::getDataExtractionRuleValues(
	std::string rule, const int type) throw(std::string)
{
	// get the list of expected parameter values for a data extraction rule of
	// the given type
	std::list<std::string> data_extraction_rule_value_list =
		getDataExtractionRuleDefinitionByType(type);
	std::vector<std::string>* values = new std::vector<std::string>();

	auto it = data_extraction_rule_value_list.begin();
	auto end = data_extraction_rule_value_list.end();
	getShortestPrefixEndingWithChar(rule, ';');
	try
	{
		while(it != end)
		{
			std::string required_value = *it;
			std::string value_assignment =
				getShortestPrefixEndingWithChar(rule, ';');
			std::string value_assignment_copy = value_assignment;
			// get the name of the parameter
			std::string value_name =
				getShortestPrefixEndingWithChar(
					value_assignment_copy, '=');
			if(value_name.empty())
			{
				throw(std::string("missing value name\n"));
			}
			value_name = value_name.substr(0, value_name.size() - 1);
			if(value_name != required_value)
			{
				throw("found \'" + value_name + "\', expected \'"
					+ required_value + "\'\n");
			}
			// get the parameter value
			std::string value =
				value_assignment.substr(value_name.size() + 1);
			value = value.substr(0, value.size() - 1);
			// and add it to the list
			values->push_back(value);
			it++;
		}
	}
	catch(std::string& error)
	{
		delete values;
		throw("[getDataExtractionRuleValues]->\n"
			+ error);
	}

	return values;
}

/*------------------------------------------------------------------------------
	validateDataExtractionRuleValues

	Checks the validity of data extraction rule definitions. Throws an
	error if a rule is not valid.
------------------------------------------------------------------------------*/
void IntermediateFormParser::validateDataExtractionRuleValues(
	std::vector<std::string>* values, const int type) throw(std::string)
{
	try
	{
		std::vector<std::string> rule_values = *values;
		switch(type)
		{
		/*----------------------------------------------------------------------
			parameter 0: prop variable name	[identifier]
			parameter 1: truth value		[identifier]
			parameter 2: true				[string]
			parameter 3: false				[string]
			parameter 4: non-deterministic	[boolean]
		----------------------------------------------------------------------*/
		case PRECONDITION_PROP_VALUE_CHECK:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleIdentifier(rule_values[1]);
			validateDataExtractionRuleString(rule_values[2]);
			validateDataExtractionRuleString(rule_values[3]);
			validateDataExtractionRuleBoolean(rule_values[4]);
			// non-deterministic variables should have 'null' for parameters
			// 1, 2 and 3
			if(toLower(rule_values[4]) == "true" && (rule_values[1] != "null"
				|| rule_values[2] != "null" || rule_values[3] != "null"))
			{
				throw(std::string("unexpected non-null value for ")
					+ "non-deterministic variable\n");
			}
			break;

		/*----------------------------------------------------------------------
			parameter 0: enum variable name	[identifier]
			parameter 1: enum value			[identifier]
			parameter 2: has none value		[boolean]
			parameter 3: non-deterministic	[boolean]
		----------------------------------------------------------------------*/
		case PRECONDITION_ENUM_VALUE_CHECK:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleIdentifier(rule_values[1]);
			validateDataExtractionRuleBoolean(rule_values[2]);
			validateDataExtractionRuleBoolean(rule_values[3]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: start time			[identifier]
			parameter 1: end time			[identifier]
		----------------------------------------------------------------------*/
		case PRECONDITION_TIME_CONSTRAINT:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleIdentifier(rule_values[1]);
			// requires at least one of start time or end time
			if(rule_values[0] == "null" && rule_values[1] == "null")
			{
				throw(std::string("time constraint has neither a start time ")
					+ "nor an end time\n");
			}
			break;

		/*----------------------------------------------------------------------
			parameter 0: prop variable name	[identifier]
			parameter 1: truth value		[identifier]
			parameter 2: true				[string]
			parameter 3: false				[string]
		----------------------------------------------------------------------*/
		case ACTION_PROP_VALUE_ASSIGNMENT:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleIdentifier(rule_values[1]);
			validateDataExtractionRuleString(rule_values[2]);
			validateDataExtractionRuleString(rule_values[3]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: enum variable name	[identifier]
			parameter 1: enum value			[identifier]
			parameter 2: resets				[boolean]
		----------------------------------------------------------------------*/
		case ACTION_ENUM_VALUE_ASSIGNMENT:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleIdentifier(rule_values[1]);
			validateDataExtractionRuleBoolean(rule_values[2]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: behaviour values	[identifier]
			parameter 1: split character	[string]
			parameter 2: ignore entries		[identifier]
		----------------------------------------------------------------------*/
		case ACTION_EXECUTE_BEHAVIOUR_NON_D:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleString(rule_values[1]);
			validateDataExtractionRuleIdentifier(rule_values[2]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: behaviour name		[identifier]
		----------------------------------------------------------------------*/
		case ACTION_EXECUTE_BEHAVIOUR:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: seconds			[identifier]
		----------------------------------------------------------------------*/
		case ACTION_DELAY:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			break;

		/*----------------------------------------------------------------------
			parameter 0: seconds			[identifier]
			parameter 1: been in state		[boolean]
			parameter 2: was in state		[boolean]
		----------------------------------------------------------------------*/
		case STATE_N_SECONDS:
			validateDataExtractionRuleIdentifier(rule_values[0]);
			validateDataExtractionRuleBoolean(rule_values[1]);
			validateDataExtractionRuleBoolean(rule_values[2]);
			// term should be either a been in state OR was in state term
			if(rule_values[1] == rule_values[2])
			{
				throw(std::string("state-n-seconds term should be ")
					+ "either a was-in-state term or a been-in-state term\n");
			}
			break;
		}
	}
	catch(std::string& error)
	{
		throw("[validateDataExtractionRuleValues]->\n"
			+ error);
	}
}

/*------------------------------------------------------------------------------
	getAutomatonType

	Given the name of an automaton, returns the type of automaton
	corresponding to that name.
------------------------------------------------------------------------------*/
const int IntermediateFormParser::getAutomatonType(
	const std::string name) const
{
	std::string name_prefix = "";
	int name_length = name.size();
	int index = 0;
	char c;
	while(index < name_length && ((c = name.at(index)) < '0' || c > '9'))
	{
		name_prefix += c;
		index++;
	}
	auto it = automaton_type_map.find(name_prefix);
	if(it != automaton_type_map.end())
	{
		std::pair<const std::string, const int> pair = *it;
		if(isInteger(name.substr(index)))
		{
			return pair.second;
		}
	}
	return NO_TYPE;
}

/*------------------------------------------------------------------------------
	parseControlRuleFile

	Parses the control rule rule file. Throws an error if parsing was
	unsuccessful.
------------------------------------------------------------------------------*/
void IntermediateFormParser::parseControlRuleFile(
	const std::string filename) throw (std::string)
{
	std::ifstream ifstream(filename);
	Behaviour* new_behaviour = nullptr;
	try
	{
		if(!ifstream)
		{
			throw("error loading file \'" + filename + "\'\n");
		}
		// read the file in as a string
		std::string input_file_string(
			(std::istreambuf_iterator<char>(ifstream)),
			std::istreambuf_iterator<char>());
		getNextLine(input_file_string);
		intermediate_form = new IntermediateForm();
		while(hasNextToken(input_file_string))
		{
			new_behaviour = nullptr;
			std::string next_line = getNextLine(input_file_string);
			std::string line_copy = next_line;
			std::string behaviour_name = getNextToken(next_line);
			std::string priority = getNextToken(next_line);
			if(!isInteger(priority))
			{
				throw("priority value \'" + priority
					+ "\' must be an integer\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string interruptible = getNextToken(next_line);
			if(interruptible != "1" && interruptible != "0")
			{
				throw("interruptible value \'"  + interruptible
					+ " \' must be 0 or 1\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string schedulable = getNextToken(next_line);
			if(schedulable != "1" && schedulable != "0")
			{
				throw("schedulable value \'"  + schedulable
					+ " \' must be 0 or 1\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string rule_order = getNextToken(next_line);
			if(!isInteger(rule_order))
			{
				throw("rule order \'"  + rule_order
					+ " \' must be an integer\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string rule_type = getNextToken(next_line);
			if(rule_type != "R" && rule_type != "A")
			{
				throw("rule type \'"  + rule_type
					+ " \' must be R or A\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string and_or_connector = getNextToken(next_line);
			if(and_or_connector != "0" && and_or_connector != "1"
				&& and_or_connector != "2")
			{
				throw("anOrConnectorValue \'"  + and_or_connector
					+ " \' must be 0, 1 or 2\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string not_connector = getNextToken(next_line);
			if(not_connector != "0" && not_connector != "1")
			{
				throw("notConnectorValue \'"  + not_connector
					+ " \' must be 0 or 1\n"
					+ "at line:\n" + line_copy + "\n");
			}
			std::string rule_text = next_line;
			if(rule_type == "R")
			{
				std::stringstream info_stream;
				info_stream	<< "disambiguation of behaviour '"
					<< behaviour_name << "\'";
				Behaviour* behaviour =
					intermediate_form->getBehaviourByName(behaviour_name,
						info_stream.str());
				info_stream.clear();
				if(behaviour != nullptr)
				{
					if(behaviour->getPriority() != stringToInteger(priority))
					{
						throw(std::string("the priority of precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (priority = "
							+ integerToString(behaviour->getPriority())
							+ ")\n");
					}
					else if(behaviour->isInterruptible()
						!= (interruptible == "1"))
					{
						throw(std::string("interruptibility in precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (interruptible = "
							+ (behaviour->isInterruptible() ? "true" : "false")
							+ ")\n");
					}
					else if(behaviour->isSchedulable()
						!= (schedulable == "1"))
					{
						throw(std::string("schedulability in precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (schedulable = "
							+ (behaviour->isSchedulable() ? "true" : "false")
							+ ")\n");
					}
					Precondition* precondition = parsePrecondition(rule_text);
					precondition->setAndOrConnector(stringToInteger(
						and_or_connector));
					precondition->setNotConnector(stringToInteger(
						not_connector));
					precondition->setPreconditionOrder(
						stringToInteger(rule_order));
					behaviour->addPrecondition(precondition);
				}
				else
				{
					new_behaviour =
						new Behaviour(behaviour_name, stringToInteger(priority),
							(interruptible == "1" ?	true : false),
							(schedulable == "1" ? true: false));
					Precondition* precondition;
					try
					{
						precondition = parsePrecondition(rule_text);
						precondition->setAndOrConnector(stringToInteger(
							and_or_connector));
						precondition->setNotConnector(stringToInteger(
							not_connector));
						precondition->setPreconditionOrder(
							stringToInteger(rule_order));
					}
					catch(std::string& error)
					{
						delete new_behaviour;
						throw(error);
					}
					new_behaviour->addPrecondition(precondition);
					intermediate_form->addBehaviour(new_behaviour);
				}
			}
			else
			{
				std::stringstream info_stream;
				info_stream	<< "disambiguation of behaviour '"
					<< behaviour_name << "\'";
				Behaviour* behaviour =
					intermediate_form->getBehaviourByName(behaviour_name,
						info_stream.str());
				info_stream.clear();
				if(behaviour != nullptr)
				{
					if(behaviour->getPriority() != stringToInteger(priority))
					{
						throw(std::string("the priority of precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (priority = "
							+ integerToString(behaviour->getPriority())
							+ ")\n");
					}
					else if(behaviour->isInterruptible()
						!= (interruptible == "1"))
					{
						throw(std::string("interruptibility in precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (interruptible = "
							+ (behaviour->isInterruptible() ? "true" : "false")
							+ ")\n");
					}
					else if(behaviour->isSchedulable()
						!= (schedulable == "1"))
					{
						throw(std::string("schedulability in precondition:\n")
							+ line_copy + "\nconflicts with that of existing "
							+ "behaviour \'" + behaviour->getName() + "\' "
							+ " (schedulable = "
							+ (behaviour->isSchedulable() ? "true" : "false")
							+ ")\n");
					}
					Action* action = parseAction(rule_text,
							stringToInteger(rule_order));
					behaviour->addAction(action);
				}
				else
				{
					new_behaviour =
						new Behaviour(behaviour_name, stringToInteger(priority),
								(interruptible == "1" ?	true : false),
								(schedulable == "1" ? true: false));
					Action* action;
					try
					{
						action = parseAction(rule_text,
							stringToInteger(rule_order));
					}
					catch(std::string& error)
					{
						delete new_behaviour;
						throw(error);
					}
					new_behaviour->addAction(action);
					intermediate_form->addBehaviour(new_behaviour);
				}
			}
		}

		intermediate_form->validateBehaviours();
		if(g_flatten_behaviours)
		{
			intermediate_form->flattenNestedBehaviourExecutions();
		}
		for(Behaviour* b : intermediate_form->getBehaviours())
		{
			b->buildLogicalExpressionTree();
		}
	}
	catch(std::string& error)
	{
		if(intermediate_form != nullptr)
		{
			delete intermediate_form;
		}
		throw(std::string("[parseActionRuleSet]->\n")
			+ "error in file \'" + filename + "\'\n"
			+ error);
	}

	ifstream.close();
}

/*------------------------------------------------------------------------------
	parseAction

	Given a string and an integer corresponding to the order of the action
	in the corresponding behaviour, constructs and returns a new Action, or
	throws an error if parsing was unsuccessful.
------------------------------------------------------------------------------*/
Action*	IntermediateFormParser::parseAction(const std::string s,
	const int order) throw(std::string)
{
	Action* new_action = nullptr;
	std::size_t pos = s.find(" and wait for completion");
	std::string modified_s = s;
	if(pos == std::string::npos)
	{
		modified_s = s;
	}
	else
	{
		modified_s = s.substr(0, pos);
	}
	auto type_info_list = getAcceptingGrammarRules(s.substr(0, pos),
		std::vector<int>
		{
			ACTION_PROP_VALUE_ASSIGNMENT,
			ACTION_ENUM_VALUE_ASSIGNMENT,
			ACTION_EXECUTE_BEHAVIOUR_NON_D,
			ACTION_EXECUTE_BEHAVIOUR,
			ACTION_DELAY
		});
	validateAcceptingGrammarRules(&type_info_list);
	try
	{
		if(type_info_list.empty())
		{
			throw(std::string("unrecognised action\n"));
		}
		else
		{
			std::pair<int, NonTerminalSymbol*>* type_info;
			type_info = disambiguateCandidates(&type_info_list, s);
			std::vector<std::string> values = getDataExtractionRuleValuesByName(
				type_info->second->getName());
			std::string string_var_1;
			std::string string_var_2;
			std::string string_var_3;
			std::string string_var_4;
			std::list<std::string> list_var;
			bool bool_var;
			PropVariable* prop_var;
			EnumVariable* enum_var;
			ActionExecuteNonDeterministic* new_ex_non_d;
			std::stringstream info_stream;
			switch(type_info->first)
			{
			case ACTION_PROP_VALUE_ASSIGNMENT:
				string_var_1 = constructIdentifier(values[0], false,
					type_info->second);
				string_var_2 = constructIdentifier(values[1], false,
					type_info->second);
				string_var_3 = constructString(values[2], false);
				string_var_4 = constructString(values[3], false);

				bool_var = toLower(string_var_2) ==
					toLower(string_var_3) ? true : false;
				info_stream	<< "disambiguation of propositional variable '"
					<< string_var_1 << "\'";
				prop_var = intermediate_form->getPropVariableByName(string_var_1,
					info_stream.str());
				if(prop_var == nullptr)
				{
					prop_var = new PropVariable(string_var_1, false);
					intermediate_form->addPropVariable(prop_var);
				}
				new_action = new ActionPropValueAssignment(
					prop_var, bool_var, order);

				break;

			case ACTION_ENUM_VALUE_ASSIGNMENT:
				string_var_1 = constructIdentifier(values[0], false,
					type_info->second);
				string_var_2 = constructIdentifier(values[1], false,
					type_info->second);

				bool_var = toLower(values[2]) == "true" ? true : false;
				info_stream	<< "disambiguation of enumerated variable '"
					<< string_var_1 << "\'";
				enum_var = intermediate_form->getEnumVariableByName(string_var_1,
					info_stream.str());
				if(enum_var == nullptr)
				{
					enum_var = new EnumVariable(string_var_1);
					intermediate_form->addEnumVariable(enum_var);
				}
				if(bool_var)
				{
					enum_var->setResets();
				}
				string_var_2 = enum_var->addValue(string_var_2);
				new_action = new ActionEnumValueAssignment(
					enum_var, string_var_2, order);

				break;

			case ACTION_EXECUTE_BEHAVIOUR_NON_D:
				string_var_1 = constructIdentifier(values[0], false,
					type_info->second);
				string_var_2 = constructString(values[1], false);
				string_var_3 = values[2];
				list_var = constructBehaviourList(string_var_1,
					string_var_2, string_var_3);

				new_ex_non_d = new ActionExecuteNonDeterministic(order);
				std::for_each(list_var.begin(), list_var.end(),
					[&](std::string behaviour_value)
					{
						new_ex_non_d->addBehaviourValue(behaviour_value);
					});
				new_action = new_ex_non_d;

				break;

			case ACTION_EXECUTE_BEHAVIOUR:
				string_var_1 = constructIdentifier(values[0], false,
						type_info->second);
				if(string_var_1.at(0) == '\''
					&& string_var_1.at(string_var_1.size() - 1) == '\'')
				{
					string_var_1 =
						string_var_1.substr(1, string_var_1.size() - 2);
				}
				new_action = new ActionExecute(string_var_1, order);

				break;

			case ACTION_DELAY:
				string_var_1 = constructIdentifier(values[0], false,
					type_info->second);
				new_action = new ActionWait(stringToFloat(string_var_1), order);

				break;
			}
		}
	}
	catch(std::string& error)
	{
		for(auto pair : type_info_list)
		{
			delete pair;
		}
		throw(std::string("[parseAction]->\n")
			+ "error in action: " + trim(s) + "\n" + error);
	}
	for(auto pair : type_info_list)
	{
		delete pair;
	}
	return new_action;
}

/*------------------------------------------------------------------------------
	validateDataExtractionRuleIdentifier

	Validates a data extraction rule identifier definition and throws an
	error if the identifier is invalid.
------------------------------------------------------------------------------*/
void IntermediateFormParser::validateDataExtractionRuleIdentifier(
	const std::string identifier) throw(std::string)
{
	if(identifier == "null")
	{
		return;
	}
	try
	{
		validateDataExtractionRuleString(identifier);
	}
	catch(std::string &error)
	{
		int identifier_size = identifier.size();
		if(identifier_size < 3 || identifier.at(0) != '[' ||
			identifier.at(identifier_size - 1) != ']')
		{
			throw("[validateDataExtractionRuleIdentifier]->\n\'"
				+ identifier + "\' is not a valid identifier\n");
		}
		std::string values = identifier.substr(1, identifier_size - 2);
		std::stringstream stream(values);
		std::string item;
		while(std::getline(stream, item, ','))
		{
			if(!isInteger(item))
			{
				throw("[validateDataExtractionRuleIdentifier]->\n\'"
					+ item + "\' is not an integer in identifier \'"
					+ identifier + "\'\n");
			}
		}
	}
}

/*------------------------------------------------------------------------------
	constructIdentifier

	Given an accepting non-terminal symbol, and an identifier definition,
	constructs and returns an identifier.
------------------------------------------------------------------------------*/
const std::string IntermediateFormParser::constructIdentifier(
	const std::string identifier, const bool can_be_null,
	NonTerminalSymbol* non_t)
{
	if(can_be_null && identifier == "null")
	{
		return identifier;
	}
	std::string return_string;
	try
	{
		validateDataExtractionRuleString(identifier);
		return constructString(identifier, can_be_null);
	}
	catch(std::string& error)
	{
		int identifier_size = identifier.size();
		std::stringstream stream(identifier.substr(1, identifier_size - 2));
		std::string item;
		while(std::getline(stream, item, ','))
		{
			std::string automaton_input =
				non_t->getInputFromSubAutomaton(stringToInteger(item));
			if(!return_string.empty())
			{
				return_string += "_";
			}
			return_string += automaton_input;
		}
	}
	return return_string;
}

/*------------------------------------------------------------------------------
	validateDataExtractionRuleString

	Validates a data extraction rule string definition and throws an
	error if the string is invalid.
------------------------------------------------------------------------------*/
void IntermediateFormParser::validateDataExtractionRuleString(
	const std::string string) throw(std::string)
{
	if(string == "null")
	{
		return;
	}
	else
	{
		int string_length = string.size();
		if(string.empty() || string_length < 3 || string.at(0) != '\"'
			|| string.at(string_length - 1) != '\"')
		{
			throw("[validateDataExtractionRuleString]->\n\'"
				+ string + "\' is not a valid identifier\n");
		}
	}
}

/*------------------------------------------------------------------------------
	constructString

	Given a parsed data extraction rule string returns a string without
	enclosing quotes.
------------------------------------------------------------------------------*/
const std::string IntermediateFormParser::constructString(
	const std::string string, const bool can_be_null)
{
	if(can_be_null && string == "null")
	{
		return string;
	}
	return string.substr(1, string.size() - 2);
}

/*------------------------------------------------------------------------------
	validateDataExtractionRuleBoolean

	Validates a data extraction rule boolean term and throws an	error if the
	boolean term is invalid.
------------------------------------------------------------------------------*/
void IntermediateFormParser::validateDataExtractionRuleBoolean(
	const std::string boolean) throw(std::string)
{
	if(boolean == "null")
	{
		return;
	}
	else
	{
		if(!caseInsensitiveCompare(boolean,"true")
			&& !caseInsensitiveCompare(boolean,"false"))
		{
			throw("[validateDataExtractionRuleBoolean]->\n\'"
				+ boolean + "\' should be \'true\', \'false\' or \'null\'\n");
		}
	}
}

/*------------------------------------------------------------------------------
	constructBehaviourList

	Splits the list using the given split character, ignoring the entries
	defined in ignore_entries, and returns the parsed list.
------------------------------------------------------------------------------*/
std::list<std::string> IntermediateFormParser::constructBehaviourList(
	const std::string list, const std::string split_char,
	const std::string ignore_entries)
{
	std::list<int> ignore_entries_list;
	if(ignore_entries != "null")
	{
		std::stringstream stream(
			ignore_entries.substr(1, ignore_entries.size() - 2));
		std::string item;
		while(std::getline(stream, item, ','))
		{
			ignore_entries_list.push_back(stringToInteger(item));
		}
	}
	std::list<std::string> behaviour_names;
	std::stringstream stream(list);
	std::string item;
	char c = split_char.at(0);
	int list_index = 1;
	while(std::getline(stream, item, c))
	{
		if(std::find_if(ignore_entries_list.begin(), ignore_entries_list.end(),
			[&](int entry)
			{
				return entry == list_index;
			}) == ignore_entries_list.end())
		{
			behaviour_names.push_back(trim(item));
		}
		list_index++;
	}
	return behaviour_names;
}

/*------------------------------------------------------------------------------
	parsePrecondition

	Given a string corresponding to a precondition, possibly with a
	state-n-seconds term, constructs and returns a new Precondition, or
	throws an error if parsing was unsuccessful.
------------------------------------------------------------------------------*/
Precondition* IntermediateFormParser::parsePrecondition(const std::string s)
	throw(std::string)
{
	Precondition* new_precondition = nullptr;
	const std::size_t next_and = getLastAndConnective(s);
	std::pair<const int, const std::pair<const bool, const bool>>*
		state_info = nullptr;
	try
	{
		if(next_and != std::string::npos)
		{
			std::string rest_of_s = s.substr(next_and + 5);
			try
			{
				state_info = parseStateNSeconds(rest_of_s);
			}
			catch(std::string& error_state)
			{
				try
				{
					new_precondition = parsePreconditionNoStateNSeconds(s);
					delete state_info;
					return new_precondition;
				}
				catch(std::string& error_precon)
				{
					throw(error_state + "OR\n" + error_precon);
				}
			}
			new_precondition = parsePreconditionNoStateNSeconds(
				s.substr(0, next_and));
			if(state_info->second.first)
			{
				new_precondition->setTimeConstraintType(
					TimeConstraintType::BEEN_IN_STATE_FOR);
			}
			else
			{
				new_precondition->setTimeConstraintType(
					TimeConstraintType::WAS_IN_STATE_WITHIN);
			}
			new_precondition->setTimeConstraintSeconds(
				state_info->first);
			delete state_info;
		}
		else
		{
			new_precondition = parsePreconditionNoStateNSeconds(s);
			delete state_info;
		}
	}
	catch(std::string& error)
	{
		if(state_info != nullptr)
		{
			delete state_info;
		}
		throw(std::string("[parsePrecondition]->\n")
			+ "error in precondition: " + trim(s) + "\n" + error);
	}
	return new_precondition;
}

/*------------------------------------------------------------------------------
	parsePreconditionNoStateNSeconds

	Given a string corresponding to a precondition, with no state-n-seconds
	term, constructs and returns a new Precondition, or	throws an error if
	parsing was unsuccessful.
------------------------------------------------------------------------------*/
Precondition* IntermediateFormParser::parsePreconditionNoStateNSeconds(
	const std::string s) throw(std::string)
{
	Precondition* new_precondition = nullptr;
	std::vector<std::pair<int, NonTerminalSymbol*>*>
		type_info_list;
	try
	{
		type_info_list = getAcceptingGrammarRules(s,
			std::vector<int>
			{
				PRECONDITION_PROP_VALUE_CHECK,
				PRECONDITION_ENUM_VALUE_CHECK,
				PRECONDITION_TIME_CONSTRAINT
			});
		validateAcceptingGrammarRules(&type_info_list);
		if(type_info_list.empty())
		{
			throw(std::string("unrecognised precondition\n"));
		}
		std::pair<int, NonTerminalSymbol*>* type_info;
		if(type_info_list.size() == 1)
		{
			type_info = type_info_list[0];
		}
		else
		{
			type_info = disambiguateCandidates(&type_info_list, s);
		}
		std::vector<std::string> values = getDataExtractionRuleValuesByName(
			type_info->second->getName());
		std::string string_var_1;
		std::string string_var_2;
		std::string string_var_3;
		std::string string_var_4;
		std::string string_var_5;
		std::list<std::string> list_var;
		bool bool_var;
		PropVariable* prop_var;
		EnumVariable* enum_var;
		std::stringstream info_stream;
		switch(type_info->first)
		{
		case PRECONDITION_PROP_VALUE_CHECK:
			string_var_1 = constructIdentifier(values[0], false,
				type_info->second);
			string_var_2 = constructIdentifier(values[1], true,
				type_info->second);
			string_var_3 = constructString(values[2], true);
			string_var_4 = constructString(values[3], true);

			bool_var = toLower(string_var_2) ==
				toLower(string_var_3) ? true : false;
			info_stream	<< "disambiguation of propositional variable '"
				<< string_var_1 << "'";
			prop_var = intermediate_form->getPropVariableByName(string_var_1,
				info_stream.str());

			if(prop_var == nullptr)
			{
				prop_var = new PropVariable(string_var_1,
					toLower(values[4]) == "true");
				intermediate_form->addPropVariable(prop_var);
			}
			else if(toLower(values[4]) == "true")
			{
				prop_var->setNonDeterministic();
			}
			new_precondition = new PreconditionPropValueCheck(prop_var,
				bool_var, TimeConstraintType::NONE, -1);

			break;

		case PRECONDITION_ENUM_VALUE_CHECK:
			string_var_1 = constructIdentifier(values[0], false,
				type_info->second);
			string_var_2 = constructIdentifier(values[1], true,
				type_info->second);

			info_stream	<< "disambiguation of enumerated variable name '"
				<< string_var_1 << '\'';
			enum_var = intermediate_form->getEnumVariableByName(
				string_var_1, info_stream.str());
			if(enum_var == nullptr)
			{
				enum_var = new EnumVariable(string_var_1);
				intermediate_form->addEnumVariable(enum_var);
			}
			if(toLower(values[2]) == "true")
			{
				enum_var->setHasNoneValue();
			}
			if(toLower(values[3]) == "true")
			{
				enum_var->setNonDeterministic();
			}
			string_var_2 = enum_var->addValue(string_var_2);
			new_precondition = new PreconditionEnumValueCheck(enum_var,
				string_var_2, TimeConstraintType::NONE, -1);

			break;

		case PRECONDITION_TIME_CONSTRAINT:
			string_var_1 = constructIdentifier(values[0], true,
				type_info->second);
			string_var_2 = constructIdentifier(values[1], true,
				type_info->second);
			if(string_var_1 == "null")
			{
				string_var_1 = "00:00:00";
			}
			else if(string_var_1.size() == 5)
			{
				string_var_1 += ":00";
			}
			else if(string_var_1.size() == 2)
			{
				string_var_1 += ":00:00";
			}
			if(string_var_2 == "null")
			{
				string_var_2 = "23:59:00";
			}
			else if(string_var_2.size() == 5)
			{
				string_var_2 += ":00";
			}
			else if(string_var_2.size() == 2)
			{
				string_var_2 += ":00:00";
			}
			new_precondition = new PreconditionTimingConstraint(
				string_var_1, string_var_2, TimeConstraintType::NONE, -1);

			break;
		}
	}
	catch(std::string& error)
	{
		for(auto pair : type_info_list)
		{
			delete pair;
		}
		throw(std::string("[parsePrecondition]->\n")
			+ "error in precondition: " + trim(s) + "\n" + error);
	}
	for(auto pair : type_info_list)
	{
		delete pair;
	}
	return new_precondition;
}

/*------------------------------------------------------------------------------
	validateAcceptingGrammarRules

	Conducts a number of additional checks to validate parsed grammar rules.
	Throws an error if a rule is not valid.
------------------------------------------------------------------------------*/
void IntermediateFormParser::validateAcceptingGrammarRules(
		std::vector<std::pair<int, NonTerminalSymbol*>*>* candidates)
			throw(std::string)
{
	unsigned int index = 0;
	while(index < candidates->size())
	{
		auto type_info = (*candidates)[index];
		std::vector<std::string> values = getDataExtractionRuleValuesByName(
			type_info->second->getName());
		std::list<std::string> list_var;
		std::string truth_value;
		std::string true_value;
		std::string false_value;
		std::string split_char;
		std::string behaviour_list;
		std::string item;
		std::stringstream stream;
		int count;
		bool is_valid = true;
		bool is_deterministic;
		switch(type_info->first)
		{
		case PRECONDITION_PROP_VALUE_CHECK:
			is_deterministic = toLower(values[4]) == "false";
			if(is_deterministic)
			{
				truth_value = toLower(constructIdentifier(values[1],
					false, type_info->second));
				true_value = toLower(constructString(values[2], false));
				false_value = toLower(constructString(values[3], false));
				if(truth_value != true_value && truth_value != false_value)
				{
					is_valid = false;
				}
			}
			break;

		case PRECONDITION_ENUM_VALUE_CHECK:
			if(g_disallow_true_false_enum)
			{
				if(toLower(constructIdentifier(values[1],
					false, type_info->second)) == "true"
					|| toLower(constructIdentifier(values[1],
					false, type_info->second)) == "false")
				{
					is_valid = false;
				}
			}
			break;

		case PRECONDITION_TIME_CONSTRAINT:

			break;

		case ACTION_PROP_VALUE_ASSIGNMENT:
			truth_value = toLower(constructIdentifier(values[1],
				true, type_info->second));
			true_value = toLower(constructString(values[2], false));
			false_value = toLower(constructString(values[3], false));
			if(truth_value != true_value && truth_value != false_value)
			{
				is_valid = false;
			}
			break;

		case ACTION_ENUM_VALUE_ASSIGNMENT:
			if(g_disallow_true_false_enum)
			{
				if(toLower(constructIdentifier(values[1],
					false, type_info->second)) == "true"
					|| toLower(constructIdentifier(values[1],
					false, type_info->second)) == "false")
				{
					is_valid = false;
				}
			}
			break;

		case ACTION_EXECUTE_BEHAVIOUR_NON_D:
			split_char = constructString(values[1], false);
			behaviour_list = constructIdentifier(values[0], false,
					type_info->second);
			stream << behaviour_list;
			count = 0;
			while(std::getline(stream, item, split_char.at(0)))
			{
				count++;
			}
			if(count < 2)
			{
				is_valid = false;
			}
			break;

		case ACTION_EXECUTE_BEHAVIOUR:

			break;

		case ACTION_DELAY:

			break;

		}
		if(!is_valid)
		{
			delete type_info;
			candidates->erase(candidates->begin() + index);
		}
		else
		{
			index++;
		}
	}
}

/*------------------------------------------------------------------------------
	disambiguateCandidates

	Prompts the user to select a matching grammar rule for a parsed term
	if more than one match is found. A pair corresponding to the type
	and accepting non-terminal symbol for the selected grammar rule is then
	returned. Throws an error if no match is found.
------------------------------------------------------------------------------*/
std::pair<int, NonTerminalSymbol*>*
	IntermediateFormParser::disambiguateCandidates(
		std::vector<std::pair<int, NonTerminalSymbol*>*>* candidates,
		const std::string s) throw(std::string)
{
	if(candidates->size() == 0)
	{
		throw(std::string("[disambiguateCandidates]->\n") +
			"did not match any data extraction rule\n");
	}
	else if(candidates->size() == 1)
	{
		return (*candidates)[0];
	}
	else
	{
		displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2,
			"multiple matching data extraction rules");
		std::cout << "multiple matches for: " << s << std::endl;
		int index = 1;
		std::for_each(candidates->begin(), candidates->end(),
			[&](std::pair<int, NonTerminalSymbol*>* pair)
			{
				std::cout << "  " << index++ << ": " << pair->second->getName()
					<< std::endl;
			});
		bool valid = false;
		std::string selected_index;
		int selected_index_int;
		do
		{
			std::cout << "select a match by index: ";
			std::cin >> selected_index;
			if(isInteger(selected_index))
			{
				selected_index_int = stringToInteger(selected_index);
				if(selected_index_int >= 1 && selected_index_int < index)
				{
					valid = true;
				}
			}

		} while (!valid);
		return (*candidates)[selected_index_int - 1];
	}
}

/*------------------------------------------------------------------------------
	getLastAndConnective

	Returns the position of the last occurrence of 'and' or 'AND' in the
	given string, or std::string::npos if none was found.
------------------------------------------------------------------------------*/
const std::size_t IntermediateFormParser::getLastAndConnective(
	const std::string s) const
{
	std::size_t pos_and;
	pos_and = toLower(s).rfind(" and ");
	if(pos_and == std::string::npos)
	{
		return std::string::npos;
	}
	else
	{
		return pos_and;
	}
}

/*------------------------------------------------------------------------------
	parseStateNSeconds

	Given a string corresponding to a state-n-seconds term, returns a pair
	where the first element is an integer corresponding to the number of
	seconds, and the second element is a pair of booleans, where the first
	element	is true and the second element is false if the term is a
	been_in_state_for term, or conversely the first element is false and the
	second element is true if the term is a was_in_state_within term.
	Throws an error if parsing was unsuccessful.
------------------------------------------------------------------------------*/
std::pair<const int, const std::pair<const bool, const bool>>*
	IntermediateFormParser::parseStateNSeconds(const std::string s)
		throw (std::string)
{
	std::pair<const int, const std::pair<const bool, const bool>>*
		state_info_pair = nullptr;
	std::vector<std::pair<int, NonTerminalSymbol*>*> state_info_list;
	try
	{
		state_info_list = getAcceptingGrammarRules(s,
			std::vector<int>
			{
				STATE_N_SECONDS
			});

		if(state_info_list.empty())
		{
			throw("[parseStateNSeconds]->\n"
				+ std::string("unrecognised state-n-seconds term: ") + s
				+ "\n");
		}
		else
		{
			std::pair<int, NonTerminalSymbol*> state_info;
			if(state_info_list.size() == 1)
			{
				state_info = *(state_info_list[0]);
			}
			else
			{
				state_info = *(disambiguateCandidates(&state_info_list, s));
			}
			std::vector<std::string> values = getDataExtractionRuleValuesByName(
				state_info.second->getName());
			std::string seconds = constructIdentifier(values[0], false,
				state_info.second);

			state_info_pair = new std::pair<const int, const std::pair<
				const bool, const bool>>(stringToInteger(seconds),
					std::pair<const bool, const bool>(
						toLower(values[1]) == "true",
						toLower(values[2]) == "true"));
			std::for_each(state_info_list.begin(), state_info_list.end(),
				[&](std::pair<int, NonTerminalSymbol*>* pair)
				{
					delete pair;
				});
		}
	}
	catch(std::string& error)
	{
		std::for_each(state_info_list.begin(), state_info_list.end(),
			[&](std::pair<int, NonTerminalSymbol*>* pair)
			{
				delete pair;
			});
		throw(error);
	}
	return state_info_pair;
}

/*------------------------------------------------------------------------------
	getAcceptingGrammarRules

	Given a string and a list of possible types, returns a list of pairs
	where for each pair the	first element is an integer corresponding to
	the type of rule, and the second element is a pointer to the accepting
	non-terminal symbol.
------------------------------------------------------------------------------*/
const std::vector<std::pair<int, NonTerminalSymbol*>*>
	IntermediateFormParser::getAcceptingGrammarRules(const std::string s,
		std::vector<int> types) const
{
	std::vector<std::pair<int, NonTerminalSymbol*>*> accepting_list;
	for(int type: types)
	{
		if(type >= 1 && type <= NUM_TYPES)
		{
			auto it = non_terminal_symbols_by_type[type - 1].begin();
			auto end = non_terminal_symbols_by_type[type - 1].end();
			while(it != end)
			{
				NonTerminalSymbol* n = *it;
				n->resetSymbol();
				std::string input_string = s;
				n->feedLine(input_string);
				if(n->isAccepting())
				{
					accepting_list.push_back(
						new std::pair<int, NonTerminalSymbol*>(type, n));
				}
				it++;
			}
		}
	}
	return accepting_list;
}

/*------------------------------------------------------------------------------
	buildPredefinedAutomatonTypeMap

	Builds the map from predefined automaton names (parsed in the grammar
	file) to a constant corresponding to the type of the predefined
	automaton.
------------------------------------------------------------------------------*/
void IntermediateFormParser::buildPredefinedAutomatonTypeMap()
{
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"pvc", PRECONDITION_PROP_VALUE_CHECK));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"evc", PRECONDITION_ENUM_VALUE_CHECK));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"tc", PRECONDITION_TIME_CONSTRAINT));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"pva", ACTION_PROP_VALUE_ASSIGNMENT));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"eva", ACTION_ENUM_VALUE_ASSIGNMENT));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"exbnd", ACTION_EXECUTE_BEHAVIOUR_NON_D));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"exb", ACTION_EXECUTE_BEHAVIOUR));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"del", ACTION_DELAY));
	automaton_type_map.insert(
		std::pair<const std::string, const int>(
			"sns", STATE_N_SECONDS));
}

/*------------------------------------------------------------------------------
	buildPredefinedAutomatonMap

	Builds the map from predefined automaton names (parsed in the grammar
	file) to pointers to instances of the corresponding predefined
	automaton.
------------------------------------------------------------------------------*/
void IntermediateFormParser::buildPredefinedAutomatonMap()
{
	predefined_automaton_map.insert(
		std::pair<const std::string, Automaton*>("time",
			new TimeSymbol()));
	predefined_automaton_map.insert(
		std::pair<const std::string, Automaton*>("integer",
			new IntegerSymbol()));
	predefined_automaton_map.insert(
		std::pair<const std::string, Automaton*>("float",
			new FloatSymbol()));
	predefined_automaton_map.insert(
		std::pair<const std::string, Automaton*>("any_text",
			new AnyTextSymbol()));
	predefined_automaton_map.insert(
		std::pair<const std::string, Automaton*>("relational_operator",
			new RelationalOperatorSymbol()));
	for(int i = 0; i < NUM_TYPES; i++)
	{
		non_terminal_symbols_by_type.push_back(
			std::list<NonTerminalSymbol*>());
	}
}

/*------------------------------------------------------------------------------
	buildDataExtractionRuleDefinitionMap

	Builds the map from constants representing the types of data extraction
	rule to	a list of strings corresponding to the expected list of
	parameters for that type.
------------------------------------------------------------------------------*/
void IntermediateFormParser::buildDataExtractionRuleDefinitionMap()
{
	std::list<std::string> precondition_prop_value_check_values =
		{
			"prop_name", "truth_value", "true", "false",
			"non_deterministic"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			PRECONDITION_PROP_VALUE_CHECK,
			precondition_prop_value_check_values));
	std::list<std::string> precondition_enum_value_check_values =
		{
			"enum_name", "enum_value", "has_none_value",
			"non_deterministic"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			PRECONDITION_ENUM_VALUE_CHECK,
			precondition_enum_value_check_values));
	std::list<std::string> precondition_time_constraint_values =
		{
			"start_time", "end_time"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			PRECONDITION_TIME_CONSTRAINT,
			precondition_time_constraint_values));
	std::list<std::string> action_prop_value_assignment_values =
		{
			"prop_name", "truth_value", "true", "false"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			ACTION_PROP_VALUE_ASSIGNMENT,
			action_prop_value_assignment_values));
	std::list<std::string> action_enum_value_assignment_values =
		{
			"enum_name", "enum_value", "resets"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			ACTION_ENUM_VALUE_ASSIGNMENT,
			action_enum_value_assignment_values));
	std::list<std::string> action_execute_behaviour_non_d_values =
		{
			"behaviour_values", "split_character", "ignore_entries"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			ACTION_EXECUTE_BEHAVIOUR_NON_D,
			action_execute_behaviour_non_d_values));
	std::list<std::string> action_execute_behaviour_values =
		{
			"behaviour_name"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			ACTION_EXECUTE_BEHAVIOUR,
			action_execute_behaviour_values));
	std::list<std::string> action_delay_values =
		{
			"seconds"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			ACTION_DELAY,
			action_delay_values));
	std::list<std::string> state_n_seconds_values =
		{
			"seconds", "been_in_state", "was_in_state"
		};
	data_extraction_rule_definition_map.insert(
		std::pair<const int, std::list<std::string>>(
			STATE_N_SECONDS,
			state_n_seconds_values));
}

/*------------------------------------------------------------------------------
	getDataExtractionRuleDefinitionByType

	Given a constant corresponding to a type of action, precondition or
	state-n-seconds term, returns the list of expected parameters for a data
	extraction rule of this type.
------------------------------------------------------------------------------*/
std::list<std::string>
	IntermediateFormParser::getDataExtractionRuleDefinitionByType(
			const int type) const
{
	return data_extraction_rule_definition_map.find(type)->second;
}

/*------------------------------------------------------------------------------
	getDataExtractionRuleValuesByName

	Given the name of a data extraction rule, returns a list of strings
	corresponding to the parameters parsed for that data extraction rule.
------------------------------------------------------------------------------*/
std::vector<std::string>
	IntermediateFormParser::getDataExtractionRuleValuesByName(
		const std::string name) const
{
	return data_extraction_rule_values_map.find(name)->second;
}
