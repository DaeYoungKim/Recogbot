#include "devices/PSDSensor.h"
#include "devices/Network.h"
#include "windows.h"
namespace Recogbot {
	PSDSensor::PSDSensor()
	{
		isCondition = false;
		if(Network::isConnected() == false)
		{
			Network::enable();
			isCondition = true;
			printf("PSD생성중\n");
			SWrite_EnablePsd(true);
			Sleep(1000);
		}
		
	}

	bool PSDSensor::getPsdData()
	{
		if(isCondition)
		{
			printf("getPsdData 실행중 \n");
			SRead_PsdData(&psdData);
			return true;
		}
		else
		{
			printf("연결안됨 \n");
			return false;
		}
	}
		
	PSDSensor::~PSDSensor()
	{
		SWrite_EnablePsd(false);
	}
}
