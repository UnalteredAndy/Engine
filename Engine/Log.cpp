#include "Log.h"



Log::Log()
{
}


Log::~Log()
{
}

void Log::console(std::string Id, std::string message)
{
	std::cout << Id << ": " << message << "   ---End log." << std::endl;
}

void Log::logFloat(std::string Id, float message)
{
	std::cout << Id << ": " << message << "   ---End log." << std::endl;
}
