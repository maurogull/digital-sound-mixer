#pragma once

// =========================================== biquad filters, 4 por canal


typedef struct {
	int active;
	float term1;
	float term2;
	float term3;
	float term4;
	float term5;
} biquad_filter;   //los términos del filtro biquad los manda el cliente ya calculados




void apply_eq(int,int);
void eq_init();