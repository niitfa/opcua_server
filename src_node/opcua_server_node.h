#ifndef OPCUA_SERVER_NODE_H
#define OPCUA_SERVER_NODE_H

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_pubsub.h>
#include <open62541/server_config_default.h>
#include <open62541/types.h>
#include <open62541/types_generated.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

// переделать в singleton

class OPCUAServerNode
{
    static UA_Server* server;
    static UA_NetworkAddressUrlDataType networkAddressUrl;
    UA_String transportProfile = UA_STRING_STATIC(
        "http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp"
    );

    static UA_NodeId connectionIdentifier;
    UA_NodeId readerGroupIdentifier;
    UA_NodeId readerIdentifier;
    UA_DataSetReaderConfig readerConfig;

    static pthread_t listenThread;
    static int listenSocket;
public:
    OPCUAServerNode();
    ~OPCUAServerNode();

    void run();
private:
    void addPubSubConnection();

    static UA_StatusCode connectionStateMachine(UA_Server *server, const UA_NodeId componentId,
                       void *componentContext, UA_PubSubState *state,
                       UA_PubSubState targetState);
    static void* listenUDP(void *);
};

#endif