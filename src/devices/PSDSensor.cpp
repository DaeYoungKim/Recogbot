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
			printf("PSD������\n");
			SWrite_EnablePsd(true);
			Sleep(1000);
		}
		
	}

	bool PSDSensor::getPsdData()
	{
		if(isCondition)
		{
			printf("getPsdData ������ \n");
			SRead_PsdData(&psdData);
			return true;
		}
		else
		{
			printf("����ȵ� \n");
			return false;
		}
	}
		
	PSDSensor::~PSDSensor()
	{
		SWrite_EnablePsd(false);
	}
}
