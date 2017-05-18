/*==============================================================================
	NuSMVTranslator

	File			: nusmv_translator.cpp
	Author			: Paul Gainer
	Created			: 09/02/2015
	Last modified	: 10/02/2015
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
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <string>

#include "define.h"
#include "utility.h"
#include "enums/action_type.h"
#include "enums/precondition_type.h"
#include "enums/time_constraint_type.h"
#include "enums/tree_node_type.h"
#include "intermediate/behaviour.h"
#include "intermediate/enum_variable.h"
#include "intermediate/intermediate_form.h"
#include "intermediate/logical_expression_tree.h"
#include "intermediate/tree_node.h"
#include "intermediate/prop_variable.h"
#include "intermediate/action.h"
#include "intermediate/action_enum_value_assignment.h"
#include "intermediate/action_execute.h"
#include "intermediate/action_execute_non_deterministic.h"
#include "intermediate/action_prop_value_assignment.h"
#include "intermediate/action_wait.h"
#include "intermediate/precondition.h"
#include "intermediate/precondition_enum_value_check.h"
#include "intermediate/precondition_prop_value_check.h"
#include "intermediate/precondition_timing_constraint.h"
#include "intermediate_parser/tokenize.h"
#include "nusmv_translator/nusmv_definitions.h"
#include "nusmv_translator/nusmv_translator.h"

// build a string corresponding to the behaviour module
const std::string NuSMVTranslator::MODULE_BEHAVIOUR =
	std::string("MODULE ") + MODULE_B + "(" + B_PRECONDITIONS + ", "
			+ B_CAN_INTERRUPT + ", " + B_CAN_BE_INT + ", " + SCHEDULE + ", "
			+ B_THIS_SCHEDULE + ", " + STEP + ", " + B_LAST_STEP + ")\n\n"
		+ TAB + "DEFINE\n"
		+ TAB + TAB + B_PRECONDITIONS_HOLD + ":= " + B_PRECONDITIONS + ";\n"
		+ TAB + TAB + B_CAN_BE_SCHEDULED + ":= ((" + SCHEDULE + " = "
			+ SCHEDULE + "_none | " + B_CAN_INTERRUPT + ") & "
			+ B_PRECONDITIONS_HOLD + ");\n"
		+ TAB + TAB + B_CAN_BE_INTERRUPTED + ":= " + B_CAN_BE_INT + ";\n"
		+ TAB + TAB + B_IS_LAST_STEP + ":= (" + B_IS_SCHEDULED + " & " + STEP
			+ " = " + B_LAST_STEP + ");\n"
		+ TAB + TAB + B_IS_SCHEDULED + ":= (" + SCHEDULE + " = "
			+ B_THIS_SCHEDULE + ");\n";

NuSMVTranslator::~NuSMVTranslator()
{
	for(auto pair : timing_constraint_map)
	{
		delete pair.second;
	}
}

/*------------------------------------------------------------------------------
	translate

	Translates the intermediate form into NuSMV input, which is returned as
	a string. Initial values for variables can be specified in the initial
	value file.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::translate(std::string& initial_value_file)
{
	if(buildInitialVariableValueMap(initial_value_file))
	{
		return translate();
	}
	else
	{
		// there was an error in the initial value file, return the empty string
		return "";
	}
}

/*------------------------------------------------------------------------------
	translate

	Translates the intermediate form into NuSMV input, which is returned as
	a string.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::translate()
{
	buildBehaviourLists();
	if(g_time_of_day == "")
	{
		// only build this if a specific time of not day has not been set
		// with the -tod command line parameter
		buildTimingConstraintMap();
	}
	buildBeenInWasInStateMaps();

	std::string schedule_var = buildScheduleVar();
	if(schedule_var == "")
	{
		// we need at least one behaviour to do anything
		displayError("no behaviours were parsed");
		return "";
	}
	std::string step_var = buildStepVar();
	if(step_var == "")
	{
		// we need at least one action to do anything
		displayError("no action rules were parsed");
		return "";
	}
	std::string last_schedule_var = buildLastScheduleVar();
	std::list<std::string> bool_vars = buildBoolsVar();
	if(bool_vars.empty())
	{
		// just let the user know no boolean variables are present
		// in the intermediate form
		displayWarning("no boolean variables were present");
	}
	std::list<std::string> enum_vars = buildEnumsVar();
	if(enum_vars.empty())
	{
		// just let the user know no enumerated variables are present
		// in the intermediate form
		displayWarning("no enumerated variables were present");
	}
	std::list<std::string> precondition_defines =
		buildPreconditionDefines();
	if(precondition_defines.empty())
	{
		// just let the user know no preconditions are present
		// in the intermediate form
		displayWarning("no precondition rules were parsed");
	}

	std::list<std::string> been_in_state_vars = buildBeenInStatesVar();
	std::string been_in_state_assigns = buildBeenInStateAssigns();
	std::list<std::string> was_in_state_vars = buildWasInStatesVar();
	std::string was_in_state_assigns = buildWasInStateAssigns();
	std::string executed_behaviour_execute_next =
		buildExecutedBehaviourExecuteNextDefine();
	std::list<std::string> interrupt_defines = buildCanInterruptDefines();
	std::string a_behaviour_can_be_executed =
		buildABehaviourCanBeScheduledDefine();
	std::string a_behaviour_is_ending = buildABehaviourIsEndingDefine();
	std::string an_executed_behaviour_is_ending =
		buildAnExecutedBehaviourIsEndingDefine();
	std::string an_executed_behaviour_is_executing =
		buildAnExecutedBehaviourIsScheduledDefine();
	std::string an_executed_behaviour_is_ending_as_a_last_action =
		buildAnExecutedBehaviourIsEndingAsALastActionDefine();
	std::list<std::string> behaviour_module_instances =
		buildBehaviourModuleInstances();
	std::string step_assign = buildStepAssign();
	std::string last_schedule_assign = buildLastScheduleAssign();
	std::string schedule_assign = buildScheduleAssign();
	std::string variable_assigns = buildVariableAssigns();

	// build the VAR section
	std::string section_var = "";
	addLine(section_var, "VAR", 1, false);
	if(g_time_of_day == "")
	{
		// only build the time variable if a specific time of not day has not
		// been set with the -tod command line parameter
		std::string time_var;
		if((time_var = buildTimeVar()) != "")
		{
			addLine(section_var, time_var, 2, true);
		}
	}
	addLine(section_var, step_var, 2, true);
	addLine(section_var, schedule_var, 2, true);
	addLine(section_var, last_schedule_var, 2, true);
	addLine(section_var, "", 2, false);
	addLine(section_var, H_LINE, 2, false);
	addLine(section_var, "--\tBehaviour Module Instances", 2, false);
	addLine(section_var, H_LINE, 2, false);
	for(std::string s : behaviour_module_instances)
	{
		addLine(section_var, s, 2, true);
	}
	addLine(section_var, "", 2, false);
	addLine(section_var, H_LINE, 2, false);
	addLine(section_var, "--\tBoolean Variables", 2, false);
	addLine(section_var, H_LINE, 2, false);
	for(std::string s : bool_vars)
	{
		addLine(section_var, s, 2, true);
	}
	addLine(section_var, "", 2, false);
	addLine(section_var, H_LINE, 2, false);
	addLine(section_var, "--\tEnumerated Types", 2, false);
	addLine(section_var, H_LINE, 2, false);

	for(std::string s : enum_vars)
	{
		addLine(section_var, s, 2, true);
	}
	addLine(section_var, "", 2, false);
	addLine(section_var, H_LINE, 2, false);
	addLine(section_var, "--\tBeen in State/Was in State Counters", 2, false);
	addLine(section_var, H_LINE, 2, false);

	for(std::string s : been_in_state_vars)
	{
		addLine(section_var, s, 2, true);
	}
	for(std::string s : was_in_state_vars)
	{
		addLine(section_var, s, 2, true);
	}

	// build the DEFINE section
	std::string section_define = "";
	addLine(section_define, "DEFINE", 1, false);
	for(std::string s : precondition_defines)
	{
		addLine(section_define, s, 2, true);
	}
	addLine(section_define, "", 2, false);
	for(std::string s : interrupt_defines)
	{
		addLine(section_define, s, 2, true);
	}
	addLine(section_define, "", 2, false);
	addLine(section_define, executed_behaviour_execute_next, 2, true);
	addLine(section_define, a_behaviour_can_be_executed, 2, true);
	addLine(section_define, a_behaviour_is_ending, 2, true);
	addLine(section_define, an_executed_behaviour_is_ending_as_a_last_action,
		2, true);
	addLine(section_define, an_executed_behaviour_is_ending, 2, true);
	addLine(section_define, an_executed_behaviour_is_executing, 2, true);

	// build the ASSIGN section
	std::string section_assign = "";
	addLine(section_assign, "ASSIGN", 1, false);
	if(g_time_of_day == "")
	{
		addLine(section_assign, buildTimeAssign(), 0, false);
	}
	addLine(section_assign, step_assign, 0, false);
	addLine(section_assign, last_schedule_assign, 0, false);
	addLine(section_assign, schedule_assign, 0, false);
	addLine(section_assign, variable_assigns, 0, false);
	addLine(section_assign, been_in_state_assigns, 0, false);
	addLine(section_assign, was_in_state_assigns, 0, false);

	// build the main module

	std::string main_module = "";

	// add some information about what settings were used to generate
	// the NuSMV file
	time_t t;
	time(&t);
	std::string date_string = ctime(&t);
	date_string = date_string.substr(0, date_string.size() - 1);
	addLine(main_module, H_LINE, 0, false);
	addLine(main_module, std::string("--\tfile generated ") + date_string
		+ " using " + EXEC_NAME + " v" + EXEC_VERSION, 0, false);
	addLine(main_module, "--", 0, false);
	addLine(main_module, std::string("--  true non-determinism: ")
		+ (g_true_non_determinism ? "yes" : "no"), 0, false);
	addLine(main_module,
		std::string("--  minimum one state for state_n_seconds: ")
		+ (g_minimum_one_state_state_n_seconds ? "yes" : "no"), 0, false);
	addLine(main_module, "--  seconds per state: "
		+ integerToString(g_state_time_seconds), 0, false);
	addLine(main_module, "--  max seconds for state_n_seconds: "
		+ integerToString(g_max_seconds_per_state_n_seconds), 0, false);
	if(g_time_of_day != "")
	{
		addLine(main_module, "--  time of day set to: " + g_time_of_day, 0,
			false);
	}
	// add some information about the behaviours constituting the
	// intermediate form
	addLine(main_module, H_LINE, 0, false);
	addLine(main_module, "--\tbehaviours:", 0, false);
	addLine(main_module, "--", 0, false);
	for(Behaviour* behaviour : intermediate_form->getBehaviours())
	{
		addLine(main_module, "--\t\t"
			+ validateIdentifier(behaviour->getName()) + "\t\t"
			+ " [priority: " + integerToString(behaviour->getPriority())
			+ "][" + (behaviour->isInterruptible() ? "" : "not ")
			+ "interruptible]["
			+ (behaviour->isSchedulable() ? "" : "not ")
			+ "schedulable]"
			, 0, false);
	}
	addLine(main_module, H_LINE, 0, false);
	addLine(main_module, "MODULE main", 0, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, "--\tVariables", 1, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, section_var, 0, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, "--\tAssignments", 1, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, section_assign, 0, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, "--\tDefinitions", 1, false);
	addLine(main_module, H_LINE, 1, false);
	addLine(main_module, section_define, 0, false);
	addLine(main_module, "", 0, false);

	// add this to generate some specifications that can be used
	// to determine if there is some world in which a behaviour can
	// be scheduled
	/*
	addLine(main_module, "LTLSPEC", 1, false);
	for(Behaviour* b : intermediate_form->getBehaviours())
	{
		addLine(main_module, "--G(schedule != schedule_"
			+ validateIdentifier(b->getName()) + ")", 2, true);
	}
	*/
	addLine(main_module, H_LINE, 0, false);
	addLine(main_module, "--\tBehaviour Module", 0, false);
	addLine(main_module, H_LINE, 0, false);

	addLine(main_module, MODULE_BEHAVIOUR, 0, false);

	return main_module;
}

