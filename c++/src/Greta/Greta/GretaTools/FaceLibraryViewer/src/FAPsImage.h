#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>

class FAPsImage : public Fl_Window 
{
public:
	Fl_Button *close;
	Fl_JPEG_Image *image;
	void draw();

	FAPsImage(void);
	~FAPsImage(void);
};
