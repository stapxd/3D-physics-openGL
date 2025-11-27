#pragma once
#include <vector>
#include <algorithm>

template<typename T>
class Observer
{
public:
	void Attach(T* subscriber) {
		m_Subscribers.push_back(subscriber);
	}

	void Detach(T* subscriber) {
		auto it = std::find(m_Subscribers.begin(), m_Subscribers.end(), subscriber);
		if(it != m_Subscribers.end())
			m_Subscribers.erase(it);
	}

	void Notify() {
		for (auto subscriber : m_Subscribers)
		{
			Invoke(subscriber);
		}
	}

protected:
	virtual void Invoke(T* subscriber) {

	}

private:
	std::vector<T*> m_Subscribers;
};

