#include "AT.h"
#include "ATMath.h"
#include "ATMatrix.h"

enum ATUIFlags
{
	ATUIF_PositionX = ATBIT(0),
	ATUIF_PositionY = ATBIT(1),
	ATUIF_PositionZ = ATBIT(2),
	ATUIF_RotationYaw = ATBIT(3),
	ATUIF_RotationPitch = ATBIT(4),
	ATUIF_RotationRoll = ATBIT(5),
	ATUIF_ScaleX = ATBIT(6),
	ATUIF_ScaleY = ATBIT(7),
	ATUIF_ScaleZ = ATBIT(8),

	ATUIF_Width = ATBIT(9),
	ATUIF_Height = ATBIT(10),
	ATUIF_PivotOffsetCenterX = ATBIT(11),
	ATUIF_PivotOffsetCenterY = ATBIT(12),

	ATUIF_PositioningFlags = ATBIT(13),
	ATUIF_Renderable = ATBIT(14),
	ATUIF_Button = ATBIT(15),
	
	ATUIF_Position = ATUIF_PositionX | ATUIF_PositionY | ATUIF_PositionZ,
	ATUIF_Rotation = ATUIF_RotationYaw | ATUIF_RotationPitch | ATUIF_RotationRoll,
	ATUIF_Scale = ATUIF_ScaleX | ATUIF_ScaleY | ATUIF_ScaleZ,
	ATUIF_Bounds = ATUIF_Width | ATUIF_Height | ATUIF_PivotOffsetCenterX | ATUIF_PivotOffsetCenterY,

	ATUIF_TRANSFORM = ATUIF_Position | ATUIF_Rotation | ATUIF_Scale,
	ATUIF_RENDERABLE_2D = ATUIF_PositionX | ATUIF_PositionY | ATUIF_RotationRoll | ATUIF_ScaleX | ATUIF_ScaleY | ATUIF_Renderable,
	ATUIF_RENDERABLE_3D = ATUIF_TRANSFORM | ATUIF_Renderable,
	ATUIF_SYSTEM_FLAGS = ATUIF_TRANSFORM | ATUIF_Bounds
};

enum ATUIContentFlags
{
	ATUIR_Image,
	ATUIR_Model,
	ATUIR_Audio
};

struct ATUIContent
{
	void* content;
	size_t contentType;
};

struct ATUIElement
{
	ATUIElement* parent;
	ATUIElement** children;
	size_t childCount;
	uint8* elementData;
	size_t flags;
};

struct ATUI : public ATUIElement
{
};

struct ATUIGlobal
{
	ATRect currentBounds;
};

ATUIGlobal* g_UIGlobal;

static void ATUI_InitializeElementData(ATUIElement* element, uint32 flags)
{
	size_t channelDataSize = flags & ATUIF_TRANSFORM ? sizeof(ATMatrix) - 4 : 0;
	channelDataSize += (size_t)ATMath_BitSetCount32(flags) * 4;

	element->elementData = (uint8*)ATAlloc(channelDataSize);
	ATMemSet(element->elementData, 0, sizeof(channelDataSize));

	uint8* elementDataPointer = element->elementData;
	// Fill in non-zero default data
	if (flags & ATUIF_TRANSFORM)
	{
		ATMemCopy(elementDataPointer, &ATMatrix::Identity, sizeof(ATMatrix));
		elementDataPointer += sizeof(ATMatrix);
	}

	if (flags & ATUIF_PositionX) elementDataPointer += sizeof(float);
	if (flags & ATUIF_PositionY) elementDataPointer += sizeof(float);
	if (flags & ATUIF_PositionZ) elementDataPointer += sizeof(float);
	if (flags & ATUIF_RotationYaw) elementDataPointer += sizeof(float);
	if (flags & ATUIF_RotationPitch) elementDataPointer += sizeof(float);
	if (flags & ATUIF_RotationRoll) elementDataPointer += sizeof(float);
	if (flags & ATUIF_ScaleX)
	{
		*(float*)elementDataPointer = 1.0f;
		elementDataPointer += sizeof(float);
	}
	if (flags & ATUIF_ScaleY)
	{
		*(float*)elementDataPointer = 1.0f;
		elementDataPointer += sizeof(float);
	}
	if (flags & ATUIF_ScaleZ)
	{
		*(float*)elementDataPointer = 1.0f;
		elementDataPointer += sizeof(float);
	}
}

