#include <stdio.h>
#include "server_pubsub_subscribe_rt_state_machine.h"
#include "opcua_server.h"
#include "opcua_metadata.h"
#include "rpi4_callbacks.h"

OPCUA_Server_t server;
OPCUA_MetaData_t readerMetaData;

// TO DO : publisher


int main(int argc, char* argv[])
{
	// OPCUA MetaData
	OPCUA_MetaData_init(&readerMetaData, UA_STRING("Reader MetaData 1"), 4);
	OPCUA_MetaData_create_scalar_field(&readerMetaData, 0, UA_STRING("DateTime"), UA_TYPES_DATETIME);
	OPCUA_MetaData_create_scalar_field(&readerMetaData, 1, UA_STRING("Byte"), UA_TYPES_BYTE);
	OPCUA_MetaData_create_scalar_field(&readerMetaData, 2, UA_STRING("Int16"), UA_TYPES_UINT16);	
	OPCUA_MetaData_create_scalar_field(&readerMetaData, 3, UA_STRING("Int64"), UA_TYPES_UINT64);	

	OPCUA_MetaData_print(&readerMetaData);


	// OPCUA Server
	rpi4_set_parent_server(&server);
	OPCUA_Server_init(&server);
	OPCUA_Server_set_network_address_url(&server,
		(UA_NetworkAddressUrlDataType){{0, NULL}, UA_STRING_STATIC("opc.udp://224.0.0.22:4840/")}
	);
	OPCUA_Server_set_transport_profile(&server,
		UA_STRING("http://opcfoundation.org/UA-Profile/Transport/pubsub-udp-uadp")
	);
	OPCUA_Server_add_pubsub_connection(&server,
		 UA_STRING("UDPMC Connection 1"),
		 (UA_UInt32)333,
		 rpi4_connection_sm
		);
	OPCUA_Server_set_reader_metadata(&server, &readerMetaData);
	OPCUA_Server_add_reader_group(&server, UA_STRING("Reader Group 1"));
	OPCUA_Server_add_data_set_reader(&server, 
		UA_STRING("DataSet Reader 1"),
		(UA_UInt16)2234,
		(UA_UInt16)100,
		(UA_UInt16)62541
	);
	OPCUA_Server_add_subscribed_variables(&server);
	OPCUA_Server_run(&server);

	return 0;
	//return server_pubsub_subscribe_rt_state_machine(argc, argv);
}
