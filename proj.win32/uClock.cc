//
//  uClock.cc
//  uClock
//
//  Created by uRyn on 2020/1/24.
//  Copyright � 2020 uRyn. All rights reserved.
//

#include <Windows.h>
#include <ScrnSave.h>
#include <gdiplus.h>
#include <string>
#include <time.h>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "resource.h"
#include "../comm/scrntime.h"

using namespace Gdiplus;
static scrntime *_pscrntime = NULL;
SolidBrush *_pbrush_bg = NULL;
SolidBrush* _pbrush_clockbg = NULL;
SolidBrush *_pbrush_clockface = NULL;
SolidBrush *_pbrush_clocknum = NULL;
SolidBrush *_pbrush_hourhand = NULL;
SolidBrush *_pbrush_minhand = NULL;
SolidBrush *_pbrush_handdot = NULL;
SolidBrush *_pbrush_sechand = NULL;
RECT g_rect;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
HDC _hdc;
HDC _hmemdc;
HBITMAP _hdcbmp;

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void fill_roundrect(Graphics* g, Brush *p, RectF rect, float r)
{
    GraphicsPath path;
    path.AddLine(rect.X + r, rect.Y, rect.X + rect.Width - (r * 2), rect.Y);
    path.AddArc(rect.X + rect.Width - (r * 2), rect.Y, r * 2, r * 2, 270, 90);
    path.AddLine(rect.X + rect.Width, rect.Y + r, rect.X + rect.Width, rect.Y + rect.Height - (r * 2));
    path.AddArc(rect.X + rect.Width - (r * 2), rect.Y + rect.Height - (r * 2), r * 2, r * 2, 0, 90);
    path.AddLine(rect.X + rect.Width - (r * 2), rect.Y + rect.Height, rect.X + r, rect.Y + rect.Height);
    path.AddArc(rect.X, rect.Y + rect.Height - (r * 2), r * 2, r * 2, 90, 90);
    path.AddLine(rect.X, rect.Y + rect.Height - (r * 2), rect.X, rect.Y + r);
    path.AddArc(rect.X, rect.Y, r * 2, r * 2, 180, 90);
    path.CloseFigure();
    g->FillPath(p, &path);
}

RectF convert_rectf(RECT &rect)
{
    return RectF(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

RectF scale_rect(RectF &rect, float x)
{
    float newsize = rect.Height * x / 100.0;
    return RectF(rect.X + (rect.Width - newsize) / 2, rect.Y + (rect.Height - newsize) / 2, newsize, newsize);
}

void fill_rotate_rect(Graphics &graphics, RectF rect, float degree, PointF ancor, SolidBrush *brush)
{
    graphics.TranslateTransform(ancor.X, ancor.Y, MatrixOrderAppend);
    graphics.RotateTransform(-degree);
    RectF rc = rect;
    rc.X = -rc.Width / 2.0f;
    rc.Y = -(ancor.Y - rect.Y);
    float radius = rc.Width * 70.0 / 100.0f;
    fill_roundrect(&graphics, brush, rc, radius);
    graphics.ResetTransform();    
}

std::wstring to_unicode(char* pstr)
{
    int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, pstr, -1, NULL, 0);
    wchar_t* pUnicode = new wchar_t[unicodeLen];
    memset(pUnicode, 0, unicodeLen * sizeof(wchar_t));
    ::MultiByteToWideChar(CP_UTF8, 0, pstr, -1, pUnicode, unicodeLen);
    std::wstring wstr(pUnicode);
    delete[]  pUnicode;
    pUnicode = NULL;
    return  wstr;
}

void draw_clockface(Graphics &graphics, RECT rect, FontFamily &fontFamily)
{
    RectF rectF = convert_rectf(rect);
    graphics.FillRectangle(_pbrush_bg, rectF);
    float height = (rectF.Width < rectF.Height ? rectF.Width : rectF.Height) * 60.0 / 100.0;
    RectF rcbkbg(rectF.X + (rectF.Width - height) / 2, rectF.Y + (rectF.Height - height) / 2, height, height);  
    fill_roundrect(&graphics, _pbrush_clockbg, rcbkbg, height * 20.0 / 100.0);
    RectF rccrl = scale_rect(rcbkbg, 90.0);
    graphics.FillEllipse(_pbrush_clockface, rccrl);
    RectF rccrlnum = scale_rect(rcbkbg, 75.0);
    RectF rc_num = scale_rect(rcbkbg, 16.5);
    float fontsize = rc_num.Height * 60.0f / 100.0f;
    Font font(&fontFamily, fontsize, FontStyleRegular, UnitPixel);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);
    PointF point_ogn(rccrlnum.X + rccrlnum.Width / 2, rccrlnum.Y + rccrlnum.Height / 2);
    for (int i = 0; i < 12; ++i)
    {
        float radians = i * 360.0f / 12.0f * M_PI / 180.0f;
        float w = sin(radians) * rccrlnum.Height / 2.0f;
        float h = cos(radians) * rccrlnum.Height / 2.0f;
        PointF p(point_ogn.X + w, point_ogn.Y - h);
        rc_num.X = p.X - rc_num.Width / 2.0f;
        rc_num.Y = p.Y - rc_num.Height / 2.0f;
        char num[3] = { 0 };
        sprintf(num, "%d", i == 0 ? 12 : i);
        graphics.DrawString(to_unicode(num).c_str(), -1, &font, rc_num, &sf, _pbrush_clocknum);
    }
    _pscrntime->update_time();
    float hourhandb = rccrlnum.Height / 2.0f * 4.0f / 100.0f;
    float hourhandl = rccrlnum.Height / 2.0f * 70.0f / 100.0f;
    RectF rchourhand;
    rchourhand.X = point_ogn.X - hourhandb / 2.0f;
    rchourhand.Y = point_ogn.Y - hourhandl;
    rchourhand.Width = hourhandb;
    rchourhand.Height = hourhandl;
    fill_rotate_rect(graphics, rchourhand, _pscrntime->_nhourdeg, point_ogn, _pbrush_hourhand);
    float minhandb = rccrlnum.Height / 2.0f * 4.0f / 100.0f;
    float minhandl = rccrlnum.Height / 2.0f * 111.0f / 100.0f;
    RectF rcminhand;
    rcminhand.X = point_ogn.X - hourhandb / 2.0f;
    rcminhand.Y = point_ogn.Y - minhandl;
    rcminhand.Width = minhandb;
    rcminhand.Height = minhandl;
    fill_rotate_rect(graphics, rcminhand, _pscrntime->_nmindeg, point_ogn, _pbrush_minhand);
    RectF rccrldot = scale_rect(rccrlnum, 5.5f);
    graphics.FillEllipse(_pbrush_handdot, rccrldot);
    float sechandb = rccrlnum.Height / 2.0f * 2.5f / 100.0f;
    float sechandl = rccrlnum.Height / 2.0f * 111.0f / 100.0f;
    float sechandcl = rccrlnum.Height / 2.0f * 14.0f / 100.0f;
    RectF rcsechand;
    rcsechand.X = point_ogn.X - sechandb / 2.0f;
    rcsechand.Y = point_ogn.Y - sechandl;
    rcsechand.Width = sechandb;
    rcsechand.Height = sechandl + sechandcl;
    RectF rcsecdot = scale_rect(rccrlnum, 2.6f);
    graphics.FillEllipse(_pbrush_sechand, rcsecdot);
    fill_rotate_rect(graphics, rcsechand, _pscrntime->_nsecdeg, point_ogn, _pbrush_sechand);
}

