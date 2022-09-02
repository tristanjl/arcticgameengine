#ifndef AT_BASIC_FONT_H
#define AT_BASIC_FONT_H

struct ATVector4;
AT_API void ATBasicFont_RenderText(const ATVector4& position, char* text, float fontSize = 36.0f, const ATVector4& color = ATVector4::White);

#endif // AT_BASIC_FONT_H