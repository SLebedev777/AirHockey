#include "AIPlayer.h"
#include "AIIdleState.h"
#include "AIAttackState.h"
#include "AIDefenseState.h"

namespace airhockey
{
	AIPlayer::AIPlayer(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr enemy_paddle, cocos2d::Sprite* puck, const AIPlayerSettings& settings):
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_enemyPaddle(enemy_paddle),
		m_puck(puck),
		m_settings(settings)
	{}

	IFSMStatePtr AIPlayer::createIdleState()
	{
		return std::make_unique<AIIdleState>(m_field, m_aiPaddle, m_enemyPaddle, m_puck, m_settings.attackRadiusFunc, m_settings.pyramid);
	}

	IFSMStatePtr AIPlayer::createDefenseState()
	{
		return std::make_unique<AIDefenseState>(m_field, m_aiPaddle, m_enemyPaddle, m_puck, m_settings.attackRadiusFunc, m_settings.pyramid);
	}

	IFSMStatePtr AIPlayer::createAttackState()
	{
		return std::make_unique<AIAttackState>(m_field, m_aiPaddle, m_enemyPaddle, m_puck, m_settings.attackRadiusFunc);
	}

}