/*------------------------------------------------------------------------------
	buildBehaviourLists

	Builds the lists of schedulable, executing, and executable behaviours.
------------------------------------------------------------------------------*/
void NuSMVTranslator::buildBehaviourLists()
{
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	if(!behaviours.empty())
	{
		auto it = behaviours.begin();
		auto end = behaviours.end();
		while(it != end)
		{
			Behaviour* behaviour = *it;
			if(behaviour->isSchedulable())
			{
				// this behaviour is schedulable so add it to the list of
				// schedulable behaviours
				schedulable_behaviours.push_back(behaviour);
			}
			bool executes_another = false;
			std::list<Action*> actions = behaviour->getActions();
			auto action_it = actions.begin();
			auto action_end = actions.end();
			while(action_it != action_end)
			{
				Action* action = *action_it;
				if(action->getActionType() == ActionType::EXECUTE)
				{
					ActionExecute* action_ex = (ActionExecute*)action;
					// add the executed behaviour to the list of
					// executable behaviours
					executable_behaviours.insert(
						intermediate_form->getBehaviourByName(
							action_ex->getBehaviour(), ""));
					executes_another = true;
				}
				else if(action->getActionType() == ActionType::EXECUTE_NON_D)
				{
					ActionExecuteNonDeterministic* action_ex_non_d =
						(ActionExecuteNonDeterministic*)action;
					// add the executable behaviours to the list of
					// executable behaviours
					for(std::string behaviour_name :
						action_ex_non_d->getBehaviourValues())
					{
						executable_behaviours.insert(
							intermediate_form->getBehaviourByName(
								behaviour_name, ""));
					}
					executes_another = true;
				}
				action_it++;
			}
			if(executes_another)
			{
				// this behaviour executes another behaviour, add it
				// to the list of executing behaviours
				executing_behaviours.push_back(behaviour);
			}
			it++;
		}
	}
}

/*------------------------------------------------------------------------------
	buildTimingConstraintMap

	Builds the timing constraint map.
------------------------------------------------------------------------------*/
void NuSMVTranslator::buildTimingConstraintMap()
{
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	std::list<PreconditionTimingConstraint*> timing_constraints;
	if(!behaviours.empty())
	{
		auto it = behaviours.begin();
		auto end = behaviours.end();
		while(it != end)
		{
			Behaviour* behaviour = *it;
			std::list<Precondition*> preconditions =
				behaviour->getPreconditions();
			auto precondition_it = preconditions.begin();
			auto precondition_end = preconditions.end();
			while(precondition_it != precondition_end)
			{
				Precondition* precondition = *precondition_it;
				if(precondition->getPreconditionType()
					== PreconditionType::TIMING_CONSTRAINT)
				{
					// this precondition is a timing constraint, add it
					// to the map
					PreconditionTimingConstraint* precondition_t =
						(PreconditionTimingConstraint*)precondition;
					timing_constraints.push_back(precondition_t);
					timing_constraint_map.insert(
						std::pair<PreconditionTimingConstraint*,
						std::list<std::string>*>(precondition_t,
							new std::list<std::string>()));
				}
				precondition_it++;
			}
			it++;
		}
	}
	if(timing_constraints.size() == 1)
	{
		// there is only one timing constraint, build a value for it
		// and add a no_time_constraints_hold value
		PreconditionTimingConstraint* p = timing_constraints.front();
		std::string value = validateIdentifier("_" + p->getStartTime() + "_to_"
			+ p->getEndTime());
		auto pair = *(timing_constraint_map.find(p));
		pair.second->push_back(value);
		time_intervals.push_back(value);
		time_intervals.push_back("no_time_constraints_hold");
	}
	else if(!timing_constraints.empty())
	{
		std::string time = "00:00:00";
		bool added_none_accepting_interval = false;
		std::list<PreconditionTimingConstraint*> accepting;
		// remove any time constraints from the list that hold
		// at the current time, and add them to the accepting list
		timing_constraints.remove_if(
			[&](PreconditionTimingConstraint* p)
			{
				bool remove_if = timeConstraintHoldsAtTime(p, time);
				if(remove_if)
				{
					accepting.push_back(p);
				}
				return remove_if;
			});
		// do this while there are still some time constraints that
		// are yet to be accepting, or are still accepting
		while(!timing_constraints.empty() || !accepting.empty())
		{
			PreconditionTimingConstraint* earliest_starting = nullptr;
			PreconditionTimingConstraint* earliest_finishing = nullptr;
			std::for_each(timing_constraints.begin(), timing_constraints.end(),
				[&](PreconditionTimingConstraint* p)
				{
					// get the timing constraint with the earliest
					// start time
					if(earliest_starting == nullptr ||
						timeIsBefore(p->getStartTime(),
							earliest_starting->getStartTime()))
					{
						earliest_starting = p;
					}
				});
			std::for_each(accepting.begin(), accepting.end(),
				[&](PreconditionTimingConstraint* p)
				{
					// get the timing constraint with the earliest
					// finishing time
					if(earliest_finishing == nullptr ||
						timeIsBefore(p->getEndTime(),
							earliest_finishing->getEndTime()))
					{
						earliest_finishing = p;
					}
				});
			if(earliest_starting == nullptr || (earliest_finishing != nullptr
				&& timeIsBefore(earliest_finishing->getEndTime(),
					earliest_starting->getStartTime())))
			{
				std::string value = validateIdentifier("_" + time + "_to_" +
					earliest_finishing->getEndTime());
				if(accepting.empty())
				{
					if(!added_none_accepting_interval)
					{
						time_intervals.push_back(
							"no_time_constraints_hold");
						added_none_accepting_interval = true;
					}
				}
				else
				{
					time_intervals.push_back(value);
				}
				accepting.remove_if(
					[&](PreconditionTimingConstraint* p)
					{
						bool remove_if = p->getEndTime() ==
							earliest_finishing->getEndTime();
						auto pair = *(timing_constraint_map.find(p));
						pair.second->push_back(value);
						return remove_if;
					});
				time = momentAfter(earliest_finishing->getEndTime());
			}
			else
			{
				if(timeIsBefore(time, earliest_starting->getStartTime()))
				{
					std::string value = validateIdentifier("_" + time + "_to_"
						+ momentBefore(earliest_starting->getStartTime()));
					if(accepting.empty())
					{
						if(!added_none_accepting_interval)
						{
							time_intervals.push_back(
								"no_time_constraints_hold");
							added_none_accepting_interval = true;
						}
					}
					else
					{
						time_intervals.push_back(value);
					}
					for(PreconditionTimingConstraint* p : accepting)
					{
						auto pair = *(timing_constraint_map.find(p));
						pair.second->push_back(value);
					}
				}
				timing_constraints.remove_if(
					[&](PreconditionTimingConstraint* p)
					{
						bool remove_if = p->getStartTime() ==
							earliest_starting->getStartTime();
						if(remove_if)
						{
							accepting.push_back(p);
						}
						return remove_if;
					});
				time = earliest_starting->getStartTime();
			}
		}
	}
}

/*------------------------------------------------------------------------------
	buildInitialVariableValueMap

	Builds the initial variable value map.
------------------------------------------------------------------------------*/
bool NuSMVTranslator::buildInitialVariableValueMap(std::string filename)
{
	bool successfully_parsed = true;
	std::ifstream ifstream(filename);
	try
	{
		if(!ifstream)
		{
			throw("error loading file \'" + filename + "\'");
		}
		// read in the input file as a string
		std::string input_file_string = std::string(
			std::istreambuf_iterator<char>(ifstream),
			std::istreambuf_iterator<char>());
		if(!hasNextToken(input_file_string))
		{
			// the file is empty
			throw("file \'" + filename + "\' is empty");
		}
		while(hasNextToken(input_file_string))
		{
			std::string line = getNextLine(input_file_string);
			std::string var_name = "";
			while(hasNextToken(line) &&
				(peekNextToken(line) != "="))
			{
				// read in tokens until '=' is found
				var_name += getNextToken(line);
				if(hasNextToken(line)
					&& (peekNextToken(line) != "="))
				{
					var_name += " ";
				}
			}
			if(!hasNextToken(line))
			{
				// no '=' was found
				throw("missing '=' in file \'" + filename + "\'");
			}
			getNextToken(line);
			if(!hasNextToken(line))
			{
				// no assignment was found
				throw("missing initial variable value in file \'" + filename
					+ "\'");
			}
			// trim the enclosing whitespace
			std::string value_name = trim(line);
			if(value_name.size() < 3)
			{
				throw(std::string("initial values must have the form")
				 + " \"value name\"");
			}
			// value names must be enclosed in quotation marks
			if(value_name.at(0) != '\"' ||
				value_name.at(value_name.size() - 1) != '\"')
			{
				throw(std::string("initial values must have the form")
				 + " \"value name\"");
			}
			// get the value name
			value_name = value_name.substr(1, value_name.size() - 2);
			PropVariable* p = intermediate_form->getPropVariableByName(var_name,
				"");
			if(p != nullptr)
			{
				if(p->isNonDeterministic())
				{
					throw(std::string("cannot assign initial value to")
						+ " non-deterministic propositional variable \'"
						+ p->getName() + "\'");
				}
				if(value_name != "true" && value_name != "false")
				{
					throw(std::string("initial values for propositional")
						+ " variables must be \"true\" or \"false\"");
				}
				else
				{
					// the value is valid, add it to the map
					initial_variable_values.insert(
						std::pair<std::string, std::string>(
							validateIdentifier(var_name), value_name));
				}
			}
			else
			{
				// no propositional variable was found with the given name
				EnumVariable* e = intermediate_form->getEnumVariableByName(
					var_name, "");
				if(e != nullptr)
				{
					if(e->isNonDeterministic())
					{
						throw(std::string("cannot assign initial value to")
							+ " non-deterministic enumerated variable \'"
							+ e->getName() + "\'");
					}
					if(e->resets())
					{
						throw(std::string("cannot assign initial value to")
							+ " resetting enumerated variable \'"
							+ e->getName() + "\'");
					}
					bool matched = false;
					for(std::string value : e->getValues())
					{
						if(value_name == value)
						{
							matched = true;
						}
					}
					if(!matched)
					{
						throw("\'" + value_name + "\' is not a valid value for"
							+ " enumerated variable variable \'" + var_name
							+ "\'");
					}
					else
					{
						// the value is valid, add it to the map
						initial_variable_values.insert(
							std::pair<std::string, std::string>(
								validateIdentifier(var_name),
								validateIdentifier(value_name)));
					}
				}
				else
				{
					// the variable name did not match any variable
					throw("variable \'" + var_name + "\' was not found");
				}
			}
		}
	}
	catch(std::string& error)
	{
		displayError("[buildInitialVariableValueMap]->\n"
			+ error);
		successfully_parsed = false;
	}
	ifstream.close();
	return successfully_parsed;
}

