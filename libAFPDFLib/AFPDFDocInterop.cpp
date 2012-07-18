#pragma once
#include "AFPDFDocInterop.h"
#include "AFPDFDoc.h"

AFPDFDocInterop::AFPDFDocInterop(char *configFile)
: _ptr(new AFPDFDoc(configFile))
, _resultsPtr(0)
, _pageLinks(0,0)
{
}
AFPDFDocInterop::~AFPDFDocInterop(){
	if(_ptr>0)
		delete (AFPDFDoc *)_ptr;	//when deleting cast _ptr to right type, so that 
									//destructor of AFPDFDoc is called!
	_ptr=0;
	if(_resultsPtr!=0)
		free(_resultsPtr);
}

long AFPDFDocInterop::LoadFromFile(char * sFileName){
	return ((AFPDFDoc *)_ptr)->LoadFromFile(sFileName);
}

long AFPDFDocInterop::LoadFromStream(void *callback,int lenght){
	return ((AFPDFDoc *)_ptr)->LoadFromStream(callback,lenght,0,0);
}
long AFPDFDocInterop::RenderPage(long lhWnd, bool bForce){
	return ((AFPDFDoc *)_ptr)->RenderPage(lhWnd,bForce);
}

long AFPDFDocInterop::RenderPage(long lhWnd, bool bForce, bool enableThread){
	return ((AFPDFDoc *)_ptr)->RenderPage(lhWnd,bForce,enableThread);
}

long AFPDFDocInterop::RenderPageThread(long lhWnd, bool bForce){
	return ((AFPDFDoc *)_ptr)->RenderPageThread(lhWnd,bForce);
}
long AFPDFDocInterop::RenderPage(long lhWnd){
	return ((AFPDFDoc *)_ptr)->RenderPage(lhWnd);
}
long AFPDFDocInterop::GetCurrentX(void){
	return ((AFPDFDoc *)_ptr)->GetCurrentX();
}
void AFPDFDocInterop::SetCurrentX(long newVal){
	((AFPDFDoc *)_ptr)->SetCurrentX(newVal);
}
long AFPDFDocInterop::GetCurrentY(void){
	return ((AFPDFDoc *)_ptr)->GetCurrentY();
}
void AFPDFDocInterop::SetCurrentY(long newVal){
	((AFPDFDoc *)_ptr)->SetCurrentY(newVal);
}
void AFPDFDocInterop::ZoomIN(){
	((AFPDFDoc *)_ptr)->ZoomIN();
}
void AFPDFDocInterop::ZoomOUT(){
	((AFPDFDoc *)_ptr)->ZoomOut();
}

long AFPDFDocInterop::GetCurrentPage(void){
	return ((AFPDFDoc *)_ptr)->GetCurrentPage();
}
void AFPDFDocInterop::SetCurrentPage(long newVal){
	return ((AFPDFDoc *)_ptr)->SetCurrentPage(newVal);
}

long AFPDFDocInterop::GetPageCount(void){
	return ((AFPDFDoc *)_ptr)->GetPageCount();
}
long AFPDFDocInterop::FitScreenWidth(long lhWnd){
	return ((AFPDFDoc *)_ptr)->FitScreenWidth(lhWnd);
}
long AFPDFDocInterop::FitScreenHeight(long lhWnd){
	return ((AFPDFDoc *)_ptr)->FitScreenHeight(lhWnd);
}
long AFPDFDocInterop::GetPageWidth(void){
	return ((AFPDFDoc *)_ptr)->GetPageWidth();
}
long AFPDFDocInterop::GetPageCropWidth(void)
{
	return ((AFPDFDoc *)_ptr)->GetPageCropWidth();
}
long AFPDFDocInterop::GetPageHeight(void){
	return ((AFPDFDoc *)_ptr)->GetPageHeight();
}
long AFPDFDocInterop::GetPageCropHeight(void)
{
	return ((AFPDFDoc *)_ptr)->GetPageCropHeight();
}

