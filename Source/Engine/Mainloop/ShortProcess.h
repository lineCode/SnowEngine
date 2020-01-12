#pragma once
#include"Process.h"
#include"../3rdParty/FastDelegate/FastDelegate.h"

class ShortProcess :public Process {
public:
	explicit ShortProcess(int _TimeToDelay);
	explicit ShortProcess(int _TimeToDelay, fastdelegate::FastDelegate0<void> delegate);

public:
	void virtual VOnUpdate(unsigned long deltaMs) override;

private:
	int m_TimeToDelay;
	int m_PastTime;
	fastdelegate::FastDelegate0<void> m_Delegate;

};

template<class T>
class ShortProcess1 :public Process {
public:
	ShortProcess1(int _TimeToDelay);

	ShortProcess1(int _TimeToDelay, fastdelegate::FastDelegate1<T, void> delegate1, T param);

public:
	void virtual VOnUpdate(unsigned long deltaMs) override;

private:
	int m_TimeToDelay;
	int m_PastTime;
	fastdelegate::FastDelegate1<T,void> m_Delegate1;
	T m_Param;
};

ShortProcess::ShortProcess(int _TimeToDelay)
{
	m_TimeToDelay = _TimeToDelay;
	m_PastTime = 0;
}

ShortProcess::ShortProcess(int _TimeToDelay, fastdelegate::FastDelegate0<void> delegate)
{
	m_TimeToDelay = _TimeToDelay;
	m_PastTime = 0;
	m_Delegate = delegate;
}

void ShortProcess::VOnUpdate(unsigned long deltaMs)
{
	if (m_Delegate) {
		m_Delegate();
	}

	m_PastTime += deltaMs;
	if (m_PastTime >= m_TimeToDelay) {
		Succeed();

	}
}

template<class T>
ShortProcess1<T>::ShortProcess1(int _TimeToDelay)
{
	m_TimeToDelay = _TimeToDelay;
	m_PastTime = 0;
}

template<class T>
inline ShortProcess1<T>::ShortProcess1(int _TimeToDelay, fastdelegate::FastDelegate1<T, void> delegate, T param)
{
	m_TimeToDelay = _TimeToDelay;
	m_PastTime = 0;
	m_Delegate1 = delegate;
	m_Param = param;
}

template<class T>
void ShortProcess1<T>::VOnUpdate(unsigned long deltaMs)
{
	if (m_Delegate1) {
		m_Delegate1(m_Param);
	}

	m_PastTime += deltaMs;
	if (m_PastTime >= m_TimeToDelay) {
		Succeed();
	}
}
