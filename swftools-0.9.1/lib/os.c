/* os.c

operating system dependent functions

Part of the swftools package. 

Copyright (c) 2005 Matthias Kramm <kramm@quiss.org>

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

#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#else
#undef HAVE_STAT
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#else
#undef HAVE_MMAP
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
#undef HAVE_STAT
#endif

#if defined(CYGWIN)
char path_seperator = '/';
#elif defined(WIN32)
char path_seperator = '\\';
#else
char path_seperator = '/';
#endif

#ifdef WIN32
/*double fmin(double a,double b)	{return (a<b)?a:b;}
double fmax(double a,double b)	{return (a>b)?a:b;}
float fmin(float a,float b)		{return (a<b)?a:b;}
float fmax(float a,float b)		{return (a>b)?a:b;}
*/

char* getRegistryEntry(char*path)
{
    int res = 0;
    HKEY key;
    long rc;
    long size = 0;
    DWORD type;
    char*buf;
    rc = RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &key);
    if(rc)
	rc = RegOpenKeyExA(HKEY_CURRENT_USER, path, 0, KEY_READ, &key);
    if(rc)
	rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0, KEY_ALL_ACCESS, &key);
    if(rc)
	rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key);

    if (rc) {
	fprintf(stderr, "RegOpenKeyEx failed\n");
	return 0;
    }
    rc = RegQueryValueExA(key, NULL, 0, 0, 0, (LPDWORD)&size) ;
    if(rc) {
	fprintf(stderr, "RegQueryValueEx(1) failed: %d\n", rc);
	return 0;
    }
    buf = (char*)malloc(size+1);
    rc = RegQueryValueExA(key, NULL, 0, &type, (BYTE*)buf, (LPDWORD)&size);
    if(rc) {
	fprintf(stderr, "RegQueryValueEx(2) failed: %d\n", rc);
	return 0;
    }
    if(type == REG_SZ || type == REG_EXPAND_SZ) {
	while(size && buf[size-1] == '\0')
          --size;
	buf[size] = 0;
	/* TODO: convert */
	return buf;
    } else if(type == REG_BINARY) {
	return buf;
    }
    return 0;
}

int setRegistryEntry(char*key,char*value)
{
    HKEY hkey1;
    HKEY hkey2;
    int ret1 = 0, ret2=0;
    ret1 = RegCreateKeyA(HKEY_CURRENT_USER, key, &hkey1);
    ret2 = RegCreateKeyA(HKEY_LOCAL_MACHINE, key, &hkey2);
    if(ret1 && ret2) {
	fprintf(stderr, "registry: CreateKey %s failed\n", key);
	return 0;
    }
    if(!ret1)
	ret1 = RegSetValueA(hkey1, NULL, REG_SZ, value, strlen(value)+1);
    if(!ret2)
	ret2 = RegSetValueA(hkey2, NULL, REG_SZ, value, strlen(value)+1);
    if(ret1 && ret2) {
	fprintf(stderr, "registry: SetValue %s failed\n", key);
	return 0;
    }
    return 1;
}


#endif

//HINSTANCE me =  GetModuleHandle(NULL);

char* getInstallationPath()
{
#if defined(WIN32)
    char* path = getRegistryEntry("Software\\xPDFWin\\swftools\\InstallPath");
    if(path)
	return path;
    else
	return 0;
#elif defined(CYGWIN)
    return SWFTOOLS_DATADIR;
#else
    return SWFTOOLS_DATADIR;
#endif
}

char* concatPaths(const char*base, const char*add)
{
    int l1 = strlen(base);
    int l2 = strlen(add);
    int pos = 0;
    char*n = 0;
    while(l1 && base[l1-1] == path_seperator)
	l1--;
    while(pos < l2 && add[pos] == path_seperator)
	pos++;

    n = (char*)malloc(l1 + (l2-pos) + 2);
    memcpy(n,base,l1);
    n[l1]=path_seperator;
    strcpy(&n[l1+1],&add[pos]);
    return n;
}

