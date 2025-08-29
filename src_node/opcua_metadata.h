#ifndef OPCUA_METADATA_H
#define OPCUA_METADATA_H

#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_pubsub.h>
#include <open62541/server_config_default.h>
#include <open62541/types.h>
#include <open62541/types_generated.h>

typedef struct
{
    UA_DataSetMetaDataType metaData;
} OPCUA_MetaData_t;

void OPCUA_MetaData_init(OPCUA_MetaData_t* self, UA_String name, size_t fieldSize);
void OPCUA_MetaData_create_scalar_field(OPCUA_MetaData_t* self, UA_UInt16 index, UA_String name, UA_Byte type);
void OPCUA_MetaData_print(OPCUA_MetaData_t* self);
UA_DataSetMetaDataType* OPCUA_MetaData_get(OPCUA_MetaData_t* self);

#endif