// Ryzom - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.




#include "stdpch.h"
// net
#include "nel/net/message.h"
// misc
#include "nel/misc/bit_mem_stream.h"
//
#include "server_share/msg_ai_service.h"
//
#include "special_power_mod_defense.h"
#include "phrase_manager/phrase_utilities_functions.h"
#include "special_power_phrase.h"
#include "player_manager/player_manager.h"
#include "player_manager/character.h"
#include "player_manager/player.h"
#include "mod_defense_effect.h"


using namespace std;
using namespace NLMISC;
using namespace NLNET;

extern CPlayerManager PlayerManager;


//--------------------------------------------------------------
//					apply()  
//--------------------------------------------------------------
void CSpecialPowerModDefense::apply()
{
	if (!_Phrase)
		return;

	CCharacter *actor = PlayerManager.getChar(_ActorRowId);
	if (!actor)
	{
		nlwarning("<CSpecialPowerModDefense::apply> Cannot find actor entity or not a player");
		return;
	}

	// disable power
	actor->forbidPower(_PowerType, _Phrase->getConsumableFamilyId(), CTickEventHandler::getGameCycle() + _DisablePowerTime);

	const TGameCycle endDate = _Duration + CTickEventHandler::getGameCycle();

	EFFECT_FAMILIES::TEffectFamily effectFamily;
	switch( _PowerType )
	{
		case POWERS::ModDodgeSkill:
			effectFamily = EFFECT_FAMILIES::PowerModDodgeSkill;
			break;
		case POWERS::ModParrySkill:
			effectFamily = EFFECT_FAMILIES::PowerModParrySkill;
			break;
		case POWERS::ModDefenseSkill:
			effectFamily = EFFECT_FAMILIES::PowerModDefenseSkill;
			break;
		default:
			break;
	}
	
	// create effect and apply it on target
	CModDefenseEffect *effect = new CModDefenseEffect(_ActorRowId, 
													endDate, 
													effectFamily,
													_DefenseMode,
													_Modifier1,
													_Modifier2);
	if (!effect)
	{
		nlwarning("<CSpecialPowerModDefense::apply> Failed to allocate new CModDefenseEffect");
		return;
	}

	actor->addSabrinaEffect(effect);

	// send messages
	SM_STATIC_PARAMS_1(params, STRING_MANAGER::power_type);
//	TVectorParamCheck params;
	// for actor
	if (actor->getId().getType() == RYZOMID::player)
	{
		params[0].Enum = _PowerType;
		PHRASE_UTILITIES::sendDynamicSystemMessage(_ActorRowId, "POWER_USE", params);
	}
} // apply //
