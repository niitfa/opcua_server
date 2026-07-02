#include "tcp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <iostream>
#include <chrono>
#include <ctime>

TCPClient::TCPClient(std::string ip, uint16_t port, uint32_t rxBufferSize, uint32_t txBufferSize)
 	: ip { ip }, port { port }, rxBufferSize { rxBufferSize }, txBufferSize { txBufferSize }
{
	if( rxBufferSize )
	{
		this->rxBuffer = new uint8_t[rxBufferSize];
		this->rxBufferUser = new uint8_t[rxBufferSize];
	}

	if( txBufferSize )
	{
		this->txBuffer = new uint8_t[txBufferSize];
		this->txBufferUser = new uint8_t[rxBufferSize];
	}

	is_launched.store(false);
}

TCPClient::~TCPClient()
{
	delete[] this->rxBuffer;
	delete[] this->rxBufferUser;
	delete[] this->txBuffer;
	delete[] this->txBufferUser;
}

void TCPClient::start()
{
	if (!is_launched.load())
	{
		is_initialized.store(false);
		is_stop_forced.store(false);
		is_launched.store(true);
		std::thread thrd(&TCPClient::handler, this);
		if(!this->cpus.empty())
		{
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			for(auto cpu : this->cpus)
			{
				CPU_SET(cpu, &cpuset);
			}
            pthread_setaffinity_np(thrd.native_handle(), sizeof(cpuset), &cpuset);
		}
		thrd.detach();
		while (!is_initialized.load()) {}
	}
}

void TCPClient::stop()
{
	is_stop_forced.store(true);
	while(is_launched.load()) {}
}

void TCPClient::setCPUs(std::vector<int> cpus)
{
    this->cpus = cpus;
}

void TCPClient::setIP(std::string ip)
{
    this->ip = ip;
}

void TCPClient::setPort(uint16_t port)
{
    this->port = port;
}

void TCPClient::setMessageReceivedCallback(std::function<void ()>f)
{
    this->messageReceivedCallback = f;
}

void TCPClient::setVerbose(bool verbose)
{
	this->verbose = verbose;	
}

void TCPClient::copyFromRx(void* dest, uint32_t offset, uint32_t size)
{
	rxMutex.lock();
    if(offset < this->rxBufferSize)
	{
		memcpy(dest, this->rxBufferUser + offset, std::min(size, (this->rxBufferSize - offset)));
	}
	rxMutex.unlock();
}

void TCPClient::copyToTx(const void* src, uint32_t offset, uint32_t size)
{
	txMutex.lock();
    if(offset < this->txBufferSize)
	{
		memcpy(this->txBufferUser + offset, src, std::min(size, (this->txBufferSize - offset)));
	}
    txMutex.unlock();
}

bool TCPClient::isConnected()
{
	return connected.load();
}

uint32_t TCPClient::getLastUpdatePeriodMicroseconds()
{
	return updatePeriodMicroseconds.load();
}

void TCPClient::handler()
{
	init();
	is_initialized.store(true);
	while (!is_stop_forced.load())
	{
		loop();
	}
	deinit();
	is_launched.store(false);
}

void TCPClient::init()
{
	this->startDuration = std::chrono::system_clock::now();
}

void TCPClient::loop()
{
	switch(this->socketState)
	{
	case TCPSocketState::Opening:
		if (openSocket() != -1) 
		{
			setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &this->timeout, sizeof (timeout));
			setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &this->timeout, sizeof (timeout));
			this->socketState = TCPSocketState::Connecting;
		}
		break;
	case TCPSocketState::Connecting:
        if(connect(sockfd, reinterpret_cast<struct sockaddr*>(&this->serverAddress), sizeof(serverAddress)) != -1)
		{
			this->socketState = TCPSocketState::Established;
			if (verbose) 
			{
                std::cout << "TCP client: " << getCurrentTimeStr()
                    << " connection established " << this->ip << ":" << this->port << std::endl;
			}
		}
		else
		{
			if (verbose) 
			{ 
                std::cout << "TCP client: " << getCurrentTimeStr() << " connection error. Retry...\n";
			}
			this->socketState = TCPSocketState::Closing;
		}
		break;
	case TCPSocketState::Established:
        if((this->transmit() == -1) | (this->receive() == -1))
		{
			if (verbose)  
			{ 
                std::cout << "TCP client: " << getCurrentTimeStr() << " connection lost.\n";
			}
			this->socketState = TCPSocketState::Closing;
			connected.store(false);
		}	
		else
		{
			connected.store(true);
			measureUpdateTime();
            messageReceivedCallback();
		}
		break;
	case TCPSocketState::Closing:
		closeSocket();
		this->socketState = TCPSocketState::Opening;
		break;
	}
}
void TCPClient::deinit()
{
	closeSocket();
}

int TCPClient::openSocket()
{
	socklen_t addrLen = sizeof(struct sockaddr_in);
	memset( &this->serverAddress, 0, addrLen ); 
	if ( ( this->serverAddress.sin_addr.s_addr = inet_addr( this->ip.c_str() ) ) == INADDR_NONE ) { 
		if(verbose) 
		{ 
            std::cout << "TCP client: " << getCurrentTimeStr() << " invalid IP. Abort.\n";
		}
		this->stop();
	}
	this->serverAddress.sin_family = AF_INET; 
	this->serverAddress.sin_port = htons( this->port );
	if(verbose)
	{ 
        std::cout << "TCP client: " << getCurrentTimeStr()
            << " opening socket " << this->ip << ":" << this->port << std::endl;
	}
    return (this->sockfd = socket( AF_INET, SOCK_STREAM, 0 ));
}

void TCPClient::closeSocket()
{
	close( this->sockfd );
	if(verbose)
	{
        std::cout << "TCP client: " << getCurrentTimeStr() << " closing socket.\n\n";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

ssize_t TCPClient::transmit()
{
	if(this->txBuffer)
	{
		txMutex.lock();
		memcpy(this->txBuffer, this->txBufferUser, this->txBufferSize);
		memset(this->txBufferUser, 0, this->txBufferSize);
		txMutex.unlock();
        ssize_t s = send(this->sockfd, this->txBuffer, this->txBufferSize, 0);
		memset(this->txBuffer, 0, this->txBufferSize);
        return s;
	}
	return 0;
}

ssize_t TCPClient::receive()
{
	if(this->rxBuffer)
	{
        ssize_t r = read(this->sockfd, this->rxBuffer, this->rxBufferSize);
		rxMutex.lock();
		memcpy(this->rxBufferUser, this->rxBuffer, this->rxBufferSize);
		rxMutex.unlock();
        return r;
	}
	return 0;
}

std::string TCPClient::getCurrentTimeStr()
{
	time_t timestamp = time(&timestamp);
	struct tm datetime = *localtime(&timestamp);
	std::string res;

    res += ((datetime.tm_hour < 10) ? "0" : "") +  std::to_string ( datetime.tm_hour ) + ":";
    res += ((datetime.tm_min < 10) ? "0" : "") + std::to_string ( datetime.tm_min )  + ":";
    res += ((datetime.tm_sec < 10) ? "0" : "") + std::to_string ( datetime.tm_sec );
	return res;
}

void TCPClient::measureUpdateTime()
{
    auto endDuration = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsedSeconds = endDuration - this->startDuration;
    this->updatePeriodMicroseconds.store(static_cast<uint32_t>(elapsedSeconds.count() * 1e+6));
	this->startDuration = std::chrono::system_clock::now();
}


