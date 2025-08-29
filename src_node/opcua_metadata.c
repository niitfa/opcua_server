#include "opcua_metadata.h"

void OPCUA_MetaData_init(OPCUA_MetaData_t* self, UA_String name, size_t fieldSize)
{
    memset(self, 0, sizeof(*self));
    self->metaData.name = name;
    self->metaData.fieldsSize = fieldSize;
    self->metaData.fields = (UA_FieldMetaData*)UA_Array_new (self->metaData.fieldsSize,
        &UA_TYPES[UA_TYPES_FIELDMETADATA]);
}

void OPCUA_MetaData_create_scalar_field(OPCUA_MetaData_t* self, UA_UInt16 index, UA_String name, UA_Byte type)
{
    if(index >= 0 && index < self->metaData.fieldsSize)
    {
        // Type = UA_TYPES_...
        UA_FieldMetaData_init (&self->metaData.fields[index]);
        UA_NodeId_copy (&UA_TYPES[type].typeId,
                       &self->metaData.fields[index].dataType);
        self->metaData.fields[index].builtInType = type + 1;
        self->metaData.fields[index].name = name;
        self->metaData.fields[index].valueRank = -1; /* scalar */
    }
}

void OPCUA_MetaData_print(OPCUA_MetaData_t* self)
{
    for(int i = 0; i < self->metaData.fieldsSize; i++)
    {
        printf("%s\n", (char*)self->metaData.name.data);
    }
}

UA_DataSetMetaDataType* OPCUA_MetaData_get(OPCUA_MetaData_t* self)
{
    return &self->metaData;
}