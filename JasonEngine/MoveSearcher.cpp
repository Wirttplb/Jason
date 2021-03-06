#include "pch.h"
#include "MoveSearcher.h"
#include "BitboardUtility.h"
#include <assert.h>
#include <iterator>
#include <algorithm>

static PieceType _type = PieceType::Pawn;
static int _from = 0;
//static std::vector<Move> _moves;
static MoveList<MaxMoves>::iterator _movesIt; //SHOULD BE INSERT_ITERATOR!
static void MakeMove(int to);
static void GenerateMoveList(PieceType type, int from, const Bitboard& to, MoveList<MaxMoves>& moveList)
{
	_type = type;
	_from = from;
	_movesIt = moveList.end();
	LoopOverSetBits(to, MakeMove);

	moveList.resize(std::distance(moveList.begin(), _movesIt));
}

static void MakeMove(int to)
{
	_movesIt->Reset();
	_movesIt->SetFrom(_type, static_cast<Square>(_from));
	_movesIt->SetTo(_type, static_cast<Square>(to));
	_movesIt++;
}

/// <summary> Converts list of squares to bitboard </summary>
static Bitboard ConvertToBitboard(const std::vector<std::array<int, 2>>& squares)
{
	std::vector<Bitboard> squaresAsBitboards;
	for (const std::array<int, 2> & square : squares)
	{
		squaresAsBitboards.emplace_back(Bitboard(square[0] + 8 * square[1]));
	}

	Bitboard squaresAsBitboard;
	for (const Bitboard& b : squaresAsBitboards)
	{
		squaresAsBitboard |= b;
	}

	return squaresAsBitboard;
}

/// <summary>
/// Generates simple move table (no castling, no pawn capture or en passant, no queening)
/// </summary>
/// <param name="isPawnDoubleStep">if false, only return single steps for pawn, if true, return double steps</param>
static std::vector<Bitboard> GenerateMoves(PieceType type, bool isWhitePiece = true, bool isPawnDoubleStep = false)
{
	Position position; //ignored
	position.InitEmptyBoard();
	std::vector<Bitboard> moveTable(64);
	for (int square = 0; square < 64; square++)
	{
		Piece piece(type, square);
		Bitboard accessibleSquares = MoveSearcher::GetAccessibleBitboard(position, piece, isWhitePiece, isPawnDoubleStep);
		if (accessibleSquares > 0)
			moveTable[square] = accessibleSquares;
	}

	return moveTable;
}

/// <summary>
/// Generates capture move table for pawns
/// </summary>
static std::vector<Bitboard> GeneratePawnCaptureMoves(bool isWhitePiece)
{
	Position position; //add enemy pieces on every square
	position.InitEmptyBoard();
	std::vector<Piece>& enemyPieces = (isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList());
	for (int square = 0; square < 64; square++)
	{
		enemyPieces.emplace_back(Piece(PieceType::Knight, square));
	}

	std::vector<Bitboard> moveTable(64);
	for (int square = 0; square < 64; square++)
	{
		Piece piece(PieceType::Pawn, square);
		std::vector<std::array<int, 2>> captureSquares = MoveSearcher::GetPawnCaptureSquares(position, piece, isWhitePiece);
		Bitboard accessibleSquares = ConvertToBitboard(captureSquares);
		if (accessibleSquares > 0)
			moveTable[square] = accessibleSquares;
	}

	return moveTable;
}

static Position _rookAttacksPosition;
static void AddPiece(int squareIdx)
{
	_rookAttacksPosition.GetBlackPiecesList().emplace_back(Piece(PieceType::Knight, squareIdx));
}

static std::array<std::array<Bitboard, 256>, 8> GenerateRookAttacks()
{
	_rookAttacksPosition.InitEmptyBoard();
	bool isWhiteToPlay = true;
	std::array<std::array<Bitboard, 256>, 8> rookAttacksTable;
	
	for (int rookSquare = 0; rookSquare < 8; rookSquare++)
	{
		for (int i = 1; i < rookAttacksTable.front().size(); i++)
		{
			uint8_t bitset = i;
			_rookAttacksPosition.GetBlackPiecesList().clear();
			LoopOverSetBits(bitset, AddPiece);
			//Remove dummy piece on rook's square
			for (size_t pieceIdx = 0; pieceIdx < _rookAttacksPosition.GetBlackPiecesList().size(); pieceIdx++)
			{
				if (_rookAttacksPosition.GetBlackPiecesList()[pieceIdx].m_Square == rookSquare)
				{
					_rookAttacksPosition.GetBlackPiecesList().erase(_rookAttacksPosition.GetBlackPiecesList().begin() + pieceIdx);
					break;
				}
			}

			std::vector<Move> moves = MoveSearcher::GetLegalMoves(_rookAttacksPosition, Piece(PieceType::Rook, rookSquare), isWhiteToPlay);
			std::vector<std::array<int, 2>> attackSquares;
			for (const Move& move : moves)
			{
				attackSquares.emplace_back(std::array<int, 2>{ move.GetToSquare() % 8, move.GetToSquare() / 8 });
			}
			
			rookAttacksTable[rookSquare][bitset] = ConvertToBitboard(attackSquares) & _1; //attack squares on 1st row
		}
	}

	return rookAttacksTable;
}


//AttackTable
static const std::array<std::array<Bitboard, 256>, 8> RookAttackTable = GenerateRookAttacks(); //first idx is rook, 2nd is row/file occupancy (including rook)