/*------------------------------------------------------------------------------
	buildBeenInWasInStateMaps

	Builds the been in state and was in state maps.
------------------------------------------------------------------------------*/
void NuSMVTranslator::buildBeenInWasInStateMaps()
{
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	for(Behaviour* b : behaviours)
	{
		std::list<Precondition*> preconditions = b->getPreconditions();
		for(Precondition* p : preconditions)
		{
			if(p->getTimeConstraintType() ==
				TimeConstraintType::BEEN_IN_STATE_FOR)
			{
				int time_constraint_seconds = p->getTimeConstraintSeconds();
				// bound the number of seconds
				if(time_constraint_seconds > g_max_seconds_per_state_n_seconds)
				{
					time_constraint_seconds = g_max_seconds_per_state_n_seconds;
				}
				double num_steps_double = (double)time_constraint_seconds
					/(double)g_state_time_seconds;
				int num_steps = static_cast<int>(round(num_steps_double));
				if(g_minimum_one_state_state_n_seconds && num_steps == 0)
				{
					num_steps = 1;
				}
				if(num_steps > 0)
				{
					std::list<std::string> values;
					int num_secs = time_constraint_seconds;
					while(num_steps >= 0)
					{
						// add the time values to the list
						if(num_steps == 0)
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ BEEN_IN_WAS_IN_START);
						}
						else if(num_secs == time_constraint_seconds)
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ BEEN_IN_WAS_IN_FINAL);
						}
						else
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ integerToString(num_secs));
						}
						num_steps--;
						num_secs -=	g_state_time_seconds;
					}
					// add the values to the map
					been_in_state_map.insert(
						std::pair<Precondition*, std::list<std::string>>(
							p, values));
				}
			}
			else if(p->getTimeConstraintType() ==
					TimeConstraintType::WAS_IN_STATE_WITHIN)
			{
				int time_constraint_seconds = p->getTimeConstraintSeconds();
				if(time_constraint_seconds > g_max_seconds_per_state_n_seconds)
				{
					time_constraint_seconds = g_max_seconds_per_state_n_seconds;
				}
				double num_steps_double = (double)time_constraint_seconds
					/(double)g_state_time_seconds;
				int num_steps = static_cast<int>(round(num_steps_double));
				if(g_minimum_one_state_state_n_seconds && num_steps == 0)
				{
					num_steps = 1;
				}
				if(num_steps > 0)
				{
					std::list<std::string> values;
					int num_secs = time_constraint_seconds;
					while(num_steps >= 0)
					{
						if(num_steps == 0)
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ BEEN_IN_WAS_IN_START);
						}
						else if(num_secs == time_constraint_seconds)
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ BEEN_IN_WAS_IN_FINAL);
						}
						else
						{
							values.push_front(std::string(BEEN_IN_WAS_IN_PREFIX)
								+ integerToString(num_secs));
						}
						num_steps--;
						num_secs -=	g_state_time_seconds;
					}
					was_in_state_map.insert(
						std::pair<Precondition*, std::list<std::string>>(
							p, values));
				}
			}
		}
	}
}

