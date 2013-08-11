#include "hls.h"

ARGB HSL::TOARGB()
{
	double H,S,L;
	H = (double)((m_value&HMask) >> HShift)/255.0;
	S = (double)GetS()/100.0;
	L = (double)GetL()/100.0;
	if ( S == 0 )
	{
		int tmp = (int)(L*255);
		return Color( 255, tmp, tmp, tmp ).GetValue();		
	}
	double temp1, temp2;
	if ( L < 0.5 )
	{
		temp2 = L*(1.0+S);
	}
	else
	{
		temp2 = L+S-L*S;
	}
	temp1 = 2.0*L-temp2;

	double rgb[3];
	rgb[0] = H + 1.0/3.0;
	rgb[1] = H;
	rgb[2] = H-1.0/3.0;
	for ( int _i = 0; _i < 3; ++_i )
	{
		if ( rgb[_i] < 0 )
		{
			rgb[_i] += 1.0;
		}
		else if( rgb[_i] > 1.0 )
		{
			rgb[_i] -= 1.0;
		}
		if ( 6.0*rgb[_i] < 1.0 )
		{
			rgb[_i] = temp1 + (temp2-temp1)*6.0*rgb[_i];
		}
		else if ( 2.0*rgb[_i] < 1.0 )
		{
			rgb[_i] = temp2;
		}
		else if ( 3.0*rgb[_i] < 2 )
		{
			rgb[_i] = temp1 + (temp2-temp1)*((2.0/3.0)-rgb[_i])*6.0;
		}
		else
		{
			rgb[_i] = temp1;
		}
	}
	for ( int _i = 0; _i < 3; ++_i )
	{
		rgb[_i]*=255;
		if ( (int)(rgb[_i]*10)%10 > 0 )
		{
			rgb[_i] = (int)rgb[_i]+1;
		}
	}
	return Color( GetAlpha(), (BYTE)(rgb[0]), (BYTE)(rgb[1]), (BYTE)(rgb[2]) ).GetValue();
}

HSL::HSLVALUE HSL::FromARGB( IN ARGB argb )
{
	double r,g,b,cmax,cmin, delta, H, S, L;
	r = (double)Color(argb).GetR()/255.0;
	g = (double)Color(argb).GetG()/255.0;
	b = (double)Color(argb).GetB()/255.0;

	cmax = r;
	if( g>cmax )cmax=g;
	if ( b>cmax )cmax=b;
	cmin = r;
	if ( g < cmin ) cmin=g;
	if ( b < cmin ) cmin = b;

	L = (cmax+cmin)/2;
	if ( cmax == cmin )
	{
		S = 0;
		H = 0;
	}
	else
	{
		if ( L < 0.5 ) S = (cmax-cmin)/(cmax+cmin);
		else S = (cmax-cmin)/(2.0-cmax-cmin);

		delta = cmax-cmin;
		if ( delta == 0 ) delta=1.0;
		if ( r == cmax ) H=(g-b)/delta;
		else if( g == cmax ) H = 2.0+(b-r)/delta;
		else H = 4.0+(r-g)/delta;
		H = H/6.0;
		if ( H < 0 ) H +=1;
	}
	H*=255;
	L*=100;
	S*=100;
	m_value =  (((HSLVALUE) ((int)L)<<LShift) |
		((HSLVALUE) ((int)S)<<SShift) |
		((HSLVALUE) ((int)H)<<HShift) |
		((HSLVALUE) (Color(argb).GetAlpha())<<AlphaShift));
	return m_value;
}