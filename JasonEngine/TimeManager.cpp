#include "pch.h"
#include "TimeManager.h"
#include <numeric>

#define NOMINMAX
#include <windows.h>

const double TimeManager::m_TimeFactorsTotal = std::accumulate(m_TimeFactorByMove.begin(), m_TimeFactorByMove.end(), 0.0, std::plus<double>());

void TimeManager::SetMoveTime(double moveTime)
{
	m_MoveTime = moveTime;
}

void TimeManager::SetMaxTime(double maxTime)
{
	if (m_TotalTime < maxTime)
		m_TotalTime = maxTime;

	//we define what should be spent on move
	m_MoveTime = std::min(maxTime,
		0.5 * m_Increment + 2.0 * m_TotalTime * (m_TimeFactorByMove[std::min(m_MoveCount, static_cast<int>(m_TimeFactorByMove.size() - 1))] / m_TimeFactorsTotal));
}

void TimeManager::InitStartTime()
{
	LARGE_INTEGER cpuFreqKhz;
	LARGE_INTEGER start;
	QueryPerformanceFrequency(&cpuFreqKhz);
	QueryPerformanceCounter(&start);
	m_CpuFreqKHz = cpuFreqKhz.QuadPart;
	m_Start = start.QuadPart;
}

bool TimeManager::IsTimeOut()
{
	m_CheckCount++; //only check every 1024 nodes
	if (m_CheckCount < 1024)
		return false;

	m_CheckCount = 0;
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	const double timeSpent = static_cast<double>(time.QuadPart - m_Start) / static_cast<double>(m_CpuFreqKHz); //in seconds

	if (timeSpent > (m_MoveTime - 0.1))
		return true;

	return false;
}

bool TimeManager::HasTimeForNewIteration(double lastIterationDuration) const
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	const double timeSpent = static_cast<double>(time.QuadPart - m_Start) / static_cast<double>(m_CpuFreqKHz);
	const double timeRemaining = m_MoveTime - timeSpent;
	//dont start next iteration if remaining time < 5 * last iteration, this is a very rough guess...
	return (timeRemaining > 5 * lastIterationDuration);
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
