/*==============================================================================
	CommandLineParser

	A command line parser to facilitate the parsing of multiple command line
	parameters. Command line options can be required or optional. Given the name
	of the executable, the version number, and a description for each option,
	usage instructions are automatically generated.

	Options of the following types are supported by the parser:

	SET_BOOL: accepts a value of 1 or 0 and assigns this value to a given
		boolean variable.
	SET_INT: accepts an integer and assigns this value to a given integer
		variable.
	SET_INT_WITH_RANGE: accepts an integer within a given range and assigns this
		value to a given integer variable.
	SET_STRING: accepts a string and assigns this value to a given string
		variable.

	File			: command_line_parser.h
	Author			: Paul Gainer
	Created			: 20/08/2014
	Last modified	: 14/04/2015
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
#ifndef COMMAND_LINE_PARSER_H_
#define COMMAND_LINE_PARSER_H_

#include <list>
#include <map>
#include <set>
#include <string>

#include "command_line_parser/parser_option_type.h"

/*==============================================================================
	CommandLineParser
==============================================================================*/
class CommandLineParser
{
public:
	/*==========================================================================
		Public Constants
	==========================================================================*/
	static const bool PARAMETER_SORT = true;
	static const bool NO_PARAMETER_SORT = false;
	static const bool REQUIRED = true;
	static const bool OPTIONAL = false;

	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		CommandLineParser

		Constructs a new parser given an executable name, an executable version
		number a string, a boolean set to true if command line options should
		be sorted when displaying usage information, argc and finally argv.
	--------------------------------------------------------------------------*/
	CommandLineParser(const std::string& name, const std::string& version,
		const bool sort_p, int argc, char** argv);

	/*--------------------------------------------------------------------------
		~CommandLineParser
	--------------------------------------------------------------------------*/
	~CommandLineParser() {}

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		addBoolOption

		Adds a new boolean option to the parser, given an option name, a string
		describing the option, a boolean set to true if this is	a mandatory
		option and a pointer to the boolean value to set when parsing.
	--------------------------------------------------------------------------*/
	void addBoolOption(const std::string& command, const std::string& help_text,
		const bool required, bool* b);

	/*--------------------------------------------------------------------------
		addIntOption

		Adds a new integer option to the parser, given an option name, a string
		describing the option, a boolean set to true if this is	a mandatory
		option, a pointer to the integer value to set when parsing, and a name
		for the integer argument to be displayed in the usage information.
	--------------------------------------------------------------------------*/
	void addIntOption(const std::string& command, const std::string& help_text,
		const bool required, int* i, const std::string& arg_name);

	/*--------------------------------------------------------------------------
		addIntRangeOption

		Adds a new ranged integer option to the parser, given an option name, a
		string describing the option, a boolean set to true if this is a
		mandatory option, a pointer to the integer value to set when parsing,
		and inclusive lower and upper bounds for the integer range.
	--------------------------------------------------------------------------*/
	void addIntRangeOption(const std::string& command,
		const std::string& help_text, const bool required, int* i,
		const int range_min, const int range_max);

	/*--------------------------------------------------------------------------
		addStringOption

		Adds a new string option to the parser, given an option name, a string
		describing the option, a boolean set to true if this is	a mandatory
		option, a pointer to the integer value to set when parsing, and a name
		for the string argument to be displayed in the usage information.
	--------------------------------------------------------------------------*/
	void addStringOption(const std::string& command,
		const std::string& help_text, const bool required, std::string* s,
		const std::string& arg_name);

	/*--------------------------------------------------------------------------
		parse

		Parses the given command line parameters.
	--------------------------------------------------------------------------*/
	bool parse();

	/*--------------------------------------------------------------------------
		displayUsageInformation

		Constructs and displays usage information.
	--------------------------------------------------------------------------*/
	void displayUsageInformation();

private:
	/*==========================================================================
		Private Structs
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Option

		A base option struct.
	--------------------------------------------------------------------------*/
	struct Option
	{
		std::string option_command;
		std::string option_help_text;
		ParserOptionType option_type;
		bool is_required;
	};

	/*--------------------------------------------------------------------------
		OptionBool:Option

		An extension of the Option struct to represent boolean options.
	--------------------------------------------------------------------------*/
	struct OptionBool:Option
	{
		bool* option_boolean;
	};

