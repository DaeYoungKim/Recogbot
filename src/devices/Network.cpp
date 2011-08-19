#include <windows.h>
#include <time.h>
#include <string.h>
#include "devices/Network.h"

namespace Recogbot {

	const char* Network::CONF_XML = "NRLAB02.xml";
	bool Network::isConnection = false;

	bool Network::Enable(){

		clock_t beginTime;
		
		if( NWrite_BootEnableNetwork((byte *)CONF_XML, strlen(CONF_XML)) ){
			beginTime = clock();

			while(clock() - beginTime < CLOCKS_PER_SEC){
				if( NRead_NetworkState() == READY ){
					isConnection = true;
					break;
				}
			}
		}
		return isConnection;
	}
	
	void Network::Disable(){

		NWrite_DisableNetwork();
		isConnection = false;
	}
	
	bool Network::IsConnected(){

		return isConnection;
	}
	
	Network::STATE Network::GetNetworkState(){

		STATE state = (STATE) NRead_NetworkState();
		return state;
	}
}