//MoveTable = on empty board
static const std::vector<Bitboard> WhitePawnMoveTable = GenerateMoves(PieceType::Pawn, true);
static const std::vector<Bitboard> BlackPawnMoveTable = GenerateMoves(PieceType::Pawn, false);
static const std::vector<Bitboard> WhitePawnDoubleStepMoveTable = GenerateMoves(PieceType::Pawn, true, true);
static const std::vector<Bitboard> BlackPawnDoubleStepMoveTable = GenerateMoves(PieceType::Pawn, false, true);
static const std::vector<Bitboard> WhitePawnCaptureMoveTable = GeneratePawnCaptureMoves(true);
static const std::vector<Bitboard> BlackPawnCaptureMoveTable = GeneratePawnCaptureMoves(false);
static const std::vector<Bitboard> KingMoveTable = GenerateMoves(PieceType::King); //omits castles
static const std::vector<Bitboard> QueenMoveTable = GenerateMoves(PieceType::Queen);
static const std::vector<Bitboard> RookMoveTable = GenerateMoves(PieceType::Rook);
static const std::vector<Bitboard> BishopMoveTable = GenerateMoves(PieceType::Bishop);
static const std::vector<Bitboard> KnightMoveTable = GenerateMoves(PieceType::Knight);
static const Bitboard WhiteKingSideCastleInBetweenSquares = _f1 | _g1;
static const Bitboard WhiteQueenSideCastleInBetweenSquares = _b1 | _c1 | _d1;
static const Bitboard BlackKingSideCastleInBetweenSquares = _f8 | _g8;
static const Bitboard BlackQueenSideCastleInBetweenSquares = _b8 | _c8 | _d8;

/// <summary> Generates rook attacks from square index and occupancy </summary>
static Bitboard GenerateRookAttacks(int fromSquare, const Bitboard& occupancy)
{
	const int row = fromSquare >> 3; //or / 8
	const int file = fromSquare & 7; //or % 8

	//Get row attacks
	Bitboard rowOccupancy = occupancy & _rows[row];
	//shift to first row to get key
	rowOccupancy = rowOccupancy >> (row) * 8;
	Bitboard rowAttacks = RookAttackTable[file][rowOccupancy];
	//unshift
	rowAttacks = rowAttacks << (row) * 8;

	//Get file attacks using rotated bitboards:
	Bitboard fileOccupancy = occupancy & _files[file];
	Bitboard rotatedFile = fileOccupancy.Rotate90Clockwise();
	//shift to first row
	rotatedFile = rotatedFile >> (7 - file) * 8;
	Bitboard fileAttacks = RookAttackTable[row][rotatedFile];
	//unshift and unrotate
	fileAttacks = fileAttacks << (7 - file) * 8;
	fileAttacks = fileAttacks.Rotate90AntiClockwise();

	return rowAttacks | fileAttacks;
}

/// <summary> Generates bishop attacks from square index and occupancy </summary>
static Bitboard GenerateBishopAttacks(int fromSquare, const Bitboard& occupancy)
{
	const int row = fromSquare >> 3; //or / 8
	const int file = fromSquare & 7; //or % 8

	//Attacks on A1H8 directed diagonal
	int diagIdx = row - file;
	uint64_t extractDiagMask = _rows[0];
	int squareIdxInDiag = file;
	
	if (diagIdx > 0)
	{
		extractDiagMask = _rows[diagIdx].m_Value;
		extractDiagMask &= FilesOnLeft[7 - diagIdx];
	}
	else if (diagIdx < 0)
	{
		//for negative row - file, diagonal is represented by 2nd part of rank
		diagIdx = 8 + diagIdx;
		extractDiagMask = _rows[diagIdx].m_Value;
		extractDiagMask &= FilesOnRight[8 - diagIdx];
	}

	uint64_t rotatedDiag = occupancy.PseudoRotate45Clockwise().m_Value & extractDiagMask;
	//shift to first row
	rotatedDiag = rotatedDiag >> (diagIdx) * 8;
	Bitboard diagAttacksA1H8 = RookAttackTable[squareIdxInDiag][rotatedDiag];
	//unshift, reapply mask and unrotate
	diagAttacksA1H8 = diagAttacksA1H8 << (diagIdx) * 8;
	diagAttacksA1H8.m_Value &= extractDiagMask;
	diagAttacksA1H8 = diagAttacksA1H8.UndoPseudoRotate45Clockwise();

	//Attacks on A8H1 directed diagonal
	diagIdx = file + row + 1;
	extractDiagMask = _rows[0];

	if (diagIdx > 7)
	{
		diagIdx = file + row - 7;
		extractDiagMask = _rows[diagIdx].m_Value;
		extractDiagMask &= FilesOnRight[diagIdx];
	}
	else
	{
		extractDiagMask = _rows[diagIdx].m_Value;
		extractDiagMask &= FilesOnLeft[diagIdx - 1];
	}

	rotatedDiag = occupancy.PseudoRotate45AntiClockwise().m_Value & extractDiagMask;
	//shift to first row
	rotatedDiag = rotatedDiag >> (diagIdx) * 8;
	Bitboard diagAttacksA8H1 = RookAttackTable[squareIdxInDiag][rotatedDiag];
	//unshift, reapply mask and unrotate
	diagAttacksA8H1 = (diagAttacksA8H1 << (diagIdx) * 8);
	diagAttacksA8H1.m_Value &= extractDiagMask;
	diagAttacksA8H1 = diagAttacksA8H1.UndoPseudoRotate45AntiClockwise();

	return diagAttacksA1H8 | diagAttacksA8H1;
}

