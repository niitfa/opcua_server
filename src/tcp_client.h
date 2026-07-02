#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <functional>
#include <arpa/inet.h>
#include <stdint.h>

// add received callback???

class TCPClient
{
	enum class TCPSocketState
	{
		Opening,
		Connecting,
		Established,
		Closing
	};

	// thread control
	std::vector<int> cpus;
	std::atomic<bool> is_launched; // для контроля наличия активного потока
	std::atomic<bool> is_initialized;  // для контроля блокировки в главном потоке в процессе инициализации
	std::atomic<bool> is_stop_forced; // для размещения в условии while и остановки при вызове stop()
	std::mutex rxMutex, txMutex;

	// tcp client/server data
	TCPSocketState socketState = TCPSocketState::Opening;
	std::string ip;
	uint16_t port;	
	int sockfd = 0;
	struct sockaddr_in serverAddress;
	const struct timeval timeout = { 0, 500 * 1000 };
	std::atomic<bool> connected { false };
	
	// rx buffer
	uint32_t rxBufferSize = 0;
	uint8_t* rxBuffer = nullptr;
	uint8_t* rxBufferUser = nullptr;

	// tx buffer
	uint32_t txBufferSize = 0;
	uint8_t* txBuffer = nullptr;
	uint8_t* txBufferUser = nullptr;

	// other
	bool verbose = 0;
	std::atomic<uint32_t> updatePeriodMicroseconds { 0 };
	std::chrono::_V2::system_clock::time_point startDuration;
public:
	TCPClient(std::string ip, uint16_t port, uint32_t rx_size, uint32_t tx_size);
	virtual ~TCPClient();

    // setup
    void setVerbose(bool verbose);
    void setCPUs(std::vector<int> cpus);
    void setIP(std::string ip);
    void setPort(uint16_t port);
    void setMessageReceivedCallback(std::function<void ()>);

    // control
	void start();
	void stop();

	bool isConnected();
	uint32_t getLastUpdatePeriodMicroseconds();

protected:
	void copyFromRx(void* dest, uint32_t offset, uint32_t size);
    void copyToTx(const void* src, uint32_t offset, uint32_t size);

private:
	// parallel thread
	void handler();
	void init();
	void loop();
    void deinit();

	// socket wrappers
	int openSocket();
	void closeSocket();
    ssize_t transmit();
    ssize_t receive();

	// time
	std::string getCurrentTimeStr();
	void measureUpdateTime();

    std::function<void(void)> messageReceivedCallback = [](void) {};
};

#endif
