#include <iostream>
#include <thread>
#include <functional>
#include <rstep_client.h>

RSTEPClient* cli = new RSTEPClient("192.168.1.12", 22250);

void msgRecvCallback()
{
	std::cout
		<< "period: " << cli->getLastUpdatePeriodMicroseconds() << " | "
		<< "num: " << cli->getMessageNum() << " | "
		<< "long: " << cli->getLongPosition() << " | "
		<< "ang: " << cli->getAngPosition()
		<< std::endl;
}


int main()
{
	std::function<void()> cb = msgRecvCallback;
	cli->setVerbose(true);
	cli->setMessageReceivedCallback(cb);
	cli->start();
	while (true);
	return 0;
}