std::vector<Move> MoveSearcher::GetLegalMoves(const Position& position)
{
	std::vector<Move> allLegalMoves;
	
	const std::vector<Piece>& piecesToMove = position.IsWhiteToPlay() ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	for (const Piece& piece : piecesToMove)
	{
		//Get Legal moves
		std::vector<Move> moves = GetLegalMoves(position, piece, position.IsWhiteToPlay());

		allLegalMoves.insert(allLegalMoves.end(),
			std::make_move_iterator(moves.begin()),
			std::make_move_iterator(moves.end()));
	}

	return allLegalMoves;
}

//static global variables, very fragile!!
static PieceType _pieceType = PieceType::Pawn;
static MoveList<MaxMoves>* __moves;
static Position* _position = nullptr;
/// <summary> same as GetLegalMovesFromBitboards but uses static global variables </summary>
static void _GetLegalMovesFromBitboards(int fromSquare)
{
	MoveSearcher::GetLegalMovesFromBitboards(*_position, _pieceType, static_cast<Square>(fromSquare), _position->IsWhiteToPlay(), *__moves);
}

void MoveSearcher::GetLegalMovesFromBitboards(Position& position, MoveList<MaxMoves>& allLegalMoves)
{
	allLegalMoves.clear();
	if (position.IsRepetitionDraw())
		return;

	const Bitboard& pawns = position.IsWhiteToPlay() ? position.GetWhitePawns() : position.GetBlackPawns();
	const Bitboard& knights = position.IsWhiteToPlay() ? position.GetWhiteKnights() : position.GetBlackKnights();
	const Bitboard& bishops = position.IsWhiteToPlay() ? position.GetWhiteBishops() : position.GetBlackBishops();
	const Bitboard& rooks = position.IsWhiteToPlay() ? position.GetWhiteRooks() : position.GetBlackRooks();
	const Bitboard& queens = position.IsWhiteToPlay() ? position.GetWhiteQueens() : position.GetBlackQueens();
	const Bitboard& king = position.IsWhiteToPlay() ? position.GetWhiteKing() : position.GetBlackKing();
	//loop on all set bits for every piece type

	__moves = &allLegalMoves;
	_position = &position;
	_pieceType = PieceType::Pawn;
	LoopOverSetBits(pawns, _GetLegalMovesFromBitboards);

	_pieceType = PieceType::Knight;
	LoopOverSetBits(knights, _GetLegalMovesFromBitboards);

	_pieceType = PieceType::Rook;
	LoopOverSetBits(rooks, _GetLegalMovesFromBitboards);

	_pieceType = PieceType::Bishop;
	LoopOverSetBits(bishops, _GetLegalMovesFromBitboards);

	_pieceType = PieceType::Queen;
	LoopOverSetBits(queens, _GetLegalMovesFromBitboards);

	_pieceType = PieceType::King;
	LoopOverSetBits(king, _GetLegalMovesFromBitboards);

	return;
}

static bool _isWhite = false;
static Bitboard _bitboard;
static void _GetPseudoLegalSquaresFromBitboards(int fromSquare)
{
	_bitboard |= MoveSearcher::GetPseudoLegalBitboardMoves(*_position, _pieceType, Bitboard(fromSquare), _isWhite, false);
}

Bitboard MoveSearcher::GetPseudoLegalSquaresFromBitboards(Position& position, bool isWhite, bool pawnControlledSquares)
{
	_bitboard = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	const Bitboard& knights = isWhite ? position.GetWhiteKnights() : position.GetBlackKnights();
	const Bitboard& bishops = isWhite ? position.GetWhiteBishops() : position.GetBlackBishops();
	const Bitboard& rooks = isWhite ? position.GetWhiteRooks() : position.GetBlackRooks();
	const Bitboard& queens = isWhite ? position.GetWhiteQueens() : position.GetBlackQueens();
	const Bitboard& king = isWhite ? position.GetWhiteKing() : position.GetBlackKing();
	//loop on all set bits for every piece type, except pawns which can be done in one step

	_bitboard |= GetPseudoLegalBitboardMoves(position, PieceType::Pawn, pawns, isWhite, pawnControlledSquares);
	
	_isWhite = isWhite;
	_position = &position;
	_pieceType = PieceType::Knight;
	LoopOverSetBits(knights, _GetPseudoLegalSquaresFromBitboards);

	_pieceType = PieceType::Rook;
	LoopOverSetBits(rooks, _GetPseudoLegalSquaresFromBitboards);

	_pieceType = PieceType::Bishop;
	LoopOverSetBits(bishops, _GetPseudoLegalSquaresFromBitboards);

	_pieceType = PieceType::Queen;
	LoopOverSetBits(queens, _GetPseudoLegalSquaresFromBitboards);

	_pieceType = PieceType::King;
	LoopOverSetBits(king, _GetPseudoLegalSquaresFromBitboards);

	return _bitboard;
}

