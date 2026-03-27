#pragma once

#ifdef HELPERS_BUILD_DLL
	#define HELPERSAPI __declspec(dllexport)
#else
	#define HELPERSAPI __declspec(dllimport)
#endif

#include <iostream>

namespace helpers {
	enum class Color {
		Black = 0,
		Blue = 1,
		Green = 2,
		Cyan = 3, 
		Red = 4,
		Magenta = 5,
		Yellow = 6, 
		White = 7,  
		Gray = 8,  
		BrightBlue = 9,
		BrightGreen = 10,
		BrightCyan = 11,
		BrightRed = 12,
		BrightMagenta = 13,
		BrightYellow = 14,
		BrightWhite = 15
	};

	HELPERSAPI int clamp(int value, int min, int max);
	HELPERSAPI float clamp(float value, float min, float max);
	HELPERSAPI double clamp(double value, double min, double max);
	
	HELPERSAPI void print(const char* message);
	HELPERSAPI void printl(const char* message);
	
	HELPERSAPI void printInt(int value);
	HELPERSAPI void printIntl(int value);
	
	HELPERSAPI void printFloat(float value, int precision);
	HELPERSAPI void printFloatl(float value, int precision);
	
	HELPERSAPI void setCursor(int x, int y);
	HELPERSAPI void setColor(Color tColor, Color bgColor);
	HELPERSAPI void resetColor();
	HELPERSAPI void setTextColor(Color color);
	HELPERSAPI void setBgColor(Color color);

	HELPERSAPI void progressBar(int maxValue, int currentValue, int width, bool showPercent = false, Color filledColor = Color::Green, Color restColor = Color::Red);

	HELPERSAPI void logWarning(const char* type, const char* message);
	HELPERSAPI void logError(const char* type, const char* message);
	HELPERSAPI void logWarning(const wchar_t*, const wchar_t* message);
	HELPERSAPI void logError(const wchar_t* type, const wchar_t* message);
}