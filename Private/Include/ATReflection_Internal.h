#ifndef AT_REFLECTION_INTERNAL_H
#define AT_REFLECTION_INTERNAL_H

#include "ATHashTable.h"

enum ATReflectionDataType
{
    ATRDT_Bool,
    ATRDT_Int8,
    ATRDT_Int16,
    ATRDT_Int32,
    ATRDT_Int64,
    ATRDT_Float,
    ATRDT_Vector4,
    ATRDT_Matrix,
    ATRDT_CString,
    ATRDT_Struct
};

enum ATReflectionTypeFlags
{
    ATRTF_Array,
    ATRTF_ArraySize,
    ATRTF_Unsigned
};

struct ATReflectionStructMetadata;

struct ATReflectionFieldMetadata
{
    char* name;
    uint16 dataType;
    uint16 typeFlags;
    ATReflectionStructMetadata* structType;
};

struct ATReflectionStructMetadata
{
    ATReflectionFieldMetadata* fields;
    int fieldCount;
};

ATHashTable<ATReflectionStructMetadata, 64> g_reflectionTypes;

#endif // AT_REFLECTION_INTERNAL_H
