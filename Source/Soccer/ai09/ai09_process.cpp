#include "ai09.h"

void ai09::Process ( WorldState * worldState , GameSetting * setting , char * commands )
{
    static int PRCS_CNT = 0;
	AIDebug.Clear();



    PRCS_CNT++;
    AIDebug.set_frame_id(PRCS_CNT);

	debugDraw = false;
	
	internalProcessData(worldState, setting);
	
	debugDraw = true;
	AddDebugCircle(ball.Position,40,Red);
//	AddDebugLine(ball.Position,Vec2(ball.velocity.x,ball.velocity.y) + ball.Position, Black);
    worldState->ball.Xpos_t->calculate();
    worldState->ball.Ypos_t->calculate();
    if(worldState->ball.Ypos_t->is_valid() && worldState->ball.Xpos_t->is_valid()) {
//        AddDebugCircle(Vec2(worldState->ball.Xpos_t->give_Y_by_X(worldState->ball.t_capture + 0.5),
//                            worldState->ball.Ypos_t->give_Y_by_X(worldState->ball.t_capture + 0.5)), 40, Blue);
        TVec2 spd_v= Vec2(worldState->ball.Xpos_t->give_derivedY_by_X(worldState->ball.t_capture + 0.5), worldState->ball.Ypos_t->give_derivedY_by_X(worldState->ball.t_capture + 0.5));
        AddDebugLine(ball.Position,spd_v + ball.Position, Black);

    }
//    AddDebugLine(ball.Position,Vec2(0,0),Cyan);
//    AddDebugRect(ball.Position,ball.Position.X,ball.Position.Y,Orange);
//    AddDebugPoint(ball.Position + Vec2(0,200),White);
	debugDraw = false;


	if ( 1 ) {
		currentPlay = "my_test";
	}
	else if ( REF_playState )
	{
		if ( lastReferee != REF_playState->get() )
		{
			timer.start();
			lastReferee = REF_playState->get();
			randomParam = random.get();
			target_str = strategy_weight();
			FUNC_state = 0;
			FUNC_CNT = 0;
		}
		
		if ( REF_playState->get() == GameState::GAME_OFF )
		{
			oppRestarted = false;
			if (side*ball.Position.X>3000) {
				currentPlay = "Stop_def";
			}
			else {
				currentPlay = "Stop";
			}
		}
		else if ( REF_playState->get() == GameState::GAME_ON )
		{
			// Nik Uncomment
			
			//currentPlay = "strategy_maker";
			//currentPlayParam = playBook->strategy_size()-1;

			
			currentPlay = "NewNormalPlay";
			//currentPlay = "tech_mexico";
			//currentPlay = "tech_motion_ann";
			//if ( timer.time() > 30.0 || ball.seenState == CompletelyOut )
			//	currentPlay = "sharifcup_post_play";
//            targetBallPlacement->X = -3700;
//            targetBallPlacement->Y = -2500;
//            currentPlay = "our_place_ball";
//            cout<<"IT's NORMALLLLLLLL"<<endl;
		}
		else if ( REF_playState->ourKickoff ( ) )
		{
			currentPlay = "kickoff_us_chip";
			//currentPlay = "kickoff_us_farar";
			currentPlay = "kickoff_us_pass";
			
			currentPlayParam = worldState ->refereeState -> State->canKickBall();
		}
		else if ( ( REF_playState->ourDirectKick ( ) ) || ( REF_playState->ourIndirectKick ( ) ) )
		{
            if(1)
            {
                //currentPlay="corner_simple_pass";
                currentPlay="kickoff_us_zamini";
				currentPlayParam = REF_playState->canKickBall();
            }
            else {
                if (target_str != -1) {
                    currentPlay = "strategy_maker";
                    currentPlayParam = target_str;
                } else {
                    //if (side*ball.Position.X<800)
                    //    currentPlay = "throwin_us_outgir";
                    //else
                    currentPlay = "throwin_chip_shoot";
                }
            }
		}
		
		
		else if ( REF_playState->ourPenaltyKick ( ) )
		{
			currentPlay = "penalty_us_ghuz";
			currentPlayParam = worldState ->refereeState -> State->canKickBall();
		}
		
		else if ( REF_playState->theirFreeKick() )
		{
			//currentPlay = "corner_their_mrl";
			currentPlay = "corner_their_global";
			//currentPlay = "Stop";
		}
		
		else if ( REF_playState->theirKickoff() )
		{
			currentPlay = "kickoff_their_one_wall";
		}
		
		else if ( REF_playState->theirPenaltyKick() )
		{
			currentPlay = "penalty_their_simple";
		}
		else if( REF_playState->get() == GameState::HALTED )
		{
			currentPlay = "HaltAll";
		}
		else
		{
			currentPlay = "Stop";
		}
		
		if ( REF_playState->theirRestart() )
		{
			oppRestarted = true;
		}
	}
	else
	{
        currentPlay = "Stop";
		//currentPlay = "HaltAll";
	}
	
	
	if ( AIPlayBook.find(currentPlay) != AIPlayBook.end() )
		(this->*AIPlayBook[currentPlay])();
	else {
        HaltAll();
        //cout<<"Oh NO!!!"<<endl;
    }
	
	
	for ( int i = 0 ; i < 6 ; i ++ )
	{
		if ( ( OwnRobot[i].State.seenState == CompletelyOut ) || ( !navigated[i] ) )
		{
			Halt ( i );
		}
	}

	internalFinalize(worldState, setting, commands);
}