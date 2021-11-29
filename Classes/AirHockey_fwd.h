#ifndef __AIRHOCKEY_FWD_H__
#define __AIRHOCKEY_FWD_H__

namespace airhockey
{
	class IFSMState;
	typedef std::unique_ptr<IFSMState> IFSMStatePtr;

	class FSMContext;
	typedef std::shared_ptr<FSMContext> FSMContextPtr;

	class GameField;
	typedef std::unique_ptr<GameField> GameFieldPtr;

	class Paddle;
	typedef std::shared_ptr<Paddle> PaddlePtr;
}

#endif

