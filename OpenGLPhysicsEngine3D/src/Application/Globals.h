#pragma once
#include <mutex>

class Globals
{
public:
	static std::mutex s_EntitySpawnMutex;
	static std::mutex s_EntityTransformMutex;
};

