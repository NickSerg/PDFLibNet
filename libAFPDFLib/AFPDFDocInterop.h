#pragma once
#include "OutlineItemInterop.h"
#include "SearchResultInterop.h"
#include "PageLinksInterop.h"
#include "PDFPageInterop.h"
#include "SaveSWFParams.h"

typedef int (__stdcall *NOTIFYHANDLE)();
typedef int (__stdcall *PROGRESSHANDLE)(int, int);

class AFPDFDocInterop 
{
private:
	void *_ptr;
	SearchResultInterop *_resultsPtr;
	PageLinksInterop _pageLinks;
public:
	AFPDFDocInterop(char *configFile);
	virtual ~AFPDFDocInterop();
	long LoadFromFile(char * sFileName);
	long LoadFromStream(void *callback,int lenght);
	long RenderPage(long lhWnd);
	long RenderPage(long lhWnd, bool bForce);
	long RenderPage(long lhWnd, bool bForce, bool enableThread);
	long RenderPageThread(long lhWnd, bool bForce);
	long DrawPage(int page, long hdc, int width, int height, double pdi, bool bThread, void *callback, bool bAntialising);
	bool ThumbInQueue(int page);
	void SetUserPassword(char *pass);
	void SetOwnerPassword(char *pass);
	void SetSliceBox(int x, int y, int w, int h);
	
	long SaveJpg(char *fileName,int firstPage, int lastPage,float renderDPI, int quality, int waitProc);
	long SaveTxt(char *fileName,int firstPage, int lastPage,bool physLayout, bool rawOrder,bool htmlMeta);
	long SaveHtml(char *outFileName, int firstPage, int lastPage, double zoom, bool noFrames, bool complexMode, bool htmlLinks,bool ignoreImages, bool outputHiddenText, char *encName, char *imgExt, int jpegQuality);
	long SaveXML(char *outFilename, int firstPage, int lastPage, char *encName);
	long SaveSWF(char *fileName, SaveSWFParams *params);

	void CancelJpgExport();
	void SetExportProgressHandler(void *);
	void SetExportFinishedHandler(void *);
	void SetRenderFinishedHandler(void *);
	void SetRenderNotifyFinishedHandler(void *);

	
	void CancelSwfExport();
	void SetExportSwfProgressHandler(void *);
	void SetExportSwfFinishedHandler(void *);
	//Returns true if there is a process running
	bool IsSwfBusy();

	//Returns true if there is a process running
	bool IsJpgBusy();
	//Returns true if there is a background thread rendering next page
	bool IsBusy();
	long GetCurrentPage(void);
	void SetCurrentPage(long newVal);
	void ZoomIN();
	void ZoomOUT();
	
	void NextPage();
	void PreviousPage();

	long GetCurrentX(void);
	void SetCurrentX(long newVal);
	long GetCurrentY(void);
	void SetCurrentY(long newVal);
	long GetViewWidth();
	long GetViewHeight();
	void SetViewWidth(long newVal);
	void SetViewHeight(long newVal);

	long GetViewX();
	void SetViewX(long newVal);
	long GetViewY();
	void SetViewY(long newVal);

	long GetPageCount(void);
	
	long FitScreenWidth(long lhWnd);
	long FitScreenHeight(long lhWnd);
	long GetPageWidth(void);
	long GetPageCropWidth(void);
	long GetPageHeight(void);
	long GetPageCropHeight(void);	
	long GetOutlineCount(void);
	long GetOutlinePtr(long iOutline);
	long ProcessLinkAction(long lPtrLinkAction);
	PageLinksInterop *getPageLinksInterop(long iPage);
	LinkDestInterop *findDest(char *destName);
	int findPage(int num, int gen);
	PDFPageInterop *getPage(int page);
	void Dispose();

	float GetRenderDPI(void);
	void SetRenderDPI(float newVal);
	wchar_t *GetPDFVersion(void);
	
	long FindString(const wchar_t *sText, long iPage, long SearchOrder, bool bCaseSensitive, bool bBackward, bool bMarkAll, bool bWholeDoc, bool bWholeWord, bool stopOnFirstPageResults);
	long FindNext(const wchar_t *sText);
	long FindPrior(const wchar_t *sText);
	long FindFirst(const wchar_t *sText, long SearchOrder, bool Backward, bool WholeWord);
	long RenderHDC(long lHdc);


	long GetSearchPage(void);
	long SetSearchPage(long lNewValue);
	bool GetSearchCaseSensitive(void);
	void SetSearchCaseSensitive(bool newVal);
	double GetZoom();
	void SetZoom(double newValue);
	long PrintToFile(const char *fileName,int fromPage,int toPage);

	SearchResultInterop *GetSearchResults();
	long GetSearchCount();

	void cvtUserToDev(double ux, double uy, int *dx, int *dy);
	void cvtDevToUser(double ux, double uy, double *dx, double *dy);

	wchar_t *GetTitle();
	wchar_t *GetAuthor();
	wchar_t *GetSubject();
	wchar_t *GetKeywords();
	wchar_t *GetCreator();
	wchar_t *GetProducer();
	char *GetCreationDate();
	char *GetLastModifiedDate();
	
	bool GetSupportsMuPDF();
	bool GetUseMuPDF();
	void SetUseMuPDF(bool bUse);
};

