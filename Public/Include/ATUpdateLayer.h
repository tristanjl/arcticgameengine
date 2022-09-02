#ifndef AT_UPDATE_LAYER_H
#define AT_UPDATE_LAYER_H

typedef void (*ATUpdateLayerCallback)(void);

#define AT_UPDATE_LAYER_REGISTER(state) \
	void state##_Init(); \
	void state##_Update(); \
	void state##_Render(); \
	void state##_Deinit(); \
	ATUpdateLayer state; \
	state.init = state##_Init; \
	state.update = state##_Update; \
	state.render = state##_Render; \
	state.deinit = state##_Deinit; \
	ATUpdateLayer_Push(state)

struct ATUpdateLayer
{
	ATUpdateLayerCallback init;
	ATUpdateLayerCallback deinit;
	ATUpdateLayerCallback update;
	ATUpdateLayerCallback render;
};

void ATUpdateLayer_Push(const ATUpdateLayer &updateLayer);

void ATUpdateLayer_Pop();

void ATUpdateLayer_Clear();

void ATUpdateLayer_Update();

void ATUpdateLayer_Render();

#endif // AT_UPDATE_LAYER_H
