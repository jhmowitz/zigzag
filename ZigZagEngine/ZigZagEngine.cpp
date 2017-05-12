// This is the main DLL file.

#include "MOW_DEFS.H"
#include "graphics.h"
#include <stdarg.h>
#include <string.h>
#include <tchar.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <vcclr.h>
#include MOW_LINK_H
#include "ALG_GEO.H"
#include "PRC_STR.H"
#include "PRC_DEF.H"
#include "IZigZagHost.h"
#include "ZigZagEngine.h"

extern void DomainGraphInit( BOOL );    // set screen parameters: ratio
extern void DomainCadreInit( BOOL );    // set screen parameters: ratio
extern void ZoomTo(INT2 , INT2, REAL);	// viewgrph.cpp

// Implementations for graphics support
// These methods use ZigZagEngine::Host to perform their task,
// converting parameters between managed and unmanaged code if necessary

#define HOST ZigZag::ZigZagEngine::Host

void moveto(INT2 x, INT2 y)
{
  HOST->moveto(x, y);
}

void lineto(INT2 x, INT2 y)
{
  HOST->lineto(x, y);
}

INT2 getmaxx()
{
  return HOST->getmaxx();
}

INT2 getmaxy()
{
  return HOST->getmaxy();
}

void outtextxy(INT2 l, INT2 u, pCHAR t)
{
  HOST->outtextxy(l, u, gcnew String(t));
}

void cleardevice()
{
  HOST->cleardevice();
}

void putpixel(INT2 x, INT2 y, COLORS c)
{
  HOST->putpixel(x, y, c);
}

void setcolor(COLORS c)
{
  HOST->setcolor(c);
}

void setactivepage(INT2 page)
{
  HOST->setactivepage(page);
}

void setvisualpage(INT2 page)
{
  HOST->setvisualpage(page);
}

INT2 textwidth(pCHAR t)
{
  return HOST->textwidth(gcnew String(t));
}

INT2 textheight(pCHAR t)
{
  return HOST->textheight(gcnew String(t));
}

void initgraph(INT2 *D, INT2 *M, pCHAR dir)
{
  HOST->initgraph(*D, *M, gcnew String(dir));
}

void closegraph()
{
  HOST->closegraph();
}

void delay(uINT2 d)
{
  HOST->delay(d);
}

size_t imagesize(INT2 u, INT2 d, INT2 l, INT2 r)
{
  // return HOST->imagesize(u, d, l, r);
  return sizeof(System::Drawing::Image ^);
}

void getimage(INT2 u, INT2 d, INT2 l, INT2 r, void *buffer)
{
  System::Drawing::Image ^image = HOST->getimage(u, d, l, r);
  *((System::Drawing::Image^ *)buffer) = image;
}

void putimage(INT2 u, INT2 d, void *buffer, INT2 mode)
{
  // mode should be COPY_PUT
  if (mode != COPY_PUT)
    Exit(_T("mode should be COPY_PUT"));

  System::Drawing::Image ^image = *((System::Drawing::Image^ *)buffer);
  HOST->putimage(u, d, image, mode);
}

void circle(REAL x, REAL y, REAL r)
{
  HOST->circle(x, y, r);
}

void line(INT2 x1, INT2 y1, INT2 x2, INT2 y2)
{
  HOST->line(x1, y1, x2, y2);
}

INT4 biostime(INT2 v1, INT4 v2)
{
  return HOST->biostime(v1, v2);
}

void settextstyle(INT2 font, INT2 direction, INT2 mode)
{
  HOST->settextstyle(font, direction, mode);
}

void setusercharsize(INT2 n1, INT2 n2, INT2 n3, INT2 n4)
{
  HOST->setusercharsize(n1, n2, n3, n4);
}

void setviewport(INT2 x1, INT2 y1, INT2 x2, INT2 y2, INT2 mode)
{
  HOST->setviewport(x1, y1, x2, y2, mode);
}

void getaspectratio(INT2 *x, INT2 *y)
{
  HOST->getaspectratio(*x, *y);
}

/*
 * Return whether there is a key available at the host
 */
BOOL kbhit()
{
  return HOST->kbhit();
}

/*
 * Get a character from the host. In true DOS fashion, some keys generate two characters: a 0 followed by the actual character
 */
