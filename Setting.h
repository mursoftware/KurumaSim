#pragma once


#include <stdio.h>
#include <Windows.h>
#include "MatrixVector.h"


float GetPrivateProfileFloat(const char* AppName, const char* KeyName, const char* FileName);
Vector3 GetPrivateProfileVector3(const char* AppName, const char* KeyName, const char* FileName);
std::string GetPrivateProfileStdString(const char* AppName, const char* KeyName, const char* FileName);
