/*==============================================================================
	IntermediateForm

	File			: intermediate_form.cpp
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
#include <algorithm>
#include <iostream>
#include <locale>

#include "global.h"
#include "utility.h"
#include "intermediate/action.h"
#include "intermediate/action_execute.h"
#include "intermediate/action_execute_non_deterministic.h"
#include "intermediate/behaviour.h"
#include "intermediate/intermediate_form.h"

IntermediateForm::~IntermediateForm()
{
	for(PropVariable* p : prop_variables)
	{
		delete p;
	}
	for(EnumVariable* e : enum_variables)
	{
		delete e;
	}
	for(Behaviour* b : behaviours)
	{
		delete b;
	}
}

/*------------------------------------------------------------------------------
	getPropVariableByName

	Returns a propositional variable if one exists with the given name, or
	nullptr otherwise.
------------------------------------------------------------------------------*/
PropVariable* IntermediateForm::getPropVariableByName(const std::string& name,
		const std::string& info)
{
	return getByName<PropVariable>(prop_variables, name,
		"propositional variable", info);
}

/*------------------------------------------------------------------------------
	getEnumVariableByName

	Returns an enumerated variable if one exists with the given name, or nullptr
	otherwise.
------------------------------------------------------------------------------*/
EnumVariable* IntermediateForm::getEnumVariableByName(const std::string& name,
	const std::string& info)
{
	return getByName<EnumVariable>(enum_variables, name,
		"enumerated variable", info);
}

/*------------------------------------------------------------------------------
	getBehaviourByName

	Returns a behaviour if one exists with the the given name, or nullptr
	otherwise.
------------------------------------------------------------------------------*/
Behaviour* IntermediateForm::getBehaviourByName(const std::string& name,
	const std::string& info)
{
	return getByName<Behaviour>(behaviours, name,
		"behaviour", info);
}

/*------------------------------------------------------------------------------
	disambiguationPrompt

	Prompts the user to disambiguate the name of a given precondition,
	action or behaviour.
------------------------------------------------------------------------------*/
const int IntermediateForm::disambiguationPrompt(
	const std::string& existing_name, const std::string& type)
{
	std::cout << "existing " << type << " \'" << existing_name << "\'\n"
		<< "\tr: replace existing " << type << " name\n"
		<< "\tu: use existing " << type << " name\n"
		<< "\ti: ignore match\n";
	std::string input = "";
	while(input != "r" && input != "u" && input != "i"
		&& input != "R" && input != "U" && input != "I")
	{
		std::cout << "\tenter selection: ";
		std::cin >> input;
		std::cin.clear();
	}
	if(input == "r" || input == "R")
	{
		return DISAMBIGUATION_REPLACE_EXISTING;
	}
	else if(input == "u" || input == "U")
	{
		return DISAMBIGUATION_USE_EXISTING;
	}
	else
	{
		return DISAMBIGUATION_IGNORE;
	}
}

/*--------------------------------------------------------------------------
		noPreconditionPrompt

		Prompts the user to choose how to interpret a behaviour with no
		preconditions.
------------------------------------------------------------------------------*/
const int IntermediateForm::noPreconditionPrompt(
	const std::string& behaviour_name)
{
	std::cout << "schedulable behaviour \'" << behaviour_name
		<< "\' has no preconditions\n"
		<< "\ti: ignore and continue\n"
		<< "\tr: remove behaviour\n"
		<< "\tf: flag behaviour as non-schedulable\n";
	std::string input = "";
	while(input != "i" && input != "r" && input != "f"
		&& input != "I" && input != "R" && input != "F")
	{
		std::cout << "\tenter selection: ";
		std::cin >> input;
		std::cin.clear();
	}
	if(input == "i" || input == "I")
	{
		return NO_PRECONDITIONS_IGNORE;
	}
	else if(input == "r" || input == "R")
	{
		return NO_PRECONDITIONS_REMOVE;
	}
	else
	{
		return NO_PRECONDITIONS_SET_NON_SCHEDULABLE;
	}
}

