#include "../ai09.h"
#include <iostream>

#include <vector>

void ai09::Stop ( void )
{
	for (std::map<int*, int>::const_iterator i = markMap.begin(); i != markMap.end(); ++i) {
		markMap[i->first] = -1;
	}

	debugDraw = true;
	AddDebugCircle(ball.Position,500,Red);
	debugDraw = false;

	if (OwnRobot[attack].State.OutForSubsitute)
	{
		if (!OwnRobot[mid1].State.OutForSubsitute)
		{
			std::swap(attack, mid1);
		}
		else if (!OwnRobot[mid2].State.OutForSubsitute)
		{
			std::swap(attack, mid2);
		}
	}
	if ( side * OwnRobot[mid1].State.Position.Y < side * OwnRobot[mid2].State.Position.Y )//Nice :D
	{
		std::swap(mid1, mid2);
	}

	GKHi( gk, true );
	//DefHi(def,NULL, true);
	DefMid(def, rw, lw, NULL, true);
	
	ERRTSetObstacles ( dmf , true , true , true , true );
	OwnRobot[dmf].face(ball.Position);
	ERRTNavigate2Point ( dmf , PointOnConnectingLine(ball.Position, Vec2(side*field_width, 0), DIS(ball.Position, Vec2(side*field_width, 0))/3.0f) ,0 , 100,&VELOCITY_PROFILE_AROOM);
	OwnRobot[dmf].Shoot(0);

	ERRTSetObstacles ( lmf , true , true , true , true );
	OwnRobot[lmf].face(Vec2(ball.Position.X,ball.Position.Y));
	ERRTNavigate2Point ( lmf , CircleAroundPoint(Vec2(ball.Position.X,ball.Position.Y),NormalizeAngle(-20+AngleWith(ball.Position , Vec2(side*field_width,0))),650) ,0 , 100,&VELOCITY_PROFILE_AROOM);
	OwnRobot[lmf].Shoot(0);

	ERRTSetObstacles ( rmf , true , true , true , true );
	OwnRobot[rmf].face(Vec2(ball.Position.X,ball.Position.Y));
	ERRTNavigate2Point ( rmf , CircleAroundPoint(Vec2(ball.Position.X,ball.Position.Y),NormalizeAngle(20+AngleWith(ball.Position , Vec2(side*field_width,0))),650) ,0 , 100,&VELOCITY_PROFILE_AROOM);
	OwnRobot[rmf].Shoot(0);

	ERRTSetObstacles ( cmf , true , true , true , true );
	OwnRobot[cmf].face(Vec2(ball.Position.X,ball.Position.Y));
	ERRTNavigate2Point ( cmf , CircleAroundPoint(Vec2(ball.Position.X,ball.Position.Y),NormalizeAngle(AngleWith(ball.Position , Vec2(side*field_width,0))),650) ,0 , 100,&VELOCITY_PROFILE_AROOM);
	OwnRobot[cmf].Shoot(0);
}