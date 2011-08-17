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
	Recogbot::PSD psd;
	Recogbot::Wheel wheel;
	Recogbot::DeviceSwig devSwig;

	Recogbot::Network::Enable();

	printf("state : %d", Recogbot::Network::GetNetworkState());
	Sleep(500);
	printf("state : %d", Recogbot::Network::GetNetworkState());
	
	Recogbot::Network::Disable();
}