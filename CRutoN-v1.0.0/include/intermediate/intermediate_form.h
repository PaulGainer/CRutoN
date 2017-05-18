/*==============================================================================
	IntermediateForm

	A set of robot behaviours with corresponding propositional and enumerated
	variables.

	File			: intermediate_form.h
	Author			: Paul Gainer
	Created			: 04/01/2015
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
#ifndef INTERMEDIATE_FORM_H_
#define INTERMEDIATE_FORM_H_

#define DISAMBIGUATION_REPLACE_EXISTING 0
#define DISAMBIGUATION_USE_EXISTING 1
#define DISAMBIGUATION_IGNORE 2

#define NO_PRECONDITIONS_IGNORE 0
#define NO_PRECONDITIONS_REMOVE 1
#define NO_PRECONDITIONS_SET_NON_SCHEDULABLE 2

#include <list>
#include <ostream>
#include <string>

#include "define.h"
#include "global.h"
#include "utility.h"
#include "intermediate/enum_variable.h"
#include "intermediate/prop_variable.h"

class Behaviour;

class IntermediateForm
{
public:
	/*==========================================================================
		Public Constructors/Deconstructor
	==========================================================================*/
	IntermediateForm()  {}

	~IntermediateForm();

	/*==========================================================================
		Public Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		getBehaviours

		Accessor for behaviours.
	............................................................................
		@return					the list of behaviours
	--------------------------------------------------------------------------*/
	std::list<Behaviour*> getBehaviours() {return behaviours;}

	/*--------------------------------------------------------------------------
		getPropVariables

		Accessor for prop_variables.
	............................................................................
		@return					the list of propositional variables
	--------------------------------------------------------------------------*/
	std::list<PropVariable*> getPropVariables() {return prop_variables;}

	/*--------------------------------------------------------------------------
		getEnumVariables

		Accessor for enum_variables.
	............................................................................
		@return					the list of enumerated variables
	--------------------------------------------------------------------------*/
	std::list<EnumVariable*> getEnumVariables() {return enum_variables;}

	/*--------------------------------------------------------------------------
		setPropVariables

		Mutator for prop_variables.
	............................................................................
		@param	p				the new list of propositional variables
	--------------------------------------------------------------------------*/
	void setPropVariables(std::list<PropVariable*> p) {prop_variables = p;}

	/*--------------------------------------------------------------------------
		setEnumVariables

		Mutator for enum_variables.
	............................................................................
		@param	e				the new list of enumerated variables
	--------------------------------------------------------------------------*/
	void setEnumVariables(std::list<EnumVariable*> e) {enum_variables = e;}

	/*--------------------------------------------------------------------------
		getPropVariableByName

		Returns a propositional variable if one exists with the given name, or
		nullptr otherwise.
	............................................................................
		@param	name			the name of the propositional variable
		@param	info			some information to display to the user
		@return					a pointer to the propositional variable
	--------------------------------------------------------------------------*/
	PropVariable* getPropVariableByName(const std::string& name,
		const std::string& info);

	/*--------------------------------------------------------------------------
		getEnumVariableByName

		Returns an enumerated variable if one exists with the given name, or
		nullptr otherwise.
	............................................................................
		@param	name			the name of the enumerated variable
		@param	info			some information to display to the user
		@return					a pointer to the enumerated variable
	--------------------------------------------------------------------------*/
	EnumVariable* getEnumVariableByName(const std::string& name,
		const std::string& info);

	/*--------------------------------------------------------------------------
		getBehaviourByName

		Returns a behaviour if one exists with the the given name, or nullptr
		otherwise.
	............................................................................
		@param	name			the name of the behaviour
		@param	info			some information to display to the user
		@return					a pointer to the behaviour
	--------------------------------------------------------------------------*/
	Behaviour* getBehaviourByName(const std::string& name,
		const std::string& info);

	/*--------------------------------------------------------------------------
		addBehaviour

		Adds a new behaviour.
	............................................................................
		@param	b				the behaviour to add to the list
	--------------------------------------------------------------------------*/
	void addBehaviour(Behaviour* b) {behaviours.push_back(b);}

	/*--------------------------------------------------------------------------
		addPropVariable

		Adds a new propositional variable.
	............................................................................
		@param	p				the propositional variable to add to the list
	--------------------------------------------------------------------------*/
	void addPropVariable(PropVariable* p) {prop_variables.push_back(p);}

	/*--------------------------------------------------------------------------
		addEnumVariable

		Adds a new enumerated variable.
	............................................................................
		@param	e				the enumerated variable to add to the list
	--------------------------------------------------------------------------*/
	void addEnumVariable(EnumVariable* e) {enum_variables.push_back(e);}

	/*--------------------------------------------------------------------------
		validateBehaviours

		Firstly, ensures that all behaviour executions reference existing
		behaviours. Secondly, prompts the user to decide how to interpret
		behaviours with no preconditions.
	............................................................................
		@throws					an error message if validation failed
	--------------------------------------------------------------------------*/
	void validateBehaviours() throw (std::string);

	/*--------------------------------------------------------------------------
		flattenNestedBehaviourExecutions

		'Flattens' nested behaviour executions by substituting actions of
		executed behaviours into executing behaviour.  This process repeats
		until all nested executions have been flattened.

		Any behaviour executed by a non-interruptible behaviour automatically
		has its actions substituted into the executing behaviour.

		The actions of a non-interruptible behaviour executed by an
		interruptible behaviour cannot be substituted into the executing
		behaviour.

		NOTE 1: non-deterministic behaviour executions cannot be substituted
		into any executing behaviour, regardless of whether it is interruptible
		or not.
	--------------------------------------------------------------------------*/
	void flattenNestedBehaviourExecutions();

	friend std::ostream& operator<<(std::ostream& os,
		const IntermediateForm& a);

