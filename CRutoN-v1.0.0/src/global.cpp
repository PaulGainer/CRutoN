/*==============================================================================
	Global Variables

	File			: global.cpp
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
#include <string>

#include "define.h"
#include "global.h"

bool g_flatten_behaviours = true;
bool g_no_prompt_case_insensitivity = true;
bool g_no_prompt_identifier_matching = true;
bool g_disallow_true_false_enum = true;
bool g_true_non_determinism = true;
bool g_minimum_one_state_state_n_seconds = false;
int g_string_matching_threshhold = 90;
int g_state_time_seconds = 600;
int g_max_seconds_per_state_n_seconds = 5000;
std::string g_time_of_day = "";
const std::string EXEC_VERSION = "1.0.0";
const std::string EXEC_NAME = "CRuToN";
std::string g_control_rule_file = "";
std::string g_grammar_rule_file = "";
std::string g_data_extraction_rule_file = "";
std::string g_initial_values_file = "";
std::string g_intermediate_form_file = "";
std::string g_NuSMV_file = "";
