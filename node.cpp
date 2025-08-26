#include <stdio.h>
#include "server_pubsub_subscribe_rt_state_machine.h"
#include "opcua_server_node.h"

int main(int argc, char* argv[])
{
	/*OPCUAServerNode* node = new OPCUAServerNode;
	node->run();
	return 0; */
	return server_pubsub_subscribe_rt_state_machine(argc, argv);
}