/*------------------------------------------------------------------------------
	buildEnumsVar

	Builds the variable definitions for	the enumerated variables.
------------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildEnumsVar()
{
	std::list<std::string> enum_vars;
	std::list<EnumVariable*> enum_variables =
		intermediate_form->getEnumVariables();
	for(EnumVariable* e : enum_variables)
	{
		// add the definition for this variable to the list
		enum_vars.push_back(buildEnumVar(validateIdentifier(e->getName()),
			e->getValues()));
	}
	return enum_vars;
}

/*------------------------------------------------------------------------------
	buildBoolsVar

	Builds the variable definitions for	the boolean variables.
------------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildBoolsVar()
{
	std::list<std::string> bool_vars;
	std::list<PropVariable*> prop_variables =
		intermediate_form->getPropVariables();
	for(PropVariable* p : prop_variables)
	{
		// add the definition for this variable to the list
		bool_vars.push_back(buildBoolVar(validateIdentifier(p->getName())));
	}
	return bool_vars;
}

/*------------------------------------------------------------------------------
	buildBeenInStatesVar

	Builds the the definitions for the enumerated variables used for
	been_in_state suffixed preconditions.
-----------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildBeenInStatesVar()
{
	std::list<std::string> been_in_state_vars;
	for(auto pair : been_in_state_map)
	{
		std::string been_in_state_var = "";
		Precondition* p = pair.first;
		std::list<std::string> values = pair.second;
		PreconditionType t = p->getPreconditionType();
		if(t == PreconditionType::PROP_VALUE_CHECK)
		{
			PreconditionPropValueCheck* p_prop =
				(PreconditionPropValueCheck*)p;
			PropVariable* prop_var = p_prop->getPropVariable();
			been_in_state_var += validateIdentifier(prop_var->getName()
				+ "_" + (p_prop->getTruthValue() ? "TRUE" : "FALSE"))
				+ "_" + BEEN_IN_STATE + ": {";
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				been_in_state_var += *it;
				if(++it != end)
				{
					been_in_state_var += ", ";
				}
			}
			been_in_state_var += "}";
		}
		else if(t == PreconditionType::ENUM_VALUE_CHECK)
		{
			PreconditionEnumValueCheck* p_enum =
				(PreconditionEnumValueCheck*)p;
			EnumVariable* enum_var = p_enum->getEnumVariable();
			been_in_state_var += validateIdentifier(enum_var->getName()
				+ "_" + validateIdentifier(p_enum->getEnumValue()))
				+ "_" + BEEN_IN_STATE + ": {";
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				been_in_state_var += *it;
				if(++it != end)
				{
					been_in_state_var += ", ";
				}
			}
			been_in_state_var += "}";
			been_in_state_var += "}";
		}
		else
		{

		}
		been_in_state_vars.push_back(been_in_state_var);
	}
	return been_in_state_vars;
}

/*------------------------------------------------------------------------------
	buildWasInStatesVar

	Builds the the definitions for the enumerated variables used for
	was_in_state suffixed preconditions.
------------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildWasInStatesVar()
{
	std::list<std::string> was_in_state_vars;
	for(auto pair : was_in_state_map)
	{
		std::string was_in_state_var = "";
		Precondition* p = pair.first;
		std::list<std::string> values = pair.second;
		PreconditionType t = p->getPreconditionType();
		if(t == PreconditionType::PROP_VALUE_CHECK)
		{
			PreconditionPropValueCheck* p_prop =
				(PreconditionPropValueCheck*)p;
			PropVariable* prop_var = p_prop->getPropVariable();
			was_in_state_var += validateIdentifier(prop_var->getName()
				+ "_" + (p_prop->getTruthValue() ? "TRUE" : "FALSE"))
				+ "_" + WAS_IN_STATE + ": {";
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				was_in_state_var += *it;
				if(++it != end)
				{
					was_in_state_var += ", ";
				}
			}
			was_in_state_var += "}";
		}
		else if(t == PreconditionType::ENUM_VALUE_CHECK)
		{
			PreconditionEnumValueCheck* p_enum =
				(PreconditionEnumValueCheck*)p;
			EnumVariable* enum_var = p_enum->getEnumVariable();
			was_in_state_var += validateIdentifier(enum_var->getName()
				+ "_" + validateIdentifier(p_enum->getEnumValue()))
				+ "_" + WAS_IN_STATE + ": {";
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				was_in_state_var += *it;
				if(++it != end)
				{
					was_in_state_var += ", ";
				}
			}
			was_in_state_var += "}";
		}
		else
		{

		}
		was_in_state_vars.push_back(was_in_state_var);
	}
	return was_in_state_vars;
}

/*------------------------------------------------------------------------------
	buildEnumVar

	Builds the the definition for a given enumerated variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildEnumVar(const std::string name,
	std::list<std::string> values)
{
	std::string enum_var = "";
	if(values.empty())
	{
		return enum_var;
	}
	else
	{
		enum_var += name + ": {";
		auto it = values.begin();
		auto end = values.end();
		while(it != end)
		{
			enum_var += validateIdentifier(*it);
			if(++it != end)
			{
				enum_var += ", ";
			}
		}
		enum_var += "}";
	}
	return enum_var;
}

/*------------------------------------------------------------------------------
	buildBoolVar

	Builds the the definition for a given boolean variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildBoolVar(const std::string name)
{
	std::string bool_var = "";
	bool_var += validateIdentifier(name) + ": boolean";
	return bool_var;
}

/*------------------------------------------------------------------------------
	buildTimeVar

	Builds the variable definition for the time variable which indicates
	the time of day.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildTimeVar()
{
	std::string time = "";
	if(!time_intervals.empty())
	{
		time += std::string(TIME) + ": {";
		auto it = time_intervals.begin();
		auto end = time_intervals.end();
		while(it != end)
		{
			time += *it;
			if(++it != end)
			{
				time += ", ";
			}
		}
		time += "}";
	}
	return time;
}

/*------------------------------------------------------------------------------
	buildStepVar

	Builds the variable definition for the step variable, this records
	which action is currently being performed by a behaviour.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildStepVar()
{
	std::string steps = "";
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	max_num_actions = 0;
	// get the maximum number of actions in a behaviour
	std::for_each(behaviours.begin(), behaviours.end(),
		[&](Behaviour* b)
		{
			max_num_actions = max_num_actions > b->getActions().size() ?
				max_num_actions : b->getActions().size();
		});
	if(max_num_actions > 0)
	{
		steps = std::string(STEP) +  ": {" + STEP + "_" + "none, ";
		for(unsigned int i = 1; i <= max_num_actions; i++)
		{
			steps += std::string(STEP) + "_" + integerToString(i);
			if(i < max_num_actions)
			{
				steps += ", ";
			}
		}
		steps += "}";
	}
	return steps;
}

/*------------------------------------------------------------------------------
	buildScheduleVar

	Builds the variable definition for the schedule variable, this records
	which behaviour is currently scheduled for execution.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildScheduleVar()
{
	std::string schedule = "";
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	if(!behaviours.empty())
	{
		schedule += std::string(SCHEDULE) + ": {" + SCHEDULE + "_none, ";
		auto it = behaviours.begin();
		auto end = behaviours.end();
		while(it != end)
		{
			Behaviour* b = *it;
			schedule += std::string(SCHEDULE) + "_"
				+ validateIdentifier(b->getName());
			if(++it != end)
			{
				schedule += ", ";
			}
		}
		schedule += "}";
	}
	return schedule;
}

/*------------------------------------------------------------------------------
	buildLastScheduleVar

	Builds the variable definition for the last_schedule variable, this
	is used to 'remember' any previous behaviour that may have executed the
	current behaviour.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildLastScheduleVar()
{
	std::string last_schedule = "";
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	if(!behaviours.empty())
	{
		last_schedule += std::string(LAST_SCHEDULE) + ": {" + SCHEDULE
			+ "_none, ";
		auto it = behaviours.begin();
		auto end = behaviours.end();
		while(it != end)
		{
			Behaviour* b = *it;
			last_schedule += std::string(SCHEDULE) + "_"
				+ validateIdentifier(b->getName());
			if(++it != end)
			{
				last_schedule += ", ";
			}
		}
		last_schedule += "}";
	}
	return last_schedule;
}

/*------------------------------------------------------------------------------
	buildPreconditionDefines

	For all behaviours, builds an expression that evaluates to true if all
	of the behaviour's preconditions hold.
------------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildPreconditionDefines()
{
	std::list<std::string> precondition_defines;
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	if(!behaviours.empty())
	{
		auto it = behaviours.begin();
		auto end = behaviours.end();
		while(it != end)
		{
			std::string behaviour_precondition;
			Behaviour* behaviour = *it;
			LogicalExpressionTree* logical_expression_tree =
				behaviour->getLogicalExpressionTree();
			if(logical_expression_tree != nullptr)
			{
				behaviour_precondition += PRECONDITION_PREFIX
					+ validateIdentifier(behaviour->getName()) + ":= ";
				behaviour_precondition += buildPreconditionDefine(
					logical_expression_tree->getRoot());
				precondition_defines.push_back(behaviour_precondition);
			}
			else
			{
				behaviour_precondition += PRECONDITION_PREFIX
					+ validateIdentifier(behaviour->getName()) + ":= TRUE";
				precondition_defines.push_back(behaviour_precondition);
			}
			it++;
		}
	}
	return precondition_defines;
}

/*------------------------------------------------------------------------------
	buildPreconditionDefine

	Given the root node of a logical expression tree, recursively builds an
	expression that evaluates to true if all of the tree's preconditions hold.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildPreconditionDefine(
	const TreeNode* node)
{
	std::string precondition_define;
	if(node->getNodeType() == TreeNodeType::INTERNAL_AND)
	{
		precondition_define =  "(" + buildPreconditionDefine(node->getLeft())
			+ " & " + buildPreconditionDefine(node->getRight()) + ")";
	}
	else if(node->getNodeType() == TreeNodeType::INTERNAL_OR)
	{
		precondition_define =  "(" + buildPreconditionDefine(node->getLeft())
			+ " | " + buildPreconditionDefine(node->getRight()) + ")";
	}
	else if(node->getNodeType() == TreeNodeType::INTERNAL_NOT)
	{
		precondition_define =  "!" + buildPreconditionDefine(node->getLeft());
	}
	else
	{
		Precondition* p = node->getPrecondition();
		if(p->getPreconditionType() == PreconditionType::PROP_VALUE_CHECK)
		{
			PreconditionPropValueCheck* p_cast = (PreconditionPropValueCheck*)p;
			PropVariable* prop_var = p_cast->getPropVariable();
			if(prop_var->isNonDeterministic())
			{
				precondition_define = validateIdentifier(prop_var->getName());
			}
			else
			{
				precondition_define = (p_cast->getTruthValue() ? "" : "!")
					+ validateIdentifier(prop_var->getName());
			}
			// if this has a time constraint add the additional conditions
			if(p_cast->getTimeConstraintType() ==
				TimeConstraintType::BEEN_IN_STATE_FOR)
			{
				auto it = been_in_state_map.find(p);
				if(it != been_in_state_map.end())
				{
					auto pair = *it;
					std::string been_in_var_name =
						validateIdentifier(prop_var->getName()) + "_"
							+ (p_cast->getTruthValue() ? "TRUE" : "FALSE")
							+ "_" + BEEN_IN_STATE;
					precondition_define += " & " + been_in_var_name + " = "
						+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_FINAL;
				}
			}
			else if(p_cast->getTimeConstraintType() ==
					TimeConstraintType::WAS_IN_STATE_WITHIN)
			{
				auto it = was_in_state_map.find(p);
				if(it != was_in_state_map.end())
				{
					auto pair = *it;
					std::string been_in_var_name =
						validateIdentifier(prop_var->getName()) + "_"
							+ (p_cast->getTruthValue() ? "TRUE" : "FALSE")
							+ "_" + WAS_IN_STATE;
					precondition_define += " & " + been_in_var_name + " != "
						+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START;
				}
			}
		}
		else if(p->getPreconditionType() == PreconditionType::ENUM_VALUE_CHECK)
		{
			PreconditionEnumValueCheck* p_cast = (PreconditionEnumValueCheck*)p;
			EnumVariable* enum_var = p_cast->getEnumVariable();
			precondition_define = validateIdentifier(enum_var->getName())
				+ " = " + validateIdentifier(p_cast->getEnumValue());
			if(p_cast->getTimeConstraintType() ==
				TimeConstraintType::BEEN_IN_STATE_FOR)
			{
				auto it = been_in_state_map.find(p);
				if(it != been_in_state_map.end())
				{
					auto pair = *it;
					std::string been_in_var_name =
						validateIdentifier(enum_var->getName()) + "_"
							+ p_cast->getEnumValue() + "_" + BEEN_IN_STATE;
					precondition_define += " & " + been_in_var_name + " = "
						+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_FINAL;
				}
			}
			else if(p_cast->getTimeConstraintType() ==
					TimeConstraintType::WAS_IN_STATE_WITHIN)
			{
				auto it = was_in_state_map.find(p);
				if(it != was_in_state_map.end())
				{
					auto pair = *it;
					std::string been_in_var_name =
						validateIdentifier(enum_var->getName()) + "_"
							+ p_cast->getEnumValue() + "_" + WAS_IN_STATE;
					precondition_define += " & " + been_in_var_name + " != "
						+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START;
				}
			}
		}
		else
		{
			PreconditionTimingConstraint* p_cast =
				(PreconditionTimingConstraint*)p;
			if(g_time_of_day == "")
			{
				// if no time of day has been set with -tod
				auto pair = *(timing_constraint_map.find(p_cast));
				if(pair.second->size() > 1)
				{
					precondition_define += "(";
				}
				auto it = pair.second->begin();
				auto end = pair.second->end();
				while(it != end)
				{
					precondition_define += std::string(TIME) + " = " + *it;
					if(++it != end)
					{
						precondition_define += " | ";
					}
				}
				if(pair.second->size() > 1)
				{
					precondition_define += ")";
				}
			}
			else
			{
				precondition_define +=
					(timeConstraintHoldsAtTime(p_cast, g_time_of_day) ?
						"TRUE" : "FALSE");
			}
		}
	}
	return precondition_define;
}

/*------------------------------------------------------------------------------
	buildCanInterruptDefines

	For all behaviours, builds an expression that evaluates to true when
	that behaviour can interrupt a currently scheduled behaviour in the next
	moment in time.
------------------------------------------------------------------------------*/
const std::list<std::string> NuSMVTranslator::buildCanInterruptDefines()
{
	std::list<std::string> can_interrupt_defines;
	std::set<int> priorities;
	// get the priorities
	std::for_each(schedulable_behaviours.begin(), schedulable_behaviours.end(),
		[&](Behaviour* b)
		{
			priorities.insert(b->getPriority());
		});
	// sort the list into priority order, highest first
	std::sort(schedulable_behaviours.begin(), schedulable_behaviours.end(),
		[&](Behaviour* this_b, Behaviour* that_b)
		{
			return this_b->getPriority() > that_b->getPriority();
		});
	int priority = *(priorities.begin());
	priorities.erase(priority);
	std::string interrupt_string = CAN_INTERRUPT_PREFIX;
	interrupt_string += integerToString(priority);
	std::string previous_interrupt_string = interrupt_string;
	interrupt_string += ":= FALSE";
	can_interrupt_defines.push_back(interrupt_string);
	unsigned int index = schedulable_behaviours.size() - 1;
	while(!priorities.empty())
	{
		std::list<Behaviour*> lower_priority_behaviours;
		int priority = *(priorities.begin());
		priorities.erase(priority);
		std::string interrupt_string = CAN_INTERRUPT_PREFIX;
		interrupt_string += integerToString(priority) + ":= (";
		// build the list of lower priority behaviours
		while(index >= 0
			&& (schedulable_behaviours[index]->getPriority() < priority))
		{
			lower_priority_behaviours.push_back(
				schedulable_behaviours[index]);
			index--;
		}
		if(!lower_priority_behaviours.empty())
		{
			std::string interruptibles = "";
			interrupt_string += previous_interrupt_string;
			auto it = lower_priority_behaviours.begin();
			auto end = lower_priority_behaviours.end();
			while(it != end)
			{
				Behaviour* b = *it;
				if(b->isInterruptible())
				{
					interruptibles += SCHEDULE;
					interruptibles += " = ";
					interruptibles += SCHEDULE;
					interruptibles += "_" + validateIdentifier(b->getName());
					if(++it != end)
					{
						auto it_copy = it;
						bool is_another = false;
						while(it_copy != end)
						{
							if((*it_copy)->isInterruptible())
							{
								is_another = true;
								break;
							}
							else
							{
								it_copy++;
							}
						}
						if(is_another)
						{
							interruptibles += " | ";
						}
					}
				}
				else
				{
					it++;
				}
			}
			if(interruptibles != "")
			{
				interrupt_string += " | ";
				interrupt_string += "(";
				interrupt_string += interruptibles;
				interrupt_string += "))";
			}
			else
			{
				interrupt_string += ")";
			}
			can_interrupt_defines.push_back(interrupt_string);
			previous_interrupt_string = CAN_INTERRUPT_PREFIX;
			previous_interrupt_string += integerToString(priority);
		}
	}
	return can_interrupt_defines;
}

