#include "devices/PSDSensor.h"
#include "devices/Network.h"
namespace Recogbot {
	PSDSensor::PSDSensor()
	{
		isCondition = false;
		if(Network::isConnected() == false)
		{
			Network::enable();
			isCondition = true;
			printf("PSD생성중\n");
		}
		SWrite_EnablePsd(true);
	}

	bool PSDSensor::getPsdData()
	{
		
		if(isCondition)
		{
			printf("getPsdData 실행중 \n");
			SRead_PsdData(&psddata);
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