long AFPDFDocInterop::GetOutlineCount(void){
	return ((AFPDFDoc *)_ptr)->GetOutlineCount();
}
long AFPDFDocInterop::GetOutlinePtr(long iOutline){
	return ((AFPDFDoc *)_ptr)->GetOutlinePtr(iOutline);
}
long AFPDFDocInterop::ProcessLinkAction(long lPtrLinkAction){
	return ((AFPDFDoc *)_ptr)->ProcessLinkAction(lPtrLinkAction);
}	
void AFPDFDocInterop::Dispose(){
	((AFPDFDoc *)_ptr)->Dispose();
}
float AFPDFDocInterop::GetRenderDPI()
{
	return ((AFPDFDoc *)_ptr)->GetRenderDPI();
}
void AFPDFDocInterop::SetRenderDPI(float newVal){
	((AFPDFDoc *)_ptr)->SetRenderDPI(newVal);
}
wchar_t *AFPDFDocInterop::GetPDFVersion(void){
	return ((AFPDFDoc *)_ptr)->GetPDFVersion();
}
long AFPDFDocInterop::FindString(const wchar_t *sText, long iPage, long SearchOrder, bool bCaseSensitive, bool bBackward, bool bMarkAll, bool bWholeDoc,bool bWholeWord, bool stopOnFirstPageResults){
	return ((AFPDFDoc *)_ptr)->FindText(sText,iPage,SearchOrder,bCaseSensitive,bBackward,bMarkAll,bWholeDoc,bWholeWord,stopOnFirstPageResults);
}
long AFPDFDocInterop::FindNext(const wchar_t *sText){
	return ((AFPDFDoc *)_ptr)->FindNext(sText);
}
long AFPDFDocInterop::FindPrior(const wchar_t *sText){
	return ((AFPDFDoc *)_ptr)->FindPrior(sText);
}
long AFPDFDocInterop::FindFirst(const wchar_t *sText, long SearchOrder, bool Backward, bool WholeWord){
	return ((AFPDFDoc *)_ptr)->FindFirst(sText,SearchOrder,Backward, WholeWord);
}
long AFPDFDocInterop::RenderHDC(long lHdc){
	return ((AFPDFDoc *)_ptr)->RenderHDC(lHdc);
}
long AFPDFDocInterop::GetSearchPage(void){
	return ((AFPDFDoc *)_ptr)->GetSearchPage();
}
long AFPDFDocInterop::SetSearchPage(long lNewValue){
	return ((AFPDFDoc *)_ptr)->SetSearchPage(lNewValue);
}
bool AFPDFDocInterop::GetSearchCaseSensitive(void){
	return ((AFPDFDoc *)_ptr)->GetSearchCaseSensitive();
}
void AFPDFDocInterop::SetSearchCaseSensitive(bool newVal){
	((AFPDFDoc *)_ptr)->SetSearchCaseSensitive(newVal);
}

 void AFPDFDocInterop::NextPage(){
	((AFPDFDoc *)_ptr)->NextPage();
}
 void AFPDFDocInterop::PreviousPage(){
	((AFPDFDoc *)_ptr)->PreviousPage();
}

 SearchResultInterop *AFPDFDocInterop::GetSearchResults(){
	 CPDFSearchResult *res=((AFPDFDoc *)_ptr)->GetSearchResults();
	 long count = ((AFPDFDoc *)_ptr)->GetSearchCount();
	 if(_resultsPtr!=0)
		 free(_resultsPtr);
	 _resultsPtr = (SearchResultInterop *)malloc(count*sizeof(SearchResultInterop));
	 for(int i=0;i<count;i++){
		 SearchResultInterop *item=new SearchResultInterop(res[i].PageFound,res[i].left,res[i].top,res[i].right,res[i].bottom);
		 _resultsPtr[i]=*item;
	 }
	 return (SearchResultInterop *)_resultsPtr;
 }

 long AFPDFDocInterop::GetSearchCount(){
	 return ((AFPDFDoc *)_ptr)->GetSearchCount();
 }

 long AFPDFDocInterop::PrintToFile(const char *fileName,int fromPage,int toPage){
	 return ((AFPDFDoc *)_ptr)->PrintToFile(fileName,fromPage,toPage);
 }

 double AFPDFDocInterop::GetZoom(){
	return  (double)((AFPDFDoc *)_ptr)->GetRenderDPI();	 
 }
 void AFPDFDocInterop::SetZoom(double newValue){
	((AFPDFDoc *)_ptr)->SetRenderDPI((FLOAT)newValue);
 }

 wchar_t * AFPDFDocInterop::GetTitle(){
	return ((AFPDFDoc *)_ptr)->getTitle();
 }

 wchar_t * AFPDFDocInterop::GetAuthor(){
	return ((AFPDFDoc *)_ptr)->getAuthor();
 }

 PageLinksInterop *AFPDFDocInterop::getPageLinksInterop(long iPage){
	 if(iPage>=0 && iPage<this->GetPageCount()){
		PageLinksInterop *pl =new PageLinksInterop(((AFPDFDoc *)_ptr)->GetLinksPage(iPage),this);
		return pl;
	 }
	 return 0;
 }

 void AFPDFDocInterop::cvtUserToDev(double ux, double uy, int *dx, int *dy){
		((AFPDFDoc *)_ptr)->cvtUserToDev(ux,uy,dx,dy);	
	}

	void AFPDFDocInterop::cvtDevToUser(double ux, double uy, double *dx, double *dy){
		((AFPDFDoc *)_ptr)->cvtDevToUser(ux,uy,dx,dy);
	}

	long AFPDFDocInterop::SaveJpg(char *fileName,int firstPage, int lastPage,float renderDPI, int quality, int waitProc){
		return ((AFPDFDoc *)_ptr)->SaveJpg(fileName,renderDPI,firstPage,lastPage,quality,waitProc);
	}
	long AFPDFDocInterop::SaveSWF(char *fileName, SaveSWFParams *params)
	{
		return ((AFPDFDoc  *)_ptr)->SaveSWF(fileName, params);
	}
	
	long AFPDFDocInterop::SaveTxt(char *fileName,int firstPage, int lastPage,bool physLayout, bool rawOrder,bool htmlMeta){
		return ((AFPDFDoc *)_ptr)->SaveTxt(fileName,firstPage,lastPage,htmlMeta,physLayout,rawOrder);
	}

	long AFPDFDocInterop::SaveHtml(char *outFileName, int firstPage, int lastPage, 
		double zoom, bool noFrames, bool complexMode, bool htmlLinks,bool ignoreImages, bool outputHiddenText, char *encName, char *imgExt, int jpegQuality)
	{
		return ((AFPDFDoc *)_ptr)->SaveHtml(outFileName,firstPage,lastPage,zoom,noFrames,complexMode, htmlLinks,ignoreImages, outputHiddenText, encName, imgExt, jpegQuality);
	}

	wchar_t *AFPDFDocInterop::GetSubject(){ return ((AFPDFDoc *)_ptr)->getSubject(); }
	wchar_t *AFPDFDocInterop::GetKeywords(){ return ((AFPDFDoc *)_ptr)->getKeywords(); }
	wchar_t *AFPDFDocInterop::GetCreator(){ return ((AFPDFDoc *)_ptr)->getCreator(); }
	wchar_t *AFPDFDocInterop::GetProducer(){ return ((AFPDFDoc *)_ptr)->getProducer(); }
	char *AFPDFDocInterop::GetCreationDate(){ return ((AFPDFDoc *)_ptr)->getCreationDate(); }
	char *AFPDFDocInterop::GetLastModifiedDate(){ return ((AFPDFDoc *)_ptr)->getLastModifiedDate(); }

	void AFPDFDocInterop::SetUserPassword(char *pass){
		((AFPDFDoc *)_ptr)->SetUserPassword(pass);
	}
	void AFPDFDocInterop::SetOwnerPassword(char *pass){
		((AFPDFDoc *)_ptr)->SetOwnerPassword(pass);
	}

	long AFPDFDocInterop::GetViewWidth(){
		return ((AFPDFDoc *)_ptr)->GetViewWidth();
	}
	long AFPDFDocInterop::GetViewHeight(){
		return ((AFPDFDoc *)_ptr)->GetViewHeight();
	}
	void AFPDFDocInterop::SetViewWidth(long newVal){
		((AFPDFDoc *)_ptr)->SetViewWidth(newVal);
	}
	void AFPDFDocInterop::SetViewHeight(long newVal){
		((AFPDFDoc *)_ptr)->SetViewHeight(newVal);
	}

	long AFPDFDocInterop::GetViewX(){
		return ((AFPDFDoc *)_ptr)->GetViewX();
	}
	void AFPDFDocInterop::SetViewX(long newVal){
		((AFPDFDoc *)_ptr)->SetViewX(newVal);
	}
	long AFPDFDocInterop::GetViewY(){
		return ((AFPDFDoc *)_ptr)->GetViewY();
	}
	void AFPDFDocInterop::SetViewY(long newVal){
		((AFPDFDoc *)_ptr)->SetViewY(newVal);
	}
	LinkDestInterop *AFPDFDocInterop::findDest(char *destName)
	{
		return new LinkDestInterop( ((AFPDFDoc *)_ptr)->findDest(destName),this);
	}

	int AFPDFDocInterop::findPage(int num, int gen)
	{
		return ((AFPDFDoc *)_ptr)->findpage(num,gen);
	}

	void AFPDFDocInterop::SetExportProgressHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_ExportProgressHandle = static_cast<PROGRESSHANDLE>(handler);
	}
	void AFPDFDocInterop::SetExportFinishedHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_ExportFinishHandle = static_cast<NOTIFYHANDLE>(handler);
	}

	void AFPDFDocInterop::SetRenderFinishedHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_RenderFinishHandle = static_cast<NOTIFYHANDLE>(handler);
	}
	void AFPDFDocInterop::SetRenderNotifyFinishedHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_RenderNotifyFinishHandle = static_cast<PAGERENDERNOTIFY>(handler);
	}

	void AFPDFDocInterop::SetExportSwfFinishedHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_ExportSwfFinishHandle = static_cast<NOTIFYHANDLE>(handler);
	}
	
	void AFPDFDocInterop::SetExportSwfProgressHandler(void *handler){
		((AFPDFDoc *)_ptr)->m_ExportSwfProgressHandle = static_cast<PROGRESSHANDLE>(handler);
	}
	
	void AFPDFDocInterop::CancelSwfExport()
	{
		((AFPDFDoc *)_ptr)->CancelSwfSave();
	}

	void AFPDFDocInterop::CancelJpgExport(){
		((AFPDFDoc *)_ptr)->CancelJpgSave();
	}

	//Returns true if there is a process running
	bool AFPDFDocInterop::IsSwfBusy(){
		return ((AFPDFDoc *)_ptr)->SwfIsBusy();
	}
	//Returns true if there is a process running
	bool AFPDFDocInterop::IsJpgBusy(){
		return ((AFPDFDoc *)_ptr)->JpgIsBusy();
	}
	//Returns true if there is a background thread rendering next page
	bool AFPDFDocInterop::IsBusy(){
		return ((AFPDFDoc *)_ptr)->IsBusy();
	}

	void AFPDFDocInterop::SetSliceBox(int x, int y, int w, int h){
		((AFPDFDoc *)_ptr)->SetSliceBox(x,y,w,h);
	}
	PDFPageInterop *AFPDFDocInterop::getPage(int page){
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		Page * p = doc->getDoc()->getCatalog()->getPage(page);
		if(p)
			return new PDFPageInterop(page,(void *)p,(void *)_ptr);
		return 0;
	}

	long AFPDFDocInterop::DrawPage(int page, long hdc, int width, int height, double pdi, bool bThread, void *callback, bool bAntialising)
	{
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		return doc->DrawPage(page,hdc,width,height,pdi,bThread,callback, bAntialising);
	}

	bool AFPDFDocInterop::ThumbInQueue(int page){
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		return doc->ThumbInQueue(page);
	}

	bool AFPDFDocInterop::GetSupportsMuPDF()
	{
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		return doc->SupportsMuPDF();
	}
	bool AFPDFDocInterop::GetUseMuPDF()
	{
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		return doc->GetUseMuPDF();
	}
	void AFPDFDocInterop::SetUseMuPDF(bool bUse)
	{
		AFPDFDoc *doc =((AFPDFDoc *)_ptr);
		doc->SetUseMuPDF(bUse);
	}