	/*--------------------------------------------------------------------------
		OptionInt:Option

		An extension of the Option struct to represent integer options.
	--------------------------------------------------------------------------*/
	struct OptionInt:Option
	{
		int* option_int;
		std::string option_arg_name;
	};

	/*--------------------------------------------------------------------------
		OptionIntWithRange:Option

		An extension of the OptionInt struct to represent integer options with
		a given range.
	--------------------------------------------------------------------------*/
	struct OptionIntWithRange:OptionInt
	{
		int range_min;
		int range_max;
	};

	/*--------------------------------------------------------------------------
		OptionString:Option

		An extension of the Option struct to represent string options.
	--------------------------------------------------------------------------*/
	struct OptionString:Option
	{
		std::string* option_string;
		std::string option_arg_name;
	};

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The name of the executable.
	--------------------------------------------------------------------------*/
	const std::string executable_name;

	/*--------------------------------------------------------------------------
		The version number of the executable.
	--------------------------------------------------------------------------*/
	const std::string executable_version;

	/*--------------------------------------------------------------------------
		Set to true if there are non-mandatory options.
	--------------------------------------------------------------------------*/
	bool options_available;

	/*--------------------------------------------------------------------------
		Set to true if there are mandatory options.
	--------------------------------------------------------------------------*/
	bool required_available;

	/*--------------------------------------------------------------------------
		Set to true if options are to be sorted when displaying usage
		information.

		Options will still be displayed in the order:
		1. string options
		2. integer options
		3. ranged integer options
		4. boolean options
	--------------------------------------------------------------------------*/
	const bool sort_options;

	/*--------------------------------------------------------------------------
		Maps parsed arguments to option types.
	--------------------------------------------------------------------------*/
	std::map<std::string, ParserOptionType> option_type_map;

	/*--------------------------------------------------------------------------
		A list of all mandatory options.
	--------------------------------------------------------------------------*/
	std::list<std::string> required_options;

	/*--------------------------------------------------------------------------
		The boolean options.
	--------------------------------------------------------------------------*/
	std::list<OptionBool> option_list_bool;

	/*--------------------------------------------------------------------------
		The integer options.
	--------------------------------------------------------------------------*/
	std::list<OptionInt> option_list_int;

	/*--------------------------------------------------------------------------
		The ranged integer options.
	--------------------------------------------------------------------------*/
	std::list<OptionIntWithRange> option_list_int_range;

	/*--------------------------------------------------------------------------
		The string options.
	--------------------------------------------------------------------------*/
	std::list<OptionString> option_list_string;

	/*--------------------------------------------------------------------------
		All of the parsed command line parameters
	--------------------------------------------------------------------------*/
	std::list<std::string> command_line_parameters;

	/*--------------------------------------------------------------------------
		Command line parameters parsed so far, used to detect duplicates.
	--------------------------------------------------------------------------*/
	std::set<std::string> parsed_parameters;

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		parseBool

		Parses a boolean option, ensuring that the given argument is a boolean
		(0/1) value.
	--------------------------------------------------------------------------*/
	bool parseBool(const std::string&);

	/*--------------------------------------------------------------------------
		parseInt

		Parses an integer option, ensuring that the given argument is an integer
		value.
	--------------------------------------------------------------------------*/
	bool parseInt(const std::string&);

	/*--------------------------------------------------------------------------
		parseIntRange

		Parses a ranged integer option, ensuring that the given argument is an
		integer and is within the given range.
	--------------------------------------------------------------------------*/
	bool parseIntRange(const std::string&);

	/*--------------------------------------------------------------------------
		parseIntRange

		Parses a string option.
	--------------------------------------------------------------------------*/
	bool parseString(const std::string&);

	/*--------------------------------------------------------------------------
		stringToInt

		Converts a string to an integer and returns the integer, or INVALID (-1)
		if the string does not represent a valid integer.
	--------------------------------------------------------------------------*/
	int stringToInt(const std::string&);

	/*--------------------------------------------------------------------------
		displayError

		Displays the given error message.
	--------------------------------------------------------------------------*/
	void displayError(const std::string&);
};

#endif
