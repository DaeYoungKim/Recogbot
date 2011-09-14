#include <cstdio>
#include "devices/Network.h"
using namespace Recogbot;

void main(void) {
	
	if(Network::enable()) printf("success loading config file!\n");
	else printf("%s \n", "faild to load...\n");
	
	printf("state : %d\n", Network::getNetworkState());
	
	Network::disable();
	printf("isConnection(0 or 1) : %d\n", Network::isConnected());
}