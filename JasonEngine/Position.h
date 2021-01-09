#pragma once
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

	enum class PieceType
	{
		Pawn,
		Knight,
		Bishop,
		Rook,
		Queen,
		King
	};

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
	void SetEnPassantSquare(std::array<int, 2> square) { m_EnPassantSquare = square; };
	void ResetEnPassantSquare() { m_EnPassantSquare.reset(); };

	bool CanWhiteCastleKingSide() const { return m_CanWhiteCastleKingSide; };
	void SetCanWhiteCastleKingSide(bool value) { m_CanWhiteCastleKingSide = value; };

	bool CanBlackCastleKingSide() const { return m_CanBlackCastleKingSide; };
	void SetCanBlackCastleKingSide(bool value) { m_CanBlackCastleKingSide = value; };

	bool CanWhiteCastleQueenSide() const { return m_CanWhiteCastleQueenSide; };
	void SetCanWhiteCastleQueenSide(bool value) { m_CanWhiteCastleQueenSide = value; };

	bool CanBlackCastleQueenSide() const { return m_CanBlackCastleQueenSide; };
	void SetCanBlackCastleQueenSide(bool value) { m_CanBlackCastleQueenSide = value; };

	bool IsInsufficientMaterial() const;

	/// <summary>description of a piece and its position</summary>
	class Piece
	{
	public:
		Piece() {};
		Piece(PieceType type, int x, int y) : m_Type(type), m_Position({ x,y }) {};
		Piece(PieceType type, const std::array<int, 2> square) : m_Type(type), m_Position(square) {};
		std::array<int, 2> m_Position = { 0, 0 }; //values should range from 0 to 7
		PieceType m_Type = PieceType::Pawn;

		bool operator==(const Piece& piece) const
		{
			return (piece.m_Position == m_Position) && (piece.m_Type == m_Type);
		}
	};

	struct Move
	{
	public:
		Position::Piece m_From;
		Position::Piece m_To;
		bool m_IsCapture = false;

		bool operator==(const Move& move) const
		{
			return (m_From == move.m_From && m_To == move.m_To); //dont check for capture
		}

		bool IsCastling() const
		{
			return ((m_From.m_Type == Position::PieceType::King) && abs(m_From.m_Position[0] - m_To.m_Position[0]) > 1);
		}
	};

	const std::vector<Piece>& GetWhitePieces() const { return m_WhitePieces; };
	const std::vector<Piece>& GetBlackPieces() const { return m_BlackPieces; };

	std::vector<Piece>& GetWhitePieces() { return m_WhitePieces; };
	std::vector<Piece>& GetBlackPieces() { return m_BlackPieces; };

	bool IsSquareOccupiedByOpponent(const std::array<int, 2>& square) const;

	/// <summary>
	/// Returns pieces for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::vector<Piece> GetPiecesToPlay(PieceType type) const;

	const std::vector<Position::Move>& GetMoves() const { return m_Moves; };
	std::vector<Position::Move>& GetMoves() { return m_Moves; };

private:

	std::vector<Piece> m_WhitePieces;
	std::vector<Piece> m_BlackPieces;
	bool m_IsWhiteToPlay = true;
	GameStatus m_GameStatus = GameStatus::Running;
	bool m_CanWhiteCastleKingSide = true;
	bool m_CanBlackCastleKingSide = true;
	bool m_CanWhiteCastleQueenSide = true;
	bool m_CanBlackCastleQueenSide = true;

	std::optional<std::array<int, 2>> m_EnPassantSquare; //if last move is two step from pawn, the new square of the pawn that moved

	std::vector<Position::Move> m_Moves; //list of moves made to reach the position
};