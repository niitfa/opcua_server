#include "opcua_server.h"

OPCUA_STATE_MACHINE(_empty_sm) {}

void OPCUA_Server_init(OPCUA_Server_t *self)
{
    memset(self, 0, sizeof(*self));
    self->server = UA_Server_new(); 
}

void OPCUA_Server_deinit(OPCUA_Server_t *self)
{
    UA_Server_delete(self->server); 
}

void OPCUA_Server_set_network_address_url(OPCUA_Server_t *self, UA_NetworkAddressUrlDataType url)
{
    self->networkAddressUrl = url;
}

void OPCUA_Server_set_transport_profile(OPCUA_Server_t *self, UA_String profile)
{
    self->transportProfile = profile;

}

void OPCUA_Server_add_pubsub_connection(OPCUA_Server_t* self,
     UA_String name,
     UA_UInt32 pubId, 
     OPCUA_STATE_MACHINE((*sm))
    )
{
    memset (&self->connectionConfig, 0, sizeof(self->connectionConfig));
    self->connectionConfig.name = name;
    self->connectionConfig.transportProfileUri = self->transportProfile;

    UA_Variant_setScalar(
        &self->connectionConfig.address,
        &self->networkAddressUrl,
        &UA_TYPES[UA_TYPES_NETWORKADDRESSURLDATATYPE]
    );

    self->connectionConfig.publisherId.idType = UA_PUBLISHERIDTYPE_UINT32;
    self->connectionConfig.publisherId.id.uint32 = pubId;   
    self->connectionConfig.customStateMachine = sm;

    UA_Server_addPubSubConnection(
        self->server,
        &self->connectionConfig,
        &self->readerGroupIdentifier
    );
}

void OPCUA_Server_add_reader_group(OPCUA_Server_t* self, UA_String name)
{
    memset (&self->readerGroupConfig, 0, sizeof(self->readerGroupConfig));
    self->readerGroupConfig.name = name;
    UA_Server_addReaderGroup(
        self->server,
        self->connectionIdentifier, 
        &self->readerGroupConfig,
        &self->readerGroupIdentifier
    );
}

void OPCUA_Server_set_reader_metadata(OPCUA_Server_t* self,
    OPCUA_MetaData_t* metaData
)
{
    memcpy(&self->opcuaReaderMetaData, metaData, sizeof(metaData));    
}

void OPCUA_Server_add_data_set_reader(OPCUA_Server_t* self,
    UA_String name,
    UA_UInt16 publisherId,
    UA_UInt16 writerGroupId,
    UA_UInt16 dataSetWriterId  
)
{
    memset (&self->readerConfig, 0, sizeof(self->readerConfig));
    self->readerConfig.name = name;
    self->readerConfig.publisherId.idType = UA_PUBLISHERIDTYPE_UINT16;
    self->readerConfig.publisherId.id.uint16 = publisherId;
    self->readerConfig.writerGroupId    = writerGroupId;
    self->readerConfig.dataSetWriterId  = dataSetWriterId;

    memcpy(
        &self->readerConfig.dataSetMetaData, 
        OPCUA_MetaData_get(&self->opcuaReaderMetaData),
        sizeof(UA_DataSetMetaDataType)
    );

    UA_Server_addDataSetReader(self->server,
        self->readerGroupIdentifier, 
        &self->readerConfig,
        &self->readerIdentifier
    );
}

void OPCUA_Server_add_subscribed_variables(OPCUA_Server_t* self)
{
    UA_NodeId folderId;
    UA_String folderName = self->readerConfig.dataSetMetaData.name;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    UA_QualifiedName folderBrowseName;

    if(folderName.length > 0) {
        oAttr.displayName.locale = UA_STRING ("en-US");
        oAttr.displayName.text = folderName;
        folderBrowseName.namespaceIndex = 1;
        folderBrowseName.name = folderName;
    }
    else
    {
        oAttr.displayName = UA_LOCALIZEDTEXT ("en-US", "Subscribed Variables");
        folderBrowseName = UA_QUALIFIEDNAME (1, "Subscribed Variables");
    }

    UA_Server_addObjectNode(self->server, UA_NODEID_NULL,
                            UA_NS0ID(OBJECTSFOLDER), UA_NS0ID(ORGANIZES),
                            folderBrowseName, UA_NS0ID(BASEOBJECTTYPE), oAttr, NULL, &folderId);
    
    /* Create the TargetVariables with respect to DataSetMetaData fields */
    UA_FieldTargetDataType *targetVars = (UA_FieldTargetDataType *)
        UA_calloc(self->readerConfig.dataSetMetaData.fieldsSize, sizeof(UA_FieldTargetDataType));
        
    for(size_t i = 0; i < self->readerConfig.dataSetMetaData.fieldsSize; i++) {
        /* Variable to subscribe data */
        UA_VariableAttributes vAttr = UA_VariableAttributes_default;
        UA_LocalizedText_copy(&self->readerConfig.dataSetMetaData.fields[i].description,
                              &vAttr.description);
        vAttr.displayName.locale = UA_STRING("en-US");
        vAttr.displayName.text = self->readerConfig.dataSetMetaData.fields[i].name;
        vAttr.dataType = self->readerConfig.dataSetMetaData.fields[i].dataType;

        UA_NodeId newNode;
        UA_Server_addVariableNode(self->server, UA_NODEID_NUMERIC(1, (UA_UInt32)i + 50000),
                                  folderId, UA_NS0ID(HASCOMPONENT),
                                  UA_QUALIFIEDNAME(1, (char *)self->readerConfig.dataSetMetaData.fields[i].name.data),
                                  UA_NS0ID(BASEDATAVARIABLETYPE),
                                  vAttr, NULL, &newNode);

        /* For creating Targetvariables */
        targetVars[i].attributeId  = UA_ATTRIBUTEID_VALUE;
        targetVars[i].targetNodeId = newNode;
    }

    UA_Server_DataSetReader_createTargetVariables(
        self->server, 
        self->readerIdentifier,                                        
        self->readerConfig.dataSetMetaData.fieldsSize,
        targetVars
    );

    UA_free(targetVars);
}

void OPCUA_Server_run(OPCUA_Server_t* self)
{
    UA_Server_enableAllPubSubComponents(self->server);
    UA_Server_runUntilInterrupt(self->server);
}