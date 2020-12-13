#include"Textures.h"


/*Loads Bitmap Texture*/
unsigned int loadTexture(TCHAR imageResourceID[])
{
	//GLuint textureId;
	// variable declarations
	unsigned int textureID;
	HBITMAP hBitmap = NULL;
	BITMAP bmp; // bmp is struct
	//BOOL bStatus = FALSE;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), // get handle of self by passing NULL
		imageResourceID, // image ID 
		IMAGE_BITMAP, // IMAGE_CURSOR / IMAGE_ICON
		0, // width of image
		0, // height of image giving 0,0 means we are telling i dant to create bitmap not icon
		LR_CREATEDIBSECTION); // how you want to load this image? LR(Load Resource) CREATE DIB SECTION (DIB Device Independent Bitmap)
							  // create DIB type bitmap, keep it in DIB section and load resorce
	if (hBitmap)
	{
		//bStatus = TRUE; // because we got handle of bitmap

		GetObject(hBitmap, sizeof(BITMAP), &bmp); // take this bitmap resource handle, and give me bitmap data in 'bmp' & data size should be sizeof(BITMAP)
		glPixelStorei(GL_UNPACK_ALIGNMENT, // keep image data unpack, keep space in row and columns as microsoft suggests bmp should be unpacked
			4); // how much space to keep between row by row? 4 for RGBA 

		glGenTextures(1, &textureID); // how many textures you want, pointer variable to store ID of created texture
		glBindTexture(GL_TEXTURE_2D, // i want to use this mem for 2D 
			textureID); // texture no)

		glTexParameteri(GL_TEXTURE_2D, // bind point/ target
			GL_TEXTURE_MAG_FILTER, // which parameter you want to twick : magnification
			GL_LINEAR); // map each pixel by taking avarage of colors from nearby Texcels 

		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER, // minification
			GL_LINEAR_MIPMAP_LINEAR); // use GL_LINEAR for all mipmapped images

		glTexImage2D(GL_TEXTURE_2D, // bind point: put all other 8 parameters in 1st
			0, // mipmap levels, 0: I am not giving u decide; values can be 1,2,4,6
			GL_RGBA, // internal image format
			bmp.bmWidth,
			bmp.bmHeight,
			0, // border width
			GL_BGR_EXT, // external / pixel/ image data format
			GL_UNSIGNED_BYTE, // type of image data we are giving data in bytes
			bmp.bmBits); // actual image data

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0); // explicitly unbind, we should bind in display();

		DeleteObject(hBitmap); // we got image data copied from cpu mem to gpu mem; so bmp is no more needed, but bmp is local hence no need to free, but handle is global so free it
	}

	return textureID;
}


/*Common method to load all bmp textures*/
void LoadTexture()
{
	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_LEAF)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_STAR)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_MOON)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_HEART)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_SIR)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_AMC)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_BARK)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_BACKRAIN)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_STUDYROOM)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_COLLAGE)));

	textures.push_back(loadTexture(MAKEINTRESOURCE(IDBITMAP_HEART_PINK)));

}


/*Loads .ktx texture*/
void loadktxTextures(char modulePath[255]) {
	char  char_array[255];
	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\grass_length.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\grass_orientation.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\grass_color.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\grass_bend.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\terragen1.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\SB7Resource\\terragen_color.ktx");
	textures.push_back(sb7::ktx::file::load(char_array));
}

/*Loads texture using soil library*/
unsigned int loadTextureUsingSOIL(vector<std::string> faces)
{
	unsigned int textureID;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			fprintf_s(gpFile, "Failed to load image");
		}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


/*Load Cubemap texture using SOIL*/
unsigned int loadCubemapTexture(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//stbi_image_free(data);
		}
		else
		{
			fprintf_s(gpFile, "Failed to load image");
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	return textureID;
}
