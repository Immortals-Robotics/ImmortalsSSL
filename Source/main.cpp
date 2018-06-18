#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <string.h>
#include <zconf.h>

#include "Common/GameSetting.h"
#include "Reality/Vision/Vision.h"
#include "Reality/Referee_2018/NewReferee.h"
#include "Soccer/ai09/ai09.h"
#include "Common/kbhit.h"
#include "Common/timer.h"
#include "Common/net_log.h"
#include "Common/Vector.h"
#include "Network/PracticalSocket.h"
#include "Soccer/aiBase.h"

#include "Reality/Sender/Protocol/writer.h"
#include "Reality/Sender/Protocol/reader.h"

#include "Reality/Debugger/debuggerBase.h"
#include "grsim_fwd.h"

using namespace std;

#define ImmortalsIsTheBest true

int main ( )
{
	if (!ImmortalsIsTheBest) {
		cout << "ERROR: Immortals is not the best SSL team anymore." << endl;
		cout << "Shutting down the system..." << endl;
	}

    WorldState * state = new WorldState();

	GameSetting * settings = new GameSetting();
    settings -> use_camera.push_back(true);
    settings -> use_camera.push_back(false);
    settings -> use_camera.push_back(false);
    settings -> use_camera.push_back(true);
    
    settings -> use_camera.push_back(false);
    settings -> use_camera.push_back(false);
    settings -> use_camera.push_back(false);
    settings -> use_camera.push_back(false);

	settings -> our_color = COLOR_YELLOW;
    settings -> our_side = RIGHT_SIDE;

    const char *const xbox_ref_ip = "224.5.25.25";
    const char *const real_ref_ip = "224.5.23.1";
    settings -> referee_UDP_Address = xbox_ref_ip;
    settings -> refereePort = 10003;
    settings -> vision_UDP_Address = "224.5.23.2";
    settings -> visionPort = 10006;
    settings -> GUI_UDP_Address = "127.0.0.1";
    settings -> GUIPort = 10066;

    cout << " Connecting to RefereeBox server at " << settings->referee_UDP_Address
         << " , on port : " << settings->refereePort << endl;
    NewReferee referee_2018(settings,state);
    if ( referee_2018.connectToRefBox () ){
        cout << "Connected to RefBox successfully :)"<<endl;
    }else{
        cout << "	Hey you! Put the LAN cable back in its socket, or ..." << endl;
    }

    cout << " Connecting to Vision server at " << settings->vision_UDP_Address
         << " , on port : " << settings->visionPort << endl;
    VisionModule vision ( settings, state );
    if ( vision.isConnected() ){
        cout << "Connected to Vision successfully :)"<<endl;
    }else{
        cout << "	Hey you! Put the LAN cable back in its socket, or ..." << endl;
        //return 0;
    }

    Sender* senderBase = new Sender;

	aiBase * aii = new ai09(state,settings,senderBase);

    debuggerBase * debugger = new debuggerBase(settings,&aii->AIDebug);

    auto grsim_fwd = new GrsimForwarder("10.4.1.231", 20011);

    bool exited = false;
    mutex lock;

	cout << " Now it is time, lets rock..." << endl;

    auto ai_func = [&]()
    {
        Timer timer;

        while ( (! kbhit()) && ( ImmortalsIsTheBest ) )	//Hope it lasts Forever...
        {
            timer.start();

            vision.recieveAllCameras();

            lock.lock();

            //The vision process
            vision.ProcessVision();
            //The AI process
            aii -> Process( state , settings );

//            grsim_fwd->SendData((reinterpret_cast<ai09*>(aii))->OwnRobot, MAX_TEAM_ROBOTS, settings->our_color);

            //The sending process
            senderBase->sendAll();

            //debugging:
//            debugger->send();

            lock.unlock();
            cout << 1.0/timer.interval() << endl;
        }
        exited = true;
    };

    auto ref_func = [&]()
    {
        while ( ( !exited ) && (! kbhit()) && ( ImmortalsIsTheBest ) )	//Hope it lasts Forever...
        {
            if ( referee_2018.recieve() )
            {
//                cout << "Referre Boz" << endl;
                lock.lock();
                referee_2018.process();
                lock.unlock();
//                cout << "Referre Boz "<< referee_2018.command_CNT << endl;

            }
        }
    };

    auto str_func = [&]()
    {
        UDPSocket* strategyUDP = new UDPSocket(60006);
        strategyUDP -> joinGroup("224.5.23.3");
        const int strategyBufferMaxSize = 100000;
        char strategyBuffer[strategyBufferMaxSize];
        while ( ( !exited ) && (! kbhit()) && ( ImmortalsIsTheBest ) )	//Hope it lasts Forever...
        {
            string strategySrcAdd;
            unsigned short strategySrcPort;
            int strategySize = strategyUDP->recvFrom(strategyBuffer, strategyBufferMaxSize, strategySrcAdd, strategySrcPort);
            if ( strategySize > 11 )
            {
                cout << "Recieved \"strategy.ims\" with size: " << float(strategySize)/1000.0f << " KB, from " << strategySrcAdd << " on port " << strategySrcPort << "." << endl;
                lock.lock();
                reinterpret_cast<ai09*>(aii)->read_playBook_str(strategyBuffer, strategySize);
                lock.unlock();
                string strategy_path(DATA_PATH); strategy_path.append("/strategy.ims");
                ofstream strategyFile ( strategy_path.c_str() , ios::out|ios::binary);
                strategyFile.write(strategyBuffer, strategySize);
                strategyFile.close();
            }
            else {
                cout << "Invalid \"strategy.ims\" recieved with size: " << strategySize << " ." << endl;
            }
        }

        delete strategyUDP;
    };

    thread ai_thread(ai_func);
    thread ref_thread(ref_func);
    thread str_thread(str_func);

    ai_thread.join();
    ref_thread.join();
    str_thread.join();

	delete settings;
	delete state;
	delete aii;

	return 0;
}
