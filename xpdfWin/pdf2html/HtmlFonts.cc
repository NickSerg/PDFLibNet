#include "HtmlFonts.h"
#include "GlobalParams.h"
#include "UnicodeMap.h"
#include <stdio.h>

#ifndef _WIN32
#include <unistd.h>
#endif

 struct Fonts{
    char *Fontname;
    char *name;
  };

const int font_num=13;

static Fonts fonts[font_num+1]={  
     {"Courier",               "Courier" },
     {"Courier-Bold",           "Courier"},
     {"Courier-BoldOblique",    "Courier"},
     {"Courier-Oblique",        "Courier"},
     {"Helvetica",              "Helvetica"},
     {"Helvetica-Bold",         "Helvetica"},
     {"Helvetica-BoldOblique",  "Helvetica"},
     {"Helvetica-Oblique",      "Helvetica"},
     {"Symbol",                 "Symbol"   },
     {"Times-Bold",             "Times"    },
     {"Times-BoldItalic",       "Times"    },
     {"Times-Italic",           "Times"    },
     {"Times-Roman",            "Times"    },
     {" "          ,            "Times"    },
};

#define xoutRound(x) ((int)(x + 0.5))
extern GBool xml;

GString* HtmlFont::DefaultFont=new GString("Times"); // Arial,Helvetica,sans-serif

HtmlFontColor::HtmlFontColor(GfxRGB rgb){
 /*
  r=static_cast<int>(255*rgb.r);
  g=static_cast<int>(255*rgb.g);
  b=static_cast<int>(255*rgb.b);
*/
  r=colToByte(rgb.r);
  g=colToByte(rgb.g);
  b=colToByte(rgb.b);
  if (!(Ok(r)&&Ok(b)&&Ok(g))) {printf("Error : Bad color \n");r=0;g=0;b=0;}

}

GString *HtmlFontColor::convtoX(unsigned int xcol) const{
  GString *xret=new GString();
  char tmp;
  unsigned  int k;
  k = (xcol/16);
  if ((k>=0)&&(k<10)) tmp=(char) ('0'+k); else tmp=(char)('a'+k-10);
  xret->append(tmp);
  k = (xcol%16);
  if ((k>=0)&&(k<10)) tmp=(char) ('0'+k); else tmp=(char)('a'+k-10);
  xret->append(tmp);
 return xret;
}

GString *HtmlFontColor::toString() const{
  GString *tmp=new GString("#");
  GString *tmpr=convtoX(r); 
  GString *tmpg=convtoX(g);
  GString *tmpb=convtoX(b);
  tmp->append(tmpr);
  tmp->append(tmpg);
  tmp->append(tmpb);
  delete tmpr;
  delete tmpg;
  delete tmpb;
  return tmp;
} 

HtmlFont::HtmlFont(GString* ftname,int _size, double _charspace, GfxRGB rgb){
  //if (col) color=HtmlFontColor(col); 
  //else color=HtmlFontColor();
  color=HtmlFontColor(rgb);

  GString *fontname = NULL;
  charspace = _charspace;

  if( ftname ){
    fontname = new GString(ftname);
    FontName=new GString(ftname);
  }
  else {
    fontname = NULL;
    FontName = NULL;
  }
  
  lineSize = -1;

  size=(_size-1);
  italic = gFalse;
  bold = gFalse;
  oblique = gFalse;

  if (fontname){
    if (strstr(fontname->lowerCase()->getCString(),"bold"))  bold=gTrue;    
    if (strstr(fontname->lowerCase()->getCString(),"italic")) italic=gTrue;
    if (strstr(fontname->lowerCase()->getCString(),"oblique")) oblique=gTrue;
    /*||strstr(fontname->lowerCase()->getCString(),"oblique"))  italic=gTrue;*/ 
    
    int i=0;
    while (strcmp(ftname->getCString(),fonts[i].Fontname)&&(i<font_num)) 
	{
		i++;
	}
    pos=i;
    delete fontname;
  }  
  if (!DefaultFont) DefaultFont=new GString(fonts[font_num].name);

}
 
HtmlFont::HtmlFont(const HtmlFont& x){
   size=x.size;
   lineSize=x.lineSize;
   italic=x.italic;
   oblique=x.oblique;
   bold=x.bold;
   pos=x.pos;
   charspace=x.charspace;
   color=x.color;
   if (x.FontName) FontName=new GString(x.FontName);
 }


HtmlFont::~HtmlFont(){
  if (FontName) delete FontName;
}

HtmlFont& HtmlFont::operator=(const HtmlFont& x){
   if (this==&x) return *this; 
   size=x.size;
   lineSize=x.lineSize;
   italic=x.italic;
   oblique=x.oblique;
   bold=x.bold;
   pos=x.pos;
   color=x.color;
   charspace=x.charspace;
   if (FontName) delete FontName;
   if (x.FontName) FontName=new GString(x.FontName);
   return *this;
}

void HtmlFont::clear(){
  if(DefaultFont) delete DefaultFont;
  DefaultFont = NULL;
}



/*
  This function is used to compare font uniquily for insertion into
  the list of all encountered fonts
*/
GBool HtmlFont::isEqual(const HtmlFont& x) const{
  return ((size==x.size) && (lineSize==x.lineSize) && (charspace==x.charspace) &&
	  (pos==x.pos) && (bold==x.bold) && (oblique==x.oblique) && (italic==x.italic) &&
	  (color.isEqual(x.getColor())));
}

