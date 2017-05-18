/*==============================================================================
	CRuToN	v1.0.0 	-	Control Rules To NuSMV translation tool

	CruToN is a translation tool that given a set of Care-O-Bot control rules,
	can construct an intermediate form representation of these rules. An input
	file for the model checker NuSMV can then be constructed with a given
	intermediate form representation.

	File			: cruton.cpp
	Author			: Paul Gainer
	Created			: 11/03/2015
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
#include <climits>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "define.h"
#include "global.h"
#include "intermediate/intermediate_form.h"
#include "command_line_parser/command_line_parser.h"
#include "command_line_parser/parser_option_type.h"
#include "intermediate_parser/intermediate_form_parser.h"
#include "nusmv_translator/nusmv_translator.h"

/*==============================================================================
	Function Prototypes
==============================================================================*/
bool parseCommandLineParameters(int parameter_count, char** parameters);
bool validateTime(const std::string& time);
bool writeStringToFile(const std::string& file, const std::string& s);

/*==============================================================================
	Main
==============================================================================*/
int main(int argc, char** argv)
{
	if(!parseCommandLineParameters(argc, argv))
	{
		// there was an error parsing the command line parameters, exit
		return 0;
	}

	IntermediateFormParser parser;

	if(parser.parse(g_grammar_rule_file, g_data_extraction_rule_file,
		g_control_rule_file))
	{
		// the files parsed successfully
		IntermediateForm* intermediate_form = parser.getIntermediateForm();
		if(g_intermediate_form_file != "")
		{
			// an intermediate form output file was specified, write to it
			std::stringstream ss;
			ss << *intermediate_form;
			if(writeStringToFile(g_intermediate_form_file, ss.str()))
			{
				std::cout << "intermediate form written to file \'"
					+ g_intermediate_form_file + "\' successfully\n";
			}
		}
		else
		{
			// no output file was specified, write it to standard output
			std::cout << *intermediate_form;
		}
		NuSMVTranslator t(intermediate_form);
		std::string output_NuSMV;
		// translate the intermediate form into NuSMV input
		if(!g_initial_values_file.empty())
		{
			// an initial values file was specified
			output_NuSMV = t.translate(g_initial_values_file);
		}
		else
		{
			output_NuSMV = t.translate();
		}

		if(output_NuSMV == "")
		{
			// there was an error during translation, exit
			return 0;
		}
		/*	if behaviour flattening was not enabled, display a warning.
			behaviours with nested executions that are not flattened may
			result in NuSMV translations that are not correct */
		if(!g_flatten_behaviours)
		{
			displayWarning(std::string("behaviour flattening was not ")
				+ "enabled, nested behaviour executions may not"
				+ "\nbe correctly represented in the NuSMV model");
		}
		if(g_NuSMV_file != "")
		{
			// an NuSMV output file was specified, write to it
			if(writeStringToFile(g_NuSMV_file, output_NuSMV))
			{
				std::cout << "NuSMV code written to file \'"
					+ g_NuSMV_file + "\' successfully\n";
			}
		}
		else
		{
			// no output file was specified, write it to standard error
			std::cerr << output_NuSMV;
		}
	}
}

