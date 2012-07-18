/* polyops.c

   Part of the swftools package.

   Copyright (c) 2008 Matthias Kramm <kramm@quiss.org> 
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <memory.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../mem.h"
#include "../gfxdevice.h"
#include "../gfxtools.h"
#include "../gfxpoly.h"
#include "../log.h"
#include "polyops.h"

typedef struct _clip {
    gfxpoly_t*poly;
    int openclips;
    struct _clip*next;
} clip_t;

typedef struct _internal {
    gfxdevice_t*out;
    clip_t*clip;
    gfxpoly_t*polyunion;
    
    int good_polygons;
    int bad_polygons;
} internal_t;

static int verbose = 0;

static void dbg(char*format, ...)
{
    if(!verbose)
	return;
    char buf[1024];
    int l;
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(buf, sizeof(buf)-1, format, arglist);
    va_end(arglist);
    l = strlen(buf);
    while(l && buf[l-1]=='\n') {
	buf[l-1] = 0;
	l--;
    }
    printf("(device-polyops) %s\n", buf);
    fflush(stdout);
}

int polyops_setparameter(struct _gfxdevice*dev, const char*key, const char*value)
{
    dbg("polyops_setparameter");
    internal_t*i = (internal_t*)dev->internal;
    if(i->out) return i->out->setparameter(i->out,key,value);
    else return 0;
}

void polyops_startpage(struct _gfxdevice*dev, int width, int height)
{
    dbg("polyops_startpage");
    internal_t*i = (internal_t*)dev->internal;
    if(i->out) i->out->startpage(i->out,width,height);
}

void polyops_startclip(struct _gfxdevice*dev, gfxline_t*line)
{
    dbg("polyops_startclip");
    internal_t*i = (internal_t*)dev->internal;

    gfxpoly_t* oldclip = i->clip?i->clip->poly:0;
    gfxpoly_t* poly = gfxpoly_from_fill(line, DEFAULT_GRID);
    if(poly) 
        i->good_polygons++;
    else
        i->bad_polygons++;

    gfxpoly_t* currentclip = 0;
    int type = 0;

    /* we can't rely on gfxpoly actually being able to convert
       a gfxline into a gfxpoly- for polygons which are too
       complex or just degenerate, this might fail. So handle
       all the cases where polygon conversion or intersection
       might go awry 
       UPDATE: this is not needed anymore. The new gfxpoly
       implementation is stable enough so it always returns
       a valid result. Still, it's good practice.
     */
    if(!poly && !oldclip) {
	i->out->startclip(i->out,line);
	currentclip = 0;
	type = 1;
    } else if(!poly && oldclip) {
	gfxline_t*oldclipline = gfxline_from_gfxpoly(oldclip);
	i->out->startclip(i->out,oldclipline);
	i->out->startclip(i->out,line);
	currentclip = 0;
	type = 2;
    } else if(poly && oldclip) {
	gfxpoly_t*intersection = gfxpoly_intersect(poly, oldclip);
	if(intersection) {
            i->good_polygons++;
	    // this case is what usually happens 
	    gfxpoly_destroy(poly);poly=0;
	    currentclip = intersection;
	    type = 0;
	} else {
            i->bad_polygons++;
	    gfxline_t*oldclipline = gfxline_from_gfxpoly(oldclip);
	    i->out->startclip(i->out, oldclipline);
	    currentclip = poly;
	    type = 1;
	}
    } else if(poly && !oldclip) {
	currentclip = poly;
	type = 0;
    }

    clip_t*n = i->clip;
    i->clip = (clip_t*)rfx_calloc(sizeof(clip_t));
    i->clip->next = n;
    i->clip->poly = currentclip;
    i->clip->openclips = type;
}

void polyops_endclip(struct _gfxdevice*dev)
{
    dbg("polyops_endclip");
    internal_t*i = (internal_t*)dev->internal;

    if(!i->clip) {
	msg("<error> endclip without startclip (in: polyops)\n");
	return;
    }

    clip_t*old = i->clip;
    i->clip = i->clip->next;
    if(old->poly) {
	gfxpoly_destroy(old->poly);old->poly = 0;
    }
    int t;
    for(t=0;t<old->openclips;t++)
	i->out->endclip(i->out);

    old->next = 0;free(old);
}

static void addtounion(struct _gfxdevice*dev, gfxpoly_t*poly)
{
    internal_t*i = (internal_t*)dev->internal;
    if(poly && i->polyunion) {
	gfxpoly_t*old = i->polyunion;
	gfxpoly_t*newpoly = gfxpoly_union(poly,i->polyunion);
	i->polyunion = newpoly;
	gfxpoly_destroy(old);
    }
}

