#pragma once
#include "BasicDefinitions.h"
#include "ZobristHash.h"
#include <array>
#include <vector>
#include <optional>
#include <string>

/// <summary>
/// Description of a move
/// </summary>

/// <summary>description of a chess position</summary>
class Position
{
public:
	/// <summary>
	/// Default constructor (starting position)
	/// </summary>
	Position();

	/// <summary>
	/// Constructor from FEN notation
	/// </summary>
	Position(const std::string& fen);

	/// <summary>
	/// Init to empty board
	/// </summary>
	void InitEmptyBoard();

	void UpdatePosition(const Move& move);

	enum class GameStatus
	{
		Running,
		CheckMate,
		StaleMate
	};

	bool IsWhiteToPlay() const { return m_IsWhiteToPlay; };
	void SetWhiteToPlay(bool isWhiteToPlay) { m_IsWhiteToPlay = isWhiteToPlay; };

	GameStatus GetGameStatus() const { return m_GameStatus; };
	void SetGameStatus(GameStatus gameStatus) { m_GameStatus = gameStatus; };

	const std::optional<std::array<int, 2>>& GetEnPassantSquare() const { return m_EnPassantSquare; };
	void SetEnPassantSquare(std::array<int, 2> square);
	void ResetEnPassantSquare();

	bool CanWhiteCastleKingSide() const { return m_CanWhiteCastleKingSide; };
	void SetCanWhiteCastleKingSide(bool value);

	bool CanBlackCastleKingSide() const { return m_CanBlackCastleKingSide; };
	void SetCanBlackCastleKingSide(bool value);

	bool CanWhiteCastleQueenSide() const { return m_CanWhiteCastleQueenSide; };
	void SetCanWhiteCastleQueenSide(bool value);

	bool CanBlackCastleQueenSide() const { return m_CanBlackCastleQueenSide; };
	void SetCanBlackCastleQueenSide(bool value);

	bool IsInsufficientMaterial() const;

	const std::vector<Piece>& GetWhitePieces() const { return m_WhitePieces; };
	const std::vector<Piece>& GetBlackPieces() const { return m_BlackPieces; };

	std::vector<Piece>& GetWhitePieces() { return m_WhitePieces; };
	std::vector<Piece>& GetBlackPieces() { return m_BlackPieces; };

	bool IsSquareOccupiedByOpponent(const std::array<int, 2>& square) const;

	/// <summary>
	/// Returns pieces for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::vector<const Piece*> GetPieces(PieceType type, bool isWhite) const;

	/// <summary>
	/// Returns first piece found for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	const Piece* GetPiece(PieceType type, bool isWhite) const;

	/// <summary>
	/// Returns pieces for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::vector<const Piece*> GetPiecesToPlay(PieceType type) const;

	const std::vector<Move>& GetMoves() const { return m_Moves; };
	std::vector<Move>& GetMoves() { return m_Moves; };

	uint64_t GetZobristHash() const { return m_ZobristHash; };
	void SetZobristHash(uint64_t hash) { m_ZobristHash = hash; };

	/// <summary>
	/// Recompute zobrist hash from scratch
	/// </summary>
	uint64_t ComputeZobristHash() const;

	bool operator==(const Position& position) const
	{
		return (m_ZobristHash == position.GetZobristHash());
	}

private:

	std::vector<Piece> m_WhitePieces; //Position representation is piece-centric, this might change in the future
	std::vector<Piece> m_BlackPieces;
	bool m_IsWhiteToPlay = true;
	GameStatus m_GameStatus = GameStatus::Running;
	bool m_CanWhiteCastleKingSide = true;
	bool m_CanBlackCastleKingSide = true;
	bool m_CanWhiteCastleQueenSide = true;
	bool m_CanBlackCastleQueenSide = true;

	std::optional<std::array<int, 2>> m_EnPassantSquare; //if last move is two step from pawn, the square behind the pawn that moved

	std::vector<Move> m_Moves; //list of moves made to reach the position

	uint64_t m_ZobristHash = 0;
};

template<>
struct std::hash<Position>
{
	uint64_t operator()(const Position& position) const
	{
		return position.GetZobristHash();
	}
};