std::vector<Move> MoveSearcher::GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece)
{
	//check and keep legal moves
	std::vector<std::array<int, 2>> legalSquares;
	std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, piece, isWhitePiece);
	
	for (const std::array<int, 2> & square : accessibleSquares)
	{
		//check square is not blocked by piece
		bool isBlocked = false;
		//check friendly pieces
		const std::vector<Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
		for (const Piece& friendlyPiece : friendlyPieces)
		{
			if ((piece.m_Square != friendlyPiece.m_Square) && //dont check itself! /!\ may not share same address
				IsMoveBlocked(friendlyPiece, piece, square))
			{
				isBlocked = true;
				break;
			}
		}

		if (isBlocked)
			continue;

		//check enemy pieces
		const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
		for (const Piece& enemyPiece : enemyPieces)
		{
			if (IsMoveBlocked(enemyPiece, piece, square))
			{
				//Check if can take (not blocked by other enemy pieces)
				//todo: Could optimize if pieces are adjacent
				if ((enemyPiece.Position() == square) &&
					!((piece.m_Type == PieceType::King) && (abs(piece.Position()[0] - square[0]) > 1)) && //castles can't take!
					((piece.m_Type != PieceType::Pawn) || (piece.Position()[0] != enemyPiece.Position()[0]))) //pawn can only take sideways
				{
					for (const Piece& enemyPiece2 : enemyPieces)
					{
						if ((&enemyPiece != &enemyPiece2) &&
							IsMoveBlocked(enemyPiece2, piece, enemyPiece.Position()))
						{
							isBlocked = true;
							break;
						}
					}
				}
				else
					isBlocked = true;

				break;
			}
		}

		if (isBlocked)
			continue;

		//move is not blocked by collision, check if illegal move
		Move move(piece.m_Type, piece.m_Square, piece.m_Square);
		move.SetToSquare(static_cast<Square>(square[0] + 8 * square[1]));
		if (IsMoveIllegal(position, move, isWhitePiece))
			continue;

		legalSquares.push_back(square);
	}

	std::vector<Move> legalMoves;
	for (const std::array<int, 2> & legalSquare : legalSquares)
	{
		Move move(piece.m_Type, piece.m_Square, piece.m_Square);
		move.SetToSquare(static_cast<Square>(legalSquare[0] + 8 * legalSquare[1]));
		//Add all possible queening moves if pawn
		if ((piece.m_Type == PieceType::Pawn) &&
			((isWhitePiece && (move.GetTo().Position()[1] == 7)) || (!isWhitePiece && (move.GetTo().Position()[1] == 0))))
		{
			move.SetToType(PieceType::Queen);
			legalMoves.emplace_back(move);
			move.SetToType(PieceType::Rook);
			legalMoves.emplace_back(move);
			move.SetToType(PieceType::Bishop);
			legalMoves.emplace_back(move);
			move.SetToType(PieceType::Knight);
			legalMoves.emplace_back(move);
		}
		else
			legalMoves.emplace_back(move);
	}

	return legalMoves;
}

/// <summary>Append to list so that idx stays valid ; idx should NOT be > list.size()</summary>
static void MakeValidIdx(MoveList<MaxMoves>& list, size_t i)
{
	if (i >= list.size())
		list.resize(list.size() + 1);
}

void MoveSearcher::GetLegalMovesFromBitboards(Position& position, PieceType type, Square square, bool isWhitePiece, MoveList<MaxMoves>& legalMoves)
{
	MoveList<MaxMoves>::iterator legalMovesInsertIt = legalMoves.end();
	GetPseudoLegalMovesFromBitboards(position, type, square, isWhitePiece, legalMoves);
	
	if (legalMoves.end() != legalMovesInsertIt)
	{
		MoveList<MaxMoves>::iterator newEnd = std::remove_if(legalMovesInsertIt, legalMoves.end(), [&position, isWhitePiece](Move& move)->bool
			{ return IsMoveIllegalFromBitboards(position, move, isWhitePiece); });

		//recover queening moves
		const size_t startIdx = (legalMovesInsertIt - legalMoves.begin());
		const size_t newEndIdx = (newEnd - legalMoves.begin());
		size_t queeningMovesIdx = newEndIdx; //we will add Rook, Bishop and Knight queening after this idx	
		if (legalMoves[startIdx].IsQueening()) //only check first move, the rest is necessarily queening too
		{
			for (size_t i = startIdx; i < newEndIdx; i++)
			{
				MakeValidIdx(legalMoves, queeningMovesIdx);
				legalMoves[queeningMovesIdx] = legalMoves[i];
				legalMoves[queeningMovesIdx].SetToType(PieceType::Rook);
				queeningMovesIdx++;

				MakeValidIdx(legalMoves, queeningMovesIdx);
				legalMoves[queeningMovesIdx] = legalMoves[i];
				legalMoves[queeningMovesIdx].SetToType(PieceType::Bishop);
				queeningMovesIdx++;

				MakeValidIdx(legalMoves, queeningMovesIdx);
				legalMoves[queeningMovesIdx] = legalMoves[i];
				legalMoves[queeningMovesIdx].SetToType(PieceType::Knight);
				queeningMovesIdx++;

				legalMoves[i].SetToType(PieceType::Queen);
			}
		}

		legalMoves.resize(queeningMovesIdx);
	}
}