static gfxline_t* handle_poly(gfxdevice_t*dev, gfxpoly_t*poly, char*ok)
{
    internal_t*i = (internal_t*)dev->internal;
    if(i->clip && i->clip->poly) {
	gfxpoly_t*old = poly;
	if(poly) {
	    poly = gfxpoly_intersect(poly, i->clip->poly);
	    gfxpoly_destroy(old);
	}
    }

    if(poly) 
        i->good_polygons++;
    else
        i->bad_polygons++;

    addtounion(dev, poly);
    gfxline_t*gfxline = 0;
    if(poly) {
	// this is the case where everything went right
	gfxline_t*line = gfxline_from_gfxpoly(poly);
	gfxpoly_destroy(poly);
        *ok = 1;
	return line;
    } else {
	if(i->clip && i->clip->poly) {
	    /* convert current clipping from a polygon to an
	       actual "startclip" written to the output */
	    assert(i->clip->openclips <= 1);
	    gfxline_t*clipline = gfxline_from_gfxpoly(i->clip->poly);
	    i->out->startclip(i->out, clipline);
	    gfxline_free(clipline);
	    gfxpoly_destroy(i->clip->poly);i->clip->poly = 0;
	    i->clip->openclips++;
	    return 0;
	} else {
	    return 0;
	}
    }
}

void polyops_stroke(struct _gfxdevice*dev, gfxline_t*line, gfxcoord_t width, gfxcolor_t*color, gfx_capType cap_style, gfx_joinType joint_style, gfxcoord_t miterLimit)
{
    dbg("polyops_stroke");
    internal_t*i = (internal_t*)dev->internal;

    gfxpoly_t* poly = gfxpoly_from_stroke(line, width, cap_style, joint_style, miterLimit, DEFAULT_GRID);
    char ok = 0;
    gfxline_t*line2 = handle_poly(dev, poly, &ok);

    if(ok) {
	if(i->out && line2) i->out->fill(i->out, line2, color);
	gfxline_free(line2);
    } else {
        msg("<error> ..");
	if(i->out) i->out->stroke(i->out, line, width, color, cap_style, joint_style, miterLimit);
    }
}

void polyops_fill(struct _gfxdevice*dev, gfxline_t*line, gfxcolor_t*color)
{
    dbg("polyops_fill");
    internal_t*i = (internal_t*)dev->internal;

    gfxpoly_t*poly = gfxpoly_from_fill(line, DEFAULT_GRID);
    char ok = 0;
    gfxline_t*line2 = handle_poly(dev, poly, &ok);

    if(ok) {
	if(i->out && line2) i->out->fill(i->out, line2, color);
	gfxline_free(line2);
    } else {
	if(i->out) i->out->fill(i->out, line, color);
    }
}

void polyops_fillbitmap(struct _gfxdevice*dev, gfxline_t*line, gfximage_t*img, gfxmatrix_t*matrix, gfxcxform_t*cxform)
{
    dbg("polyops_fillbitmap");
    internal_t*i = (internal_t*)dev->internal;
    
    gfxpoly_t*poly = gfxpoly_from_fill(line, DEFAULT_GRID);
    char ok = 0;
    gfxline_t*line2 = handle_poly(dev, poly, &ok);

    if(ok) {
	if(i->out && line2) i->out->fillbitmap(i->out, line2, img, matrix, cxform);
	gfxline_free(line2);
    } else {
	if(i->out) i->out->fillbitmap(i->out, line, img, matrix, cxform);
    }
}

void polyops_fillgradient(struct _gfxdevice*dev, gfxline_t*line, gfxgradient_t*gradient, gfxgradienttype_t type, gfxmatrix_t*matrix)
{
    dbg("polyops_fillgradient");
    internal_t*i = (internal_t*)dev->internal;
    
    gfxpoly_t*poly = gfxpoly_from_fill(line, DEFAULT_GRID);
    char ok = 0;
    gfxline_t*line2 = handle_poly(dev, poly, &ok);

    if(ok) {
	if(i->out && line2) i->out->fillgradient(i->out, line2, gradient, type, matrix);
	gfxline_free(line2);
    } else {
	if(i->out) i->out->fillgradient(i->out, line, gradient, type, matrix);
    }
}

void polyops_addfont(struct _gfxdevice*dev, gfxfont_t*font)
{
    dbg("polyops_addfont");
    internal_t*i = (internal_t*)dev->internal;
    if(i->out) i->out->addfont(i->out, font);
}

