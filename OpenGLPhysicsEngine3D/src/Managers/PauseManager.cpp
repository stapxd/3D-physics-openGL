#include "PauseManager.h"

PauseManager::PauseManager()
{
}

PauseManager::~PauseManager()
{
}

void PauseManager::ChangeState(ApplicationStates newState)
{
	if (m_CurrentState == newState)
		return;

	m_CurrentState = newState;

	Notify();
}

void PauseManager::Invoke(IPausable* subscriber)
{
	subscriber->ChangeState(m_CurrentState);
}