/*------------------------------------------------------------------------------
		getByName

		Finds and returns a precondition, action or behaviour with the given
		name. If automatic identifier matching is disabled then the user is
		prompted when ambiguity or case-insensitive matches are encountered.
------------------------------------------------------------------------------*/
template<typename T>
T* IntermediateForm::getByName(std::list<T*>& instances, const std::string& name,
	const std::string& type, const std::string& info)
{
	T* case_insensitive_match = nullptr;
	std::list<std::pair<T*, const int>> similarity_matches;
	std::for_each(instances.begin(), instances.end(),
		[&](T* variable)
		{
			if(caseInsensitiveCompare(variable->getName(), name))
			{
				// a case-insensitive match has been found
				case_insensitive_match = variable;
			}
			else
			{
				auto pair = compareStrings(variable->getName(), name);
				if(pair.second >= g_string_matching_threshhold)
				{
					// a match has been found
					similarity_matches.push_back(std::pair<T*,
						const int>(variable, pair.second));
				}
			}
		});

	if(case_insensitive_match != nullptr)
	{
		// a case insensitive match was found
		if(case_insensitive_match->getName() != name)
		{
			if(!g_no_prompt_case_insensitivity)
			{
				/* 	prompt the user to choose whether to replace the
					existing identifier, to use the existing identifier,
					or to simply ignore the match and treat both
					identifiers as being distinct */
				displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2, info);
				switch(disambiguationPrompt(case_insensitive_match->getName(),
					type))
				{
				case DISAMBIGUATION_REPLACE_EXISTING:
					case_insensitive_match->setName(name);
					return case_insensitive_match;
					break;

				case DISAMBIGUATION_USE_EXISTING:
					return case_insensitive_match;
					break;

				case DISAMBIGUATION_IGNORE:
					return nullptr;
					break;
				}
			}
			else
			{
				// automatically match
				displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2,
					"automatic case-insensitive match");
				std::cout << "matched " << type << " \'" << name
					<< "\'\nwith existing " << type << " \'"
					<<	case_insensitive_match->getName() << "\'\n";
				return case_insensitive_match;
			}
		}
		else
		{
			return case_insensitive_match;
		}
	}
	else
	{
		if(similarity_matches.empty())
		{
			// no matches were found
			return nullptr;
		}
		else
		{
			// sort the matches by similarity
			similarity_matches.sort(
				[&](const std::pair<T*, const int>& this_pair,
					const std::pair<T*, const int>& that_pair)
				{
					return this_pair.second > that_pair.second;
				});

			if(!g_no_prompt_identifier_matching)
			{
				/* 	prompt the user to choose whether to replace the
					existing identifier, to use the existing identifier,
					or to simply ignore the match and treat both
					identifiers as being distinct */
				displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2, info);
				auto it = similarity_matches.begin();
				auto end = similarity_matches.end();
				int count = 1;
				while(it != end)
				{
					std::pair<T*, const int> matched_pair = *it;
					std::cout << "match " << count << " of "
						<< similarity_matches.size() << " ("
						<< matched_pair.second << "% similarity)\n";
					switch(disambiguationPrompt(matched_pair.first->getName(),
						type))
					{
					case DISAMBIGUATION_REPLACE_EXISTING:
						matched_pair.first->setName(name);
						return matched_pair.first;
						break;

					case DISAMBIGUATION_USE_EXISTING:
						return matched_pair.first;
						break;

					case DISAMBIGUATION_IGNORE:
						break;
					}
					count++;
					it++;
				}
				return nullptr;
			}
			else
			{
				// automatically use the most similar match
				displayTitle(DIVIDER_DISAMBIGUATION, DIVIDER_2,
					"automatic similarity match");
				std::locale loc;
				auto first_pair = *similarity_matches.begin();
				std::string matched_name = first_pair.first->getName();
				int matched_name_spc = 0;
				for(char c : matched_name)
				{
					if(std::isspace(c, loc))
					{
						matched_name_spc++;
					}
				}
				int name_spc = 0;
				for(char c : name)
				{
					if(std::isspace(c, loc))
					{
						name_spc++;
					}
				}
				std::cout << "matched " << type << " \'" << name
					<< "\'\nwith existing " << type << " \'"
					<<	first_pair.first->getName()
					<< "\'\nhaving " << first_pair.second << "% similarity\n";
				if(name_spc < matched_name_spc)
				{
					std::cout << "new identifier contains less whitespace: "
						<< "replacing original identifier\n";
					first_pair.first->setName(name);
				}
				return first_pair.first;
			}
		}
	}
	return nullptr;
}

/*------------------------------------------------------------------------------
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
------------------------------------------------------------------------------*/
void IntermediateForm::flattenNestedBehaviourExecutions()
{
	for(Behaviour* behaviour : behaviours)
	{
		bool expanded;
		do
		{
			expanded = false;
			auto action_it = behaviour->actions.begin();
			auto action_end = behaviour->actions.end();
			while(action_it != action_end)
			{
				Action* action = *action_it;
				if(action->getActionType() == ActionType::EXECUTE)
				{
					ActionExecute* action_ex = (ActionExecute*)action;
					Behaviour* behaviour_ex = getBehaviourByName(
						action_ex->getBehaviour(), "");
					/*	if this behaviour is not interruptible, or if
						the executed behaviour is interruptible, then
						we can substitute the actions of the executed
						behaviour into this behaviour */
					if(!behaviour->isInterruptible()
						|| behaviour_ex->isInterruptible())
					{
						expanded = true;
						action_it = behaviour->actions.erase(action_it);
						std::list<Action*> actions_ex =
							behaviour_ex->getActions();
						int order = action_ex->getActionOrder();
						int num_actions = actions_ex.size();
						auto action_it_copy = action_it;
						while(action_it_copy != action_end)
						{
							Action* a = *action_it_copy;
							a->setActionOrder(a->getActionOrder()
								+ num_actions);
							action_it_copy++;
						}
						for(Action* a : actions_ex)
						{
							Action* copy = a->clone();
							copy->setActionOrder(order++);
							behaviour->addAction(copy);
						}
						delete action_ex;
					}
					else
					{
						action_it++;
					}
				}
				else
				{
					action_it++;
				}
			}
		} while (expanded);
	}
}

