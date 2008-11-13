//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


// RBdefinitions.h:
//
//////////////////////////////////////////////////////////////////////

#pragma once

// Module name
#define INPUTGRETA_MODULE_NAME ".InputGreta"
#define INPUTWATSON_MODULE_NAME "InputWatson"
#define REALTIMEFMLPSYDULE_MODULE_NAME ".RealtimeFMLPsydule"
#define REALTIMEBMLPSYDULE_MODULE_NAME ".RealtimeBMLPsydule"
#define REALTIMEPLAYERPSYDULE_MODULE_NAME ".RealtimePlayerPsydule"

// Greta Message Header in the Psyclone module.
#define PSYCLONE_CLOCK_MESSAGE_HEADER ".Data.Clock"
#define PSYCLONE_SYSTEM_FEEDBACK_MESSAGE_HEADER ".System.Feedback"
#define PSYCLONE_AGENTSTATE_MESSAGE_HEADER ".Data.AgentState"
#define PSYCLONE_UPDATEAGENTSTATE_MESSAGE_HEADER ".Data.UpdateAgentState"
#define PSYCLONE_COGNITIVE_FML_MESSAGE_HEADER ".Data.CognitiveFML"
#define PSYCLONE_REALTIME_PLAYER_COMMAND ".RealtimePlayer.Command"
#define PSYCLONE_DATA_WAV ".Data.WAV"
#define PSYCLONE_FAP_FRAME ".Data.FAPFrame"
#define PSYCLONE_BAP_FRAME ".Data.BAPFrame"
#define PSYCLONE_COMPLEX_INPUT ".Data.Complex.Input"
#define PSYCLONE_COMPLEX_OUTPUT ".Data.Complex.Output"

// Header for the input messages on DataInput.Whiteboard
#define PSYCLONE_INPUT_MESSAGE_HEADER "Greta.Data.Input"


// Greta Whiteboard
#define PSYCLONE_GRETA_WHITEBOARD ".Whiteboard"
// Input Witeboard
#define PSYCLONE_INPUT_WHITEBOARD "DataInput.Whiteboard"

// Greta Message Header to send
#define PSYCLONE_FML_MESSAGE_HEADER ".Data.FML"
#define PSYCLONE_BML_MESSAGE_HEADER ".Data.BML"

#define PSYCLONE_REACTIVE_FML_MESSAGE_HEADER ".Data.ReactiveFML"
#define PSYCLONE_MIMICRY_BML_MESSAGE_HEADER ".Data.MimicryBML"

// Log messages
#define LOG_MESSAGE_PSYCLONE_CONNEXION_OK " registered to Psyclone"
#define LOG_MESSAGE_PSYCLONE_CONNEXION_FAILED " could not register to Psyclone"
