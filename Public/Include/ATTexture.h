#ifndef AT_TEXTURE_H
#define AT_TEXTURE_H

struct ATTexture;

AT_API void ATTexture_Create(ATTexture** texture, const char* filename);

AT_API void ATTexture_Destroy(ATTexture* texture);

#endif //AT_TEXTURE_H
