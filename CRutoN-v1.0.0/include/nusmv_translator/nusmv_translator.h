/*==============================================================================
	NuSMVTranslator

	Translates an IntermediateForm into input for the model checker NuSMV.

	File			: nusmv_translator.h
	Author			: Paul Gainer
	Created			: 09/02/2015
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
#ifndef NUSMV_TRANSLATOR_H_
#define NUSMV_TRANSLATOR_H_

#include <string>
#include <list>
#include <map>

#include "intermediate/precondition_timing_constraint.h"
#include "nusmv_translator/nusmv_definitions.h"

class NuSMVTranslator
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	/*--------------------------------------------------------------------------
		Constructor
	............................................................................
		@param	inter			the intermediate form representation
	--------------------------------------------------------------------------*/
	NuSMVTranslator(IntermediateForm* inter) : intermediate_form(inter),
		max_num_actions(0) {}

	~NuSMVTranslator();

	/*===========================================================================
		Public Functions
	===========================================================================*/
	/*--------------------------------------------------------------------------
		translate

		Translates the intermediate form into NuSMV input, which is returned as
		a string.
	............................................................................
		@return					the NuSMV input as a string, or an empty string
								if there was an error
	--------------------------------------------------------------------------*/
	const std::string translate();

	/*--------------------------------------------------------------------------
		translate

		Translates the intermediate form into NuSMV input, which is returned as
		a string. Initial values for variables can be specified in the initial
		value file.
	............................................................................
		@param	initial_value_file	the file containing initial values for
									variables
		@return						the NuSMV input as a string, or an empty
									string if there was an error
	--------------------------------------------------------------------------*/
	const std::string translate(std::string& initial_value_file);

