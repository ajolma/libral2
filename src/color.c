#include "ral2/ral.h"
#include "priv/priv.h"

#define to255(i,s) floor((255.0*(double)(i)/(double)(s))+0.5)

ral_object_t *Color_new(ral_object_t *_self)
{
    ral_color_t * self = (ral_color_t *)_self;
    self->red = 0;
    self->green = 0;
    self->blue = 0;
    self->alpha = 255;
    return self;
}

ral_color_t *ral_color_new(int red, int green, int blue, int alpha)
{
    ral_color_t *self = ral_new(ralColor);
    if (self) {
        self->red = red;
        self->green = green;
        self->blue = blue;
        self->alpha = alpha;
    }
    return self;
}

int Color_set(ral_object_t *_self, ral_object_t *b)
{
    ral_color_t * self = (ral_color_t *)_self;
    if (class_of(b) == ralColor) {
        self->red = ((ral_color_t *)b)->red;
        self->green = ((ral_color_t *)b)->green;
        self->blue = ((ral_color_t *)b)->blue;
        self->alpha = ((ral_color_t *)b)->alpha;
        return 1;
    }
    return 0;
}

const char *Color_name()
{
    return "Color";
}

void Color_to_string(ral_object_t *_self)
{
    ral_color_t * self = (ral_color_t *)_self;
    if (self->string) free(self->string);
    TEST(self->string = malloc(20));
    sprintf(self->string, "%i %i %i %i", self->red, self->green, self->blue, self->alpha);
fail:;
}

const struct Class _Color = {
    sizeof(struct ral_color),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Color_new,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ Color_set,
    /*  5 */ NULL,
    /*  6 */ Color_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ Color_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class Color = &_Color;
ral_class ralColor = &_Color;

void ral_color_set(ral_color_t *self, int red, int green, int blue, int alpha)
{
    self->red = red;
    self->green = green;
    self->blue = blue;
    self->alpha = alpha;
}

/* after www.cs.rit.edu/~ncs/color/t_convert.html */
void rgb2hsv(ral_color_t * rgb)
{
    float r = (float)rgb->red/256;
    float g = (float)rgb->green/256;
    float b = (float)rgb->blue/256;
    float min, max, delta;
    float h,s,v;

    min = MIN( r, MIN(g, b) );
    max = MAX( r, MAX(g, b) );
    v = max;				// v

    delta = max - min;

    if( max != 0 )
        s = delta / max;		// s
    else {
        // r = g = b = 0		// s = 0, v is undefined
        s = 0;
        h = -1;
        rgb->red = -1;
        rgb->green = 0;
        rgb->blue = floor(v * 100.999);
    }

    if( r == max )
        h = ( g - b ) / delta;		// between yellow & magenta
    else if( g == max )
        h = 2 + ( b - r ) / delta;	// between cyan & yellow
    else
        h = 4 + ( r - g ) / delta;	// between magenta & cyan

    h *= 60;				// degrees
    if( h < 0 )
        h += 360;

    rgb->red = floor(h);
    rgb->green = floor(s * 100.999);
    rgb->blue = floor(v * 100.999);
}

void hsv2rgb(ral_color_t * hsv)
{
    int i;
    float h = hsv->red; float s = (float)hsv->green/100.0; float v = (float)hsv->blue/100.0;
    float r; float g; float b;
    float f, p, q, t;

    if( s == 0 ) {
        // achromatic (grey)
        r = g = b = v;
        hsv->red = floor(255.999*r);
        hsv->green = floor(255.999*g);
        hsv->blue = floor(255.999*b);
    }

    h /= 60;			// sector 0 to 5
    i = floor( h );
    if (i == 6) i = 5;
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );

    switch( i ) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    default:		// case 5:
        r = v;
        g = p;
        b = q;
        break;
    }
    hsv->red = floor(255.999*r);
    hsv->green = floor(255.999*g);
    hsv->blue = floor(255.999*b);
}
