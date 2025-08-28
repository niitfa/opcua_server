#ifndef OPCUA_SERVER_H
#define OPCUA_SERVER_H

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

#define OPCUA_STATE_MACHINE(sm)                     \
    UA_StatusCode sm (UA_Server *server,            \      
                    const UA_NodeId componentId,    \  
                    void *componentContext,         \      
                    UA_PubSubState *state,          \       
                    UA_PubSubState targetState)     \

typedef struct
{
    UA_Server* server;
    UA_NetworkAddressUrlDataType networkAddressUrl;
    UA_String transportProfile;
    UA_NodeId connectionIdentifier;
    UA_NodeId readerGroupIdentifier;
    UA_NodeId readerIdentifier;
    UA_DataSetReaderConfig readerConfig;

    OPCUA_STATE_MACHINE((*connectionStateMachine));
} OPCUA_Server_t;

void OPCUA_Server_init(OPCUA_Server_t *self);
void OPCUA_Server_new(OPCUA_Server_t *self);
void OPCUA_Server_set_network_address_url(OPCUA_Server_t *self, UA_NetworkAddressUrlDataType url);
void OPCUA_Server_set_transport_profile(OPCUA_Server_t *self, UA_String profile);
void OPCUA_Server_set_connection_state_machine(
    OPCUA_Server_t *self,
    OPCUA_STATE_MACHINE((*sm))
);



#endif