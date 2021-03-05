#pragma once
#include "Engine/OpenGLComponents.h"

float Clamp(float val, float min, float max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
}

vec3 HSVtoRGB(float H, float S, float V) 
{
	H = Clamp(H, 0, 360);
	S = Clamp(S, 0, 100);
	V = Clamp(V, 0, 100);

	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	int R = (r + m) * 255;
	int G = (g + m) * 255;
	int B = (b + m) * 255;

	return vec3(R / 255.0, G / 255.0, B / 255.0);
}

vec3 GetColorRainbow(float parameter)
{
	parameter *= 360;
	return HSVtoRGB(parameter, 100, 100);
}

vec3 GetInterpolatedColor(float parameter, const vec3& colorA, const vec3& colorB)
{
	parameter = Clamp(parameter, 0, 1);
	return (1 - parameter) * colorA + parameter * colorB;
}