/*------------------------------------------------------------------------------
	buildABehaviourCanBeScheduledDefine

	Builds the a_behaviour_can_be_scheduled definition, which evaluates to
	true if there is a behaviour that can be scheduled for execution in the
	next moment in time.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildABehaviourCanBeScheduledDefine()
{
	std::string a_behaviour_can_be_executed;
	a_behaviour_can_be_executed += A_BEHAVIOUR_CAN_BE_SCHEDULED;
	a_behaviour_can_be_executed += ":= (";
	auto it = schedulable_behaviours.begin();
	auto end = schedulable_behaviours.end();
	while(it != end)
	{
		Behaviour* b = *it;
		a_behaviour_can_be_executed += BEHAVIOUR_PREFIX
			+ validateIdentifier(b->getName())
			+ "." + B_CAN_BE_SCHEDULED;
		if(++it != end)
		{
			a_behaviour_can_be_executed += " | ";
		}
	}
	a_behaviour_can_be_executed += ")";
	return a_behaviour_can_be_executed;
}

/*------------------------------------------------------------------------------
	buildABehaviourIsEndingDefine

	Builds the a_behaviour_is_ending definition, which evaluates to
	true if the currently scheduled behaviour is executing its last action.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildABehaviourIsEndingDefine()
{
	std::string a_behaviour_is_ending;
	a_behaviour_is_ending += A_BEHAVIOUR_IS_ENDING;
	a_behaviour_is_ending += ":= (";
	auto it = schedulable_behaviours.begin();
	auto end = schedulable_behaviours.end();
	while(it != end)
	{
		Behaviour* b = *it;
		a_behaviour_is_ending += BEHAVIOUR_PREFIX
			+ validateIdentifier(b->getName())
			+ "." + B_IS_LAST_STEP;
		if(++it != end)
		{
			a_behaviour_is_ending += " | ";
		}
	}
	a_behaviour_is_ending += ")";
	return a_behaviour_is_ending;
}

/*------------------------------------------------------------------------------
	buildAnExecutedBehaviourIsEndingDefine

	Builds the an_executed_behaviour_is_ending definition, which evaluates
	to true if the currently scheduled behaviour is executing its last
	action, and this scheduled behaviour was executed by another behaviour.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildAnExecutedBehaviourIsEndingDefine()
{
	std::string an_executed_behaviour_is_ending = "";
	an_executed_behaviour_is_ending += AN_EXECUTED_BEHAVIOUR_IS_ENDING;
	an_executed_behaviour_is_ending += ":= ";
	if(executable_behaviours.empty())
	{
		an_executed_behaviour_is_ending += "FALSE";
	}
	else
	{
		an_executed_behaviour_is_ending += "(";
		auto it = executable_behaviours.begin();
		auto end = executable_behaviours.end();
		while(it != end)
		{
			Behaviour* b = *it;
			an_executed_behaviour_is_ending += BEHAVIOUR_PREFIX
				+ validateIdentifier(b->getName())
				+ "." + B_IS_LAST_STEP;
			if(++it != end)
			{
				an_executed_behaviour_is_ending += " | ";
			}
		}
		an_executed_behaviour_is_ending += ")";
	}
	return an_executed_behaviour_is_ending;
}

/*------------------------------------------------------------------------------
	buildAnExecutedBehaviourIsEndingAsALastActionDefine

	Builds the an_executed_behaviour_is_ending_as_a_last_action definition,
	which evaluates	to true if the currently scheduled behaviour is
	executing its last action, and this scheduled behaviour was executed by
	another behaviour, and the action that executed this behaviour was the
	last action in the ordered list of actions in the executing behaviour.
------------------------------------------------------------------------------*/
const std::string
	NuSMVTranslator::buildAnExecutedBehaviourIsEndingAsALastActionDefine()
{
	std::string an_executed_behaviour_is_ending_as_a_last_action_define = "";
	an_executed_behaviour_is_ending_as_a_last_action_define +=
		AN_EXECUTED_BEHAVIOUR_IS_ENDING_AS_A_LAST_ACTION;
	an_executed_behaviour_is_ending_as_a_last_action_define += ":= ";
	if(executing_behaviours.empty())
	{
		an_executed_behaviour_is_ending_as_a_last_action_define += "FALSE";
	}
	else
	{
		an_executed_behaviour_is_ending_as_a_last_action_define += "(";
		auto it = executing_behaviours.begin();
		auto end = executing_behaviours.end();
		bool this_is_the_first = true;
		bool exists_an_executable_behaviour_as_a_last_action = false;
		while(it != end)
		{
			Behaviour* executing_behaviour = *it;
			Action* last_action = *executing_behaviour->getActions().rbegin();
			if(last_action->getActionType() == ActionType::EXECUTE)
			{
				if(this_is_the_first)
				{
					this_is_the_first = false;
				}
				else
				{
					an_executed_behaviour_is_ending_as_a_last_action_define +=
						" | ";
				}
				ActionExecute* last_action_ex = (ActionExecute*)last_action;
				Behaviour* executed_behaviour =
					intermediate_form->getBehaviourByName(
						last_action_ex->getBehaviour(), "");
				an_executed_behaviour_is_ending_as_a_last_action_define += "(";
				an_executed_behaviour_is_ending_as_a_last_action_define +=
					BEHAVIOUR_PREFIX + validateIdentifier(
						executed_behaviour->getName()) + "." + B_IS_LAST_STEP;
				an_executed_behaviour_is_ending_as_a_last_action_define +=
					" & " + std::string(LAST_SCHEDULE) + " = "
					+ SCHEDULE + "_"
					+ validateIdentifier(executing_behaviour->getName()) + ")";
				exists_an_executable_behaviour_as_a_last_action = true;
				it++;
			}
			else if(last_action->getActionType() == ActionType::EXECUTE_NON_D)
			{
				if(this_is_the_first)
				{
					this_is_the_first = false;
				}
				else
				{
					an_executed_behaviour_is_ending_as_a_last_action_define +=
						" | ";
				}
				ActionExecuteNonDeterministic* last_action_ex_non_d =
					(ActionExecuteNonDeterministic*)last_action;
				std::list<std::string> executed_behaviours =
					last_action_ex_non_d->getBehaviourValues();
				auto it_name = executed_behaviours.begin();
				auto end_name = executed_behaviours.end();
				while(it_name != end_name)
				{
					std::string behaviour_name = *it_name;
					Behaviour* executed_behaviour =
						intermediate_form->getBehaviourByName(behaviour_name,
							"");
					an_executed_behaviour_is_ending_as_a_last_action_define
						+= "(";
					an_executed_behaviour_is_ending_as_a_last_action_define
						+=	BEHAVIOUR_PREFIX + validateIdentifier(
							executed_behaviour->getName()) + "."
								+ B_IS_LAST_STEP;
					an_executed_behaviour_is_ending_as_a_last_action_define
						+= " & " + std::string(LAST_SCHEDULE) + " = "
						+ SCHEDULE + "_"
						+ validateIdentifier(executing_behaviour->getName())
						+ ")";
					if(++it_name != end_name)
					{
						an_executed_behaviour_is_ending_as_a_last_action_define
							+= " | ";
					}
				}
				it++;
				exists_an_executable_behaviour_as_a_last_action = true;
			}
			else
			{
				it++;
			}
		}
		if(!exists_an_executable_behaviour_as_a_last_action)
		{
			an_executed_behaviour_is_ending_as_a_last_action_define += "FALSE";
		}
		else
		{
			an_executed_behaviour_is_ending_as_a_last_action_define += ")";
		}
	}
	return an_executed_behaviour_is_ending_as_a_last_action_define;
}

/*------------------------------------------------------------------------------
	buildAnExecutedBehaviourIsScheduledDefine

	Builds the an_executed_behaviour_is_scheduled definition, which
	evaluates to true if the currently scheduled behaviour was executed by
	another behaviour.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildAnExecutedBehaviourIsScheduledDefine()
{
	std::string an_executed_behaviour_is_executing = "";
	an_executed_behaviour_is_executing += AN_EXECUTED_BEHAVIOUR_IS_SCHEDULED;
	an_executed_behaviour_is_executing += ":= ";
	if(executable_behaviours.empty())
	{
		an_executed_behaviour_is_executing += "FALSE";
	}
	else
	{
		an_executed_behaviour_is_executing += "(";
		auto it = executable_behaviours.begin();
		auto end = executable_behaviours.end();
		while(it != end)
		{
			an_executed_behaviour_is_executing += BEHAVIOUR_PREFIX
				+ validateIdentifier((*it)->getName()) + "."
				+ B_IS_SCHEDULED;
			if(++it != end)
			{
				an_executed_behaviour_is_executing += " | ";
			}
		}
		an_executed_behaviour_is_executing += ")";
	}
	return an_executed_behaviour_is_executing;
}

/*------------------------------------------------------------------------------
	buildBehaviourModuleInstances

	Builds the instances of the	behaviour modules. One instance is created
	for each behaviour in the intermediate form.
------------------------------------------------------------------------------*/
std::list<std::string> NuSMVTranslator::buildBehaviourModuleInstances()
{
	std::list<std::string> behaviour_module_instances;
	std::list<Behaviour*> behaviours = intermediate_form->getBehaviours();
	std::for_each(behaviours.begin(), behaviours.end(),
		[&](Behaviour* behaviour)
		{
			behaviour_module_instances.push_back(
				buildBehaviourModuleInstance(behaviour));
		});
	return behaviour_module_instances;
}