AT_API void ATUI_Create(ATUI** ui, const char* /*filename*/)
{
	ATUI* uiSystem = (ATUI*)ATAlloc(sizeof(ATUI));
	ATMemSet(uiSystem, 0, sizeof(ATUI));

	ATUI_InitializeElementData(uiSystem, ATUIF_SYSTEM_FLAGS);

	*ui = uiSystem;
}

static void ATUI_DestroyElement(ATUI* ui, ATUIElement* element)
{
	for (uint32 i = 0; i < element->childCount; ++i)
	{
		ATUI_DestroyElement(ui, element->children[i]);
	}

	ATFree(element->children);
}

AT_API void ATUI_Destroy(ATUI* ui)
{
	ATUI_DestroyElement(ui, ui);
}

static void ATUI_UpdateElement(ATUI* ui, ATUIElement* element)
{
	uint8* elementDataPointer = element->elementData;

	// Generate the current transform matrix
	if (element->flags & ATUIF_TRANSFORM)
	{
		ATUIElement* parentTransformElement = element->parent;
		ATMatrix parentTransformMatrix = ATMatrix::Identity;
		ATMatrix localTransformMatrix = ATMatrix::Identity;
		while (parentTransformElement)
		{
			if (element->flags & ATUIF_TRANSFORM)
			{
				ATMemCopy(&parentTransformMatrix, parentTransformElement->elementData, sizeof(ATMatrix));
				break;
			}
			parentTransformElement = parentTransformElement->parent;
		}
		elementDataPointer += sizeof(ATMatrix); // ignore existing matrix - we are recalculating it

		ATVector4 translation = ATVector4_Create(0.0f, 0.0f, 0.0f);
		if (element->flags & ATUIF_PositionX)
		{
			translation.v.x = *(float*)elementDataPointer;
			elementDataPointer += sizeof(float);
		}
		if (element->flags & ATUIF_PositionY)
		{
			translation.v.y = *(float*)elementDataPointer;
			elementDataPointer += sizeof(float);
		}
		if (element->flags & ATUIF_PositionZ)
		{
			translation.v.z = *(float*)elementDataPointer;
			elementDataPointer += sizeof(float);
		}

		if (element->flags & ATUIF_Rotation)
		{
			float rotY = 0.0f;
			float rotP = 0.0f;
			float rotR = 0.0f;
			if (element->flags & ATUIF_RotationYaw)
			{
				rotY = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			if (element->flags & ATUIF_RotationPitch)
			{
				rotP = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			if (element->flags & ATUIF_RotationRoll)
			{
				rotR = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			localTransformMatrix = ATMatrix_CreateRotationYPR(rotY, rotP, rotR);
		}

		if (element->flags & ATUIF_Scale)
		{
			ATVector4 scale = ATVector4_Create(1.0f, 1.0f, 1.0f);
			if (element->flags & ATUIF_ScaleX)
			{
				scale.v.x = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			if (element->flags & ATUIF_ScaleY)
			{
				scale.v.y = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			if (element->flags & ATUIF_ScaleZ)
			{
				scale.v.z = *(float*)elementDataPointer;
				elementDataPointer += sizeof(float);
			}
			localTransformMatrix.Scale(scale);
		}
		localTransformMatrix.SetTranslation(translation);
	}

	for (uint32 i = 0; i < element->childCount; ++i)
	{
		ATUI_UpdateElement(ui, element->children[i]);
	}
}

AT_API void ATUI_Update(ATUI* ui)
{
	ATUI_UpdateElement(ui, ui);
}

static void ATUI_RenderElement(ATUI* ui, ATUIElement* element)
{
	for (uint32 i = 0; i < element->childCount; ++i)
	{
		ATUI_RenderElement(ui, element->children[i]);
	}
}

AT_API void ATUI_Render(ATUI* /*ui*/)
{
	//ATUI_RenderElement(ui, ui);
}

AT_API void ATUI_Init()
{
}

AT_API void ATUI_Deinit()
{
}
