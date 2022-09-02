#include "AT.h"
#include "ATArray.h"
#include "ATUpdateLayer.h"

static ATArray<ATUpdateLayer> s_pUpdateLayerList;

void ATUpdateLayer_Push(const ATUpdateLayer &updateLayer)
{
	s_pUpdateLayerList.Add(updateLayer);
	updateLayer.init();
}

void ATUpdateLayer_Pop()
{
	size_t lastIndex = s_pUpdateLayerList.Count();
	s_pUpdateLayerList[lastIndex].deinit();
	s_pUpdateLayerList.Remove(lastIndex);
}

void ATUpdateLayer_Clear()
{
	s_pUpdateLayerList.Clear();
}

void ATUpdateLayer_Update()
{
	s_pUpdateLayerList[s_pUpdateLayerList.Count() - 1].update();
}

void ATUpdateLayer_Render()
{
	s_pUpdateLayerList[s_pUpdateLayerList.Count() - 1].render();
}