private:
	/*==========================================================================
		Private Static Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The definition of the behaviour module.
	--------------------------------------------------------------------------*/
	static const std::string MODULE_BEHAVIOUR;

	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The intermediate form.
	--------------------------------------------------------------------------*/
	IntermediateForm* intermediate_form;

	/*--------------------------------------------------------------------------
		Used to record the maximum number of actions found in a behaviour.
	--------------------------------------------------------------------------*/
	unsigned int max_num_actions;

	/*--------------------------------------------------------------------------
		A list of all behaviours that are schedulable.
	--------------------------------------------------------------------------*/
	std::vector<Behaviour*> schedulable_behaviours;

	/*--------------------------------------------------------------------------
		A list of all behaviours that execute another behaviour.
	--------------------------------------------------------------------------*/
	std::list<Behaviour*> executing_behaviours;

	/*--------------------------------------------------------------------------
		A list of all behaviours that are executed by another behaviour.
	--------------------------------------------------------------------------*/
	std::set<Behaviour*> executable_behaviours;

	/*--------------------------------------------------------------------------
		An exhaustive list of different time intervals in a day, where for every
		time interval the list of preconditions that hold during that interval
		is not equal to the list of preconditions that hold for any other
		time interval.
	--------------------------------------------------------------------------*/
	std::list<std::string> time_intervals;

	/*--------------------------------------------------------------------------
		Maps propositional variable names to lists of statements defining the
		values of the variable in the next state, according to which behaviour
		is scheduled and which action is being undertaken.
	--------------------------------------------------------------------------*/
	std::map<std::string, std::list<std::string>> prop_variable_assignment_map;

	/*--------------------------------------------------------------------------
		Maps enumerated variable names to lists of statements defining the
		values of the variable in the next state, according to which behaviour
		is scheduled and which action is being undertaken.
	--------------------------------------------------------------------------*/
	std::map<std::string, std::list<std::string>> enum_variable_assignment_map;

	/*--------------------------------------------------------------------------
		Maps preconditions with a been_in_state suffix to a list of values used
		to construct the enumerated variable used to record the duration
		throughout which the precondition has held.
	--------------------------------------------------------------------------*/
	std::map<Precondition*, std::list<std::string>> been_in_state_map;

	/*--------------------------------------------------------------------------
		Maps preconditions with a was_in_state suffix to a list of values used
		to construct the enumerated variable used to record whether the
		the precondition has held within a previous duration of time.
	--------------------------------------------------------------------------*/
	std::map<Precondition*, std::list<std::string>> was_in_state_map;

	/*--------------------------------------------------------------------------
		Maps timing constraint preconditions to lists of values, where each
		value in a list corresponds to the time intervals during which the
		time constraint holds.
	--------------------------------------------------------------------------*/
	std::map<PreconditionTimingConstraint*, std::list<std::string>*>
		timing_constraint_map;

	/*--------------------------------------------------------------------------
		Maps variable names to their initial values, if an intial value file
		was specified.
	--------------------------------------------------------------------------*/
	std::map<std::string, std::string> initial_variable_values;

	/*==========================================================================
		Private Static Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		validateIdentifier

		Checks the given identifier to ensure that it contains no characters
		that are illegal in NuSMV. Illegal characters are replaced with
		INVALID_CHAR_REPLACEMENT_STRING defined in
		nusmv_definitions.h. The validated identifier is then returned.
	............................................................................
		@param	identifier		the unvalidated indentifier
		@return					the validated identifier
	--------------------------------------------------------------------------*/
	static const std::string validateIdentifier(std::string identifier);

	/*--------------------------------------------------------------------------
		isValidStartCharacter

		Given a character, returns true if this character is a legal starting
		character for an identifier in NuSMV.
	............................................................................
		@param	c				the character
		@return					true if the character is a legal starting
								character for an identifier in NuSMV, or false
								otherwise
	--------------------------------------------------------------------------*/
	static const bool isValidStartCharacter(const char c);

	/*--------------------------------------------------------------------------
		isValidCharacter

		Given a character, returns true if this character is a legal character
		for an identifier in NuSMV.
	............................................................................
		@param	c				the character
		@return					true if the character is a legal character for
								an identifier in NuSMV, or false otherwise
	--------------------------------------------------------------------------*/
	static const bool isValidCharacter(const char c);

	/*--------------------------------------------------------------------------
		addLine

		Adds a new line to the string s, prefixed with num_tabs tabs, and
		possibly terminating with a semi-colon.
	............................................................................
		@param	s				the string to which the line is added
		@param	line			the line to add to the string s
		@param	num_tabs		the number of tabs to insert before the line
		@param	terminating_semi_colon	true if the line should be terminated
								with a semi-colon
	--------------------------------------------------------------------------*/
	static void addLine(std::string& s, const std::string& line,
		const int num_tabs, const bool terminating_semi_colon);

	/*--------------------------------------------------------------------------
		momentAfter

		Given a time as a string, returns a string corresponding the next minute
		in time. For instance, given the string 11:23:00, this function would
		return the string 11:24:00.
	............................................................................
		@param	time			a string corresponding to a time with format
								HH:MM:SS
		@return					a string corresponding to the time one minute
								after time
	--------------------------------------------------------------------------*/
	static const std::string momentAfter(const std::string& time);

	/*--------------------------------------------------------------------------
		momentBefore

		Given a time as a string, returns a string corresponding the previous
		minute in time. For instance, given the string 11:23:00, this function
		would return the string 11:22:00.
	............................................................................
		@param	time			a string corresponding to a time with format
								HH:MM:SS
		@return					a string corresponding to the time one minute
								before time
	--------------------------------------------------------------------------*/
	static const std::string momentBefore(const std::string& time);

	/*--------------------------------------------------------------------------
		timeIsAfter

		Returns true if time_1 is later than time_2.
	............................................................................
		@param	time_2			a string corresponding to a time with format
								HH:MM:SS
		@param	time_2			a string corresponding to a time with format
								HH:MM:SS
		@return					true if time_1 is after time_2
	--------------------------------------------------------------------------*/
	static const bool timeIsAfter(const std::string& time_1,
			const std::string& time_2);

	/*--------------------------------------------------------------------------
		timeIsBefore

		Returns true if time_1 is earlier than time_2.
	............................................................................
		@param	time_2			a string corresponding to a time with format
								HH:MM:SS
		@param	time_2			a string corresponding to a time with format
								HH:MM:SS
		@return					true if time_1 is before time_2
	--------------------------------------------------------------------------*/
	static const bool timeIsBefore(const std::string& time_1,
		const std::string& time_2);

	/*--------------------------------------------------------------------------
		timeConstraintHoldsAtTime

		Returns true if the giving time constraint holds at the given time.
	............................................................................
		@param	p				a pointer to a timing constraint precondition
		@param	time			a string corresponding to a time with format
								HH:MM:SS
		@return					true if the precondition holds at the given
								time, or false otherwise
	--------------------------------------------------------------------------*/
	static const bool timeConstraintHoldsAtTime(PreconditionTimingConstraint* p,
		const std::string& time);

	/*--------------------------------------------------------------------------
		vectorPowerSet

		Given a vector v of type T, returns a vector of vectors of type T
		corresponding to the power set of the vector v.
	............................................................................
		@param	T				the type of elements in the vector
		@param	v				a vector of elements of type T
		@param	sort_increasing_size	true if the powerset of vectors should
								be sorted into increasing size, or false if the
								set should be sorted into decreasing size.
		@return					the powerset of v as a vector of vectors of type
								T
	--------------------------------------------------------------------------*/
	template<typename T>
	static std::vector<std::vector<T>> vectorPowerSet(
		std::vector<T>& v, const bool sort_increasing_size);

	/*--------------------------------------------------------------------------
		filterVectorByCharacteristicVector

		Returns a vector containing the elements of the vector v, filtered by
		the given characteristic vector.
	............................................................................
		@param	T				the type of elements in the vector
		@param	v				a vector of elements of type T
		@param	characteristic_vector	a characteristic vector, as an integer
								from 0 to (|v|^2)-1, determining which elements
								of v should be included in the returned vector
		@return					the vector v filtered by the characteristic
								vector
	--------------------------------------------------------------------------*/
	template<typename T>
	static std::vector<T> filterVectorByCharacteristicVector(
		std::vector<T>& v, int characteristic_vector);

	/*==========================================================================
		Private Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		buildBehaviourLists

		Builds the lists of schedulable, executing, and executable behaviours.
	--------------------------------------------------------------------------*/
	void buildBehaviourLists();

	/*--------------------------------------------------------------------------
		buildTimingConstraintMap

		Builds the timing constraint map.
	--------------------------------------------------------------------------*/
	void buildTimingConstraintMap();

	/*--------------------------------------------------------------------------
		buildInitialVariableValueMap

		Builds the initial variable value map.
	............................................................................
		@param	filename		the name of the file containing the initial
								variable values
		@return					true if the file was parsed successfully, or
								false otherwise
	--------------------------------------------------------------------------*/
	bool buildInitialVariableValueMap(std::string filename);

	/*--------------------------------------------------------------------------
		buildBeenInWasInStateMaps

		Builds the been in state and was in state maps.
	--------------------------------------------------------------------------*/
	void buildBeenInWasInStateMaps();

	/*--------------------------------------------------------------------------
		buildEnumsVar

		Builds the variable definitions for	the enumerated variables.
	............................................................................
		@return					the enumerated variable declarations
	--------------------------------------------------------------------------*/
	std::list<std::string> buildEnumsVar();

	/*--------------------------------------------------------------------------
		buildBoolsVar

		Builds the variable definitions for	the boolean variables.
	............................................................................
		@return					the boolean variable declarations
	--------------------------------------------------------------------------*/
	std::list<std::string> buildBoolsVar();

	/*--------------------------------------------------------------------------
		buildBeenInStatesVar

		Builds the the definitions for the enumerated variables used for
		been_in_state suffixed preconditions.
	............................................................................
		@return					the enumerated variable declarations for
								variables used for been_in_state suffixed
								preconditions
	--------------------------------------------------------------------------*/
	std::list<std::string> buildBeenInStatesVar();

	/*--------------------------------------------------------------------------
		buildWasInStatesVar

		Builds the the definitions for the enumerated variables used for
		was_in_state suffixed preconditions.
	............................................................................
		@return					the enumerated variable declarations for
								variables used for was_in_state suffixed
								preconditions
	--------------------------------------------------------------------------*/
	std::list<std::string> buildWasInStatesVar();

	/*--------------------------------------------------------------------------
		buildEnumVar

		Builds the the definition for a given enumerated variable.
	............................................................................
		@param					the name of the enumerated variable
		@return					the enumerated variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildEnumVar(const std::string name,
		std::list<std::string> values);

	/*--------------------------------------------------------------------------
		buildBoolumVar

		Builds the the definition for a given boolean variable.
	............................................................................
		@param					the name of the bool variable
		@return					the boolean variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildBoolVar(const std::string name);

	/*--------------------------------------------------------------------------
		buildTimeVar

		Builds the variable definition for the time variable which indicates
		the time of day.
	............................................................................
		@return					the time variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildTimeVar();

	/*--------------------------------------------------------------------------
		buildStepVar

		Builds the variable definition for the step variable, this records
		which action is currently being performed by a behaviour.
	............................................................................
		@return					the steo variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildStepVar();

	/*--------------------------------------------------------------------------
		buildScheduleVar

		Builds the variable definition for the schedule variable, this records
		which behaviour is currently scheduled for execution.
	............................................................................
		@return					the schedule variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildScheduleVar();

	/*--------------------------------------------------------------------------
		buildLastScheduleVar

		Builds the variable definition for the last_schedule variable, this
		is used to 'remember' any previous behaviour that may have executed the
		current behaviour.
	............................................................................
		@return					the last_schedule variable declaration
	--------------------------------------------------------------------------*/
	const std::string buildLastScheduleVar();

	/*--------------------------------------------------------------------------
		buildPreconditionDefines

		For all behaviours, builds an expression that evaluates to true if all
		of the behaviour's preconditions hold.
	............................................................................
		@return					the list of precondition definitions
	--------------------------------------------------------------------------*/
	std::list<std::string> buildPreconditionDefines();

	/*--------------------------------------------------------------------------
		buildPreconditionDefine

		Given the root node of a logical expression tree, recursively builds an
		expression that evaluates to true if all of the tree's preconditions hold.
	............................................................................
		@param	node			the root of the logical expression tree
		@return					the precondition definition
	--------------------------------------------------------------------------*/
	const std::string buildPreconditionDefine(const TreeNode* node);

	/*--------------------------------------------------------------------------
		buildCanInterruptDefines

		For all behaviours, builds an expression that evaluates to true when
		that behaviour can interrupt a currently scheduled behaviour in the next
		moment in time.
	............................................................................
		@return					the list of can_interrupt definitions
	--------------------------------------------------------------------------*/
	const std::list<std::string> buildCanInterruptDefines();

	/*--------------------------------------------------------------------------
		buildABehaviourCanBeScheduledDefine

		Builds the a_behaviour_can_be_scheduled definition, which evaluates to
		true if there is a behaviour that can be scheduled for execution in the
		next moment in time.
	............................................................................
		@return					the a_behaviour_can_be_executed definition
	--------------------------------------------------------------------------*/
	const std::string buildABehaviourCanBeScheduledDefine();

	/*--------------------------------------------------------------------------
		buildABehaviourIsEndingDefine

		Builds the a_behaviour_is_ending definition, which evaluates to
		true if the currently scheduled behaviour is executing its last action.
	............................................................................
		@return					the a_behaviour_is_ending definition
	--------------------------------------------------------------------------*/
	const std::string buildABehaviourIsEndingDefine();

	/*--------------------------------------------------------------------------
		buildAnExecutedBehaviourIsEndingDefine

		Builds the an_executed_behaviour_is_ending definition, which evaluates
		to true if the currently scheduled behaviour is executing its last
		action, and this scheduled behaviour was executed by another behaviour.
	............................................................................
		@return					the an_executed_behaviour_is_ending definition
	--------------------------------------------------------------------------*/
	const std::string buildAnExecutedBehaviourIsEndingDefine();

	/*--------------------------------------------------------------------------
		buildAnExecutedBehaviourIsEndingAsALastActionDefine

		Builds the an_executed_behaviour_is_ending_as_a_last_action definition,
		which evaluates	to true if the currently scheduled behaviour is
		executing its last action, and this scheduled behaviour was executed by
		another behaviour, and the action that executed this behaviour was the
		last action in the ordered list of actions in the executing behaviour.
	............................................................................
		@return					the an_executed_behaviour_is_ending_as_a_last
		 	 	 	 	 	 	 _action definition
	--------------------------------------------------------------------------*/
	const std::string buildAnExecutedBehaviourIsEndingAsALastActionDefine();

	/*--------------------------------------------------------------------------
		buildAnExecutedBehaviourIsScheduledDefine

		Builds the an_executed_behaviour_is_scheduled definition, which
		evaluates to true if the currently scheduled behaviour was executed by
		another behaviour.
	............................................................................
		@return					the an_executed_behaviour_is_executing
								definition
	--------------------------------------------------------------------------*/
	const std::string buildAnExecutedBehaviourIsScheduledDefine();

	/*--------------------------------------------------------------------------
		buildBehaviourModuleInstances

		Builds the instances of the	behaviour modules. One instance is created
		for each behaviour in the intermediate form.
	............................................................................
		@return					the list of behaviour module instance
								definitions
	--------------------------------------------------------------------------*/
	std::list<std::string> buildBehaviourModuleInstances();

	/*--------------------------------------------------------------------------
		buildBehaviourModuleInstance

		Builds an instance of a behaviour module for the given behaviour.
	............................................................................
		@param	behaviour		the behaviour for which a behaviour module
								instance definition should be built
		@return					the behaviour module instance definition
	--------------------------------------------------------------------------*/
	const std::string buildBehaviourModuleInstance(Behaviour* behaviour);

	/*--------------------------------------------------------------------------
		buildExecutedBehaviourExecuteNextDefine

		Builds the executed_behaviour_execute_next definition, which is true
		if an executed behaviour will be scheduled for execution in the next
		moment in time,
	............................................................................
		@return					the executed_behaviour_execute_next definitions
	--------------------------------------------------------------------------*/
	const std::string buildExecutedBehaviourExecuteNextDefine();

	/*--------------------------------------------------------------------------
		buildBeenInStateAssigns

		Builds the assignments for the enumerated variables used for
		been_in_state suffixed preconditions.
	............................................................................
		@return					the assignments for enumerated variables used
								for been_in_state suffixed preconditions
	--------------------------------------------------------------------------*/
	const std::string buildBeenInStateAssigns();

	/*--------------------------------------------------------------------------
		buildWasInStateAssigns

		Builds the assignments for the enumerated variables used for
		was_in_state suffixed preconditions.
	............................................................................
		@return					the assignments for enumerated variables used
								for was_in_state suffixed preconditions
	--------------------------------------------------------------------------*/
	const std::string buildWasInStateAssigns();

	/*--------------------------------------------------------------------------
		buildTimeAssign

		Builds the assignments for the time variable.
	............................................................................
		@return					the assignments for the time variable
	--------------------------------------------------------------------------*/
	const std::string buildTimeAssign();

	/*--------------------------------------------------------------------------
		buildStepAssign

		Builds the assignments for the step variable.
	............................................................................
		@return					the assignments for the step variable
	--------------------------------------------------------------------------*/
	const std::string buildStepAssign();

	/*--------------------------------------------------------------------------
		buildLastScheduleAssign

		Builds the assignments for the last_schedule variable.
	............................................................................
		@return					the assignments for the last_schedule variable
	--------------------------------------------------------------------------*/
	const std::string buildLastScheduleAssign();

	/*--------------------------------------------------------------------------
		buildScheduleAssign

		Builds the assignments for the schedule variable.
	............................................................................
		@return					the assignments for the schedule variable
	--------------------------------------------------------------------------*/
	const std::string buildScheduleAssign();

	/*--------------------------------------------------------------------------
		buildVariableAssigns

		Builds the assignments for the boolean and enumerated variables.
	............................................................................
		@return					the assignments for the boolean and enumerated
								variables
	--------------------------------------------------------------------------*/
	const std::string buildVariableAssigns();
};

#endif