INT2 getch()
{
  return HOST->getch();
}

/*
 * Internal function for use by DoPrintf() since arargs and ref types cannot be combined in the same method
 */
void DoPrintString(pCONSTCHAR s)
{
   HOST->ShowMessage(gcnew String(s));
}

/*
 *	This method gets called when the engine calls PRINTF, which is supposed to print output on the console
 *	which we don't have!
 */
void DoPrintf(pCONSTCHAR fmt, ...)
{
  static CHAR ScrBuff[5000];

  va_list ap;
  va_start( ap, fmt ); _vstprintf( ScrBuff, fmt, ap );
  va_end( ap );

  DoPrintString(ScrBuff);
}

void DoGets(pCONSTCHAR prompt, pCHAR s, INT2 maxlength)
{
	String^ result = HOST->ReadString(gcnew String(prompt), maxlength);

  pin_ptr<const CHAR> p = PtrToStringChars(result);
  pCONSTCHAR pnative = p;
  STRCPY(s, pnative);
}

/*
 * Notify the host we have an event. Generic means to communicate with host.
 */
void NotifyEvent(pCONSTCHAR event, pCONSTCHAR message)
{
	HOST->NotifyEvent(gcnew String(event), gcnew String(message));
}

/*
 * Read a string from the host. Store the result in the pCHAR s,
 * using a prompt. The initial length of s determines the maximum length of the result!
 */
void ReadString(pCHAR s, pCONSTCHAR prompt)
{
	INT2 n = (INT2)STRLEN(s);

	String^ result = HOST->ReadString(gcnew String(prompt), n);
  pin_ptr<const CHAR> p = PtrToStringChars(result);
  pCONSTCHAR pnative = p;
  STRCPY(s, pnative);
}

/*
 * Read an INT2 from the host
 */
INT2 ReadInt(pCONSTCHAR prompt)
{
	return HOST->ReadInt(gcnew String(prompt));
}

/*
 * Read a REAL from the host
 */
REAL ReadReal(pCONSTCHAR prompt)
{
	return HOST->ReadReal(gcnew String(prompt));
}

/*
 * Open a file using normal stdio methods, but translate the relative file name to a full data file name
 */
pFILE DoFopen(pCONSTCHAR name, pCONSTCHAR mode)
{
	String^ result = HOST->MakeDataFileName(gcnew String(name));
  pin_ptr<const CHAR> p = PtrToStringChars(result);
	return _tfopen(p, mode);
}

namespace ZigZag
{
  ZigZagEngine::ZigZagEngine(IZigZagHost ^host, array<String^> ^arguments)
  {
    // Store the supplied host in our static Host method so others can access
    // it using ZigZagEngine::Host
    Host = host;

    //int n1 = sizeof(char); // 1
    //int n2 = sizeof(wchar_t); // 2

    // Convert the supplied arguments-array into argc and argv

    // Allocate space for the correct amount of pCHARs
    pCHAR *argv = new pCHAR[arguments->Length];
    // Set argc
    int argc = arguments->Length;
    // Create strings for all arguments
    for (int i = 0; i < argc; i++)
    {
      // Allocate string for argument #i
      argv[i] = new CHAR[arguments[i]->Length + 1];
      // Pin, then copy the argument-string into argv
      pin_ptr<const CHAR> p = PtrToStringChars(arguments[i]);
			pCONSTCHAR pnative = p;
      STRCPY(argv[i], pnative);
    }
    
    Prcs.GRPH::CommLine( argc, argv );                  // command line for graphics
    
    // Clean up argv
    for (int i = 0; i < argc; i++)
    {
      delete argv[i];
    }
    delete argv;
    // Exit( "" );                                         // finished
  }

	void ZigZagEngine::Run()
	{
    Prcs.GRPH::Open();                                  // open GRAPHICS

    DomainGraphInit( Prcs.GRPH::IsNT );                 // set screen parameter

    Prcs.Process();                                     // do processes
		Prcs.GRPH::Close();
	}

	void ZigZagEngine::ZoomTo(INT2 viewportX, INT2 viewportY, REAL zoomfactor)
	{
		::ZoomTo(viewportX, viewportY, zoomfactor);
	}

	void ZigZagEngine::InitCadre()
	{
		::DomainCadreInit(FALSE);
	}
}