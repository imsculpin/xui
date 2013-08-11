#ifndef __UI_ULS_H__
#define __UI_ULS_H__
#include "Switch.h"
#include "ImagingPlus.h"

 class HSL
 {
 public:
 	typedef DWORD HSLVALUE;
 	HSL()
 	{
 		m_value = 0xFF000000;
 	}
 
 	HSL(IN BYTE a,
 		IN SHORT h,
 		IN BYTE s,
 		IN BYTE l)
 	{
 		m_value = MakeHSL(a, h, s, l);
 	}
 
 	HSL(IN DWORD ahsl)
 	{
 		m_value = ahsl;
 	}
 
 	BYTE GetAlpha() const
 	{
 		return (BYTE) (m_value >> AlphaShift);
 	}

	void SetAlpha( BYTE alpha )
	{
		m_value &= (~AlphaMask);
		m_value |= (alpha<<AlphaShift)&AlphaMask;
	}
 
 	SHORT GetH() const
 	{
 		int h = (m_value&HMask) >> HShift;
 		h = (h*359)/255;
 		return (SHORT)h;
 	}

	void SetH( SHORT h )
	{
		h = (h*255)/359;
		m_value &= (~HMask);
		m_value |= (h<<HShift)&HMask;
	}
 
 	BYTE GetS() const
 	{
 		return (BYTE) (m_value >> SShift);
 	}
 
	void SetS( BYTE s )
	{
		m_value &= (~SMask);
		m_value |= (s<<SShift)&SMask;
	}

 	BYTE GetL() const
 	{
 		return (BYTE) (m_value >> LShift);
 	}
 
	void SetL( BYTE l )
	{
		m_value &= (~LMask);
		m_value |= (l<<LShift)&LMask;
	}

 	HSLVALUE GetValue() const
 	{
 		return m_value;
 	}
 
 	void SetValue(IN HSLVALUE hsl)
 	{
 		m_value = hsl;
 	}
 	HSLVALUE FromARGB(IN ARGB argb);
  	ARGB TOARGB( );
 public:
 	enum
 	{
 		AlphaShift  = 24,
 		HShift    = 16,
 		SShift  = 8,
 		LShift   = 0
 	};
 
 	enum
 	{
 		AlphaMask   = 0xff000000,
 		HMask     = 0x00ff0000,
 		SMask   = 0x0000ff00,
 		LMask    = 0x000000ff
 	};
 
 	// Assemble A, R, G, B values into a 32-bit integer
 
 	static HSLVALUE MakeHSL(IN BYTE a,
 		IN int h,
 		IN BYTE s,
 		IN BYTE l)
 	{
 		h = (h*255)/359;
 		return ((((HSLVALUE) (l) <<  LShift) |
 			((HSLVALUE) (s) << SShift) |
 			((HSLVALUE) (h) << HShift) |
 			((HSLVALUE) (a) << AlphaShift)));
 	}
 
 protected:
 	HSLVALUE m_value;
};


#endif