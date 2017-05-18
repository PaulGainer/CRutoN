/*==============================================================================
	CommandLineParser

	File			: command_line_parser.cpp
	Author			: Paul Gainer
	Created			: 20/08/2014
	Last modified	: 16/04/2015
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
#define INVALID -1
#define INF std::string("oo")
#define NEG_INF "-" + INF

#include <algorithm>
#include <climits>
#include <iostream>
#include <locale>
#include <sstream>

#include "utility.h"
#include "command_line_parser/command_line_parser.h"

/*==============================================================================
	Public Constructors/Deconstructor
==============================================================================*/
CommandLineParser::CommandLineParser(const std::string& name,
	const std::string& version,	const bool sort_p, int argc, char** argv):
	executable_name(name), executable_version(version),
	options_available(false), required_available(false), sort_options(sort_p)
{
	// add all the parameters to a list
	for(int i = 1; i < argc; i++)
	{
		command_line_parameters.push_back(argv[i]);
	}
}

void CommandLineParser::addBoolOption(const std::string& command,
	const std::string& help_text, const bool required, bool* b)
{
	OptionBool new_option;
	new_option.option_command = command;
	new_option.option_help_text = help_text;
	new_option.option_type = ParserOptionType::SET_BOOL;
	new_option.is_required = required;
	new_option.option_boolean = b;
	option_list_bool.push_back(new_option);
	if(sort_options)
	{
		option_list_bool.sort(
			[](const OptionBool& first, const OptionBool& second)
			{
				return first.option_command < second.option_command;
			});
	}
	option_type_map[command] = new_option.option_type;
	if(required)
	{
		required_options.push_back(command);
		required_available = true;
	}
	else
	{
		options_available = true;
	}
}

/*==============================================================================
	Public Functions
==============================================================================*/
void CommandLineParser::addIntOption(const std::string& command,
	const std::string& help_text,  const bool required, int* i,
	const std::string& arg_name)
{
	OptionInt new_option;
	new_option.option_command = command;
	new_option.option_help_text = help_text;
	new_option.option_type = ParserOptionType::SET_INT;
	new_option.is_required = required;
	new_option.option_int = i;
	new_option.option_arg_name = arg_name;
	option_list_int.push_back(new_option);
	if(sort_options)
	{
		option_list_int.sort(
			[](const OptionInt& first, const OptionInt& second)
			{
				return first.option_command < second.option_command;
			});
	}
	option_type_map[command] = new_option.option_type;
	if(required)
	{
		required_options.push_back(command);
		required_available = true;
	}
	else
	{
		options_available = true;
	}
}

void CommandLineParser::addIntRangeOption(const std::string& command,
	const std::string& help_text, const bool required, int* i,
	const int range_min, const int range_max)
{
	OptionIntWithRange new_option;
	new_option.option_command = command;
	new_option.option_help_text = help_text;
	new_option.option_type = ParserOptionType::SET_INT_WITH_RANGE;
	new_option.is_required = required;
	new_option.option_int = i;
	new_option.range_min = range_min;
	new_option.range_max = range_max;
	option_list_int_range.push_back(new_option);
	if(sort_options)
	{
		option_list_int_range.sort(
			[](const OptionIntWithRange& first,
				const OptionIntWithRange& second)
			{
				return first.option_command < second.option_command;
			});
	}
	option_type_map[command] = new_option.option_type;
	if(required)
	{
		required_options.push_back(command);
		required_available = true;
	}
	else
	{
		options_available = true;
	}
}

void CommandLineParser::addStringOption(const std::string& command,
	const std::string& help_text, const bool required, std::string* s,
	const std::string& arg_name)
{
	OptionString new_option;
	new_option.option_command = command;
	new_option.option_help_text = help_text;
	new_option.option_type = ParserOptionType::SET_STRING;
	new_option.is_required = required;
	new_option.option_string = s;
	new_option.option_arg_name = arg_name;
	option_list_string.push_back(new_option);
	if(sort_options)
	{
		option_list_string.sort(
			[](const OptionString& first, const OptionString& second)
			{
				return first.option_command < second.option_command;
			});
	}
	option_type_map[command] = new_option.option_type;
	if(required)
	{
		required_options.push_back(command);
		required_available = true;
	}
	else
	{
		options_available = true;
	}
}

