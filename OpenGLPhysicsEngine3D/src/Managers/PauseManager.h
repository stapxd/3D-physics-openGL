#pragma once
#include "Patterns/Observer.h"
#include "Interfaces/IPausable.h"

class PauseManager : public Observer<IPausable>
{
public:
	PauseManager();
	~PauseManager();

	void ChangeState(ApplicationStates newState);

	ApplicationStates GetCurrentState() const { return m_CurrentState; }

protected:
	virtual void Invoke(IPausable* subscriber);

private:
	ApplicationStates m_CurrentState = ApplicationStates::Play;
};

