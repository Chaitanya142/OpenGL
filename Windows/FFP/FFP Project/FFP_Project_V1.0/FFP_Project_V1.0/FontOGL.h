///////////////////////////////////////////////////////////////////////////////
// FontOGL.h
// ========
//
///////////////////////////////////////////////////////////////////////////////
#ifndef FONTOGL_H
#define FONTOGL_H
#include <vector>
#ifdef _WIN32
#include <windows.h>  
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
class FontOGL
{
public:
	// ctor/dtor
	FontOGL();
	FontOGL(HDC hdc, GLuint height = 1.0f);
	~FontOGL() {}
	GLvoid BuildFont(GLvoid);
	void set(HDC hdc, GLuint height = 1.0f);
	void setFontHeight(GLuint fontHeight = 1.0f);
	GLvoid KillFont(GLvoid);
	GLvoid glPrint(const char *fmt, ...);
	void setHDC(HDC hdc);
protected:
private:
	GLuint base;
	GLuint height;
	HDC ghdc;
};
#endif
