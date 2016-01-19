#include "colour.h"
#include "../random.h"
#include <stm32f10x_rcc.h>
void colour_set_rgb_from_hsv(ColourRgb* colour_rgb,ColourHsv *colour_hsv){
	uint8_t i;
	float f, p, q, t, h, s, v;

	h=(float)colour_hsv->h;
	s=(float)colour_hsv->s;
	v=(float)colour_hsv->v;

	s /= 255;

	if( s == 0 ) {
		colour_rgb->r = colour_rgb->g = colour_rgb->b = v;
		return;
	}

	h /= 60;
	i = floor( h );
	f = h - i;
	p = (unsigned char)(v * ( 1 - s ));
	q = (unsigned char)(v * ( 1 - s * f ));
	t = (unsigned char)(v * ( 1 - s * ( 1 - f ) ));

	switch( i ) {
	case 0:
		colour_rgb->r = v;
		colour_rgb->g = t;
		colour_rgb->b = p;
		break;
	case 1:
		colour_rgb->r = q;
		colour_rgb->g = v;
		colour_rgb->b = p;
		break;
	case 2:
		colour_rgb->r = p;
		colour_rgb->g = v;
		colour_rgb->b = t;
		break;
	case 3:
		colour_rgb->r = p;
		colour_rgb->g = q;
		colour_rgb->b = v;
		break;
	case 4:
		colour_rgb->r = t;
		colour_rgb->g = p;
		colour_rgb->b = v;
		break;
	default:
		colour_rgb->r = v;
		colour_rgb->g = p;
		colour_rgb->b = q;
		break;
	}
}

void colour_set_hsv_from_rgb(ColourHsv* colour_hsv,ColourRgb *colour_rgb){
	 uint8_t min, max, delta;

	 if( colour_rgb->r < colour_rgb->g )
		 min = colour_rgb->r;
	 else
		 min = colour_rgb->g;
	 if( colour_rgb->b < min )
		 min = colour_rgb->b;

	 if( colour_rgb->r > colour_rgb->g )
		 max = colour_rgb->r;
	 else
		 max = colour_rgb->g;
	 if( colour_rgb->b > max )
		 max = colour_rgb->b;

	 colour_hsv->v = max;

	 delta = max - min;

	 if( max != 0 )
		 colour_hsv->s = (uint8_t)(delta)*255 / max;
	 else {
		 colour_hsv->s = 0;
		 colour_hsv->h = 0;
		 return;
	 }

	 if( colour_rgb->r == max )
		 colour_hsv->h = (colour_rgb->g - colour_rgb->b) * 60 / delta;
	 else if( colour_rgb->g == max )
		 colour_hsv->h = 120 + (colour_rgb->b - colour_rgb->r) * 60 / delta;
	 else
		 colour_hsv->h = 240 + (colour_rgb->r - colour_rgb->g) * 60 / delta;

	 if( colour_hsv->h < 0 )
		 colour_hsv->h += 360;
}
void colour_add_to_rgb(ColourRgb* colour_rgb,ColourRgb* colour_add_rgb){
	colour_rgb -> r = (colour_rgb -> r + colour_add_rgb -> r) / 2;
	colour_rgb -> g = (colour_rgb -> g + colour_add_rgb -> g) / 2;
	colour_rgb -> b = (colour_rgb -> b + colour_add_rgb -> b) / 2;
}
ColourRgb colour_add_with_shift_to_rgb(ColourRgb* colour_rgb,ColourRgb* colour_add_rgb,double shift){
	double n_shift = 1.0 - shift;
	ColourRgb res_col;
	res_col.r = colour_rgb -> r * n_shift + colour_add_rgb -> r * shift;
	res_col.g = colour_rgb -> g * n_shift + colour_add_rgb -> g * shift;
	res_col.b = colour_rgb -> b * n_shift + colour_add_rgb -> b * shift;
	return res_col;
}
ColourRgb colour_get_random_colour(){
	ColourRgb res_col;
	res_col.r = random_number(256);
	res_col.g = random_number(256);
	res_col.b = random_number(256);
	return res_col;
}
void colour_set_saturation_to_rgb(ColourRgb* colour_rgb){}
//void RGB_string2RGB_struct(char *tuple, struct ColourRgb *RGB){
// char tmp[3];
//
// strncpy(tmp, tuple, 2); tmp[2]= '\0';
// RGB->r=(unsigned char)(strtoul(tmp, NULL, 16));
//
// strncpy(tmp, &tuple[2], 2); tmp[2]= '\0';
// RGB->g=(unsigned char)(strtoul(tmp, NULL, 16));
//
// strncpy(tmp, &tuple[4], 2); tmp[2]= '\0';
// RGB->b=(unsigned char)(strtoul(tmp, NULL, 16));
//}