Bitboard MoveSearcher::GetPseudoLegalBitboardMoves(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece, bool pawnAttackSquares)
{
	Bitboard toSquares;
	const Bitboard& friendlyPieces = (isWhitePiece ? position.GetWhitePieces() : position.GetBlackPieces());
	const Bitboard& enemyPieces = (isWhitePiece ? position.GetBlackPieces() : position.GetWhitePieces());
	const Bitboard& allPieces = (friendlyPieces | enemyPieces);

	switch (type)
	{
		case PieceType::King:
		{
			//bitboard has necessarily only 1 set bit
			toSquares = KingMoveTable[bitboard.GetSquare()] & ~friendlyPieces; //check collisions
			//Add castling moves, will be ignored if illegal
			if (isWhitePiece && position.CanWhiteCastleKingSide() &&
				((WhiteKingSideCastleInBetweenSquares & allPieces).m_Value == 0)) //check collisions
				toSquares |= Bitboard(g1);
			if (isWhitePiece && position.CanWhiteCastleQueenSide() &&
				((WhiteQueenSideCastleInBetweenSquares & allPieces).m_Value == 0))
				toSquares |= Bitboard(c1);
			if (!isWhitePiece && position.CanBlackCastleKingSide() &&
				((BlackKingSideCastleInBetweenSquares & allPieces).m_Value == 0))
				toSquares |= Bitboard(g8);
			if (!isWhitePiece && position.CanBlackCastleQueenSide() &&
				((BlackQueenSideCastleInBetweenSquares & allPieces).m_Value == 0))
				toSquares |= Bitboard(c8);
			break;
		}
		case PieceType::Knight:
		{
			toSquares = KnightMoveTable[bitboard.GetSquare()] & ~friendlyPieces; //check collisions
			break;
		}
		case PieceType::Pawn:
		{
			if (!pawnAttackSquares)
			{
				//For pawns we are able to parallelize (bitboard isn't single square)
				//Add single steps, check collisions too
				toSquares = (isWhitePiece ? (~_8 & bitboard) << 8 : (~_1 & bitboard) >> 8) & ~(allPieces);

				//Add double steps
				Bitboard doubleSteptoSquares = (isWhitePiece ? (_2 & bitboard) << 16 : (_7 & bitboard) >> 16) & ~(allPieces);
				//check collisions on row 3 or 6
				Bitboard blockingPieces = (isWhitePiece ? _3 : _6) & allPieces;
				doubleSteptoSquares &= ~(isWhitePiece ? blockingPieces << 8 : blockingPieces >> 8);
				toSquares |= doubleSteptoSquares;
			}

			//Add captures and en passant
			const Bitboard allButA_Pawns = (~_a) & bitboard;
			const Bitboard allButH_Pawns = (~_h) & bitboard;
			Bitboard attackSquares = ((isWhitePiece ? allButH_Pawns << 9 : allButH_Pawns >> 7) | (isWhitePiece ? allButA_Pawns << 7 : allButA_Pawns >> 9));
			if (!pawnAttackSquares)
			{
				const Bitboard enPassant = (position.GetEnPassantSquare().has_value() ? Bitboard(*position.GetEnPassantSquare()) : Bitboard());
				attackSquares &= (enemyPieces | enPassant);
			}
			toSquares |= attackSquares;

			break;
		}
		case PieceType::Queen:
		case PieceType::Rook:
		{
			toSquares = GenerateRookAttacks(bitboard.GetSquare(), allPieces);
			//mask with friendlyPieces to remove unwanted captures
			toSquares &= (~friendlyPieces);
			if (type == PieceType::Rook)
				break;
		}
		[[fallthrough]];
		case PieceType::Bishop:
		{
			toSquares |= GenerateBishopAttacks(bitboard.GetSquare(), allPieces);
			//mask with friendlyPieces to remove unwanted captures
			toSquares &= (~friendlyPieces);
			break;
		}
	}

	return toSquares;
}

void MoveSearcher::GetPseudoLegalMovesFromBitboards(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece, MoveList<MaxMoves>& moves)
{
	//collision checked but checks unchecked ("pseudo-legal")
	Bitboard toSquares = GetPseudoLegalBitboardMoves(position, type, bitboard, isWhitePiece, false);
	GenerateMoveList(type, bitboard.GetSquare(), toSquares, moves);
}

