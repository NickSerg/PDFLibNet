#pragma once
#include <vcclr.h>
#include "AFPDFDocInterop.h"
#include "PDFSearchResult.h"
#include "OutlineItem.h"
#include "OutlineItemCollection.h"
#include "PageLink.h"
#include "PageLinkCollection.h"
#include "LinkDest.h"
#include "PDFPage.h"
#include "ExportSWFParams.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

namespace PDFLibNet {

	public delegate void PDFLoadCompletedHandler();
	public delegate void PDFLoadBeginHandler();
	public delegate void PageChanged(int lastPage, int newPage);
	public delegate void PageZoomChanged(System::Drawing::Size lastSize, System::Drawing::Size newSize);
	public delegate bool ExportJpgProgressHandler(int pageCount, int currentPage);
	public delegate void ExportJpgFinishedHandler();
	public delegate	void RenderFinishedHandler();
	

	public enum class PDFSearchOrder
	{
		PDFSearchFromdBegin = 0,
		PDFSearchFromCurrent = 1,
	};


	/// <summary>
	///
	/// </summary>
	public ref class PDFWrapper
	{
	internal:
		AFPDFDocInterop *_pdfDoc;
	//Unmanaged code
	private:
		wchar_t *title;
		wchar_t *author;
		wchar_t *subject;
		wchar_t *producer;
		wchar_t *creator;
		wchar_t *keywords;

	private:
		OutlineItemCollection<OutlineItem^> ^_childrens;
		System::Collections::Generic::List<PDFSearchResult^> ^_searchResults;
		System::Collections::Generic::Dictionary<int,PDFPage ^> _pages;
		System::Collections::Generic::Dictionary<int,PageLinkCollection<PageLink ^> ^> _linksCache;
		String ^_title;
		String ^_author;
		String ^_subject;
		String ^_keywords;
		String ^_creator;
		String ^_producer;
		DateTime _creationdate;
		DateTime _lastmodifieddate;
		PDFLibNet::xPDFBinaryReader ^_binaryReader;
		bool _bLoading;
		bool _ExportJpgProgress(int pageCount, int currentPage);
		void _ExportJpgFinished();
		void _RenderFinished();
		void _RenderNotifyFinished(int, bool);

		bool _ExportSwfProgress(int pageCount, int currentPage);
		void _ExportSwfFinished();

		ExportJpgProgressHandler ^_internalExportJpgProgress;
		ExportJpgProgressHandler ^_evExportJpgProgress;
		ExportJpgFinishedHandler ^_internalExportJpgFinished;
		ExportJpgFinishedHandler ^_evExportJpgFinished;

		ExportJpgProgressHandler ^_internalExportSwfProgress;
		ExportJpgProgressHandler ^_evExportSwfProgress;
		ExportJpgFinishedHandler ^_internalExportSwfFinished;
		ExportJpgFinishedHandler ^_evExportSwfFinished;

		RenderFinishedHandler ^_internalRenderFinished;
		RenderFinishedHandler ^_evRenderFinished;

		RenderNotifyFinishedHandler ^_internalRenderNotifyFinished;
		RenderNotifyFinishedHandler ^_evRenderNotifyFinished;

		GCHandle _gchSwfProgress;
		GCHandle _gchSwfFinished;

		GCHandle _gchProgress;
		GCHandle _gchFinished;
		GCHandle _gchRenderFinished;
		GCHandle _gchRenderNotifyFinished;
	public:
		PDFWrapper()
			: _pdfDoc(0)
			, _childrens(nullptr)
			, _searchResults(nullptr)
			, _title(nullptr)
			, _author(nullptr)
			, _subject(nullptr)
			, _keywords(nullptr)
			, _creator(nullptr)
			, _producer(nullptr)
			, _creationdate(DateTime::MinValue)
			, _lastmodifieddate(DateTime::MinValue)
			, _binaryReader(nullptr)
			, title(0)
			, author(0)
			, producer(0)
			, creator(0)
			, subject(0)
			, keywords(0)
		{
			IntPtr ptr = Marshal::StringToCoTaskMemAnsi(System::Convert::ToString(System::Configuration::ConfigurationSettings::GetConfig("xpdfrc")));
			char *singleByte= (char*)ptr.ToPointer();
			int ret;
			try{
				_pdfDoc = new AFPDFDocInterop(singleByte);
			}finally{
				Marshal::FreeCoTaskMem(ptr);
			}
		}

		PDFWrapper(System::String ^fileConfig)
			: _pdfDoc(nullptr)
			, _childrens(nullptr)
			, _searchResults(nullptr)
			, _title(nullptr)
			, _author(nullptr)
			, _subject(nullptr)
			, _keywords(nullptr)
			, _creator(nullptr)
			, _producer(nullptr)
			, _creationdate(DateTime::MinValue)
			, _lastmodifieddate(DateTime::MinValue)
			, _binaryReader(nullptr)
			, title(0)
			, author(0)
			, producer(0)
			, creator(0)
			, subject(0)
			, keywords(0)
		{
			IntPtr ptr = Marshal::StringToCoTaskMemAnsi(fileConfig);
			char *singleByte= (char*)ptr.ToPointer();
			int ret;
			try{
				_pdfDoc = new AFPDFDocInterop(singleByte);
			}finally{
				Marshal::FreeCoTaskMem(ptr);
			}
		}

		long ExportJpg(System::String ^fileName, System::Int32 quality);
		long ExportJpg(System::String ^fileName,System::Int32 fromPage, System::Int32 toPage, System::Double renderDPI, System::Int32 quality);
		long ExportJpg(System::String ^fileName,System::Int32 fromPage, System::Int32 toPage, System::Double renderDPI, System::Int32 quality, System::Int32 waitProc);

		void CancelSwfExport(){
			_pdfDoc->CancelSwfExport();
		}

		void CancelJpgExport(){
			_pdfDoc->CancelJpgExport();
		}

		property System::Collections::Generic::Dictionary<int,PDFPage ^> ^Pages{
			System::Collections::Generic::Dictionary<int,PDFPage ^> ^get(){
				return %_pages;
			}

		}

		property bool SupportsMuPDF{
			bool get(){
				return _pdfDoc->GetSupportsMuPDF();
			}
		}
		property bool UseMuPDF{
			bool get(){
				return _pdfDoc->GetUseMuPDF();
			}
			void set(bool bUse){
				_pdfDoc->SetUseMuPDF(bUse);
			}
		}
		
		///<sumary>
		/// Returns true if exist a background process exporting to jpeg
		///</sumary>
		property bool IsSwfBusy{
			bool get(){
				return _pdfDoc->IsSwfBusy();
			}
		}

		///<sumary>
		/// Returns true if exist a background process exporting to jpeg
		///</sumary>
		property bool IsJpgBusy{
			bool get(){
				return _pdfDoc->IsJpgBusy();
			}
		}
		property bool IsBusy{
			bool get(){
				return _pdfDoc->IsBusy();
			}
		}
		long ExportSWF(System::String ^fileName, ExportSWFParams ^exportParams);
		long ExportText(System::String ^fileName, System::Int32 firstPage, System::Int32 lastPage,System::Boolean physLayout,System::Boolean rawOrder);
		long ExportHtml(System::String ^fileName, System::Int32 firstPage, System::Int32 lastPage, ExportHtmlParams ^params);
		long PerfomLinkAction(System::Int32 linkPtr);
		bool LoadPDF(System::String ^fileName);
		bool LoadPDF(System::IO::Stream ^stream);
		
		bool RenderPageThread(IntPtr hwndHandle, bool bForce);
		bool RenderPage(IntPtr handler);
		bool RenderPage(IntPtr handler, System::Boolean bForce);
		bool RenderPage(IntPtr handler, System::Boolean bForce, System::Boolean bEnableThread);
		
		bool DrawPageHDC(IntPtr hdc);

		void FitToWidth(IntPtr handler);
		void FitToHeight(IntPtr handler);

		void ZoomIN();
		void ZoomOut();

		void NextPage(){
			_pdfDoc->NextPage();
		}
		void PreviousPage(){
			_pdfDoc->PreviousPage();
		}

		LinkDest ^FindDestination(String ^destName);

		long FindText(String ^sText, Int32 iPage, PDFSearchOrder SearchOrder, Boolean bCaseSensitive, Boolean bBackward, Boolean bMarkAll, Boolean bWholeDoc, Boolean bWholeWord, Boolean stopOnFirstPageResults);
		long FindText(String ^sText, Int32 iPage, PDFSearchOrder SearchOrder, Boolean bCaseSensitive, Boolean bBackward, Boolean bMarkAll, Boolean bWholeDoc, Boolean bWholeWord);
		long FindText(String ^sText, Int32 iPage, PDFSearchOrder SearchOrder, Boolean bCaseSensitive, Boolean bBackward, Boolean bMarkAll, Boolean bWholeDoc);
		long FindFirst(String ^sText,PDFSearchOrder SearchOrder, Boolean Backward, Boolean bWholeWord);
		long FindNext(String ^sText);
		long FindPrevious(String ^sText);
		long PrintToFile(String ^fileName, Int32 fromPage, Int32 toPage);
		System::Collections::Generic::List<PageLink ^> ^GetLinks(int iPage);

		PointF PointUserToDev(PointF point);
		PointF PointDevToUser(PointF point);

		property System::Drawing::Rectangle SliceBox{
			System::Drawing::Rectangle get(){
				return System::Drawing::Rectangle(0,0,0,0);
			}
			void set(System::Drawing::Rectangle  value){
				_pdfDoc->SetSliceBox(value.Left,value.Top,value.Width,value.Height);
			}
		}
		property bool SearchCaseSensitive{
			bool get(){
				return _pdfDoc->GetSearchCaseSensitive();
			}
			void set(bool value){
				_pdfDoc->SetSearchCaseSensitive(value);
			}
		}
		property double RenderDPI {
			double get(){
				return _pdfDoc->GetRenderDPI();
			}
			void set(double value){
				_pdfDoc->SetRenderDPI(value);
			}
		}
		
		property String ^Author{
			String ^get(){
				if(_author==nullptr){
					author=_pdfDoc->GetAuthor();
					if(title!=0)
						_author = gcnew String(author);
					else 
						return String::Empty;
				}
				return _author;
			}
		}
		property String ^Title{
			String ^get(){
				
				if(_title==nullptr){
					title=_pdfDoc->GetTitle();
					if(title!=0)
						_title = gcnew String(title);
					else 
						return String::Empty;
				}
				return _title;
			}
		}
		property String ^Subject{
			String ^get(){
				
				if(_title==nullptr){
					subject=_pdfDoc->GetSubject();
					if(subject!=0)
						_subject = gcnew String(subject);
					else 
						return String::Empty;
				}
				return _subject;
			}
		}
		property String ^Keywords{
			String ^get(){
				
				if(_keywords==nullptr){
					keywords=_pdfDoc->GetKeywords();
					if(keywords!=0)
						_keywords = gcnew String(keywords);
					else 
						return String::Empty;
				}
				return _keywords;
			}
		}
		property String ^Creator{
			String ^get(){
				
				if(_creator==nullptr){
					creator=_pdfDoc->GetCreator();
					if(creator!=0)
						_creator = gcnew String(creator);
					else 
						return String::Empty;
				}
				return _creator;
			}
		}
		property String ^Producer{
			String ^get(){
				
				if(_producer==nullptr){
					producer=_pdfDoc->GetProducer();
					if(title!=0)
						_producer = gcnew String(producer);
					else 
						return String::Empty;
				}
				return _producer;
			}
		}
		property DateTime CreationDate{
			DateTime get(){
				
				if(_creationdate.Equals(DateTime::MinValue)){
					char *creationDate=_pdfDoc->GetCreationDate();
					if(title!=0 && title[0]!='\0')
						_creationdate = DateTime::Parse(%String(creationDate), System::Globalization::CultureInfo::GetCultureInfo("en-US"));
					else 
						return DateTime::MinValue;
					delete creationDate;
				}
				return _creationdate;
			}
		}
		property DateTime LastModifiedDate{
			DateTime get(){
				
				if(_lastmodifieddate.Equals(DateTime::MinValue)){
					char *lastmodifieddate=_pdfDoc->GetLastModifiedDate();
					if(title!=0 && title[0]!='\0')
						_lastmodifieddate = DateTime::Parse(%String(lastmodifieddate), System::Globalization::CultureInfo::GetCultureInfo("en-US"));
					else 
						return DateTime::MinValue;
					delete lastmodifieddate;
				}
				return _lastmodifieddate;
			}
		}

		property double Zoom {
			double get(){
				return _pdfDoc->GetZoom();
			}
			void set(double value){
				_pdfDoc->SetZoom(value);
			}
		}
		property long PageWidth {
			long get(){
				return _pdfDoc->GetPageWidth();
			}
		}
		property long PageHeight {
			long get(){
				return _pdfDoc->GetPageHeight();
			}
		}
		property long CurrentX {
			long get(){
				return _pdfDoc->GetCurrentX();
			}
			void set(long posX){
				_pdfDoc->SetCurrentX(posX);
			}
		}
		property long CurrentY {
			long get(){
				return _pdfDoc->GetCurrentY();
			}
			void set(long posY){
				_pdfDoc->SetCurrentY(posY);
			}
		}

		property long PageCount
		{
			long get(){
				//if(doc.IsNull())
				//	throw gcnew System::NullReferenceException();
				return _pdfDoc->GetPageCount();
			}	
		}
		property long CurrentPage{
			long get(){
				//if(doc.IsNull())
				//	throw gcnew System::NullReferenceException();
				return _pdfDoc->GetCurrentPage();
			}
			void set(long page){
				//if(doc.IsNull())
				//	throw gcnew System::NullReferenceException();
				_pdfDoc->SetCurrentPage(page);
			}
		}

		
		property System::Drawing::Rectangle ClientBounds{
			System::Drawing::Rectangle get(){
				return System::Drawing::Rectangle(_pdfDoc->GetViewX(),_pdfDoc->GetViewY(),_pdfDoc->GetViewWidth(),_pdfDoc->GetViewHeight());
			}
			void set(System::Drawing::Rectangle newRect){
				_pdfDoc->SetViewX(newRect.X);
				_pdfDoc->SetViewY(newRect.Y);
				_pdfDoc->SetViewWidth(newRect.Width);
				_pdfDoc->SetViewHeight(newRect.Height);
			}
		}
		property  System::Collections::Generic::List<OutlineItem^> ^Outline
		{
			System::Collections::Generic::List<OutlineItem^> ^get()
			{
				if(_childrens==nullptr)
				{
					_childrens = gcnew  OutlineItemCollection<OutlineItem ^>();
					for(long i=1;i<=_pdfDoc->GetOutlineCount();i++)
						_childrens->Add(gcnew OutlineItem(_pdfDoc->GetOutlinePtr(i),this));
				}
				return _childrens;
			}
		}
		property System::Collections::Generic::List<PDFSearchResult ^> ^SearchResults
		{
			System::Collections::Generic::List<PDFSearchResult ^> ^get()
			{
				if(_searchResults==nullptr){
					_searchResults=gcnew System::Collections::Generic::List<PDFSearchResult ^>();
					
					SearchResultInterop *sr =_pdfDoc->GetSearchResults();
					long count =_pdfDoc->GetSearchCount();
					for(int i=0;i<count;i++)
						_searchResults->Add(gcnew PDFSearchResult(sr[i].Page,sr[i].x0,sr[i].y0,sr[i].x1,sr[i].y1));
				}
				return _searchResults;
			}

		}
		property System::String ^UserPassword{
			void set(System::String ^value){
				IntPtr ptr = Marshal::StringToCoTaskMemAnsi(value);
				char *singleByte= (char*)ptr.ToPointer();
				int ret;
				try{
					_pdfDoc->SetUserPassword(singleByte);
				}finally{
					Marshal::FreeCoTaskMem(ptr);
				}
					
			}
		}
		property System::String ^OwnerPassword{
			void set(System::String ^value){
				IntPtr ptr = Marshal::StringToCoTaskMemAnsi(value);
				char *singleByte= (char*)ptr.ToPointer();
				int ret;
				try{
					_pdfDoc->SetOwnerPassword(singleByte);
				}finally{
					Marshal::FreeCoTaskMem(ptr);
				}
					
			}
		}


		event PDFLoadCompletedHandler ^PDFLoadCompeted;
		event PDFLoadBeginHandler ^PDFLoadBegin;
		
		
		event RenderFinishedHandler ^RenderFinished{
			void add(RenderFinishedHandler ^ ev){
				this->_evRenderFinished += ev;
			}
			void remove(RenderFinishedHandler ^ev){
				this->_evRenderFinished -= ev;
			}
			 void raise() {
				 RenderFinishedHandler^ tmp = _evRenderFinished;
				 if (tmp) {
					return tmp->Invoke();
				 }
			  }

		}

		event RenderNotifyFinishedHandler ^RenderNotifyFinished{
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


		event ExportJpgProgressHandler ^ExportJpgProgress{
			void add(ExportJpgProgressHandler ^ ev){
				this->_evExportJpgProgress += ev;
			}
			void remove(ExportJpgProgressHandler ^ev){
				this->_evExportJpgProgress -= ev;
			}
			 bool raise(int a,int b) {
				 ExportJpgProgressHandler^ tmp = _evExportJpgProgress;
				 if (tmp) {
					return tmp->Invoke(a,b);
				 }
				 return false;
			  }

		}

		event ExportJpgProgressHandler ^ExportSwfProgress{
			void add(ExportJpgProgressHandler ^ ev){
				this->_evExportSwfProgress += ev;
			}
			void remove(ExportJpgProgressHandler ^ev){
				this->_evExportSwfProgress -= ev;
			}
			 bool raise(int a,int b) {
				 ExportJpgProgressHandler^ tmp = _evExportSwfProgress;
				 if (tmp) {
					return tmp->Invoke(a,b);
				 }
				 return false;
			  }

		}


		event ExportJpgFinishedHandler ^ExportSwfFinished{
			void add(ExportJpgFinishedHandler ^ev){
				_evExportSwfFinished+=ev;
			}
			void remove(ExportJpgFinishedHandler ^ev){
				_evExportSwfFinished-=ev;
			}
			void raise()
			{
				ExportJpgFinishedHandler ^tmp =_evExportSwfFinished;
				if(tmp)
					_evExportSwfFinished->Invoke();
			}

		}

		event ExportJpgFinishedHandler ^ExportJpgFinished{
			void add(ExportJpgFinishedHandler ^ev){
				_evExportJpgFinished+=ev;
			}
			void remove(ExportJpgFinishedHandler ^ev){
				_evExportJpgFinished-=ev;
			}
			void raise()
			{
				ExportJpgFinishedHandler ^tmp =_evExportJpgFinished;
				if(tmp)
					_evExportJpgFinished->Invoke();
			}

		}
	protected:	

		!PDFWrapper()
		{	
			if(_searchResults != nullptr)
				delete _searchResults;
			
			//Release managed resources
			GC::Collect();
			if(_gchProgress.IsAllocated)
				_gchProgress.Free();
			if(_gchFinished.IsAllocated)
				_gchFinished.Free();
			if(_gchSwfProgress.IsAllocated)
				_gchSwfProgress.Free();
			if(_gchSwfFinished.IsAllocated)
				_gchSwfFinished.Free();
			if(_binaryReader!=nullptr)
			{
				_binaryReader->Close();
				_binaryReader=nullptr;
			}

			_linksCache.Clear();

			for(int i=1; i <= _pages.Count; i++)
				delete _pages[i];

			delete _title;
			delete _author;
			delete _subject;
			delete _keywords;
			delete _creator;
			delete _producer;

			delete title;
			delete author;
			delete subject;
			delete keywords;
			delete creator;
			delete producer;
		}

		~PDFWrapper()
		{
			//Release unmanaged Resources			
			if(_pdfDoc!=0){
				//_pdfDoc->Dispose();   //Dispose is called by the destructor!
				delete _pdfDoc;			//So right here!
			}
		}

	};
}
