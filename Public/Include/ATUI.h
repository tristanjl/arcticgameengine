#ifndef AT_UI_H
#define AT_UI_H

AT_API void ATUI_Create(ATUI** ui, const char* filename);

AT_API void ATUI_Destroy(ATUI* ui);

AT_API void ATUI_Update(ATUI* ui);

AT_API void ATUI_Render(ATUI* ui);

#endif // AT_UI_H