std::vector<std::array<int, 2>> MoveSearcher::GetAccessibleSquares(const Position& position, const Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> accessibleSquares;
	switch (piece.m_Type)
	{
	case PieceType::Pawn://Pawn is a special case
	{
		if (isWhitePiece && (piece.Position()[1] < 7) && (piece.Position()[1] >= 1)) //cannot move from last row
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 1 });
			if (piece.Position()[1] == 1) //two steps
				accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 2 });
		}
		else if (!isWhitePiece && (piece.Position()[1] <= 6) && (piece.Position()[1] > 0))
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 1 });
			if (piece.Position()[1] == 6) //two steps
				accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 2 });
		}
		//Add en-passant square
		if (position.GetEnPassantSquare().has_value())
		{
			int enPassantSquare = *position.GetEnPassantSquare();
			const std::array<int, 2>& square = { enPassantSquare % 8, enPassantSquare / 8 };
			if (abs(square[0] - piece.Position()[0]) == 1)
			{
				const int dy = (square[1] - piece.Position()[1]);
				if ((isWhitePiece && dy == 1) || (!isWhitePiece && dy == -1))
					accessibleSquares.push_back(square);
			}
		}
		//Add other possible captures
		std::vector<std::array<int, 2>> captureSquares = GetPawnCaptureSquares(position, piece, isWhitePiece);
		accessibleSquares.insert(accessibleSquares.end(), std::make_move_iterator(captureSquares.begin()), std::make_move_iterator(captureSquares.end()));

		break;
	}
	case PieceType::King:
		if (piece.Position()[1] < 7)
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 1 });
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] + 1 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] + 1 });
		}
		if (piece.Position()[1] > 0)
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 1 });
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] - 1 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] - 1 });
		}
		if (piece.Position()[0] > 0)
			accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] });
		if (piece.Position()[0] < 7)
			accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] });

		//Add castling moves, will be ignored if illegal
		if (isWhitePiece && position.CanWhiteCastleKingSide())
			accessibleSquares.push_back({ 6, 0 });
		if (isWhitePiece && position.CanWhiteCastleQueenSide())
			accessibleSquares.push_back({ 2, 0 });
		if (!isWhitePiece && position.CanBlackCastleKingSide())
			accessibleSquares.push_back({ 6, 7 });
		if (!isWhitePiece && position.CanBlackCastleQueenSide())
			accessibleSquares.push_back({ 2, 7 });

		break;
	case PieceType::Knight:
		if (piece.Position()[1] < 6)
		{
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] + 2 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] + 2 });
		}
		if (piece.Position()[1] > 1)
		{
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] - 2 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] - 2 });
		}
		if (piece.Position()[0] > 1)
		{
			if (piece.Position()[1] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 2, piece.Position()[1] - 1 });
			if (piece.Position()[1] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] - 2, piece.Position()[1] + 1 });
		}
		if (piece.Position()[0] < 6)
		{
			if (piece.Position()[1] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] + 2, piece.Position()[1] - 1 });
			if (piece.Position()[1] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 2, piece.Position()[1] + 1 });
		}
		break;
	case PieceType::Queen:
	case PieceType::Rook:
		//Along X
		for (int x = 0; x <= 7; x++)
		{
			if (x != piece.Position()[0])
				accessibleSquares.push_back({ x, piece.Position()[1] });
		}
		//Along Y
		for (int y = 0; y <= 7; y++)
		{
			if (y != piece.Position()[1])
				accessibleSquares.push_back({ piece.Position()[0], y });
		}
		if (piece.m_Type == PieceType::Rook)
			break; //continue for queen
		[[fallthrough]];
	case PieceType::Bishop:
	{
		constexpr std::array<int, 2> directions = { 1, -1 };
		for (int u : directions)
		{
			for (int d = 1; d <= 7; d++)
			{
				if (((u > 0) && (piece.Position()[0] + d * u <= 7)) ||
					((u < 0) && (piece.Position()[0] + d * u >= 0)))
				{
					for (int v : directions)
					{
						if (((v > 0) && (piece.Position()[1] + d * v <= 7)) ||
							((v < 0) && (piece.Position()[1] + d * v >= 0)))
						{
							accessibleSquares.push_back({ piece.Position()[0] + d * u, piece.Position()[1] + d * v });
						}
					}
				}
				else
					break;
			}
		}
		break;
	}
	default:
		assert(false); //Invalid type!
	}

	return accessibleSquares;
}

std::vector<std::array<int, 2>> MoveSearcher::GetPawnCaptureSquares(const Position& position, const Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> captureSquares;

	size_t count = 0;
	const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
	for (const Piece& enemyPiece : enemyPieces)
	{
		if (abs(enemyPiece.Position()[0] - piece.Position()[0]) == 1)
		{
			const int dy = (enemyPiece.Position()[1] - piece.Position()[1]);
			if (isWhitePiece && dy == 1 || !isWhitePiece && dy == -1)
			{
				captureSquares.push_back(enemyPiece.Position());
				count++;
			}
		}

		if (count == 2)
			break;
	}

	return captureSquares;
}

Bitboard MoveSearcher::GetAccessibleBitboard(const Position& position, const Piece& piece, bool isWhitePiece, bool isPawnDoubleStep)
{
	std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, piece, isWhitePiece);
	std::vector<std::array<int, 2>> pawnSquare;
	if (piece.m_Type == PieceType::Pawn)
	{
		for (const std::array<int, 2>& square : accessibleSquares)
		{
			const int dy = abs(piece.m_Square / 8 - square[1]);
			if ((isPawnDoubleStep && dy > 1) || (!isPawnDoubleStep && dy == 1))
				pawnSquare.push_back(square);
		}

		accessibleSquares = pawnSquare;
	}

	return ConvertToBitboard(accessibleSquares);
}

