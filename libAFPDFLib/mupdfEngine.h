#pragma once
#ifndef _MUPDF
#error "You must include muPDF lib before this file"
#endif
#include "PageMemory.h"

extern "C"
{
	#include <fitz.h>
	#include <mupdf.h>
	//#include <fitz_draw.h>
}

#define MUPDF_PAGES_CACHE 32
class mupdfEngine
{
	private:
		pdf_xref *      _xref;		//Current xRef
		pdf_page **     _pages;		//Pages cache
		pdf_page *		_page;		//Pages cache
		fz_glyphcache * _drawcache;
		//fz_renderer *   _rast;		//Rasterizer
	//Load from file, expect an utf8 encoded string
public:
	mupdfEngine();
	~mupdfEngine();
	fz_matrix viewctm(pdf_page *page, float zoom, int rotate);
	int LoadFile(char * fileName,char *own_pwd,char *usr_pwd);
	pdf_page *GetPage(int pageNo);
	fz_pixmap* display(AuxOutputDev *out,int page, int rotate, double zoom,
		   GBool (*abortCheckCbk)(void *data),
		   void *abortCheckCbkData);
	HBITMAP renderBitmap( int pageNo, double zoomReal, int rotation,
                           fz_rect *pageRect,
                           BOOL (*abortCheckCbkA)(void *data),
                           void *abortCheckCbkDataA, int *width, int *height);
};