private:
	/*==========================================================================
		Private Member Variables
	==========================================================================*/
	/*--------------------------------------------------------------------------
		The set of behaviours.
	--------------------------------------------------------------------------*/
	std::list<Behaviour*> behaviours;

	/*--------------------------------------------------------------------------
		The set of propositional variables.
	--------------------------------------------------------------------------*/
	std::list<PropVariable*> prop_variables;

	/*--------------------------------------------------------------------------
		The set of enumerated variables.
	--------------------------------------------------------------------------*/
	std::list<EnumVariable*> enum_variables;


	/*==========================================================================
		Private Static Member Functions
	==========================================================================*/
	/*--------------------------------------------------------------------------
		disambiguationPrompt

		Prompts the user to disambiguate the name of a given precondition,
		action or behaviour.
	............................................................................
		@param	existing_name	the name of the existing propositional variable
								/enumerated variable/behaviour
		@param	type			text describing the type being disambiguated
								(either propositional variable, enumerated
								variable, or behaviour)
		@return					one of DISAMBIGUATION_REPLACE_EXISTING,
								DISAMBIGUATION_USE_EXISTING, or
								DISAMBIGUATION_IGNORE
	--------------------------------------------------------------------------*/
	static const int disambiguationPrompt(const std::string& existing_name,
		const std::string& type);

	/*--------------------------------------------------------------------------
		noPreconditionPrompt

		Prompts the user to choose how to interpret a behaviour with no
		preconditions.
	............................................................................
		@param	behaviour_name	the name of the behaviour having no
								preconditions
		@return					one of NO_PRECONDITIONS_IGNORE,
								NO_PRECONDITIONS_REMOVE,
								or NO_PRECONDITIONS_SET_NON_SCHEDULABLE
	--------------------------------------------------------------------------*/
	static const int noPreconditionPrompt(const std::string& behaviour_name);

	/*--------------------------------------------------------------------------
		getByName

		Finds and returns a precondition, action or behaviour with the given
		name. If automatic identifier matching is disabled then the user is
		prompted when ambiguity or case-insensitive matches are encountered.
	............................................................................
		@param	variable		the list of propositional variables/enumerated
								variables/behaviour
		@param	name			the name of the propositional variable/
								enumerated variable/behaviour to retrieve
		@param	type			text describing the type being disambiguated
								(either propositional variable, enumerated
								variable, or behaviour)
		@param	info			some information to display to the user
		@return					the propositional variable/enumerated variable/
								behaviour, or nullptr if none is found
	--------------------------------------------------------------------------*/
	template<typename T>
	static T* getByName(std::list<T*>& variables, const std::string& name,
		const std::string& type, const std::string& info);
};

#endif