void execute_draws(HWND hwnd)
{
    BitBlt(_hmemdc, 0, 0, g_rect.right - g_rect.left, g_rect.bottom - g_rect.top, _hdc, 0, 0, SRCCOPY);
    Graphics graphics(_hmemdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    FontFamily fontFamily(L"Microsoft YaHei");
    draw_clockface(graphics, g_rect, fontFamily);
    BitBlt(_hdc, 0, 0, g_rect.right - g_rect.left, g_rect.bottom - g_rect.top, _hmemdc, 0, 0, SRCCOPY);
}

void create_brushes()
{
    _pscrntime->update_time();
    if (_pscrntime->_isnewyear)
    {
        _pbrush_bg = new SolidBrush(Color(255, 0, 0, 0));
        _pbrush_clockbg = new SolidBrush(Color(255, 255, 15, 15));
        _pbrush_clockface = new SolidBrush(Color(255, 245, 245, 245));
        _pbrush_clocknum = new SolidBrush(Color(255, 0, 0, 0));
        _pbrush_hourhand = new SolidBrush(Color(255, 255, 0, 0));
        _pbrush_minhand = new SolidBrush(Color(255, 255, 0, 0));
        _pbrush_handdot = new SolidBrush(Color(255, 255, 0, 0));
        _pbrush_sechand = new SolidBrush(Color(255, 255, 149, 0));
    }
    else
    {
        _pbrush_bg = new SolidBrush(Color(255, 0, 0, 0));
        _pbrush_clockbg = new SolidBrush(Color(255, 15, 15, 15));
        _pbrush_clockface = new SolidBrush(Color(255, 245, 245, 245));
        _pbrush_clocknum = new SolidBrush(Color(255, 38, 38, 38));
        _pbrush_hourhand = new SolidBrush(Color(255, 38, 38, 38));
        _pbrush_minhand = new SolidBrush(Color(255, 38, 38, 38));
        _pbrush_handdot = new SolidBrush(Color(255, 38, 38, 38));
        _pbrush_sechand = new SolidBrush(Color(255, 255, 149, 0));
    }
}

LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static UINT uTimer; 
    switch (message)
    {
    case WM_CREATE:
    {
        GetClientRect(hwnd, &g_rect);
        _hdc = GetDC(hwnd);
        _hmemdc = CreateCompatibleDC(_hdc);
        _hdcbmp = CreateCompatibleBitmap(_hdc, g_rect.right - g_rect.left, g_rect.bottom - g_rect.top);
        SelectObject(_hmemdc, (HGDIOBJ)_hdcbmp);
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        SetBkColor(_hdc, RGB(0, 0, 0));
        _pscrntime = new scrntime();
        create_brushes();
        uTimer = SetTimer(hwnd, 1, 40, NULL);      
    }
    break;

    case WM_TIMER:
        execute_draws(hwnd);
        break;

    case WM_DESTROY:

        if (uTimer)
            KillTimer(hwnd, uTimer);
        DeleteObject(_hdcbmp);
        DeleteObject(_hmemdc);
        DeleteObject(_hdc);
        GdiplusShutdown(gdiplusToken);
        break;
    }

    return DefScreenSaverProc(hwnd, message, wParam, lParam);
}