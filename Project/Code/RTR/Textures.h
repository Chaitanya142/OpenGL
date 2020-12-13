#include"Common.h"


/*Common method to load all bmp textures*/
void LoadTexture(void);

/*Load .ktx texture*/
void loadktxTextures(char modulePath[255]);

/*Loads Texture using SOIL Library*/
unsigned int loadTextureUsingSOIL(vector<std::string> faces);


/*Load Cube map texture using SOIL Library*/
unsigned int loadCubemapTexture(vector<std::string> faces);
