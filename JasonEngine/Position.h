#pragma once
#include "BasicDefinitions.h"
#include "Bitboard.h"
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

	/// <remark>move is updated too to take into account captures</remark>
	void Update(Move& move);
	void Undo(const Move& move);

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

	const std::optional<Square>& GetEnPassantSquare() const { return m_EnPassantSquare; };
	void SetEnPassantSquare(Square square);
	void ResetEnPassantSquare();

	bool CanWhiteCastleKingSide() const { return m_CanWhiteCastleKingSide; };
	void SetCanWhiteCastleKingSide(bool value);

	bool CanBlackCastleKingSide() const { return m_CanBlackCastleKingSide; };
	void SetCanBlackCastleKingSide(bool value);

	bool CanWhiteCastleQueenSide() const { return m_CanWhiteCastleQueenSide; };
	void SetCanWhiteCastleQueenSide(bool value);

	bool CanBlackCastleQueenSide() const { return m_CanBlackCastleQueenSide; };
	void SetCanBlackCastleQueenSide(bool value);

	bool HasWhiteCastled() const { return m_HasWhiteCastled; };
	bool HasBlackCastled() const { return m_HasBlackCastled; };

	bool IsInsufficientMaterial() const;
	bool IsInsufficientMaterialFromBitboards() const;

	const std::vector<Piece>& GetWhitePiecesList() const { return m_WhitePiecesList; };
	const std::vector<Piece>& GetBlackPiecesList() const { return m_BlackPiecesList; };

	std::vector<Piece>& GetWhitePiecesList() { return m_WhitePiecesList; };
	std::vector<Piece>& GetBlackPiecesList() { return m_BlackPiecesList; };

	const Bitboard& GetPiecesOfType(PieceType type, bool isWhite) const;
	Bitboard& GetPiecesOfType(PieceType type, bool isWhite);

	const Bitboard& GetWhitePieces() const { return m_WhitePieces; };
	Bitboard& GetWhitePieces() { return m_WhitePieces; };
	const Bitboard& GetBlackPieces() const { return m_BlackPieces; };
	Bitboard& GetBlackPieces() { return m_BlackPieces; };

	Bitboard& GetWhitePawns() { return m_WhitePawns; };
	const Bitboard& GetWhitePawns() const { return m_WhitePawns; };
	Bitboard& GetBlackPawns() { return m_BlackPawns; };
	const Bitboard& GetBlackPawns() const { return m_BlackPawns; };

	Bitboard& GetWhiteKnights () { return m_WhiteKnights; };
	const Bitboard& GetWhiteKnights() const { return m_WhiteKnights; };
	Bitboard& GetBlackKnights() { return m_BlackKnights; };
	const Bitboard& GetBlackKnights() const { return m_BlackKnights; };

	Bitboard& GetWhiteBishops() { return m_WhiteBishops; };
	const Bitboard& GetWhiteBishops() const { return m_WhiteBishops; };
	Bitboard& GetBlackBishops() { return m_BlackBishops; };
	const Bitboard& GetBlackBishops() const { return m_BlackBishops; };

	Bitboard& GetWhiteRooks() { return m_WhiteRooks; };
	const Bitboard& GetWhiteRooks() const { return m_WhiteRooks; };
	Bitboard& GetBlackRooks() { return m_BlackRooks; };
	const Bitboard& GetBlackRooks() const { return m_BlackRooks; };

	Bitboard& GetWhiteQueens() { return m_WhiteQueens; };
	const Bitboard& GetWhiteQueens() const { return m_WhiteQueens; };
	Bitboard& GetBlackQueens() { return m_BlackQueens; };
	const Bitboard& GetBlackQueens() const { return m_BlackQueens; };

	Bitboard& GetWhiteKing() { return m_WhiteKing; };
	const Bitboard& GetWhiteKing() const { return m_WhiteKing; };
	Bitboard& GetBlackKing() { return m_BlackKing; };
	const Bitboard& GetBlackKing() const { return m_BlackKing; };

	/*Bitboard& GetWhiteUndevelopedPieces() { return m_WhiteUndevelopedPieces; };
	const Bitboard& GetWhiteUndevelopedPieces() const { return m_WhiteUndevelopedPieces; };
	Bitboard& GetBlackUndevelopedPieces() { return m_BlackUndevelopedPieces; };
	const Bitboard& GetBlackUndevelopedPieces() const { return m_BlackUndevelopedPieces; };*/

	/// <summary>
	/// Returns pieces for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::vector<Piece> GetPieces(PieceType type, bool isWhite) const;

	/// <summary>
	/// Returns first piece found for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::optional<Piece> GetPiece(PieceType type, bool isWhite) const;

	/// <summary>
	/// Returns pieces for given type and their position of side whose turn is to move (if there is one! Can have more than 1)
	/// </summary>
	std::vector<Piece> GetPiecesToPlay(PieceType type) const;

	/// <summary>
	/// Returns all pieces side whose turn is to move (if there is one! Can have more than 1) ; slow as pieces are regenerated and not read from a maintained list of pieces
	/// </summary>
	std::vector<Piece> GetPiecesToPlay() const;

	const std::vector<Move>& GetMoves() const { return m_Moves; };
	std::vector<Move>& GetMoves() { return m_Moves; };
	/// <returns>Returns list of moves as pair of pieces (for debugging purposes)</returns>
	std::vector<std::pair<Piece, Piece>> GetPieceMoves() const;

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

	/// <summary>
	/// Slow method, for debugging only (use zobrist instead)
	/// </summary>
	bool AreEqual(const Position& position) const;

	bool CheckBitboardsSanity() const;

	void SetMaintainPiecesList(bool value) { m_MaintainPiecesList = value; };

private:

	void UpdatePiece(const Move& move, bool isWhite);
	void UndoPiece(const Move& move, bool isWhite);
	void UpdateSquare(PieceType type, Square square, bool isWhite);

	/// <param name="capturedPiece">Captured piece if any [OUT]</param>
	void UpdateCapturedPiece(Square square, Move& move);

	Bitboard m_WhitePieces;
	Bitboard m_WhitePawns;
	Bitboard m_WhiteKnights;
	Bitboard m_WhiteBishops;
	Bitboard m_WhiteRooks;
	Bitboard m_WhiteQueens;
	Bitboard m_WhiteKing;
	Bitboard m_WhiteUndevelopedPieces; //all pieces except king

	Bitboard m_BlackPieces;
	Bitboard m_BlackPawns;
	Bitboard m_BlackKnights;
	Bitboard m_BlackBishops;
	Bitboard m_BlackRooks;
	Bitboard m_BlackQueens;
	Bitboard m_BlackKing;
	Bitboard m_BlackUndevelopedPieces;

	std::vector<Piece> m_WhitePiecesList; //Position representation is piece-centric, this might change in the future
	std::vector<Piece> m_BlackPiecesList;
	bool m_MaintainPiecesList = false; //false by default, actual Jason.exe should have turned this off

	bool m_IsWhiteToPlay = true;
	GameStatus m_GameStatus = GameStatus::Running;
	bool m_CanWhiteCastleKingSide = true;
	bool m_CanBlackCastleKingSide = true;
	bool m_CanWhiteCastleQueenSide = true;
	bool m_CanBlackCastleQueenSide = true;
	bool m_HasWhiteCastled = false;
	bool m_HasBlackCastled = false;

	std::optional<Square> m_EnPassantSquare; //if last move is two step from pawn, the square behind the pawn that moved

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