bool MoveSearcher::IsMoveBlocked(const Piece& blockingPiece, const Piece& piece, const std::array<int, 2>& square)
{
	bool isBlocking = false;

	switch (piece.m_Type)
	{
	case PieceType::King:
	{
		if ((square[0] - piece.Position()[0]) > 1) //kingside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.Position()) //check in between square
				isBlocking = true;
		}
		else if ((square[0] - piece.Position()[0]) < -1) //queenside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.Position())
				isBlocking = true;
			if (std::array<int, 2>{square[0] + 1, square[1]} == blockingPiece.Position())
				isBlocking = true;
		}
	}
	[[fallthrough]];
	case PieceType::Knight:
	case PieceType::Pawn:
		if (square == blockingPiece.Position())
			isBlocking = true;
		if (piece.m_Type == PieceType::Pawn && abs(square[1] - piece.Position()[1]) > 1) //double square move
		{
			if ((square[1] - piece.Position()[1]) > 0 && std::array<int, 2>{square[0], square[1] - 1} == blockingPiece.Position()) //white pawn
				isBlocking = true;
			if ((square[1] - piece.Position()[1]) < 0 && std::array<int, 2>{square[0], square[1] + 1} == blockingPiece.Position()) //black pawn
				isBlocking = true;
		}
		break;
	case PieceType::Queen:
	case PieceType::Rook:
	{
		constexpr std::array<bool, 2> checkRow = { false, true };
		for (bool row : checkRow)
		{
			const size_t i = (!row) * 1; //must be 0 for row (X)
			const size_t j = row * 1; //must be 1 for row (Y)

			//j = const => we check row
			if ((square[j] == blockingPiece.Position()[j]) && (square[j] == piece.Position()[j]))
			{
				const int min = std::min(piece.Position()[i], square[i]);
				const int max = std::max(piece.Position()[i], square[i]);
				if (min <= blockingPiece.Position()[i] && blockingPiece.Position()[i] <= max)
				{
					isBlocking = true;
					break;
				}
			}
		}
		if (piece.m_Type == PieceType::Rook)
			break;
	}
	[[fallthrough]];
	case PieceType::Bishop:
	{
		//check same diagonal
		if ((abs(piece.Position()[0] - blockingPiece.Position()[0]) == abs(piece.Position()[1] - blockingPiece.Position()[1])) && //same diagonal
			((abs(piece.Position()[0] - square[0]) == abs(piece.Position()[1] - square[1]))) &&
			((abs(blockingPiece.Position()[0] - square[0]) == abs(blockingPiece.Position()[1] - square[1]))))
		{
			//check blocking piece in between (only have to check X or Y as they're already on same diagonal)
			const int min = std::min(piece.Position()[0], square[0]);
			const int max = std::max(piece.Position()[0], square[0]);
			if (min <= blockingPiece.Position()[0] && blockingPiece.Position()[0] <= max)
				isBlocking = true;
		}

		break;
	}
	default:
		assert(false); //invalid type!
	}

	return isBlocking;
}

bool MoveSearcher::IsMoveIllegal(const Position& position, const Move& move, bool isWhitePiece)
{
	//we check new position with moved piece
	Position newPosition = position;

	Move moveCopy = move;
	newPosition.Update(moveCopy);

	bool isIllegal = IsKingInCheck(newPosition, isWhitePiece);

	//Also check castling moves
	if (!isIllegal && move.IsCastling())
	{
		if (IsKingInCheck(position, isWhitePiece))
			isIllegal = true;
		else if (move.GetToSquare() > move.GetFromSquare()) //kingside castle, check in between square
		{
			newPosition = position;
			moveCopy.SetToSquare(Square(static_cast<int>(moveCopy.GetToSquare()) - 1));
			newPosition.Update(moveCopy);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
		else //queenside
		{
			newPosition = position;
			moveCopy.SetToSquare(Square(static_cast<int>(moveCopy.GetToSquare()) + 1));
			newPosition.Update(moveCopy);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
	}

	return isIllegal;
}

bool MoveSearcher::IsMoveIllegalFromBitboards(Position& position, Move& move, bool isWhitePiece)
{
	//we check new position with moved piece
	position.Update(move);
	bool isIllegal = IsKingInCheckFromBitboards(position, isWhitePiece);
	position.Undo(move);

	//Also check castling moves
	if (!isIllegal && move.IsCastling())
	{
		if (IsKingInCheckFromBitboards(position, isWhitePiece))
			isIllegal = true;
		else
		{
			const Square squareBackup = move.GetToSquare();
			if (move.GetToSquare() > move.GetFromSquare()) //kingside castle, check in between square
				move.SetToSquare(Square(static_cast<int>(move.GetToSquare()) - 1));
			else //queenside
				move.SetToSquare(Square(static_cast<int>(move.GetToSquare()) + 1));

			position.Update(move);
			isIllegal = IsKingInCheckFromBitboards(position, isWhitePiece);
			position.Undo(move);
			move.SetToSquare(squareBackup);
		}
	}

	return isIllegal;
}

size_t MoveSearcher::Perft(Position& position, int depth, std::array<MoveList<MaxMoves>, PerftMaxDepth>& moveLists)
{
	if (depth == 0)
		return 1;

	size_t count = 0;
	GetLegalMovesFromBitboards(position, moveLists[depth - 1]);

	for (Move& move : moveLists[depth - 1])
	{
		position.Update(move);
		count += Perft(position, depth - 1, moveLists);
		position.Undo(move);
	}

	return count;
}

std::unordered_set<uint64_t> MoveSearcher::UniquePerft(Position& position, int depth, std::array<MoveList<MaxMoves>, PerftMaxDepth>& moveLists)
{
	if (depth == 0)
		return { position.GetZobristHash() };

	std::unordered_set<uint64_t> uniqueNodes;
	GetLegalMovesFromBitboards(position, moveLists[depth - 1]);
	for (Move& legalMove : moveLists[depth - 1])
	{
		position.Update(legalMove);
		std::unordered_set<uint64_t> terminalNodes = UniquePerft(position, depth - 1, moveLists);
		position.Undo(legalMove);
		uniqueNodes.insert(terminalNodes.begin(), terminalNodes.end());
	}

	return uniqueNodes;
}

bool MoveSearcher::IsKingInCheck(const Position& position, bool isWhitePiece)
{
	bool isKingInCheck = false;
	const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
	const std::vector<Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	const Piece* king = nullptr;

	for (const Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Type == PieceType::King)
		{
			king = &friendlyPiece;
			break;
		}
	}

	if (!king)
	{
		//don't assert, it may be valid for testing
		return false;
	}

	for (const Piece& enemyPiece : enemyPieces)
	{
		bool canGetToKing = false;
		std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, enemyPiece, !isWhitePiece);
		for (const std::array<int, 2> & square : accessibleSquares)
		{
			//For pawns, ignore squares in front of them (can't give a check)
			if ((enemyPiece.m_Type == PieceType::Pawn) && enemyPiece.Position()[0] == square[0])
				continue;

			if (square == king->Position())
			{
				canGetToKing = true;
				break;
			}
		}

		if (!canGetToKing)
			continue;

		bool protectedByFriend = false;
		for (const Piece& friendlyPiece : friendlyPieces)
		{
			if ((&friendlyPiece != king) && IsMoveBlocked(friendlyPiece, enemyPiece, king->Position()))
			{
				protectedByFriend = true;
				break;
			}
		}

		if (protectedByFriend)
			continue;

		bool protectedByEnemy = false;
		for (const Piece& enemyPiece2 : enemyPieces)
		{
			if ((&enemyPiece != &enemyPiece2) && IsMoveBlocked(enemyPiece2, enemyPiece, king->Position()))
			{
				protectedByEnemy = true;
				break;
			}
		}

		if (protectedByEnemy)
			continue;

		isKingInCheck = true;
		break;
	}

	return isKingInCheck;
}

