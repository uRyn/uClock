//
//  uClockView.mm
//  uClock
//
//  Created by uRyn Ruan on 2020/1/24.
//  Copyright © 2020 uRyn. All rights reserved.
//

#import "uClockView.h"
#include "../comm/scrntime.h"

@implementation uClockView

scrntime *_pscrntime = NULL;
CGColorRef _pbrush_bg = NULL;
CGColorRef _pbrush_clockbg = NULL;
CGColorRef _pbrush_clockface = NULL;
NSColor *_pbrush_clocknum = NULL;
CGColorRef _pbrush_hourhand = NULL;
CGColorRef _pbrush_minhand = NULL;
CGColorRef _pbrush_handdot = NULL;
CGColorRef _pbrush_sechand = NULL;

-(void)create_brushes
{
    _pscrntime->update_time();
    if (_pscrntime->_isnewyear)
    {
        _pbrush_bg = CGColorCreateGenericRGB(0.0f, 0.0f, 0.0f, 1.0f);
        _pbrush_clockbg = CGColorCreateGenericRGB(255.0f/255.0f, 15.0f/255.0f, 15.0f/255.0f, 1.0f);
        _pbrush_clockface = CGColorCreateGenericRGB(245.0f/255.0f, 245.0f/255.0f, 245.0f/255.0f, 1.0f);
        _pbrush_clocknum = [NSColor colorWithCalibratedRed:0.0f green:0.0f blue:0.0f alpha:1.0f];
        _pbrush_hourhand = CGColorCreateGenericRGB(1.0f, 0.0f, 0.0f, 1.0f);
        _pbrush_minhand = CGColorCreateGenericRGB(1.0f, 0.0f, 0.0f, 1.0f);
        _pbrush_handdot = CGColorCreateGenericRGB(1.0f, 0.0f, 0.0f, 1.0f);
        _pbrush_sechand = CGColorCreateGenericRGB(1.0f, 149.0f/255.0f, 0.0f, 1.0f);
    }
    else
    {
        _pbrush_bg = CGColorCreateGenericRGB(0.0f, 0.0f, 0.0f, 1.0f);
        _pbrush_clockbg = CGColorCreateGenericRGB(15.0f/255.0f, 15.0f/255.0f, 15.0f/255.0f, 1.0f);
        _pbrush_clockface = CGColorCreateGenericRGB(245.0f/255.0f, 245.0f/255.0f, 245.0f/255.0f, 1.0f);
        _pbrush_clocknum = [NSColor colorWithCalibratedRed:38.0f/255.0f green:38.0f/255.0f blue:38.0f/255.0f alpha:1.0f];        _pbrush_hourhand = CGColorCreateGenericRGB(38.0f/255.0f, 38.0f/255.0f, 38.0f/255.0f, 1.0f);
        _pbrush_minhand = CGColorCreateGenericRGB(38.0f/255.0f, 38.0f/255.0f, 38.0f/255.0f, 1.0f);
        _pbrush_handdot = CGColorCreateGenericRGB(38.0f/255.0f, 38.0f/255.0f, 38.0f/255.0f, 1.0f);
        _pbrush_sechand = CGColorCreateGenericRGB(1.0f, 149.0f/255.0f, 0.0f, 1.0f);
    }
}

- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self) {
        _pscrntime = new scrntime();
        [self create_brushes];
        [self setAnimationTimeInterval:0.04f];
    }
    return self;
}

- (void)startAnimation
{
    [super startAnimation];
}

- (void)stopAnimation
{
    [super stopAnimation];
}

- (void)fill_rotate_rect:(CGContextRef)ctx rect:(NSRect)rect degree:(float)degree ancor:(NSPoint)ancor {
    [self fill_rotate_rect:ctx rect:rect degree:degree ancor:ancor sec:false];
}

- (void)fill_rotate_rect:(CGContextRef)ctx rect:(NSRect)rect degree:(float)degree ancor:(NSPoint)ancor sec:(bool)sec {
    NSAffineTransform *transform = [NSAffineTransform transform];
    [transform translateXBy:ancor.x yBy:ancor.y];
    [transform rotateByDegrees:degree];
    [transform concat];
    NSRect rc = rect;
    rc.origin.x = -rc.size.width / 2.0f;
    if(sec)
        rc.origin.y = -(ancor.y - rect.origin.y);
    else
        rc.origin.y = 0;
    float radius = rc.size.width * 70.0f / 100.0f;
    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:rc xRadius:radius yRadius:radius];
    [path fill];
    [transform invert];
    [transform concat];
}

- (NSRect)scale_rect:(NSRect)rect x:(float)x
{
    float newsize = rect.size.height * x / 100.0f;
    NSRect rc;
    rc.origin.x = rect.origin.x + (rect.size.width - newsize) / 2;
    rc.origin.y = rect.origin.y + (rect.size.height - newsize) / 2;
    rc.size.width = newsize;
    rc.size.height = newsize;
    return rc;
}

