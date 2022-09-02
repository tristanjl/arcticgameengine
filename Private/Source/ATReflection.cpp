#include "AT.h"
#include "ATString.h"
#include "ATReflection_Internal.h"

uint32 ATReflection_GetOffset(uint32 currentOffset, ATReflectionDataType dataType)
{
    return currentOffset;
}

void ATReflection_SetFeildValue(const char* reflectionType, const char* fieldName, const void* fieldValue, void* instance)
{
    ATReflectionStructMetadata* data = g_reflectionTypes.Get(fieldName);
    
    uint32 offset = 0;
    for (uint32 i = 0; i < data->fieldCount; ++i)
    {
        ATReflectionFieldMetadata& fieldData = data->fields[i];
        
        offset = ATReflection_GetOffset(offset, (ATReflectionDataType)fieldData.dataType);

        if (ATString_Compare(fieldData.name, fieldName))
        {
            continue;
        }
    }
}