bool MoveSearcher::IsKingInCheckFromBitboards(const Position& position, bool isWhiteKing)
{
	const Bitboard& kingPosition = (isWhiteKing ? position.GetWhiteKing() : position.GetBlackKing());
	if (!kingPosition)
		return false; //no king, but valid for some tests

	Bitboard attackSquares;
	//get attack squares of a "super piece" placed at king's position, then cross with enemy pieces positions ; skip queen (bishop + rook)
	Bitboard enemyPieces = (isWhiteKing ? position.GetBlackKnights() : position.GetWhiteKnights());
	if (enemyPieces > 0)
	{
		attackSquares = KnightMoveTable[kingPosition.GetSquare()];
		if ((attackSquares & enemyPieces) > 0)
			return true;
	}

	const int kingSquare = kingPosition.GetSquare();
	enemyPieces = (isWhiteKing ? position.GetBlackBishops() : position.GetWhiteBishops()) |
		(isWhiteKing ? position.GetBlackQueens() : position.GetWhiteQueens());
	//Only check bishops, rooks or queens if in alignment with rook
	if ((enemyPieces > 0) && ((enemyPieces & BishopMoveTable[kingSquare]) > 0))
	{
		attackSquares = GetPseudoLegalBitboardMoves(position, PieceType::Bishop, kingPosition, isWhiteKing, false);
		if ((attackSquares & enemyPieces) > 0)
			return true;
	}

	enemyPieces = (isWhiteKing ? position.GetBlackRooks() : position.GetWhiteRooks()) |
		(isWhiteKing ? position.GetBlackQueens() : position.GetWhiteQueens());
	if ((enemyPieces > 0) && ((enemyPieces & RookMoveTable[kingSquare]) > 0))
	{
		attackSquares = GetPseudoLegalBitboardMoves(position, PieceType::Rook, kingPosition, isWhiteKing, false);
		if ((attackSquares & enemyPieces) > 0)
			return true;
	}

	enemyPieces = (isWhiteKing ? position.GetBlackPawns() : position.GetWhitePawns());
	if (enemyPieces > 0)
	{
		attackSquares = (isWhiteKing ? WhitePawnCaptureMoveTable[kingSquare] : BlackPawnCaptureMoveTable[kingSquare]);
		if ((attackSquares & enemyPieces) > 0)
			return true;
	}

	enemyPieces = (isWhiteKing ? position.GetBlackKing() : position.GetWhiteKing());
	if (enemyPieces > 0)
	{
		attackSquares = KingMoveTable[kingSquare];
		if ((attackSquares & enemyPieces) > 0)
			return true;
	}

	return false;
}

std::optional<Move> MoveSearcher::GetRandomMove(const Position& position)
{
	std::optional<Move> move;
	std::vector<Move> allLegalMoves = MoveSearcher::GetLegalMoves(position);
	if (allLegalMoves.empty())
		return move;

	const int rand = std::rand();
	move = allLegalMoves[rand % allLegalMoves.size()];
	return move;
}

std::optional<Move> MoveSearcher::GetRandomMoveFromBitboards(Position& position)
{
	std::optional<Move> move;
	MoveList<MaxMoves> allLegalMoves;
	GetLegalMovesFromBitboards(position, allLegalMoves);
	if (allLegalMoves.empty())
		return move;

	const int rand = std::rand();
	move = allLegalMoves[rand % allLegalMoves.size()];
	return move;
}

const std::vector<Bitboard>& MoveSearcher::GetMoveTable(PieceType type, bool isWhite)
{
	switch (type)
	{
	case PieceType::King:
		return KingMoveTable;
	case PieceType::Queen:
		return QueenMoveTable;
	case PieceType::Rook:
		return RookMoveTable;
	case PieceType::Bishop:
		return BishopMoveTable;
	case PieceType::Knight:
		return KnightMoveTable;
	case PieceType::Pawn:
	default:
		return (isWhite ? WhitePawnMoveTable : BlackPawnMoveTable);
	}
}