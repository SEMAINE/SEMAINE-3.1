#include ".\fapsimage.h"


void CLOSE_CB(Fl_Widget* w, void* v)
{
	((FAPsImage*)v)->hide();
}

FAPsImage::FAPsImage(void):Fl_Window(10,20,323,560,"FAPs and FDPs")
{
	char *nameImage;

	this->size_range(375,250,360,510,1,1,0);
	image = new Fl_JPEG_Image("FAP.jpg");
	close = new Fl_Button(120,520, 80, 25, "Close");
	close->callback((Fl_Callback *)CLOSE_CB,this);

    this->hide();	
}


FAPsImage::~FAPsImage(void)
{
}

void FAPsImage::draw()
{
	Fl_Window::draw();
	image->draw(10,10,image->w(),image->h(),0,0);	
}