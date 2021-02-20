#include "pch.h"
#include "TimeManager.h"
#include <numeric>

#define NOMINMAX
#include <windows.h>

const double TimeManager::m_TimeFactorsTotal = std::accumulate(m_TimeFactorByMove.begin(), m_TimeFactorByMove.end(), 0.0, std::plus<double>());

void TimeManager::InitStartTime()
{
	LARGE_INTEGER cpuFreqKhz;
	LARGE_INTEGER start;
	QueryPerformanceFrequency(&cpuFreqKhz);
	QueryPerformanceCounter(&start);
	m_CpuFreqKHz = cpuFreqKhz.QuadPart;
	m_Start = start.QuadPart;
}

bool TimeManager::IsTimeOut() const
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	const double timeSpent = static_cast<double>(time.QuadPart - m_Start) / static_cast<double>(m_CpuFreqKHz); //in seconds

	if (timeSpent > (m_MaxTime - 1.0))
		return true; //time out by out of time
	if (timeSpent > (0.5 * m_Increment +
		2.0 * m_TotalTime * (m_TimeFactorByMove[std::min(m_MoveCount, static_cast<int>(m_TimeFactorByMove.size() - 1))] / m_TimeFactorsTotal)))
		return true; //time out by t > what should be spent on move

	return false;
}

bool TimeManager::HasTimeForNewIteration(int iteration, double lastIterationDuration) const
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	const double timeSpent = static_cast<double>(time.QuadPart - m_Start) / static_cast<double>(m_CpuFreqKHz);
	const double timeRemaining = m_MaxTime - timeSpent;
	//dont start next iteration if remaining time < 12 * last iteration, this is a very rough guess, ok up to 8th iteration...
	return (timeRemaining > 12 * lastIterationDuration);
}

void TimeManager::StartCounter()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	m_CounterStart = time.QuadPart;
}

void TimeManager::EndCounter()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	m_CounterEnd = time.QuadPart;
}

double TimeManager::GetCounterDiff() const
{
	return (static_cast<double>(m_CounterEnd - m_CounterStart) / static_cast<double>(m_CpuFreqKHz));
}