- (void)draw_clock_face:(CGContextRef)ctx rect:(NSRect)rect
{
    NSRect rcbkbg = rect;
    float height = (rect.size.height < rect.size.width ? rect.size.height : rect.size.width) * 60.0 / 100.0f;
    rcbkbg.origin.x = rect.origin.x + (rect.size.width - height) / 2.0f;
    rcbkbg.origin.y = rect.origin.y + (rect.size.height - height) / 2.0f;
    rcbkbg.size.width = rcbkbg.size.height = height;
    CGContextSetFillColorWithColor(ctx, _pbrush_bg);
    CGContextFillRect(ctx, rcbkbg);
    float radius = height * 20.0f / 100.0f;
    NSBezierPath *pathrr = [NSBezierPath bezierPathWithRoundedRect:rcbkbg xRadius:radius yRadius:radius];
    CGContextSetFillColorWithColor(ctx, _pbrush_clockbg);
    [pathrr fill];
    NSBezierPath *pathcrl = [NSBezierPath bezierPathWithOvalInRect:[self scale_rect:rcbkbg x:90.0f]];
    CGContextSetFillColorWithColor(ctx, _pbrush_clockface);
    [pathcrl fill];
    NSRect rccrlnum = [self scale_rect:rcbkbg x:75.0f];
    NSRect rc_num = [self scale_rect:rcbkbg x:13.0f];
    NSFont *font = [NSFont fontWithName:@"Helvetica Neue Medium" size:rc_num.size.height * 85.0f / 100.0f];
    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    [style setAlignment:NSTextAlignmentCenter];
    NSDictionary *drawingAttributes = [NSDictionary dictionaryWithObjectsAndKeys:font, NSFontAttributeName, _pbrush_clocknum, NSForegroundColorAttributeName, style, NSParagraphStyleAttributeName, nil];
    NSPoint opoint = {rccrlnum.origin.x + rccrlnum.size.width / 2, rccrlnum.origin.y + rccrlnum.size.height / 2};
    for(int i = 0; i< 12; ++i)
    {
        float radians = i * 360.0f / 12.0f * M_PI / 180.0f;
        float w = sin(radians) * rccrlnum.size.height / 2.0f;
        float h = cos(radians) * rccrlnum.size.height / 2.0f;
        NSPoint p = NSMakePoint(opoint.x + w, opoint.y + h);
        rc_num.origin.x = p.x - rc_num.size.width / 2.0f;
        rc_num.origin.y = p.y - rc_num.size.height / 2.0f;
        NSString *nstimehour = [NSString stringWithFormat:@"%d", i == 0 ? 12 : i];
        [nstimehour drawInRect:rc_num withAttributes:drawingAttributes];
    }
    
    _pscrntime->update_time();
    float hourhandb = rccrlnum.size.height / 2.0f * 4.0f / 100.0f;
    float hourhandl = rccrlnum.size.height / 2.0f * 70.0f / 100.0f;
    NSRect rchourhand;
    rchourhand.origin.x = opoint.x - hourhandb / 2.0f;
    rchourhand.origin.y = opoint.y;
    rchourhand.size.width = hourhandb;
    rchourhand.size.height = hourhandl;
    CGContextSetFillColorWithColor(ctx, _pbrush_hourhand);
    [self fill_rotate_rect:ctx rect:rchourhand degree:_pscrntime->_nhourdeg ancor:opoint];
    float minhandb = rccrlnum.size.height / 2.0f * 4.0f / 100.0f;
    float minhandl = rccrlnum.size.height / 2.0f * 111.0f / 100.0f;
    NSRect rcminhand;
    rcminhand.origin.x = opoint.x - hourhandb / 2.0f;
    rcminhand.origin.y = opoint.y;
    rcminhand.size.width = minhandb;
    rcminhand.size.height = minhandl;
    CGContextSetFillColorWithColor(ctx, _pbrush_minhand);
    [self fill_rotate_rect:ctx rect:rcminhand degree:_pscrntime->_nmindeg ancor:opoint];
    NSRect rccrldot = [self scale_rect:rccrlnum x:5.5f];
    NSBezierPath *pathcrldot = [NSBezierPath bezierPathWithOvalInRect:rccrldot];
    [pathcrldot fill];
    float sechandb = rccrlnum.size.height / 2.0f * 2.5f / 100.0f;
    float sechandl = rccrlnum.size.height / 2.0f * 111.0f / 100.0f;
    float sechandcl = rccrlnum.size.height / 2.0f * 14.0f / 100.0f;
    NSRect rcsechand;
    rcsechand.origin.x = opoint.x - sechandb / 2.0f;
    rcsechand.origin.y = opoint.y - sechandcl;
    rcsechand.size.width = sechandb;
    rcsechand.size.height = sechandl + sechandcl;
    NSRect rcsecdot = [self scale_rect:rccrlnum x:2.6f];
    NSBezierPath *secdotpath = [NSBezierPath bezierPathWithOvalInRect:rcsecdot];
    CGContextSetFillColorWithColor(ctx, _pbrush_sechand);
    [secdotpath fill];
    [self fill_rotate_rect:ctx rect:rcsechand degree:_pscrntime->_nsecdeg ancor:opoint sec:true];
}

- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
    [[NSGraphicsContext currentContext] setShouldAntialias:YES];
    NSGraphicsContext *nsctx = NSGraphicsContext.currentContext;
    CGContextRef ctx = [nsctx CGContext];
    [self draw_clock_face:ctx rect:rect];
}

- (void)animateOneFrame
{
    [self setNeedsDisplay:YES];
    return;
}

- (BOOL)hasConfigureSheet
{
    return NO;
}

- (NSWindow*)configureSheet
{
    return nil;
}

@end