/*------------------------------------------------------------------------------
	buildBehaviourModuleInstance

	Builds an instance of a behaviour module for the given behaviour.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildBehaviourModuleInstance(
	Behaviour* behaviour)
{
	std::string behaviour_module_instance = "";
	std::string behaviour_name = validateIdentifier(behaviour->getName());
	behaviour_module_instance += BEHAVIOUR_PREFIX + behaviour_name + ": ";
	behaviour_module_instance += MODULE_B;
	behaviour_module_instance += "(";
	behaviour_module_instance += PRECONDITION_PREFIX + behaviour_name
		+ ", ";
	if(!behaviour->isSchedulable())
	{
		behaviour_module_instance += "FALSE, ";
	}
	else
	{
		behaviour_module_instance += CAN_INTERRUPT_PREFIX
			+ integerToString(behaviour->getPriority()) + ", ";
	}
	std::list<std::string> higher_priority_behaviours;
	for(Behaviour* b : schedulable_behaviours)
	{
		if(b->getPriority() > behaviour->getPriority())
		{
			higher_priority_behaviours.push_back(BEHAVIOUR_PREFIX
				+ validateIdentifier(b->getName()));
		}
	}
	if(higher_priority_behaviours.empty() || !behaviour->isInterruptible())
	{
		behaviour_module_instance += "FALSE, ";
	}
	else
	{
		std::string can_be_interrupted = "(";
		auto it = higher_priority_behaviours.begin();
		auto end = higher_priority_behaviours.end();
		while(it != end)
		{
			can_be_interrupted += *it + "."+ B_CAN_BE_SCHEDULED;
			if(++it != end)
			{
				can_be_interrupted += " | ";
			}
		}
		can_be_interrupted += "), ";
		behaviour_module_instance += can_be_interrupted;
	}
	behaviour_module_instance += SCHEDULE;
	behaviour_module_instance += ", " ;
	behaviour_module_instance += SCHEDULE;
	behaviour_module_instance += "_" + behaviour_name + ", ";
	behaviour_module_instance += STEP;
	behaviour_module_instance += ", ";
	behaviour_module_instance += STEP;
	behaviour_module_instance += "_"
		+ integerToString(behaviour->getActions().size());
	behaviour_module_instance += ")";
	return behaviour_module_instance;
}

/*------------------------------------------------------------------------------
	buildExecutedBehaviourExecuteNextDefine

	Builds the executed_behaviour_execute_next definition, which is true
	if an executed behaviour will be scheduled for execution in the next
	moment in time,
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildExecutedBehaviourExecuteNextDefine()
{
	std::string executed_behaviour_execute_next_define =
		std::string(EXECUTED_BEHAVIOUR_EXECUTE_NEXT) + ":= ";
	if(executing_behaviours.empty())
	{
		return executed_behaviour_execute_next_define + "FALSE";
	}
	else
	{
		executed_behaviour_execute_next_define += "(";
	}
	bool is_first = true;
	for(Behaviour* behaviour : executing_behaviours)
	{
		std::string behaviour_name = validateIdentifier(behaviour->getName());
		int action_num = 1;
		for(Action* action : behaviour->getActions())
		{
			if(action->getActionType() == ActionType::EXECUTE
				|| action->getActionType() == ActionType::EXECUTE_NON_D)
			{
				std::string s = "(!"
					+ std::string(BEHAVIOUR_PREFIX)
					+ behaviour_name + "."
					+ B_CAN_BE_INTERRUPTED + " & "
					+ std::string(BEHAVIOUR_PREFIX)
					+ behaviour_name + "."
					+ std::string(B_IS_SCHEDULED) + " & " + std::string(STEP)
					+ " = " + std::string(STEP) + "_"
					+ integerToString(action_num) + ")";
				if(!is_first)
				{
					executed_behaviour_execute_next_define += " | " + s;
				}
				else
				{
					executed_behaviour_execute_next_define += s;
					is_first = false;
				}
			}
			action_num++;
		}
	}
	executed_behaviour_execute_next_define += ")";
	return executed_behaviour_execute_next_define;
}

/*------------------------------------------------------------------------------
	buildBeenInStateAssigns

	Builds the assignments for the enumerated variables used for
	been_in_state suffixed preconditions.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildBeenInStateAssigns()
{
	std::string been_in_state_assigns;
	for(auto pair : been_in_state_map)
	{
		Precondition* p = pair.first;
		std::list<std::string> values = pair.second;
		std::string been_in_state_assign = "";
		PreconditionType t = p->getPreconditionType();
		if(t == PreconditionType::PROP_VALUE_CHECK)
		{
			PreconditionPropValueCheck* p_prop =
				(PreconditionPropValueCheck*)p;
			PropVariable* prop_var = p_prop->getPropVariable();
			std::string prop_var_name = validateIdentifier(
				prop_var->getName());
			std::string var_name = prop_var_name + "_"
				+ (p_prop->getTruthValue() ? "TRUE" : "FALSE")
				+ "_" + BEEN_IN_STATE;
			addLine(been_in_state_assign, "init(" + var_name + "):= "
				+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START, 2, true);
			addLine(been_in_state_assign, "next(" + var_name + "):=", 2, false);
			addLine(been_in_state_assign, "case", 3, false);
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				if(it == values.begin())
				{
					auto next = it;
					next++;
					addLine(been_in_state_assign, prop_var_name
						+ " = " + (!p_prop->getTruthValue() ? "TRUE" : "FALSE")
						+ ": " + *(values.begin()), 4, true);
					addLine(been_in_state_assign, "(" + var_name + " = "
						+ BEEN_IN_WAS_IN_PREFIX + "0 & " + prop_var_name
						+ " = " + (p_prop->getTruthValue() ? "TRUE" : "FALSE")
						+ "): " + *next, 4, true);
				}
				else if(*it == *values.rbegin())
				{
					addLine(been_in_state_assign, var_name + " = " + *it
						+ ": " + *it, 4, true);
				}
				else
				{
					auto next = it;
					next++;
					addLine(been_in_state_assign, var_name + " = " + *it
						+ ": " + *next, 4, true);
				}
				it++;
			}
			addLine(been_in_state_assign, "", 4, false);
			addLine(been_in_state_assign, "TRUE: " + var_name, 4, true);
			addLine(been_in_state_assign, "esac", 3, true);
		}
		else if(t == PreconditionType::ENUM_VALUE_CHECK)
		{
			PreconditionEnumValueCheck* p_enum =
				(PreconditionEnumValueCheck*)p;
			EnumVariable* enum_var = p_enum->getEnumVariable();
			std::string enum_var_name = validateIdentifier(
				enum_var->getName());
			std::string var_name = enum_var_name + "_"
				+ validateIdentifier(p_enum->getEnumValue())
				+ "_" + BEEN_IN_STATE;
			addLine(been_in_state_assign, "init(" + var_name + "):= "
				+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START, 2, true);
			addLine(been_in_state_assign, "next(" + var_name + "):=", 2, false);
			addLine(been_in_state_assign, "case", 3, false);
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				if(it == values.begin())
				{
					auto next = it;
					next++;
					addLine(been_in_state_assign, enum_var_name
						+ " != " + validateIdentifier(p_enum->getEnumValue())
						+ ": " + *(values.begin()), 4, true);
					addLine(been_in_state_assign, "(" + var_name + " = "
						+ BEEN_IN_WAS_IN_PREFIX + "0 & " + enum_var_name
						+ " = " + validateIdentifier(p_enum->getEnumValue())
						+ "): " + *next, 4, true);
				}
				else if(*it == *values.rbegin())
				{
					addLine(been_in_state_assign, var_name + " = " + *it
						+ ": " + *it, 4, true);
				}
				else
				{
					auto next = it;
					next++;
					addLine(been_in_state_assign, var_name + " = " + *it
						+ ": " + *next, 4, true);
				}
				it++;
			}
			addLine(been_in_state_assign, "", 4, false);
			addLine(been_in_state_assign, "TRUE: " + var_name, 4, true);
			addLine(been_in_state_assign, "esac", 3, true);
		}
		else
		{

		}
		addLine(been_in_state_assigns, been_in_state_assign, 0, false);
	}
	return been_in_state_assigns;
}

/*------------------------------------------------------------------------------
	buildWasInStateAssigns

	Builds the assignments for the enumerated variables used for
	was_in_state suffixed preconditions.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildWasInStateAssigns()
{
	std::string was_in_state_assigns;
	for(auto pair : was_in_state_map)
	{
		Precondition* p = pair.first;
		std::list<std::string> values = pair.second;
		std::string was_in_state_assign = "";
		PreconditionType t = p->getPreconditionType();
		if(t == PreconditionType::PROP_VALUE_CHECK)
		{
			PreconditionPropValueCheck* p_prop =
				(PreconditionPropValueCheck*)p;
			PropVariable* prop_var = p_prop->getPropVariable();
			std::string prop_var_name = validateIdentifier(
				prop_var->getName());
			std::string var_name = prop_var_name + "_"
				+ (p_prop->getTruthValue() ? "TRUE" : "FALSE")
				+ "_" + WAS_IN_STATE;
			addLine(was_in_state_assign, "init(" + var_name + "):= "
				+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START, 2, true);
			addLine(was_in_state_assign, "next(" + var_name + "):=", 2, false);
			addLine(was_in_state_assign, "case", 3, false);
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				if(it == values.begin())
				{
					auto next = it;
					next++;
					addLine(was_in_state_assign, prop_var_name
						+ " = " + (p_prop->getTruthValue() ? "TRUE" : "FALSE")
						+ ": " + *next, 4, true);
				}
				else if(*it == *values.rbegin())
				{
					addLine(was_in_state_assign, var_name + " = " + *it
						+ ": " + *(values.begin()), 4, true);
				}
				else
				{
					auto next = it;
					next++;
					addLine(was_in_state_assign, var_name + " = " + *it
						+ ": " + *next, 4, true);
				}
				it++;
			}
			addLine(was_in_state_assign, "", 4, false);
			addLine(was_in_state_assign, "TRUE: " + var_name, 4, true);
			addLine(was_in_state_assign, "esac", 3, true);
		}
		else if(t == PreconditionType::ENUM_VALUE_CHECK)
		{
			PreconditionEnumValueCheck* p_enum =
				(PreconditionEnumValueCheck*)p;
			EnumVariable* enum_var = p_enum->getEnumVariable();
			std::string enum_var_name = validateIdentifier(
				enum_var->getName());
			std::string var_name = enum_var_name + "_"
				+ validateIdentifier(p_enum->getEnumValue())
				+ "_" + WAS_IN_STATE;
			addLine(was_in_state_assign, "init(" + var_name + "):= "
				+ BEEN_IN_WAS_IN_PREFIX + BEEN_IN_WAS_IN_START, 2, true);
			addLine(was_in_state_assign, "next(" + var_name + "):=", 2, false);
			addLine(was_in_state_assign, "case", 3, false);
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				if(it == values.begin())
				{
					auto next = it;
					next++;
					addLine(was_in_state_assign, enum_var_name
						+ " = " + validateIdentifier(p_enum->getEnumValue())
						+ ": " + *next, 4, true);
				}
				else if(*it == *values.rbegin())
				{
					addLine(was_in_state_assign, var_name + " = " + *it
						+ ": " + *(values.begin()), 4, true);
				}
				else
				{
					auto next = it;
					next++;
					addLine(was_in_state_assign, var_name + " = " + *it
						+ ": " + *next, 4, true);
				}
				it++;
			}
			addLine(was_in_state_assign, "", 4, false);
			addLine(was_in_state_assign, "TRUE: " + var_name, 4, true);
			addLine(was_in_state_assign, "esac", 3, true);
		}
		else
		{

		}
		addLine(was_in_state_assigns, was_in_state_assign, 0, false);
	}
	return was_in_state_assigns;
}

/*------------------------------------------------------------------------------
	buildTimeAssign

	Builds the assignments for the time variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildTimeAssign()
{
	std::string time_assign = "";
	if(!time_intervals.empty())
	{
		std::string value_list = "";
		auto it = time_intervals.begin();
		auto end = time_intervals.end();
		while(it != end)
		{
			value_list += *it;
			if(++it != end)
			{
				value_list += ", ";
			}
		}
		addLine(time_assign, "init(" + std::string(TIME)
			+ "):= {" + value_list + "}", 2, true);
	}
	return time_assign;
}

/*------------------------------------------------------------------------------
	buildStepAssign

	Builds the assignments for the step variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildStepAssign()
{
	std::string step_assign = "";
	addLine(step_assign, "init(" + std::string(STEP)
		+ "):= " + STEP + "_none", 2, true);
	addLine(step_assign, "next(" + std::string(STEP)
		+ "):=", 2, false);
	addLine(step_assign, "case", 3, false);
	addLine(step_assign, std::string(A_BEHAVIOUR_CAN_BE_SCHEDULED)
		+ ": " + std::string(STEP) + "_1", 4, true);
	addLine(step_assign, "", 4, false);
	for(Behaviour* behaviour : executing_behaviours)
	{
		int action_num = 1;
		for(Action* action : behaviour->getActions())
		{
			if(action->getActionType() == ActionType::EXECUTE)
			{
				ActionExecute* action_ex = (ActionExecute*)action;
				addLine(step_assign, std::string("(")
					+ BEHAVIOUR_PREFIX
					+ validateIdentifier(behaviour->getName()) + "."
					+ B_IS_SCHEDULED + " & "+ STEP + " = " + STEP + "_"
					+ integerToString(action_num) + " & "
					+ BEHAVIOUR_PREFIX
					+ validateIdentifier(action_ex->getBehaviour())
					+ "." + B_PRECONDITIONS_HOLD + "): " + STEP + "_1", 4,
					true);
			}
			else if(action->getActionType() == ActionType::EXECUTE_NON_D)
			{
				addLine(step_assign, std::string("(")
					+ BEHAVIOUR_PREFIX
					+ validateIdentifier(behaviour->getName()) + "."
					+ B_IS_SCHEDULED + " & "+ STEP + " = " + STEP + "_"
					+ integerToString(action_num) + "): " + STEP + "_1", 4,
					true);
			}
			action_num++;
		}
	}
	auto it = executing_behaviours.begin();
	auto end = executing_behaviours.end();
	while(it != end)
	{
		Behaviour* executing_behaviour = *it;
		std::list<Action*> actions = executing_behaviour->getActions();
		unsigned int action_num = 1;
		for(Action* action : actions)
		{
			// don't do this for last action executions
			if(action_num < actions.size())
			{
				if(action->getActionType() == ActionType::EXECUTE)
				{
					ActionExecute* action_ex = (ActionExecute*)action;
					Behaviour* executed_behaviour =
						intermediate_form->getBehaviourByName(
							action_ex->getBehaviour(), "");
					addLine(step_assign, std::string("(")
						+ BEHAVIOUR_PREFIX + validateIdentifier(
							executed_behaviour->getName()) + "."
						+ B_IS_LAST_STEP + " & " + LAST_SCHEDULE + " = "
						+ SCHEDULE + "_" + validateIdentifier(
							executing_behaviour->getName()) + "): " + STEP + "_"
						+ integerToString(action_num + 1), 4, true);
				}
				else if(action->getActionType() == ActionType::EXECUTE_NON_D)
				{
					ActionExecuteNonDeterministic* action_ex_non_d =
						(ActionExecuteNonDeterministic*)action;
					std::list<std::string> executed_behaviours =
						action_ex_non_d->getBehaviourValues();
					auto it_name = executed_behaviours.begin();
					auto end_name = executed_behaviours.end();
					while(it_name != end_name)
					{
						std::string behaviour_name = *it_name;
						Behaviour* executed_behaviour =
							intermediate_form->getBehaviourByName(
								behaviour_name, "");
						addLine(step_assign, std::string("(")
							+ BEHAVIOUR_PREFIX + validateIdentifier(
								executed_behaviour->getName()) + "."
							+ B_IS_LAST_STEP + " & " + LAST_SCHEDULE
							+ " = " + SCHEDULE + "_"
							+ validateIdentifier(executing_behaviour->getName())
							+ "): " + STEP + "_" + integerToString(
								action_num + 1), 4, true);
						it_name++;
					}
				}
			}
			action_num++;
		}
		it++;
	}
	addLine(step_assign, "", 4, false);
	addLine(step_assign, std::string(A_BEHAVIOUR_IS_ENDING)
		+ ": " + STEP + "_none", 4, true);
	addLine(step_assign,
		std::string(AN_EXECUTED_BEHAVIOUR_IS_ENDING_AS_A_LAST_ACTION)
		+ ": " + STEP + "_none", 4, true);
	addLine(step_assign, "", 4, false);
	for(unsigned int i = 1; i < max_num_actions; i++)
	{
		addLine(step_assign, std::string(STEP) + " = " + STEP +
			+ "_" + integerToString(i) + ": " + STEP + "_"
			+ integerToString(i + 1), 4, true);
	}
	addLine(step_assign, std::string(STEP) + " = " + STEP +
		+ "_" + integerToString(max_num_actions) + ": " + STEP + "_none",
		4, true);
	addLine(step_assign, "", 4, false);
	addLine(step_assign, "TRUE: " + std::string(STEP) + "_none", 4, true);
	addLine(step_assign, "esac", 3, true);
	return step_assign;
}

/*------------------------------------------------------------------------------
	buildLastScheduleAssign

	Builds the assignments for the last_schedule variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildLastScheduleAssign()
{
	std::string last_schedule_assign = "";
	addLine(last_schedule_assign, "init(" + std::string(LAST_SCHEDULE)
		+ "):= " + SCHEDULE + "_" + "none", 2, true);
	addLine(last_schedule_assign, "next(" + std::string(LAST_SCHEDULE)
		+ "):=", 2, false);
	addLine(last_schedule_assign, "case", 3, false);
	addLine(last_schedule_assign, std::string(EXECUTED_BEHAVIOUR_EXECUTE_NEXT)
		+ ": " + LAST_SCHEDULE, 4, true);
	//addLine(last_schedule_assign, std::string(AN_EXECUTED_BEHAVIOUR_IS_ENDING)
		//+ ": FALSE", 4, true);
	addLine(last_schedule_assign,
		std::string(AN_EXECUTED_BEHAVIOUR_IS_SCHEDULED)
		+ ": " + LAST_SCHEDULE, 4, true);
	addLine(last_schedule_assign, "TRUE: " + std::string(SCHEDULE), 4, true);
	addLine(last_schedule_assign, "esac", 3, true);
	return last_schedule_assign;
}

/*------------------------------------------------------------------------------
	buildScheduleAssign

	Builds the assignments for the schedule variable.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildScheduleAssign()
{
	std::string schedule_assign = "";
	addLine(schedule_assign, "init(" + std::string(SCHEDULE) + "):= "
		+ SCHEDULE + "_none", 2, true);
	addLine(schedule_assign, "next(" + std::string(SCHEDULE) + "):=", 2, false);
	addLine(schedule_assign, "case", 3, false);
	std::sort(schedulable_behaviours.begin(), schedulable_behaviours.end(),
		[&](Behaviour* this_behaviour, Behaviour* that_behaviour)
		{
			return this_behaviour->getPriority()
				> that_behaviour->getPriority();
		});
	if(g_true_non_determinism)
	{
		unsigned int index = 0;
		while(index < schedulable_behaviours.size())
		{
			Behaviour* behaviour = schedulable_behaviours[index];
			std::vector<std::string> equal_priority_behaviours;
			equal_priority_behaviours.push_back(
				validateIdentifier(behaviour->getName()));
			while(++index < schedulable_behaviours.size()
				&& schedulable_behaviours[index]->getPriority() ==
					behaviour->getPriority())
			{
				if(schedulable_behaviours[index]->isSchedulable())
				{
					equal_priority_behaviours.push_back(validateIdentifier(
						schedulable_behaviours[index]->getName()));
				}
			}
			if(equal_priority_behaviours.size() == 1)
			{
				addLine(schedule_assign, BEHAVIOUR_PREFIX
					+ equal_priority_behaviours[0] + "."
					+ B_CAN_BE_SCHEDULED + ": " + SCHEDULE + "_"
					+ equal_priority_behaviours[0], 4, true);
			}
			else
			{
				std::vector<std::vector<std::string>> power_set =
						vectorPowerSet<std::string>(
							equal_priority_behaviours, false);
				for(std::vector<std::string> v : power_set)
				{
					if(v.size() == 1)
					{
						addLine(schedule_assign, BEHAVIOUR_PREFIX
							+ v.front() + "." + B_CAN_BE_SCHEDULED + ": "
							+ SCHEDULE + "_" + v.front(),
							4, true);
					}
					else if(v.size() > 1)
					{
						std::string premises = "(";
						std::string next_value = "{";
						auto it = v.begin();
						auto end = v.end();
						while(it != end)
						{
							std::string behaviour_name = *it;
							premises += BEHAVIOUR_PREFIX + behaviour_name
								+ "." + B_CAN_BE_SCHEDULED;
							next_value += std::string(SCHEDULE) + "_"
								+ behaviour_name;
							if(++it != end)
							{
								premises += " & ";
								next_value += ", ";
							}
						}
						premises += ")";
						next_value += "}";
						addLine(schedule_assign, premises + ": " + next_value,
							4, true);
					}
				}
			}
		}
	}
	else
	{
		for(Behaviour* behaviour : schedulable_behaviours)
		{
			std::string behaviour_name = validateIdentifier(
				behaviour->getName());
			addLine(schedule_assign, BEHAVIOUR_PREFIX
				+ behaviour_name + "."
				+ B_CAN_BE_SCHEDULED + ": " + SCHEDULE + "_"
				+ behaviour_name, 4, true);
		}
	}
	for(Behaviour* behaviour : executing_behaviours)
	{
		int action_num = 1;
		std::string behaviour_name = validateIdentifier(behaviour->getName());
		for(Action* action : behaviour->getActions())
		{
			if(action->getActionType() == ActionType::EXECUTE)
			{
				ActionExecute* action_ex = (ActionExecute*)action;
				std::string executed_behaviour_name =
					validateIdentifier(action_ex->getBehaviour());
				addLine(schedule_assign, "("
					+ std::string(BEHAVIOUR_PREFIX)
					+ behaviour_name + "." + B_IS_SCHEDULED + " & "+ STEP
					+ " = " + STEP + "_" + integerToString(action_num) + " & "
					+ BEHAVIOUR_PREFIX + executed_behaviour_name
					+ "." + B_PRECONDITIONS_HOLD
					+ "): " + SCHEDULE + "_"
					+ executed_behaviour_name, 4, true);
			}
			else if(action->getActionType() == ActionType::EXECUTE_NON_D)
			{
				ActionExecuteNonDeterministic* action_ex_non_d =
					(ActionExecuteNonDeterministic*)action;
				std::list<std::string> executed_behaviours =
					action_ex_non_d->getBehaviourValues();
				std::string behaviour_list = "";
				auto it_name = executed_behaviours.begin();
				auto end_name = executed_behaviours.end();
				while(it_name != end_name)
				{
					std::string executed_behaviour_name =
						validateIdentifier(*it_name);
					behaviour_list += std::string(SCHEDULE) + "_"
						+ executed_behaviour_name;
					if(++it_name != end_name)
					{
						behaviour_list += ", ";
					}
				}
				addLine(schedule_assign, "("
					+ std::string(BEHAVIOUR_PREFIX) + behaviour_name + "."
					+ B_IS_SCHEDULED + " & "+ STEP + " = " + STEP + "_"
					+ integerToString(action_num)
					+ "): {" + behaviour_list
					+ "}", 4, true);
			}
			action_num++;
		}
	}
	addLine(schedule_assign, "", 4, false);
	addLine(schedule_assign, std::string(A_BEHAVIOUR_IS_ENDING)
		+ ": " + SCHEDULE + "_none", 4, true);
	addLine(schedule_assign,
		std::string(AN_EXECUTED_BEHAVIOUR_IS_ENDING_AS_A_LAST_ACTION)
		+ ": " + SCHEDULE + "_none", 4, true);
	addLine(schedule_assign, std::string("(") +
		AN_EXECUTED_BEHAVIOUR_IS_ENDING + " & " + LAST_SCHEDULE + " != "
		+ SCHEDULE + "): " + LAST_SCHEDULE, 4, true);
	addLine(schedule_assign, std::string(AN_EXECUTED_BEHAVIOUR_IS_ENDING)
		+ ": " + SCHEDULE + "_none", 4, true);

	addLine(schedule_assign, "", 4, false);
	addLine(schedule_assign, "TRUE: " + std::string(SCHEDULE), 4, true);
	addLine(schedule_assign, "esac", 3, true);
	return schedule_assign;
}

/*------------------------------------------------------------------------------
	buildVariableAssigns

	Builds the assignments for the boolean and enumerated variables.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::buildVariableAssigns()
{
	std::string variable_assigns = "";
	for(Behaviour* b : intermediate_form->getBehaviours())
	{
		std::string behaviour_name = validateIdentifier(b->getName());
		int action_num = 1;
		for(Action* action : b->getActions())
		{
			if(action->getActionType() == ActionType::PROP_VALUE_ASSIGNMENT)
			{
				ActionPropValueAssignment* action_prop =
					(ActionPropValueAssignment*)action;
				std::string prop_var_name = validateIdentifier(
					action_prop->getPropVariable()->getName());
				std::string prop_var_assign = "";
				prop_var_assign += "(" + std::string(BEHAVIOUR_PREFIX)
					+ behaviour_name + "." + B_IS_SCHEDULED + " & "
					+ STEP + " = " + STEP + "_"
					+ integerToString(action_num) + "): "
					+ (action_prop->getTruthValue() ? "TRUE" : "FALSE");
				auto it = prop_variable_assignment_map.find(prop_var_name);
				if(it == prop_variable_assignment_map.end())
				{
					std::list<std::string> new_prop_list;
					new_prop_list.push_back(prop_var_assign);
					prop_variable_assignment_map.insert(
						std::pair<std::string, std::list<std::string>>(
							prop_var_name, new_prop_list));
				}
				else
				{
					(*it).second.push_back(prop_var_assign);
				}
			}
			else if(action->getActionType() ==
				ActionType::ENUM_VALUE_ASSIGNMENT)
			{
				ActionEnumValueAssignment* action_enum =
					(ActionEnumValueAssignment*)action;
				std::string enum_var_name =
					action_enum->getEnumVariable()->getName();
				std::string enum_var_assign = "";
				enum_var_assign += "(" + std::string(BEHAVIOUR_PREFIX)
					+ behaviour_name + "." + B_IS_SCHEDULED + " & "
					+ STEP + " = " + STEP + "_"
					+ integerToString(action_num) + "): "
					+ validateIdentifier(action_enum->getEnumValue());
				auto it = enum_variable_assignment_map.find(enum_var_name);
				if(it == enum_variable_assignment_map.end())
				{
					std::list<std::string> new_enum_list;
					new_enum_list.push_back(enum_var_assign);
					enum_variable_assignment_map.insert(
						std::pair<std::string, std::list<std::string>>(
							enum_var_name, new_enum_list));
				}
				else
				{
					(*it).second.push_back(enum_var_assign);
				}
			}
			action_num++;
		}
	}
	for(PropVariable* prop_variable : intermediate_form->getPropVariables())
	{
		std::string prop_variable_name = validateIdentifier(
			prop_variable->getName());
		if(prop_variable->isNonDeterministic())
		{
			addLine(variable_assigns, "init(" + prop_variable_name
				+ "):= {TRUE, FALSE}", 2, true);
			addLine(variable_assigns, "next(" + prop_variable_name
				+ "):= {TRUE, FALSE}", 2, true);
			addLine(variable_assigns, "", 2, false);
		}
	}
	for(EnumVariable* enum_variable : intermediate_form->getEnumVariables())
	{
		std::string enum_variable_name = validateIdentifier(
			enum_variable->getName());
		if(enum_variable->isNonDeterministic())
		{
			std::string enum_values = "";
			std::list<std::string> values = enum_variable->getValues();
			auto it = values.begin();
			auto end = values.end();
			while(it != end)
			{
				enum_values += *it;
				if(++it != end)
				{
					enum_values += ", ";
				}
			}
			addLine(variable_assigns, "init(" + enum_variable_name
				+ "):= {" + enum_values + "}", 2, true);
			addLine(variable_assigns, "next(" + enum_variable_name
				+ "):= {" + enum_values + "}", 2, true);
			addLine(variable_assigns, "", 2, false);
		}
	}
	for(auto pair : prop_variable_assignment_map)
	{
		std::string prop_var_name = validateIdentifier(pair.first);
		std::string prop_var_assign = "";
		auto it = initial_variable_values.find(prop_var_name);
		if(it != initial_variable_values.end())
		{
			addLine(prop_var_assign, "init(" + prop_var_name
				+ "):= " + ((*it).second == "true" ? "TRUE" : "FALSE"),
				2, true);
		}
		else
		{
			addLine(prop_var_assign, "init(" + prop_var_name
				+ "):= FALSE", 2, true);
		}
		addLine(prop_var_assign, "next(" + prop_var_name
			+ "):=", 2, false);
		addLine(prop_var_assign, "case", 3, false);
		for(std::string s : pair.second)
		{
			addLine(prop_var_assign, s, 4, true);
		}
		addLine(prop_var_assign, "", 4, false);
		addLine(prop_var_assign, "TRUE: " + prop_var_name, 4, true);
		addLine(prop_var_assign, "esac", 3, true);
		addLine(variable_assigns, prop_var_assign, 0, false);
	}
	for(auto pair : enum_variable_assignment_map)
	{
		EnumVariable* enum_var = intermediate_form->getEnumVariableByName(
			pair.first, "");
		std::string enum_var_name = validateIdentifier(pair.first);
		std::string enum_var_assign = "";
		if(enum_var->resets())
		{
			addLine(enum_var_assign, "init(" + enum_var_name
				+ "):= none", 2, true);
		}
		else
		{
			auto it = initial_variable_values.find(enum_var_name);
			if(it != initial_variable_values.end())
			{
				addLine(enum_var_assign, "init(" + enum_var_name
					+ "):= " + (*it).second, 2, true);
			}
			else
			{
				std::string value_list = "";
				std::list<std::string> values = enum_var->getValues();
				auto it = values.begin();
				auto end = values.end();
				while(it != end)
				{
					value_list += validateIdentifier(*it);
					if(++it != end)
					{
						value_list += ", ";
					}
				}
				addLine(enum_var_assign, "init(" + enum_var_name
					+ "):= {" + value_list + "}", 2, true);
			}
		}
		addLine(enum_var_assign, "next(" + enum_var_name
			+ "):=", 2, false);
		addLine(enum_var_assign, "case", 3, false);
		for(std::string s : pair.second)
		{
			addLine(enum_var_assign, s, 4, true);
		}
		addLine(enum_var_assign, "", 4, false);
		if(enum_var->resets())
		{
			addLine(enum_var_assign, "TRUE: none", 4, true);
		}
		else
		{
			addLine(enum_var_assign, "TRUE: " + enum_var_name, 4, true);
		}
		addLine(enum_var_assign, "esac", 3, true);
		addLine(variable_assigns, enum_var_assign, 0, false);
	}
	return variable_assigns;
}

/*------------------------------------------------------------------------------
	validateIdentifier

	Checks the given identifier to ensure that it contains no characters
	that are illegal in NuSMV. Illegal characters are replaced with
	INVALID_CHAR_REPLACEMENT_STRING defined in
	nusmv_definitions.h. The validated identifier is then returned.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::validateIdentifier(
	std::string identifier)
{
	if(!identifier.empty())
	{
		int index = 0;
		if(!isValidStartCharacter(identifier.at(index++)))
		{
			identifier.replace(0, 1, INVALID_CHAR_REPLACEMENT_STRING);
		}
		int length = identifier.size();
		while(index < length)
		{
			if(!isValidCharacter(identifier.at(index)))
			{
				identifier.replace(index, 1, INVALID_CHAR_REPLACEMENT_STRING);
			}
			index++;
		}
	}
	return identifier;
}

/*------------------------------------------------------------------------------
	isValidStartCharacter

	Given a character, returns true if this character is a legal starting
	character for an identifier in NuSMV.
------------------------------------------------------------------------------*/
const bool NuSMVTranslator::isValidStartCharacter(const char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| c == '_';
}

