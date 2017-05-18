/*==============================================================================
	Global Variables

	File			: global.h
	Author			: Paul Gainer
	Created			: 13/02/2015
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
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <string>

/*------------------------------------------------------------------------------
	True if nested behaviour executions should be flattened.
------------------------------------------------------------------------------*/
extern bool g_flatten_behaviours;

/*------------------------------------------------------------------------------
	True if the user should be prompted when a case-insensitive match is found
	for the name of a behaviour/prop variable/enum variable.
------------------------------------------------------------------------------*/
extern bool g_no_prompt_case_insensitivity;

/*------------------------------------------------------------------------------
	True if the user should be prompted when checking identifiers for the name
	of a behaviour/prop variable/enum variable.
------------------------------------------------------------------------------*/
extern bool g_no_prompt_identifier_matching;

/*------------------------------------------------------------------------------
	True if "true" and "false" should be disallowed as valid enumeration
	values.
------------------------------------------------------------------------------*/
extern bool g_disallow_true_false_enum;

/*------------------------------------------------------------------------------
	True if true non-determinism should be used in the NuSMV model, w.r.t. the
	scheduling of behaviours with equal priority.

	WARNING: For n behaviours of equal priority, 2^n defines will be added to
	the resulting NuSMV model.
------------------------------------------------------------------------------*/
extern bool g_true_non_determinism;

/*------------------------------------------------------------------------------
	True if a minimum of one previous state should be used for state_n_seconds
	terms.
------------------------------------------------------------------------------*/
extern bool g_minimum_one_state_state_n_seconds;

/*------------------------------------------------------------------------------
	Sets the string matching threshhold used for automatic string matching of
	behaviour/prop variable/enum variable names. A Threshold of 75 would
	automatically match two strings with 75% similarity.

	This threshold is bound by STRING_MATCHING_THRESHHOLD_MIN and
	STRING_MATCHING_THRESHHOLD_MAX 100, defined in define.h.
------------------------------------------------------------------------------*/
extern int g_string_matching_threshhold;

/*------------------------------------------------------------------------------
	Sets the string matching threshhold used for automatic string matching of
	behaviour/prop variable/enum variable names. A Threshold of 75 would
	automatically match two strings with 75% similarity.
------------------------------------------------------------------------------*/
extern int g_state_time_seconds;

/*------------------------------------------------------------------------------
	Sets the maximum number of seconds for a state_n_seconds term. Used to
	restrict the size of enumerated types in the NuSMV models.
------------------------------------------------------------------------------*/
extern int g_max_seconds_per_state_n_seconds;

/*------------------------------------------------------------------------------
	Setting this to a valid time value HH:MM:SS fixes the time of day, and
	automatically sets preconditions corresponding to time constraints to be
	false or true in the NuSMV model.
------------------------------------------------------------------------------*/
extern std::string g_time_of_day;

/*------------------------------------------------------------------------------
	The executable version number.
------------------------------------------------------------------------------*/
extern const std::string EXEC_VERSION;

/*------------------------------------------------------------------------------
	The name of the executable.
------------------------------------------------------------------------------*/
extern const std::string EXEC_NAME;

/*------------------------------------------------------------------------------
	The file containing the control rule definitions.
------------------------------------------------------------------------------*/
extern std::string g_control_rule_file;

/*------------------------------------------------------------------------------
	The file containing the grammar rule definitions.
------------------------------------------------------------------------------*/
extern std::string g_grammar_rule_file;

/*------------------------------------------------------------------------------
	The file containing the data extraction rule definitions.
------------------------------------------------------------------------------*/
extern std::string g_data_extraction_rule_file;

/*------------------------------------------------------------------------------
	The file containing the initial values file.
------------------------------------------------------------------------------*/
extern std::string g_initial_values_file;

/*------------------------------------------------------------------------------
	The file to which the intermediate form should be written.
------------------------------------------------------------------------------*/
extern std::string g_intermediate_form_file;

/*------------------------------------------------------------------------------
	The file to which the NuSMV model should be written.
------------------------------------------------------------------------------*/
extern std::string g_NuSMV_file;

#endif
