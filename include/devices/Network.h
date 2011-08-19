/*
	CNU, IIMIL
	Yong-Cheol Lee

	Created in June, 2011 by Yong-Cheol Lee (budlbaram@nate.com)
*/

#ifndef RECOGBOT_DEVICES_NETWORK_H
#define RECOGBOT_DEVICES_NETWORK_H

#include "NRLAB02Defv01.h"

namespace Recogbot {
	class Network {
	private:
		static bool isConnection;       // check to connection
		static const char* CONF_XML; // path of configure file
	public:
		Network();
		~Network();

		static enum STATE {NET_INIT = 1, PRE_OP = 2, SAFE_OP = 4, READY = 8};  // 1 : initing... 2,3 : possible - input  4 : possible - input & output
						    
		static bool Enable();    
		static void Disable();
		static bool IsConnected();
		static STATE GetNetworkState();
	};
}

#endif