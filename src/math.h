
#pragma once

inline float clampf(float f) {
	const float t = (f < -1.0f) ? -1.0f : f;
  	return (t > 1.0f) ? 1.0f : t;
}


inline float absf(float v){
	return (v < 0.0f) ? -v : v;
}


inline int isinf(double x)
{
    union { long u; double f; } ieee754;
    ieee754.f = x;
    return ( (unsigned)(ieee754.u >> 32) & 0x7fffffff ) == 0x7ff00000 &&
           ( (unsigned)ieee754.u == 0 );
}

inline int isnan(double x)
{
    union { long u; double f; } ieee754;
    ieee754.f = x;
    return ( (unsigned)(ieee754.u >> 32) & 0x7fffffff ) +
           ( (unsigned)ieee754.u != 0 ) > 0x7ff00000;
}

inline float fixf(float v){
	// fix NaN and infinity values
	if (isnan(v) || isinf(v))
		return .0f;
	return v;
}


inline unsigned short int float_to_short(float f) {
	f = f + 0.5f;
	return (short int) f;
}
