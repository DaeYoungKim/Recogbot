/*
   Any Test !! OKOK~!!
*/
#include <cstdio>
#include <Windows.h>
#include <devices/Psd.h>
#include <devices/Wheel.h>
#include "devices/DeviceSwig.h"
#include "devices/Network.h"

void main(void) {
	//Recogbot::PSD psd;
	//Recogbot::Wheel wheel;
	//Recogbot::DeviceSwig devSwig;

	if(Recogbot::Network::Enable()) printf("success loading config file!\n");
	else printf("%s \n", "faild to load...\n");
	
	printf("state : %d\n", Recogbot::Network::GetNetworkState());
	
	Recogbot::Network::Disable();
	printf("isConnection(0 or 1) : %d\n", Recogbot::Network::IsConnected());
}