/*------------------------------------------------------------------------------
	parseCommandLineParameters

	Parses the command line parameters and returns true if successful or
	false otherwise.
................................................................................
	@param	parameter_count		the number of parsed parameters
	@param	parameters			the array of c-style string parameters
------------------------------------------------------------------------------*/
bool parseCommandLineParameters(int parameter_count, char** parameters)
{
	// create a new parser with the executable name and version number with
	// parameter sorting enabled
	CommandLineParser command_line_parser(EXEC_NAME, EXEC_VERSION,
			CommandLineParser::PARAMETER_SORT, parameter_count, parameters);

	// add the options
	command_line_parser.addStringOption("-ic", "control rule file\n",
		CommandLineParser::REQUIRED, &g_control_rule_file, "file");
	command_line_parser.addStringOption("-ig", "grammar rule file\n",
		CommandLineParser::REQUIRED, &g_grammar_rule_file, "file");
	command_line_parser.addStringOption("-id", "data extraction rule file\n",
		CommandLineParser::REQUIRED, &g_data_extraction_rule_file, "file");
	command_line_parser.addStringOption("-ii", "initial values file\n",
		CommandLineParser::OPTIONAL, &g_initial_values_file, "file");
	command_line_parser.addStringOption("-oi",
		"intermediate form output file\n",
		CommandLineParser::OPTIONAL, &g_intermediate_form_file, "file");
	command_line_parser.addStringOption("-on", "NuSMV output file\n",
		CommandLineParser::OPTIONAL, &g_NuSMV_file, "file");
	command_line_parser.addStringOption("-tod",
		"set time of day in the robot house\n",
		CommandLineParser::OPTIONAL, &g_time_of_day, "HH:MM:SS");
	command_line_parser.addIntRangeOption("-sps",
		std::string("set seconds per state\n")
			+ "\t\t\tdefault: "
			+ integerToString(g_state_time_seconds) + "\n",
		CommandLineParser::OPTIONAL, &g_state_time_seconds, 1, INT_MAX);
	command_line_parser.addIntRangeOption("-sms",
		std::string("set max seconds for was_in_state/been_in_state\n")
			+"\t\t\tdefault: "
			+ integerToString(g_max_seconds_per_state_n_seconds) + "\n",
		CommandLineParser::OPTIONAL, &g_max_seconds_per_state_n_seconds, 1,
			INT_MAX);
	command_line_parser.addIntRangeOption("-smt",
		std::string("set string matching threshhold\n")
			+"\t\t\tdefault: "
			+ integerToString(g_string_matching_threshhold) + "\n",
		CommandLineParser::OPTIONAL, &g_string_matching_threshhold,
			STRING_MATCHING_THRESHHOLD_MIN, STRING_MATCHING_THRESHHOLD_MAX);
	command_line_parser.addBoolOption("-cim",
		std::string("enable automatic case-insensitive matching\n")
			+ "\t\t\tdefault: "
			+ (g_no_prompt_case_insensitivity ? "enabled" : "disabled") + "\n",
		CommandLineParser::OPTIONAL, &g_no_prompt_case_insensitivity);
	command_line_parser.addBoolOption("-aim",
		std::string("enable automatic identifier matching\n")
			+ "\t\t\tdefault: "
			+ (g_no_prompt_identifier_matching ? "enabled" : "disabled")
			+ "\n",
		CommandLineParser::OPTIONAL, &g_no_prompt_identifier_matching);
	command_line_parser.addBoolOption("-dtv",
		std::string("enable disallowing truth values as enum values\n")
			+ "\t\t\tdefault: "
			+ (g_disallow_true_false_enum ? "enabled" : "disabled")
			+ "\n",
		CommandLineParser::OPTIONAL, &g_disallow_true_false_enum);
	command_line_parser.addBoolOption("-tnd",
		std::string("enable true non-determinism in scheduling behaviours\n")
			+ "\t\t\tdefault: "
			+ (g_true_non_determinism ? "enabled" : "disabled")
			+ "\n",
		CommandLineParser::OPTIONAL, &g_true_non_determinism);
	command_line_parser.addBoolOption("-mos",
		std::string("enable min of 1 state for was_in_state/been_in_state\n")
			+ "\t\t\tdefault: "
			+ (g_minimum_one_state_state_n_seconds ? "enabled" : "disabled")
			+ "\n",
		CommandLineParser::OPTIONAL, &g_minimum_one_state_state_n_seconds);
	command_line_parser.addBoolOption("-f",
		std::string("enable behaviour flattening\n")
			+ "\t\t\tdefault: "
			+ (g_flatten_behaviours ? "enabled" : "disabled")
			+ "\n",
		CommandLineParser::OPTIONAL, &g_flatten_behaviours);

	if(!command_line_parser.parse())
	{
		// there was an error parsing, display some usage information
		command_line_parser.displayUsageInformation();
		return false;
	}
	if(g_time_of_day != "" && !validateTime(g_time_of_day))
	{
		// the given time of day was invalid
		return false;
	}
	return true;
}

/*------------------------------------------------------------------------------
	validateTime

	Returns true if the given string is a valid time in the format HH:MM:SS
	or false otherwise.
................................................................................
	@param	time				the time value given by the user using the
								-tod command line parameter
	@return						true if the given time value is a valid time
	 	 	 	 	 	 	 	in the format HH:MM:SS
------------------------------------------------------------------------------*/
bool validateTime(const std::string& time)
{
	bool successful = true;
	std::string error;
	if(time.length() != 8 || time.at(2) != ':' || time.at(5) != ':')
	{
		error = "time format should be HH:MM:SS";
		successful = false;
	}
	else
	{
		std::string hours = time.substr(0, 2);
		std::string minutes = time.substr(3, 2);
		std::string seconds = time.substr(6, 2);
		if(!isInteger(hours))
		{
			error = "\'" + hours + "\' is not an integer";
			successful = false;
		}
		else if(!isInteger(minutes))
		{
			error = "\'" + minutes + "\' is not an integer";
			successful = false;
		}
		else if(!isInteger(seconds))
		{
			error = "\'" + seconds + "\' is not an integer";
			successful = false;
		}
		else
		{
			int hours_int = stringToInteger(hours);
			int minutes_int = stringToInteger(minutes);
			int seconds_int = stringToInteger(seconds);
			if(hours_int < 0 || hours_int > 23)
			{
				error = "a value of 00-23 must be used for hours";
				successful = false;
			}
			else if(minutes_int < 0 || minutes_int > 59)
			{
				error = "a value of 00-59 must be used for minutes";
				successful = false;
			}
			else if(seconds_int < 0 || seconds_int > 59)
			{
				error = "a value of 00-59 must be used for seconds";
				successful = false;
			}
		}
	}
	if(!successful)
	{
		displayError(error);
	}
	return successful;
}

/*------------------------------------------------------------------------------
	writeStringToFile

	Writes the given string s to file, prompts for permission to overwrite
	if the file already exists.
................................................................................
	@param	file				the name of the file to which the string s
								should be written
	@return						true if the string is successfully written to
								the file, or false otherwise
------------------------------------------------------------------------------*/
bool writeStringToFile(const std::string& file, const std::string& s)
{
	std::ifstream file_stream_in(file);
	bool write_to_file = true;
	if(file_stream_in.good())
	{
		displayWarning("file \'" + file
			+ "\' already exists");
		std::string input;
		do
		{
			std::cout << "overwrite this file (y/n) ? ";
			std::cin >> input;
		} while(input != "y" && input != "Y" && input != "n" && input != "N");
		if((write_to_file = (input == "y" || input == "Y")))
		{
			std::remove(file.c_str());
		}
	}
	if(write_to_file)
	{
		std::ofstream file_stream_out(file);
		file_stream_out << s;
		file_stream_out.close();
	}
	file_stream_in.close();
	return write_to_file;
}
