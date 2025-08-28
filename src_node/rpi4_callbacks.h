#ifndef OPCUA_SERVER_CALLBACKS_H
#define OPCUA_SERVER_CALLBACKS_H

#include "opcua_server.h"

void rpi4_set_parent_server(OPCUA_Server_t* opcua);
OPCUA_STATE_MACHINE(rpi4_connection_sm);

#endif