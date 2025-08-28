#include <stdio.h>
#include "server_pubsub_subscribe_rt_state_machine.h"
#include "opcua_server.h"

OPCUA_Server_t server;

int main(int argc, char* argv[])
{

	OPCUA_Server_init(&server);
	OPCUA_Server_new(&server);
	OPCUA_Server_set_network_address_url(&server,
		(UA_NetworkAddressUrlDataType){{0, NULL}, UA_STRING_STATIC("opc.udp://224.0.0.22:4840/")}
	);
	OPCUA_Server_set_transport_profile(&server,
		UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp")
	);

	return 0;
	//return server_pubsub_subscribe_rt_state_machine(argc, argv);
}