/*------------------------------------------------------------------------------
	validateBehaviours

	Firstly, ensures that all behaviour executions reference existing
	behaviours. Secondly, prompts the user to decide how to interpret
	behaviours with no preconditions.
------------------------------------------------------------------------------*/
void IntermediateForm::validateBehaviours() throw (std::string)
{
	auto b_it = behaviours.begin();
	auto b_end = behaviours.end();
	while(b_it != b_end)
	{
		Behaviour* b = *b_it;
		if(b->isSchedulable() && b->getPreconditions().empty())
		{
			displayTitle(DIVIDER_NO_PRECONDITIONS, DIVIDER_2,
				"a schedulable behaviour has no preconditions");
			int choice = noPreconditionPrompt(b->getName());
			switch(choice)
			{
			case NO_PRECONDITIONS_IGNORE:

				break;

			case NO_PRECONDITIONS_REMOVE:
				b_it = behaviours.erase(b_it);
				delete b;
				break;

			case NO_PRECONDITIONS_SET_NON_SCHEDULABLE:
				b->setSchedulable(false);
				break;
			}
		}
		b_it++;
	}
	b_it = behaviours.begin();
	b_end = behaviours.end();
	while(b_it != b_end)
	{
		Behaviour* b = *b_it;
		try
		{
			auto actions = b->getActions();
			auto it = actions.begin();
			auto end = actions.end();
			if(actions.empty())
			{
				throw(std::string("behaviour has no actions\n"));
			}
			while(it != end)
			{
				Action* a = *it;
				if(a->getActionType() == ActionType::EXECUTE)
				{
					ActionExecute* a_ex = (ActionExecute*)a;
					std::stringstream info_stream;
					info_stream	<< "disambiguation of executable behaviour name"
						<< "\nin action:\n" << *a_ex << "\nin behaviour:\n"
						<< b->getName();
					Behaviour* b = getBehaviourByName(a_ex->getBehaviour(),
						info_stream.str());
					if(b == nullptr)
					{
						std::stringstream stream;
						stream << "found no behaviour matching \'"
							<< a_ex->getBehaviour() << "\'\n"
							<< "executed by action: \n"
							<< *a_ex << std::endl;
						throw(stream.str());
					}
					a_ex->setBehaviour(b->getName());
				}
				else if(a->getActionType() == ActionType::EXECUTE_NON_D)
				{
					ActionExecuteNonDeterministic* a_ex_non_d =
						(ActionExecuteNonDeterministic*)a;
					std::list<std::string> behaviour_values =
						a_ex_non_d->getBehaviourValues();
					auto b_it = behaviour_values.begin();
					auto b_end = behaviour_values.end();
					while(b_it != b_end)
					{
						std::string behaviour = *b_it;
						std::stringstream info_stream;
						info_stream	<< "disambiguation of executable behaviour name \'"
							<< behaviour << "\' "
							<< "\n\nin action:\n" << *a_ex_non_d
							<< "\n\nin behaviour:\n" << b->getName();
						Behaviour* b = getBehaviourByName(behaviour,
							info_stream.str());
						if(b == nullptr)
						{
							std::stringstream stream;
							stream << "found no behaviour matching \'"
								<< behaviour << "\'\n"
								<< "executed by action: \n"
								<< *a_ex_non_d << std::endl;
							throw(stream.str());
						}
						a_ex_non_d->updateBehaviourName(behaviour,
							b->getName());
						b_it++;
					}
				}
				it++;
			}
			b_it++;
		}
		catch(std::string& error)
		{
			throw(std::string("[validateBehaviours]->\n")
				+ "error in behaviour \'" + b->getName() + "\'\n"
				+ error);
		}
	}
}

/*------------------------------------------------------------------------------
	<< operator overload
------------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& os,
	const IntermediateForm& a)
{
	os << DIVIDER_0 << "Propositional Variables\n" << DIVIDER_0;
	std::for_each(a.prop_variables.begin(), a.prop_variables.end(),
		[&](PropVariable* p)
		{
			os << *p << std::endl;
		});
	os << DIVIDER_0 << "Enumerated Type Variables\n" << DIVIDER_0;
	std::for_each(a.enum_variables.begin(), a.enum_variables.end(),
		[&](EnumVariable* e)
		{
			os << *e << std::endl;
		});
	os << DIVIDER_0 << "Behaviours\n" << DIVIDER_0;
	std::for_each(a.behaviours.begin(), a.behaviours.end(),
		[&](Behaviour* b)
		{
			os << *b << std::endl;
		});
	return os;
}
