#include <windows.h>
#include <time.h>
#include <string.h>
#include "devices/Network.h"


namespace Recogbot {

	const char* Network::CONF_XML = "NRLAB02.xml";

	bool Network::Enable(){

		clock_t beginTime;
		bool state;

		state = NWrite_BootEnableNetwork((byte *)Network::CONF_XML, strlen(CONF_XML));
		beginTime = clock();

		while(clock() - beginTime < CLOCKS_PER_SEC || state != READY){
			state = NWrite_BootEnableNetwork((byte *)Network::CONF_XML, strlen(CONF_XML));
		}
		return state;
	}
	
	void Network::Disable(){

		NWrite_DisableNetwork();
	}
	
	bool Network::IsConnected(){

		if(NRead_NetworkState() == READY) return true;
		else return false;
	}
	
	Network::STATE Network::GetNetworkState(){

		int state = NRead_NetworkState();

		if(state == READY) return READY;
		else if(state == SAFE_OP) return SAFE_OP;
		else if(state == PRE_OP) return PRE_OP;
		else return NET_INIT;
	}
}