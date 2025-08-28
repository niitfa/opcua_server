#include "opcua_server.h"

OPCUA_STATE_MACHINE(_empty_sm) {}

void OPCUA_Server_init(OPCUA_Server_t *self)
{
    memset(self, 0, sizeof(*self));
    self->server = NULL;
    self->connectionStateMachine = _empty_sm;
}

void OPCUA_Server_new(OPCUA_Server_t *self)
{
    self->server = UA_Server_new();    
}

void OPCUA_Server_set_network_address_url(OPCUA_Server_t *self, UA_NetworkAddressUrlDataType url)
{
    self->networkAddressUrl = url;
}

void OPCUA_Server_set_transport_profile(OPCUA_Server_t *self, UA_String profile)
{
    self->transportProfile = profile;
}

void OPCUA_Server_set_connection_state_machine(OPCUA_Server_t *self, OPCUA_STATE_MACHINE((*sm)))
{
    self->connectionStateMachine = sm;
}