bool CommandLineParser::parse()
{
	while(!command_line_parameters.empty())
	{
		std::string next_argument = command_line_parameters.front();
		command_line_parameters.pop_front();
		if(parsed_parameters.find(next_argument) == parsed_parameters.end())
		{
			parsed_parameters.insert(next_argument);
		}
		else
		{
			displayError("parsed duplicate option '" + next_argument +
				"'\n");
			return false;
		}
		bool valid;
		auto map_it = option_type_map.find(next_argument);
		if(map_it == option_type_map.end())
		{
			displayError("unrecognized option '" + next_argument + "'\n");
			return false;
		}
		else
		{
			auto map = *map_it;
			switch(map.second)
			{
			case ParserOptionType::SET_BOOL:
				valid = parseBool(next_argument);
				break;

			case ParserOptionType::SET_INT:
				valid = parseInt(next_argument);
				break;

			case ParserOptionType::SET_INT_WITH_RANGE:
				valid = parseIntRange(next_argument);
				break;

			case ParserOptionType::SET_STRING:
				valid = parseString(next_argument);
				break;
			}
		}
		if(!valid)
		{
			return false;
		}
	}

	if(required_options.empty())
	{
		return true;
	}
	else
	{
		std::for_each(required_options.begin(),
			required_options.end(),
			[&](std::string command)
			{
				displayError("missing required option '" + command + "'\n");
			});
		return false;
	}
}

