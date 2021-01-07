#pragma once
#include "Position.h"

class PositionEvaluation
{
	/// <summary>
	/// return true if white king is in check
	/// </summary>
	static bool IsWhiteInCheck(const Position& position);

	/// <summary>
	/// return true if black king is in check
	/// </summary>
	static bool IsBlackInCheck(const Position& position);
};