/*------------------------------------------------------------------------------
	isValidCharacter

	Given a character, returns true if this character is a legal character
	for an identifier in NuSMV.
------------------------------------------------------------------------------*/
const bool NuSMVTranslator::isValidCharacter(const char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9') || c == '_' || c == '\\'
		|| c == '$' || c == '#' /*|| c == '-'*/;
}

/*------------------------------------------------------------------------------
	addLine

	Adds a new line to the string s, prefixed with num_tabs tabs, and
	possibly terminating with a semi-colon.
------------------------------------------------------------------------------*/
void NuSMVTranslator::addLine(std::string& s, const std::string& line,
	const int num_tabs, const bool terminating_semi_colon)
{
	for(int i = 0; i < num_tabs; i++)
	{
		s += TAB;
	}
	s += line;
	if(terminating_semi_colon)
	{
		s += ";";
	}
	s += "\n";
}

/*------------------------------------------------------------------------------
	timeConstraintHoldsAtTime

	Returns true if the giving time constraint holds at the given time.
------------------------------------------------------------------------------*/
const bool NuSMVTranslator::timeConstraintHoldsAtTime(
	PreconditionTimingConstraint* p, const std::string& time)
{
	return timeIsBefore(time, momentAfter(p->getEndTime()))
		&& timeIsAfter(time, momentBefore(p->getStartTime()));
}