void CommandLineParser:: displayUsageInformation()
{
	std::cout << executable_name << " " << executable_version << std::endl;
	std::cout << "usage: " << executable_name;
	if(required_available)
	{
		std::for_each(option_list_string.begin(), option_list_string.end(),
			[&](OptionString p)
			{
				if(p.is_required)
				{
					std::cout << " " << p.option_command << " <" <<
						p.option_arg_name << ">";
				}
			});
		std::for_each(option_list_int.begin(), option_list_int.end(),
			[&](OptionInt p)
			{
				if(p.is_required)
				{
					std::cout << " " << p.option_command << " <" <<
						p.option_arg_name << ">";
				}
			});
		std::for_each(option_list_int_range.begin(),
			option_list_int_range.end(),
			[&](OptionIntWithRange p)
			{
				if(p.is_required)
				{
					std::cout << " " << p.option_command << " <" <<
						p.range_min << "-" << p.range_max << ">";
				}
			});
		std::for_each(option_list_bool.begin(), option_list_bool.end(),
			[&](OptionBool p)
			{
				if(p.is_required)
				{
					std::cout << " " << p.option_command << " <0|1>";
				}
			});
	}
	std::cout << (options_available ? " [options]\n" : "\n");
	if(required_available)
	{
		std::cout << "   required:\n";
		std::for_each(option_list_string.begin(), option_list_string.end(),
			[&](OptionString p)
			{
				if(p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						p.option_arg_name << ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_int.begin(), option_list_int.end(),
			[&](OptionInt p)
			{
				if(p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						p.option_arg_name << ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_int_range.begin(),
			option_list_int_range.end(),
			[&](OptionIntWithRange p)
			{
				if(p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						(p.range_min == INT_MIN ?
							NEG_INF : integerToString(p.range_min))
						<< "-"
						<< (p.range_max == INT_MAX ?
							INF : integerToString(p.range_max))
						<< ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_bool.begin(), option_list_bool.end(),
			[&](OptionBool p)
			{
				if(p.is_required)
				{
					std::cout << "\t" << p.option_command << " <0|1>\t" <<
						p.option_help_text;
				}
			});
	}
	if(options_available)
	{
		std::cout << "   options:\n";
		std::for_each(option_list_string.begin(), option_list_string.end(),
			[&](OptionString p)
			{
				if(!p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						p.option_arg_name << ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_int.begin(), option_list_int.end(),
			[&](OptionInt p)
			{
				if(!p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						p.option_arg_name << ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_int_range.begin(),
			option_list_int_range.end(),
			[&](OptionIntWithRange p)
			{
				if(!p.is_required)
				{
					std::cout << "\t" << p.option_command << " <" <<
						(p.range_min == INT_MIN ?
							NEG_INF : integerToString(p.range_min))
						<< "-"
						<< (p.range_max == INT_MAX ?
							INF : integerToString(p.range_max))
						<< ">\t" <<
						p.option_help_text;
				}
			});
		std::for_each(option_list_bool.begin(), option_list_bool.end(),
			[&](OptionBool p)
			{
				if(!p.is_required)
				{
					std::cout << "\t" << p.option_command << " <0|1>\t" <<
						p.option_help_text;
				}
			});
	}
}

/*==============================================================================
	Private Functions
==============================================================================*/
bool CommandLineParser::parseBool(const std::string& command)
{
	if(command_line_parameters.empty())
	{
		displayError("expected boolean argument for option '" + command +
			"'\n");
		return false;
	}
	auto bool_it = std::find_if(option_list_bool.begin(),
		option_list_bool.end(),
		[&](OptionBool p)
		{
			return p.option_command == command;
		});
	OptionBool parameter = *bool_it;
	std::string argument = command_line_parameters.front();
	command_line_parameters.pop_front();
	int i = stringToInt(argument);
	if(i == INVALID || i < 0 || i > 1)
	{
		displayError("invalid argument '" + argument +
			"', expected boolean value\n");
		return false;
	}
	else
	{
		*(parameter.option_boolean) = i;
		required_options.remove_if(
			[&](std::string s)
			{
				return s == command;
			});
		return true;
	}
}

bool CommandLineParser::parseInt(const std::string& command)
{
	if(command_line_parameters.empty())
	{
		displayError("expected integer argument for option '" + command +
			"'\n");
		return false;
	}
	auto bool_it = std::find_if(option_list_int.begin(),
		option_list_int.end(),
		[&](OptionInt p)
		{
			return p.option_command == command;
		});
	OptionInt parameter = *bool_it;
	std::string argument = command_line_parameters.front();
	command_line_parameters.pop_front();
	int i = stringToInt(argument);
	if(i == INVALID)
	{
		displayError("invalid argument '" + argument +
			"', expected integer value\n");
		return false;
	}
	else
	{
		*(parameter.option_int) = i;
		required_options.remove_if(
			[&](std::string s)
			{
				return s == command;
			});
		return true;
	}
}

bool CommandLineParser::parseIntRange(const std::string& command)
{
	if(command_line_parameters.empty())
	{
		displayError("expected integer argument for option '" + command +
			"'\n");
		return false;
	}
	auto bool_it = std::find_if(option_list_int_range.begin(),
		option_list_int_range.end(),
		[&](OptionIntWithRange p)
		{
			return p.option_command == command;
		});
	OptionIntWithRange parameter = *bool_it;
	std::string argument = command_line_parameters.front();
	command_line_parameters.pop_front();
	int i = stringToInt(argument);
	if(i == INVALID)
	{
		displayError("invalid argument '" + argument +
			"', expected integer value\n");
		return false;
	}
	else if(i < parameter.range_min || i > parameter.range_max)
	{
		displayError("invalid argument '" + argument +
			"', expected integer value");
		std::cout << "from range <"
			<< (parameter.range_min == INT_MIN ?
				NEG_INF : integerToString(parameter.range_min))
			<< "-"
			<< (parameter.range_max == INT_MAX ?
				INF : integerToString(parameter.range_max))
			<< "-" << parameter.range_max << ">\n";
		return false;
	}
	else
	{
		*(parameter.option_int) = i;
		required_options.remove_if(
			[&](std::string s)
			{
				return s == command;
			});
		return true;
	}
}

bool CommandLineParser::parseString(const std::string& command)
{
	if(command_line_parameters.empty())
	{
		displayError("expected string argument for option '" + command +
			"'\n");
		return false;
	}
	auto bool_it = std::find_if(option_list_string.begin(),
		option_list_string.end(),
		[&](OptionString p)
		{
			return p.option_command == command;
		});
	OptionString parameter = *bool_it;
	std::string argument = command_line_parameters.front();
	command_line_parameters.pop_front();
	*(parameter.option_string) = argument;
	required_options.remove_if(
		[&](std::string s)
		{
			return s == command;
		});
	return true;
}

int CommandLineParser::stringToInt(const std::string& s)
{
	if(s.empty())
	{
		return INVALID;
	}
	std::istringstream stream(s);
	int parsed_value;
	stream >> parsed_value;

	return stream.eof() ? parsed_value : INVALID;
}

void CommandLineParser::displayError(const std::string& error)
{
	std::cout << executable_name << " error: " << error;
}
