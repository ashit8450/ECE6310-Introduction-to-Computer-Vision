//Submitted by Ashit Mohanty C13582787
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"

#define SQR(x) ((x)*(x))
#define MAX_QUEUE 10000

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)

{
MSG			msg;
HWND		hWnd;
WNDCLASS	wc;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="PLUS";

if (!RegisterClass(&wc))
  return(FALSE);

hWnd=CreateWindow("PLUS","plus program",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

ShowPixelCoords=0;
color_r = 0;
color_b = 0;
color_g = 0;
color_y = 0;
p_mode = 0;
s_mode = 0;
ans = 0;

strcpy(filename,"");
OriginalImage=NULL;
ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}




LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam)

{
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
char				header[320],text[320];
int					BYTES,xPos,yPos,i,j,color;

switch (uMsg)
  {
  case WM_COMMAND:
    switch (LOWORD(wParam))
      {
	  case ID_SHOWPIXELCOORDS:
		ShowPixelCoords=(ShowPixelCoords+1)%2;
		PaintImage();
		break;
	  case ID_DISPLAY_RED:
		color_r = (color_r + 1) % 2;
		color_b = 0;
		color_y = 0;
		color_g = 0;
		break;
	  case ID_DISPLAY_BLUE:
		color_b = (color_b + 1) % 2;
		color_r = 0;
		color_y = 0;
		color_g = 0;
		break;
	  case ID_DISPLAY_GREEN:
		color_g = (color_g + 1) % 2;
		color_r = 0;
		color_b = 0;
		color_y = 0;
		break;
	  case ID_DISPLAY_YELLOW:
		color_y = (color_y + 1) % 2;
		color_r = 0;
		color_b = 0;
		color_g = 0;
		break;
	  case ID_CLEAR_IMAGE:
		PaintImage();
		p_mode = 0;
		s_mode = 0;
		ans = 0;
		color_r = 0;
		color_b = 0;
		color_y = 0;
		color_g = 0;
		break;
	  case ID_MODE_PLAY:
		p_mode = 1;
		s_mode = 0;
		break;
	  case ID_MODE_STEP:
		p_mode = 0;
		s_mode = 1;
		break;
	  case ID_PREDICATE_EDITPREDICATES:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogCallback);
		break;
	  case ID_FILE_LOAD:
		if (OriginalImage != NULL)
		  {
		  free(OriginalImage);
		  OriginalImage=NULL;
		  }
		memset(&(ofn),0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.lpstrFile=filename;
		filename[0]=0;
		ofn.nMaxFile=MAX_FILENAME_CHARS;
		ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL)
		  {
		  MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
		if (strcmp(header,"P5") != 0  ||  BYTES != 255)
		  {
		  MessageBox(NULL,"Not a PPM (P5 greyscale) image",filename,MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
		header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage,1,ROWS*COLS,fpt);
		fclose(fpt);
		SetWindowText(hWnd,filename);
		PaintImage();
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogCallback);
		break;

      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;
      }
    break;

  case WM_SIZE:		  /* could be used to detect when window size changes */
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_PAINT:
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
	ThreadCol = LOWORD(lParam);
	ThreadRow = HIWORD(lParam);
	_beginthread(Segment, 0, MainWnd);
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_MOUSEMOVE:
	return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	break;

  case WM_KEYDOWN:
	if (wParam == 'j' || wParam == 'J')
		ans = 1;
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }

hMenu=GetMenu(MainWnd);
if (ShowPixelCoords == 1)
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);

if (color_r == 1)
  CheckMenuItem(hMenu,ID_DISPLAY_RED,MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_DISPLAY_RED,MF_UNCHECKED);

if (color_b == 1)
  CheckMenuItem(hMenu,ID_DISPLAY_BLUE, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_DISPLAY_BLUE, MF_UNCHECKED);

if (color_g == 1)
  CheckMenuItem(hMenu,ID_DISPLAY_GREEN, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu,ID_DISPLAY_GREEN, MF_UNCHECKED);

if (color_y == 1)
  CheckMenuItem(hMenu, ID_DISPLAY_YELLOW, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu, ID_DISPLAY_YELLOW, MF_UNCHECKED);

if (p_mode == 1)
  CheckMenuItem(hMenu, ID_MODE_PLAY, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu, ID_MODE_PLAY, MF_UNCHECKED);

if (s_mode == 1)
  CheckMenuItem(hMenu, ID_MODE_STEP, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
else
  CheckMenuItem(hMenu, ID_MODE_STEP, MF_UNCHECKED);

DrawMenuBar(hWnd);

return(0L);
}




void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
unsigned char		*DisplayImage;

if (OriginalImage == NULL)
  return;		/* no image to draw */

		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
DISPLAY_ROWS=ROWS;
DISPLAY_COLS=COLS;
if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
	DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=DISPLAY_COLS;
bm_info_header.biHeight=-DISPLAY_ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=8; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=256;
bm_info_header.biClrImportant=256;
bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
bm_info->bmiHeader=bm_info_header;
for (i=0; i<256; i++)
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 

SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
			  0, /* first scan line */
			  DISPLAY_ROWS, /* number of scan lines */
			  DisplayImage,bm_info,DIB_RGB_COLORS);