/*------------------------------------------------------------------------------
	momentAfter

	Given a time as a string, returns a string corresponding the next minute
	in time. For instance, given the string 11:23:00, this function would
	return the string 11:24:00.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::momentAfter(const std::string& time)
{
	int time_hours = stringToInteger(time.substr(0, 2));
	int time_minutes = stringToInteger(time.substr(3, 2));
	if(++time_minutes == 60)
	{
		time_minutes = 0;
		time_hours++;
	}
	std::string time_hours_string = (time_hours < 10 ? "0"
		+ integerToString(time_hours) : integerToString(time_hours));
	std::string time_minutes_string = (time_minutes < 10 ? "0"
		+ integerToString(time_minutes) : integerToString(time_minutes));
	return time_hours_string + ":" + time_minutes_string + ":00";
}

/*------------------------------------------------------------------------------
	momentBefore

	Given a time as a string, returns a string corresponding the previous
	minute in time. For instance, given the string 11:23:00, this function
	would return the string 11:22:00.
------------------------------------------------------------------------------*/
const std::string NuSMVTranslator::momentBefore(const std::string& time)
{
	int time_hours = stringToInteger(time.substr(0, 2));
	int time_minutes = stringToInteger(time.substr(3, 2));
	if(--time_minutes == -1)
	{
		time_minutes = 59;
		time_hours--;
	}
	std::string time_hours_string = ((time_hours < 10 && time_hours >= 0) ? "0"
		+ integerToString(time_hours) : integerToString(time_hours));
	std::string time_minutes_string = (time_minutes < 10 ? "0"
		+ integerToString(time_minutes) : integerToString(time_minutes));
	return time_hours_string + ":" + time_minutes_string + ":00";
}

/*------------------------------------------------------------------------------
	timeIsAfter

	Returns true if time_1 is later than time_2.
------------------------------------------------------------------------------*/
const bool NuSMVTranslator::timeIsAfter(const std::string& time_1,
	const std::string& time_2)
{
	int time_1_hours = stringToInteger(time_1.substr(0, 2));
	int time_1_minutes = stringToInteger(time_1.substr(3, 2));
	int time_2_hours = stringToInteger(time_2.substr(0, 2));
	int time_2_minutes = stringToInteger(time_2.substr(3, 2));
	if(time_1_hours == time_2_hours)
	{
		return time_1_minutes > time_2_minutes;
	}
	else
	{
		return time_1_hours > time_2_hours;
	}
}

/*------------------------------------------------------------------------------
	timeIsBefore

	Returns true if time_1 is earlier than time_2.
------------------------------------------------------------------------------*/
const bool NuSMVTranslator::timeIsBefore(const std::string& time_1,
	const std::string& time_2)
{
	int time_1_hours = stringToInteger(time_1.substr(0, 2));
	int time_1_minutes = stringToInteger(time_1.substr(3, 2));
	int time_2_hours = stringToInteger(time_2.substr(0, 2));
	int time_2_minutes = stringToInteger(time_2.substr(3, 2));
	if(time_1_hours == time_2_hours)
	{
		return time_1_minutes < time_2_minutes;
	}
	else
	{
		return time_1_hours < time_2_hours;
	}
}

/*------------------------------------------------------------------------------
	vectorPowerSet

	Given a vector v of type T, returns a vector of vectors of type T
	corresponding to the power set of the vector v.
------------------------------------------------------------------------------*/
template<typename T>
std::vector<std::vector<T>> NuSMVTranslator::vectorPowerSet(std::vector<T>& v,
	const bool sort_increasing_size)
{
	std::vector<std::vector<T>> power_set;
	int num_items = v.size();
	int num_subsets = static_cast<int>(std::pow(2.0, num_items));
	for(int characteristic_vector = 0; characteristic_vector < num_subsets;
		characteristic_vector++)
	{
		power_set.push_back(filterVectorByCharacteristicVector(v,
			characteristic_vector));
	}

	if(sort_increasing_size)
	{
		std::sort(power_set.begin(), power_set.end(),
			[&](std::vector<T> this_v, std::vector<T> that_v)
			{
				return that_v.size() < this_v.size();
			});
	}
	else
	{
		std::sort(power_set.begin(), power_set.end(),
			[&](std::vector<T> this_v, std::vector<T> that_v)
			{
				return this_v.size() > that_v.size();
			});
	}

	return power_set;
}

/*------------------------------------------------------------------------------
	filterVectorByCharacteristicVector

	Returns a vector containing the elements of the vector v, filtered by
	the given characteristic vector.
------------------------------------------------------------------------------*/
template<typename T>
std::vector<T> NuSMVTranslator::filterVectorByCharacteristicVector(
	std::vector<T>& v, int characteristic_vector)
{
	std::vector<T> subset;
	int index = 0;
	while(characteristic_vector)
	{
		if(characteristic_vector & 1)
		{
			subset.push_back(v[index]);
		}
		index++;
		characteristic_vector >>= 1;
	}
	return subset;
}
