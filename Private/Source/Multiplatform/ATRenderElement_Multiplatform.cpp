#include "AT.h"
#include "ATRender_Internal.h"
#include "ATTexture_Internal.h"
#include "ATRenderElement_Internal.h"
#include "ATMatrix.h"

static ATRenderElement s_ContextElement;

AT_API void ATRenderElement_Render(ATRenderElement* element)
{
	glLoadMatrixf(element->transformation->f);

	if (s_ContextElement.texture != element->texture)
	{
		if (!element->texture && s_ContextElement.texture)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else if (element->texture)
		{
			if (!s_ContextElement.texture)
			{
				glEnable(GL_TEXTURE_2D);
			}
			glBindTexture(GL_TEXTURE_2D, element->texture->textureData);
		}
		s_ContextElement.texture = element->texture;
	}

	//if (s_ContextElement.verticies != element->verticies)
	{
		if (element->format == ATVF_PC)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			char *vertexPointer = (char *)element->verticies;
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ATVertex_PC), vertexPointer);
			glVertexPointer(3, GL_FLOAT, sizeof(ATVertex_PC), vertexPointer + 4);
		}
		else if (element->format == ATVF_PUC)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			char *vertexPointer = (char *)element->verticies;
			glTexCoordPointer(2, GL_FLOAT, sizeof(ATVertex_PUC), vertexPointer);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ATVertex_PUC), vertexPointer + 8);
			glVertexPointer(3, GL_FLOAT, sizeof(ATVertex_PUC), vertexPointer + 12);
		}
		else
		{
			ATASSERT(element->format <= ATVF_PUC, "Unsupported Vertex format");
		}
		s_ContextElement.verticies = element->verticies;
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)element->vertexCount);
}
