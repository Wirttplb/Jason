#pragma once
#include <array>
#include <stdint.h>

class TimeManager
{
public:

	void SetMaxTime(double maxTime);

	void SetIncrement(double increment) { m_Increment = increment; };

	void InitStartTime();

	void SetMoveCount(int moveCount) { m_MoveCount = moveCount; }

	bool IsTimeOut();

	bool HasTimeForNewIteration(int iteration, double lastIterationDuration) const;

	void StartCounter();
	void EndCounter();
	double GetCounterDiff() const;

private:

	double m_TotalTime = 0.0;
	double m_MaxTime = 0.0;
	double m_Increment = 0.0;
	int m_MoveCount = 0;
	int m_CheckCount = 0;

	int64_t m_CpuFreqKHz = 0;
	int64_t m_Start = 0;

	int64_t m_CounterStart = 0;
	int64_t m_CounterEnd = 0;

	static constexpr std::array<double, 38> m_TimeFactorByMove = //time factor for number of played moves, for average number of moves per chess game (38)
	{
		//1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};

	static const double m_TimeFactorsTotal;
};