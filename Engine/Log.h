#pragma once
#include<iostream>
#include<string>

class Log
{
public:
	Log();
	~Log();

	static void console(std::string Id, std::string message);
	static void logFloat(std::string Id, float message);
	
};

