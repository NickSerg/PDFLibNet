#pragma once
#include "AFPDFDocInterop.h"
#include "PDFTextBlockInterop.h"
#include "PDFPageInterop.h"
#include "PDFTextWordList.h"


using namespace System;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

namespace PDFLibNet
{
	public delegate void WriteToStreamHandler(wchar_t *text, int len);
	public delegate void ReadFromStreamHandler(unsigned char *buffer,int dir,  int pos, int len);
	public delegate	void RenderNotifyFinishedHandler(int page, bool bSuccesss);

	public ref class xPDFStream
		: public System::IO::StreamWriter {
	private:
		GCHandle _gchwriteToStream;
		WriteToStreamHandler ^_writeToStream;
		void _WriteToStreamFunc(wchar_t *str,int len);
	internal:
		void *GetWritePointer();
	public:
		xPDFStream::xPDFStream(System::IO::Stream ^stream,System::Text::Encoding ^enc);
		xPDFStream(System::IO::Stream ^stream);
		xPDFStream(String ^fileName);
		!xPDFStream();
		~xPDFStream();
	};

	public ref class xPDFBinaryReader
		: public System::IO::BinaryReader
	{
	private:
		System::Object ^_readLock;
		GCHandle _gchReadFromStream;
		void *_ptrReadFromStream;
		ReadFromStreamHandler ^_readFromStream;
		void _ReadFromStreamFunc(unsigned char *buffer,int dir,  int pos, int len);
	internal:
		void *GetReadPointer();
	public:
		xPDFBinaryReader(System::IO::Stream ^stream);
		!xPDFBinaryReader();
		~xPDFBinaryReader();
	};
	
	public ref class PDFPage
	{
	private:
		Object ^_errorRender;
		RenderNotifyFinishedHandler ^_internalRenderNotifyFinished;
		RenderNotifyFinishedHandler ^_evRenderNotifyFinished;
		GCHandle _gchRenderNotifyFinished;
		bool _isSuccesed;
		System::IntPtr _thumbHdc;
		PDFPageInterop *_page;
		AFPDFDocInterop *_pdfDoc;
		System::String ^_text;
		System::Drawing::Bitmap ^_thumbNail;
		System::Drawing::Graphics ^_thumbG;
		PDFTextWordList<PDFTextWord^> ^_wordObjList;
		PDFTextWordInterop *_wordList;
		bool _loaded;
		int _pageNumber;
		void loadPage();
		void extractImages();
		void _RenderNotifyFinished(int page, bool bSuccesss);
		static int MakeCOLORREF(unsigned char r, unsigned char g, unsigned char b)
		{
			return  (int) (((unsigned int)r) | ( ((unsigned int)g) <<8 ) |  ( ((unsigned int)b) << 16 ));
		}
	internal:
		PDFPage(AFPDFDocInterop *pdfDoc, PDFPageInterop *page);
		PDFPage(AFPDFDocInterop *pdfDoc, int page);
		
	public:
		~PDFPage(void);
		property int ImagesCount{
			int get(){
				extractImages();
				return _page->getImagesCount();
			}
		}
		System::Drawing::Image ^GetImage(int index);
		String ^ExtractText(System::Drawing::Rectangle rect);
		//String ^ExtractWord(System::Drawing::Point p);
		//String ^ExtractWord(System::Drawing::Point p, int iWord, int iCount);

		property System::Collections::Generic::IEnumerable<PDFTextWord^> ^WordList
		{
			System::Collections::Generic::IEnumerable<PDFTextWord^> ^get()
			{
				if(_wordObjList == nullptr)
				{
					loadPage();
					_wordList =	_page->getRawWordList();
					_wordObjList = gcnew PDFTextWordList<PDFTextWord^>(_wordList);
				}
				return _wordObjList;
			}
		}
		property String ^ISOAName
		{
			String ^get(){
				char *iso = _page->getISOANum();
				String ^isoS = gcnew String(iso);
				delete iso;
				return isoS;
			}
		}
		property int Rotation
		{
			int get() {
				loadPage();
				return _page->getPageRotate();
			}
		}
		property double Width
		{
			double get(){
				loadPage();
				return _page->getPageWidth();
			}
		}
		property double Height
		{
			double get(){
				loadPage();
				return _page->getPageHeight();
			}
		}

		property System::IO::StreamWriter ^TextStream
		{
			System::IO::StreamWriter ^get(){
				loadPage();
				System::IO::MemoryStream ^mems =  gcnew System::IO::MemoryStream();
				xPDFStream ^str =gcnew xPDFStream((System::IO::Stream ^)mems);
				
				_page->getTextStream(str->GetWritePointer());
				//swr->Flush();
				mems->Flush();
				return str;
			}
		}
		property System::String ^Text{
			String ^get(){
				loadPage();
				if(_text==nullptr){
					wchar_t *text=_page->getText();
					if(text!=0){
						_text = gcnew String(text);
					}else 
						return String::Empty;
				}
				return _text;
			}
		}
		Boolean HasText(Int32 x, Int32 y){
			loadPage();
			if(_page)
				return _page->hasText(x,y);
			return false;
		}

		
		///<summary>
		///Get a bitmap asynchronous
		///</summary>
		System::Drawing::Bitmap ^LoadThumbnail(System::Int32 width, System::Int32 height);

		System::Drawing::Bitmap ^GetBitmap(System::Int32 width, System::Int32 height);
		System::Drawing::Bitmap ^GetBitmap(System::Int32 width, System::Int32 height, System::Boolean antialias);
		System::Drawing::Bitmap ^GetBitmap(System::Double dpi, System::Boolean antialias);
		System::Drawing::Bitmap ^GetBitmap(System::Double dpi);
		/*///<summary>
		///Calculate the proportional Height
		///</summary>
		System::Int32 CalcHeight(System::Int32 width);
		System::Int32 CalcWidth(System::Int32 height);
		System::Int32 CalcDPI(System::Int32 width,System::Int32 height);
		*/

		
		event RenderNotifyFinishedHandler ^RenderThumbnailFinished{
			void add(RenderNotifyFinishedHandler ^ ev){
				this->_evRenderNotifyFinished += ev;
			}
			void remove(RenderNotifyFinishedHandler ^ev){
				this->_evRenderNotifyFinished -= ev;
			}
			 void raise(int p, bool b) {
				 RenderNotifyFinishedHandler^ tmp = _evRenderNotifyFinished;
				 if (tmp) {
					return tmp->Invoke(p,b);
				 }
			  }

		}
	};
}