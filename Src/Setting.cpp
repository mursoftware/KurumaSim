
#include "Pch.h"
#include "Setting.h"


float GetPrivateProfileFloat(const char* AppName, const char* KeyName, const char* FileName)
{
	char str[256]{};
	GetPrivateProfileString(AppName, KeyName, "", str, 256, FileName);

	float value;
	sscanf(str, "%f", &value);

	return value;
}



Vector3 GetPrivateProfileVector3(const char* AppName, const char* KeyName, const char* FileName)
{
	char str[256]{};
	GetPrivateProfileString(AppName, KeyName, "", str, 256, FileName);

	Vector3 value;
	sscanf(str, "%f,%f,%f", &value.X, &value.Y, &value.Z);

	return value;
}


std::string GetPrivateProfileStdString(const char* AppName, const char* KeyName, const char* FileName)
{
	char str[256]{};
	GetPrivateProfileString(AppName, KeyName, "", str, 256, FileName);

	std::string string(str);

	return string;
}


