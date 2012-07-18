#include "PageLinksInterop.h"
#include "LinkInterop.h"
#include "AFPDFDoc.h"


PageLinksInterop::PageLinksInterop(void *lptr, AFPDFDocInterop *pdfdoc)
: _links(lptr)
, _pdfDoc(pdfdoc)
, _cache(0)
{
	//_cache=malloc(this->getLinkCount()*sizeof(LinkInterop));
	//memset(_cache,0,this->getLinkCount()*sizeof(LinkInterop));
}

PageLinksInterop::~PageLinksInterop(void)
{
	delete _links;
	//free(_cache);
}

LinkInterop *PageLinksInterop::getLink(int iLink){
	if(iLink>=0 && iLink<this->getLinkCount() && _links){
		fprintf(stderr,"New LinkInterop\n");
		LinkInterop *l=new LinkInterop(((Links *)_links)->getLink(iLink),_pdfDoc);
		return l;
	}
	return 0;
}
int PageLinksInterop::getLinkCount(){
	if(_links)
		return ((Links *)_links)->getNumLinks();
	return 0;
}
bool PageLinksInterop::onLink(double x, double y){
	if(_links)
		return ((Links *)_links)->onLink(x,y)?true:false;
	return false;
}

void PageLinksInterop::setPointers(int lptr,int pdfdoc){
	_pdfDoc=(AFPDFDocInterop *)pdfdoc;
	_links =(void *)lptr;
	
//	free(_cache);
//	_cache=malloc(this->getLinkCount()*sizeof(LinkInterop));
//	memset(_cache,0,this->getLinkCount()*sizeof(LinkInterop));
}
