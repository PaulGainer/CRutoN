/*==============================================================================
	NuSMV definitions

	File			: nusmv_definitions.h
	Author			: Paul Gainer
	Created			: 10/02/2015
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
#ifndef NUSMV_DEFINITIONS_H_
#define NUSMV_DEFINITIONS_H_

#define TAB "    "

#define H_LINE "--------------------------------------------------------------------------------"

// behaviour definitions
#define MODULE_B "behaviour"
#define B_PRECONDITIONS "preconditions"
#define B_CAN_INTERRUPT "can_interrupt"
#define B_CAN_BE_INT "can_be_int"
#define B_CAN_BE_INTERRUPTED "can_be_interrupted"
#define B_THIS_SCHEDULE "this_schedule"
#define B_LAST_STEP "last_step"
#define B_PRECONDITIONS_HOLD "preconditions_hold"
#define B_CAN_BE_SCHEDULED "can_be_scheduled"
#define B_IS_LAST_STEP "is_last_step"
#define B_IS_SCHEDULED "is_scheduled"

#define SCHEDULE "schedule"
#define LAST_SCHEDULE "last_schedule"
#define STEP "step"
#define TIME "time"
#define PRECONDITION_PREFIX "pre_"
#define BEEN_IN_WAS_IN_PREFIX "s_"
#define BEEN_IN_WAS_IN_FINAL "final"
#define BEEN_IN_WAS_IN_START "0"
#define BEEN_IN_STATE "BEEN_IN_STATE"
#define WAS_IN_STATE "WAS_IN_STATE"
#define EXECUTED_BEHAVIOUR_EXECUTE_NEXT "executed_behaviour_execute_next"
#define CAN_INTERRUPT_PREFIX "can_interrupt_"
#define A_BEHAVIOUR_CAN_BE_SCHEDULED "a_behaviour_can_be_scheduled"
#define A_BEHAVIOUR_IS_ENDING "a_behaviour_is_ending"
#define AN_EXECUTED_BEHAVIOUR_IS_SCHEDULED "an_executed_behaviour_is_scheduled"
#define AN_EXECUTED_BEHAVIOUR_IS_ENDING "an_executed_behaviour_is_ending"
#define AN_EXECUTED_BEHAVIOUR_IS_ENDING_AS_A_LAST_ACTION "an_executed_behaviour_is_ending_as_a_last_action"
#define BEHAVIOUR_PREFIX "b_"

#define INVALID_CHAR_REPLACEMENT_STRING "_"

#endif