void polyops_drawchar(struct _gfxdevice*dev, gfxfont_t*font, int glyphnr, gfxcolor_t*color, gfxmatrix_t*matrix)
{
    dbg("polyops_drawchar");
    if(!font)
	return;
    internal_t*i = (internal_t*)dev->internal;
    gfxline_t*glyph = gfxline_clone(font->glyphs[glyphnr].line);
    gfxline_transform(glyph, matrix);

    if(i->clip && i->clip->poly) {
	gfxbbox_t bbox = gfxline_getbbox(glyph);
	gfxpoly_t*dummybox = gfxpoly_createbox(bbox.xmin,bbox.ymin,bbox.xmax,bbox.ymax, DEFAULT_GRID);
	gfxline_t*dummybox2 = gfxline_from_gfxpoly(dummybox);
	bbox = gfxline_getbbox(dummybox2);
	gfxline_free(dummybox2);

        char ok=0;
	gfxline_t*gfxline = handle_poly(dev, dummybox, &ok);
	if(ok) {
	    gfxbbox_t bbox2 = gfxline_getbbox(gfxline);
	    double w = bbox2.xmax - bbox2.xmin;
	    double h = bbox2.ymax - bbox2.ymin;
	    if(fabs((bbox.xmax - bbox.xmin) - w) > DEFAULT_GRID*2 ||
	       fabs((bbox.ymax - bbox.ymin) - h) > DEFAULT_GRID*2) {
		/* notable change in character size: character was clipped 
		   TODO: how to deal with diagonal cuts?
		 */
		msg("<trace> Character %d was clipped: (%f,%f,%f,%f) -> (%f,%f,%f,%f)",
			glyphnr, 
			bbox.xmin,bbox.ymin,bbox.xmax,bbox.ymax,
			bbox2.xmin,bbox2.ymin,bbox2.xmax,bbox2.ymax);
		polyops_fill(dev, glyph, color);
	    } else {
		if(i->out) i->out->drawchar(i->out, font, glyphnr, color, matrix);
	    }
	} else {
	    if(i->out) i->out->drawchar(i->out, font, glyphnr, color, matrix);
	}
	gfxline_free(gfxline);
    } else {
	if(i->out) i->out->drawchar(i->out, font, glyphnr, color, matrix);
    }
    
    gfxline_free(glyph);
}

void polyops_drawlink(struct _gfxdevice*dev, gfxline_t*line, const char*action)
{
    dbg("polyops_drawlink");
    internal_t*i = (internal_t*)dev->internal;
    if(i->out) i->out->drawlink(i->out, line, action);
}

void polyops_endpage(struct _gfxdevice*dev)
{
    dbg("polyops_endpage");
    internal_t*i = (internal_t*)dev->internal;
    if(i->out) i->out->endpage(i->out);
}

gfxresult_t* polyops_finish(struct _gfxdevice*dev)
{
    dbg("polyops_finish");
    internal_t*i = (internal_t*)dev->internal;

    if(i->polyunion) {
	gfxpoly_destroy(i->polyunion);i->polyunion=0;
    } else {
        if(i->bad_polygons) {
            msg("<notice> --flatten success rate: %.1f%% (%d failed polygons)", i->good_polygons*100.0 / (i->good_polygons + i->bad_polygons), i->bad_polygons);
        }
    }
    gfxdevice_t*out = i->out;
    free(i);memset(dev, 0, sizeof(gfxdevice_t));
    if(out) {
	return out->finish(out);
    } else {
	return 0;
    }
}

gfxline_t*gfxdevice_union_getunion(struct _gfxdevice*dev)
{
    internal_t*i = (internal_t*)dev->internal;
    return gfxline_from_gfxpoly(i->polyunion);
}

void gfxdevice_removeclippings_init(gfxdevice_t*dev, gfxdevice_t*out)
{
    dbg("gfxdevice_removeclippings_init");
    internal_t*i = (internal_t*)rfx_calloc(sizeof(internal_t));
    memset(dev, 0, sizeof(gfxdevice_t));
    
    dev->name = "removeclippings";

    dev->internal = i;

    dev->setparameter = polyops_setparameter;
    dev->startpage = polyops_startpage;
    dev->startclip = polyops_startclip;
    dev->endclip = polyops_endclip;
    dev->stroke = polyops_stroke;
    dev->fill = polyops_fill;
    dev->fillbitmap = polyops_fillbitmap;
    dev->fillgradient = polyops_fillgradient;
    dev->addfont = polyops_addfont;
    dev->drawchar = polyops_drawchar;
    dev->drawlink = polyops_drawlink;
    dev->endpage = polyops_endpage;
    dev->finish = polyops_finish;

    i->out = out;
    i->polyunion = 0;
}

void gfxdevice_union_init(gfxdevice_t*dev,gfxdevice_t*out)
{
    dbg("gfxdevice_getunion_init");
    internal_t*i = (internal_t*)rfx_calloc(sizeof(internal_t));
    memset(dev, 0, sizeof(gfxdevice_t));
    
    dev->name = "union";

    dev->internal = i;

    dev->setparameter = polyops_setparameter;
    dev->startpage = polyops_startpage;
    dev->startclip = polyops_startclip;
    dev->endclip = polyops_endclip;
    dev->stroke = polyops_stroke;
    dev->fill = polyops_fill;
    dev->fillbitmap = polyops_fillbitmap;
    dev->fillgradient = polyops_fillgradient;
    dev->addfont = polyops_addfont;
    dev->drawchar = polyops_drawchar;
    dev->drawlink = polyops_drawlink;
    dev->endpage = polyops_endpage;
    dev->finish = polyops_finish;

    i->out = out;
    /* create empty polygon */
    i->polyunion = gfxpoly_from_stroke(0, 0, gfx_capButt, gfx_joinMiter, 0, DEFAULT_GRID);
}