char* stripFilename(const char*filename, const char*newext)
{
    char*last1 =(char *) strrchr(filename, '/');
    char*last2 =(char *) strrchr(filename, '\\');
    const char*pos = filename;
    char*name;
    char*dot;
    if(last1>pos) pos = last1 + 1;
    if(last2>pos) pos = last2 + 1;
    name = (char*)malloc(strlen(pos)+2+(newext?strlen(newext):3));
    strcpy(name, pos);
    dot = strrchr(name, '.');
    if(dot) {
	*dot = 0;
    }
    if(newext)
	strcat(name, newext);
    return name;
}

static char* getTempDir()
{
#ifdef WIN32
    char*dir = getenv("TMP");
    if(!dir) dir = getenv("TEMP");
    if(!dir) dir = getenv("tmp");
    if(!dir) dir = getenv("temp");
    if(!dir) dir = "C:\\";
#else
    char* dir = "/tmp/";
#endif
    return dir;
}

char* mktmpname(char*ptr) {
	return mktempname(ptr, NULL);
}

char* mktempname(char*ptr, const char*ext) {
    static char tmpbuf[160];
    char*dir = getTempDir();
    int l = strlen(dir);
    char*sep = "";
    if(!ptr)
	ptr = tmpbuf;
    if(l && dir[l-1]!='/' && dir[l-1]!='\\') {
#ifdef WIN32
	sep = "\\";
#else
	sep = "/";
#endif
    }

#ifdef HAVE_LRAND48
    unsigned int r1 = (unsigned int)lrand48();
    unsigned int r2 = (unsigned int)lrand48();
#elif HAVE_RAND
    unsigned int r1 = rand();
    unsigned int r2 = rand();
#else
    static int count = 1;
    unsigned int r1 = time(0);
    unsigned int r2 = (unsigned int)tmpbuf<<8^count;
    count ++;
#endif
    if(ext) {
	sprintf(ptr, "%s%s%04x%04x.%s",dir,sep,r1,r2,ext);
    } else {
	sprintf(ptr, "%s%s%04x%04x",dir,sep,r1,r2);
    }
    return ptr;
}

memfile_t* memfile_open(const char*path)
{
    memfile_t*file = (memfile_t *)malloc(sizeof(memfile_t));
	
#if defined(HAVE_MMAP) && defined(HAVE_STAT)
    int fi = open(path, O_RDONLY);
    if(fi<0) {
        perror(path);
        free(file);
        return 0;
    }
    struct stat sb;
    if(fstat(fi, &sb)<0) {
        perror(path);
        return 0;
    }
    file->len = sb.st_size;
    file->data = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fi, 0);
#else
    FILE*fi = fopen(path, "rb");
    if(!fi) {
        perror(path);
        free(file);
        return 0;
    }
    fseek(fi, 0, SEEK_END);
    file->len = ftell(fi);
    fseek(fi, 0, SEEK_SET);
    file->data = malloc(file->len);
    if(!file->data) {
        fprintf(stderr, "Out of memory while allocating memory for file %s\n", path);
        free(file);
        return 0;
    }
    fread(file->data, file->len, 1, fi);
    fclose(fi);
#endif
    return file;
}

void memfile_close(memfile_t*file)
{
#if defined(HAVE_MMAP) && defined(HAVE_STAT)
    munmap(file->data, file->len);
#else
    free(file->data);
#endif
    file->data = 0;
    file->len = 0;
    free(file);
}

void move_file(const char*from, const char*to)
{
    int result = rename(from, to);

    if(result==0) return; //done!

    /* if we can't rename, for some reason, copy the file
       manually */
    FILE*fi = fopen(from, "rb");
    if(!fi) {
	perror(from);
	return;
    }
    FILE*fo = fopen(to, "wb");
    if(!fo) {
	perror(to);
	return;
    }
    char buffer[16384];
    while(1) {
	int bytes = fread(buffer, 16384, 1, fi);
	if(bytes<=0)
	    return;
	fwrite(buffer, bytes, 1, fo);
    }

    fclose(fo);
    fclose(fi);
}

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#define _S_IREAD 256
#define _S_IWRITE 128
int mkstemp(char *tmpl)
{
	int ret=-1;
	mktemp(tmpl); 
	ret=open(tmpl,O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE); 
	return ret;
}

#endif