/*
  This one is used to decide whether two pieces of text can be joined together
  and therefore we don't care about bold/italics properties
*/
GBool HtmlFont::isEqualIgnoreBold(const HtmlFont& x) const{
  return ((size==x.size) &&
	  (color.isEqual(x.getColor())));
}

GString* HtmlFont::getFontName(){
   if (pos!=font_num) return new GString(fonts[pos].name);
    else return new GString(DefaultFont);
}

GString* HtmlFont::getFullName(){
  if (FontName)
    return new GString(FontName);
  else return new GString(DefaultFont);
} 

void HtmlFont::setDefaultFont(GString* defaultFont){
  if (DefaultFont) delete DefaultFont;
  DefaultFont=new GString(defaultFont);
}


GString* HtmlFont::getDefaultFont(){
  return DefaultFont;
}

// this method if plain wrong todo
GString* HtmlFont::HtmlFilter(Unicode* u, int uLen) {
  GString *tmp = new GString();
  UnicodeMap *uMap;
  char buf[8];
  int n;

  // get the output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    return tmp;
  }

  for (int i = 0; i < uLen; ++i) {
    switch (u[i])
      { 
	case '"': tmp->append("&quot;");  break;
	case '&': tmp->append("&amp;");  break;
	case '<': tmp->append("&lt;");  break;
	case '>': tmp->append("&gt;");  break;
	default:  
	  {
	    // convert unicode to string
	    if ((n = uMap->mapUnicode(u[i], buf, sizeof(buf))) > 0) {
	      tmp->append(buf, n); 
	  }
      }
    }
  }

  uMap->decRefCnt();
  return tmp;
}

GString* HtmlFont::simple(HtmlFont* font, Unicode* content, int uLen){
  GString *cont=HtmlFilter (content, uLen); 

  /*if (font.isBold()) {
    cont->insert(0,"<b>",3);
    cont->append("</b>",4);
  }
  if (font.isItalic()) {
    cont->insert(0,"<i>",3);
    cont->append("</i>",4);x.oblique
    } */

  return cont;
}

HtmlFontAccu::HtmlFontAccu(){
  accu=new GVector<HtmlFont>();
}

HtmlFontAccu::~HtmlFontAccu(){
  if (accu) delete accu;
}

int HtmlFontAccu::AddFont(const HtmlFont& font){
 GVector<HtmlFont>::iterator i; 
 for (i=accu->begin();i!=accu->end();i++)
 {
	if (font.isEqual(*i)) 
	{
		return (int)(i-(accu->begin()));
	}
 }

 accu->push_back(font);
 return (accu->size()-1);
}

// get CSS font name for font #i 
GString* HtmlFontAccu::getCSStyle(int i, GString* content){
  GString *tmp;
  GString *iStr=GString::fromInt(i);
  
  if (!xml) {
    tmp = new GString("<span class=\"ft");
    tmp->append(iStr);
    tmp->append("\">");
    tmp->append(content);
    tmp->append("</span>");
  } else {
    tmp = new GString("");
    tmp->append(content);
  }

  delete iStr;
  return tmp;
}

// get CSS font definition for font #i 
GString* HtmlFontAccu::CSStyle(int i){
   GString *tmp=new GString();
   GString *iStr=GString::fromInt(i);

   GVector<HtmlFont>::iterator g=accu->begin();
   g+=i;
   HtmlFont font=*g;
   GString *Size=GString::fromInt(font.getSize());
   GString *colorStr=font.getColor().toString();
   GString *fontName=font.getFontName();
   GString *lSize;
   double _charspace = font.getCharSpace();
   char *cspace = new char [20];
   sprintf(cspace,"%0.05f",_charspace);
   if(!xml){
     tmp->append(".ft");
     tmp->append(iStr);
     tmp->append("{virtical-align:top;font-size:");
     tmp->append(Size);
     if( font.getLineSize() != -1 )
     {  // char *cspace = galloc(
	 lSize = GString::fromInt(font.getLineSize());
//	 tmp->append("px;line-height:");
	 tmp->append("px;line-height:");
	 tmp->append(lSize);
	 delete lSize;
     }     
//     tmp->append("px;font-family:");
     tmp->append("px;font-family:");
     tmp->append(fontName); //font.getFontName());
     tmp->append(";color:");
     tmp->append(colorStr);
     if (font.isBold() == gTrue)
     {
        tmp->append(";font-weight:bold");
     }
     if (font.isItalic() == gTrue)
     {
	tmp->append(";font-style:italic");
     }
     if (font.isOblique() == gTrue)
     {
	tmp->append(";font-style:oblique");
     }
     tmp->append(";letter-spacing:");
     tmp->append(cspace);  
     tmp->append("px;}");   
//     tmp->append("px;}");
     
   }
   if (xml) {
     tmp->append("<fontspec id=\"");
     tmp->append(iStr);
     tmp->append("\" size=\"");
     tmp->append(Size);
     tmp->append("\" family=\"");
     tmp->append(fontName); //font.getFontName());
     tmp->append("\" color=\"");
     tmp->append(colorStr);
     tmp->append("\"/>");
   }

   delete fontName;
   delete colorStr;
   delete iStr;
   delete Size;
   delete cspace;
   return tmp;
}
 

