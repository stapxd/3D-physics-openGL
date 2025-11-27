#pragma once
#include "Enumerators/ApplicationStates.h"

class IPausable
{
public:
	virtual void ChangeState(ApplicationStates newState) = 0;
};

