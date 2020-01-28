//
//  scrntime.cc
//  uClock
//
//  Created by uRyn on 2020/1/24.
//  Copyright © 2020 uRyn. All rights reserved.
//

#include "scrntime.h"

#include <time.h>
#include <stdio.h>

#if defined __APPLE__ || defined __linux__
#include <sys/time.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <stdint.h>
#define __x86_64__
#endif

#ifndef __x86_64__
#define int_tt int32_t
#define uint_tt uint32_t
#else
#define int_tt int64_t
#define uint_tt uint64_t
#endif

#ifdef _WIN32
void gettimeofday(struct timeval *tv, void* tz) {
#define EPOCHFILETIME (116444736000000000ULL)
    FILETIME ft;
    LARGE_INTEGER li;
    uint_tt tt;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    tt = (li.QuadPart - EPOCHFILETIME) / 10;
    tv->tv_sec = tt / 1000000;
    tv->tv_usec = tt % 1000000;
}

struct tm* localtime_r(const time_t* timep, struct tm* result) {
    localtime_s(result, timep);
    return result;
}
#endif

double walltime_now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (static_cast<int_tt>(tv.tv_sec) * 1000000 + tv.tv_usec) * 0.000001;
}

void scrntime::update_time()
{    
    struct ::tm ti;
    double now = walltime_now();
    time_t rawtime = (time_t)now;
    int usecs = (int)((now - rawtime) * 1000000);
    localtime_r(&rawtime, &ti);
    float allsec = ti.tm_sec + usecs / 1000000.0;
    float allmin = ti.tm_min + allsec / 60.0;
    float allhour = ti.tm_hour >= 13 ? ti.tm_hour - 12 : ti.tm_hour + allmin / 60.0;
    _nsecdeg = -(allsec * 6.0);
    _nmindeg = -(allmin * 6.0);
    _nhourdeg = -(allhour * 30.0);
    if (0 <= ti.tm_mon && ti.tm_mon <= 1)
        _isnewyear = true;
    else
        _isnewyear = false;
}