ReleaseDC(MainWnd,hDC);
EndPaint(MainWnd,&Painter);

free(DisplayImage);
free(bm_info);
}

BOOL CALLBACK DialogCallback(HWND win_d, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL *lflag_1, *lflag_2;
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			predicate1 = GetDlgItemInt(win_d, IDC_EDIT1, &lflag_1, flag);
			if (!lflag_1)
			{
				MessageBox(win_d, "Error in reading Predicate 1", NULL, MB_OK);
				SendDlgItemMessage(win_d, IDC_EDIT1, EM_SETSEL, 0, -1l);
			}
			predicate2 = GetDlgItemInt(win_d, IDC_EDIT2, &lflag_2, flag);
			if (!lflag_2)
			{
				MessageBox(win_d, "Error in reading Predicate 2", NULL, MB_OK);
				SendDlgItemMessage(win_d, IDC_EDIT2, EM_SETSEL, 0, -1l);
			}
			EndDialog(win_d, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(win_d, IDCANCEL);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		return FALSE;
		break;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}



void Segment(HWND AnimationWindowHandle)
{
	int r2, c2;
	int i, j, k, key=0;
	unsigned char *labels;
	int* indices;
	int		RegionSize, * RegionPixels, TotalRegions;
	double		avg, var;

	HDC		hDC;

	int r = ThreadRow;
	int c = ThreadCol;
	labels = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	indices = (int*)calloc(ROWS * COLS, sizeof(int));

	TotalRegions = 255;
	
	hDC = GetDC(MainWnd);

	avg = var = 0.0;	/* compute average and variance in 7x7 window */
	for (r2 = -3; r2 <= 3; r2++)
		for (c2 = -3; c2 <= 3; c2++)
			avg += (double)(OriginalImage[(r + r2) * COLS + (c + c2)]);
	avg /= 49.0;
	for (r2 = -3; r2 <= 3; r2++)
		for (c2 = -3; c2 <= 3; c2++)
			var += SQR(avg - (double)OriginalImage[(r + r2) * COLS + (c + c2)]);
	var = sqrt(var) / 49.0;

	RegionGrow(OriginalImage, labels, ROWS, COLS, r, c, 0, TotalRegions,
		indices, &RegionSize);

	k = 0;
	while (k<RegionSize)
	{
		if (p_mode == 1)
		{
			if (labels[indices[k]] == 255)
			{
				if (color_y == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(255, 255, 0));
				}
				else if (color_b == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(0, 0, 255));
				}
				else if (color_g == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(0, 255, 0));
				}
				else if (color_r == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(255, 0, 0));
				}
				Sleep(1);
				k++;
			}
		}

		if ((s_mode == 1) && (ans == 1))
		{
			if (labels[indices[k]] == 255)
			{
				if (color_y == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(255, 255, 0));
					ans = 0;
				}
				else if (color_b == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(0, 0, 255));
					ans = 0;
				}
				else if (color_g == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(0, 255, 0));
					ans = 0;
				}
				else if (color_r == 1)
				{
					j = indices[k] / COLS;
					i = indices[k] % COLS;
					SetPixel(hDC, i, j, RGB(255, 0, 0));
					ans = 0;
				}
				k++;
			}
		}
	}
	ReleaseDC(AnimationWindowHandle, hDC);
}



void RegionGrow(unsigned char* image,	/* image data */
	unsigned char* labels,	/* segmentation labels */
	int ROWS, int COLS,	/* size of image */
	int r, int c,		/* pixel to paint from */
	int paint_over_label,	/* image label to paint over */
	int new_label,		/* image label for painting */
	int* indices,		/* output:  indices of pixels painted */
	int* count)		/* output:  count of pixels painted */
{
	int	r2, c2;
	int dist;
	int	queue[MAX_QUEUE], qh, qt;
	int	average, total;	/* average and total intensity in growing region */

	*count = 0;
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = new_label;
	average = total = (int)image[r * COLS + c];
	if (indices != NULL)
		indices[0] = r * COLS + c;
	queue[0] = r * COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	while (qt != qh)
	{
		if ((*count) % 50 == 0)	/* recalculate average after each 50 pixels join */
		{
			average = total / (*count);
		}
		for (r2 = -1; r2 <= 1; r2++)
			for (c2 = -1; c2 <= 1; c2++)
			{
				if (r2 == 0 && c2 == 0)
					continue;
				if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
					(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
					continue;
				if (labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] != paint_over_label)
					continue;
				/* test criteria to join region */
				dist = sqrt(pow(r - (queue[qt] / COLS + r2), 2) + pow(c - (queue[qt] % COLS + c2), 2));
				if ((abs((int)(image[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2])
					- average) > predicate1) || (dist > predicate2))
					continue;
				labels[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2] = new_label;
				if (indices != NULL)
					indices[*count] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
				total += image[(queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2];
				(*count)++;
				queue[qh] = (queue[qt] / COLS + r2) * COLS + queue[qt] % COLS + c2;
				qh = (qh + 1) % MAX_QUEUE;
			}
		qt = (qt + 1) % MAX_QUEUE;
	}
}

