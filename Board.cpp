#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
	BoardImpl(const Game& g);
	void clear();
	void block();
	void unblock();
	bool placeShip(Point topOrLeft, int shipId, Direction dir);
	bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
	void display(bool shotsOnly) const;
	bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
	bool allShipsDestroyed() const;
	bool isValidPlacement(int r, int c, int length, Direction dir);
	~BoardImpl();

private:
	const Game& m_game;
	char m_grid[MAXROWS][MAXCOLS];
	struct Ships {
		int r, c;
		string m_name;
		Direction m_dir;
		int health, ID;
		char m_symbol;
		Ships* next;
	};
	int shipsLeft;
	Ships* head;
};

BoardImpl::BoardImpl(const Game& g) : m_game(g)
{

	head = nullptr;					//intialize linked list to empty
	shipsLeft = m_game.nShips();

	for (int i = 0; i < g.rows(); i++)			//populate board with dots
		for (int j = 0; j < g.cols(); j++)
			m_grid[i][j] = '.';

}

BoardImpl::~BoardImpl() {
	while (head != nullptr) {		//delete any dynamically allocated ships in linked list
		Ships* ptr = head;
		head = head->next;
		delete ptr;
	}
}

void BoardImpl::clear()
{
	for (int r = 0; r < m_game.rows(); r++)				//clears board with dots
		for (int c = 0; c < m_game.cols(); c++)
			m_grid[r][c] = '.';

}

void BoardImpl::block()
{
	// Block cells with 50% probability
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
			if (randInt(2) == 0)
			{
				m_grid[r][c] = 'X'; // blocks corresponding cell
			}
}

void BoardImpl::unblock()
{
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (m_grid[r][c] == 'X')
				m_grid[r][c] = '.'; // unblocks any previously blocked cells
		}
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	int length = m_game.shipLength(shipId);
	if (length == -1)						//checks for invalid ship
		return false;
	int r = topOrLeft.r;
	int c = topOrLeft.c;
	if (r < 0 || c < 0 || r >= m_game.rows() || c >= m_game.cols())		//checks for invalid placement
		return false;

	if (dir == VERTICAL && r + length > m_game.rows())
		return false;

	if (dir == HORIZONTAL &&  c + length > m_game.cols())
		return false;

	if (!isValidPlacement(r, c, length, dir))					//calls function to determine if ship will fit at desired location
		return false;

	string name = m_game.shipName(shipId);
	for (Ships* ptr = head; ptr != nullptr; ptr = ptr->next) {			//checks for duplicate ships
		if (ptr->m_name == name)
			return false;
	}

	Ships* p = new Ships;			//creats new node in linked list for this ship to occupy
	p->c = c;
	p->r = r;
	p->m_name = name;
	p->next = head;
	p->m_dir = dir;
	p->health = length;
	p->ID = shipId;
	head = p;

	char symbol = m_game.shipSymbol(shipId);
	p->m_symbol = symbol;

	if (dir == HORIZONTAL) {					//replaces dots on board with ship symbol
		for (int i = 0; i < length; i++)
			m_grid[r][c + i] = symbol;
	}

	if (dir == VERTICAL) {
		for (int i = 0; i < length; i++)
			m_grid[r + i][c] = symbol;
	}

	return true;
}

bool BoardImpl::isValidPlacement(int r, int c, int length, Direction dir) {
	if (dir == HORIZONTAL) {							//checks each point that the ship would occupy if placed at a given location and returns false if any point is not a dot
		for (int i = 0; i < length; i++)
			if (m_grid[r][c + i] != '.')
				return false;
	}
	else if (dir == VERTICAL) {
		for (int i = 0; i < length; i++)
			if (m_grid[r + i][c] != '.')
				return false;
	}

	return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	int length = m_game.shipLength(shipId); //checks for invalid ship
	if (length == -1)
		return false;

	string nm = m_game.shipName(shipId);
	Ships* ptr = head;
	Ships* ptr2 = head;
	for (; ptr != nullptr && ptr->m_name != nm; ptr = ptr->next)	//loops until ptr points to target ship
		ptr2 = ptr;													//ptr2 points to one node behind ptr

	if (ptr == nullptr)			//return false if ship not found
		return false;

	if (topOrLeft.c != ptr->c || topOrLeft.r != ptr->r || dir != ptr->m_dir)	//return false if indicated point or direction is incorrect
		return false;

	if (dir == HORIZONTAL) {									//replace ship symbol on board with dots
		for (int i = 0; i < length; i++) {
			m_grid[topOrLeft.r][topOrLeft.c + i] = '.';
		}
	}

	if (dir == VERTICAL) {
		for (int i = 0; i < length; i++) {
			m_grid[topOrLeft.r + i][topOrLeft.c] = '.';
		}
	}

	if (ptr2 == head)					//removes ship node from linked list
		head = ptr->next;
	else ptr2->next = ptr->next;

	delete ptr;

	return true;
}

void BoardImpl::display(bool shotsOnly) const
{
	int c = m_game.cols();

	if (!shotsOnly) {
		cout << "  ";
		for (int i = 0; i < c; i++)				//prints out numbered columns
			cout << i;

		cout << endl;

		for (int j = 0; j < m_game.rows(); j++) {
			cout << j << " ";
			for (int i = 0; i < c; i++) {
				cout << m_grid[j][i];				//prints out numbered row and board grid
			}
			cout << endl;
		}
	}

	if (shotsOnly) {
		cout << "  ";
		for (int i = 0; i < c; i++)
			cout << i;

		cout << endl;

		for (int j = 0; j < m_game.rows(); j++) {
			cout << j << " ";
			for (int i = 0; i < c; i++) {
				if (m_grid[j][i] != 'X' && m_grid[j][i] != '.' && m_grid[j][i] != 'o') //same as above except ship symbols are replaced with dots
					cout << '.';
				else cout << m_grid[j][i];
			}
			cout << endl;
		}
	}

}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	if (p.c >= m_game.cols() || p.r >= m_game.rows() || p.c < 0 || p.r < 0) { //checks for invalid point
		shotHit = false;
		shipDestroyed = false;
		return false;
	}
	if (m_grid[p.r][p.c] == 'o' || m_grid[p.r][p.c] == 'X') { //checks for already targeted location
		shotHit = false;
		shipDestroyed = false;
		return false;
	}

	if (m_grid[p.r][p.c] != '.') {						//if shot is not a dot, it hit a ship
		char sym = m_grid[p.r][p.c];
		m_grid[p.r][p.c] = 'X';
		shotHit = true;
		for (Ships* ptr = head; ptr != nullptr; ptr = ptr->next) {	//finds corresponding ship based coordinate symbol
			if (ptr->m_symbol == sym) {
				ptr->health--;										//decrements ships health
				if (ptr->health == 0) {
					shipDestroyed = true;							//checks if ship is destroyed
					shipsLeft--;
					shipId = ptr->ID;
				}
				else shipDestroyed = false;

				break;
			}
		}
	}
	else {							//if shot missed, update board with 'o'
		shotHit = false;
		shipDestroyed = false;
		m_grid[p.r][p.c] = 'o';
	}

	return true;
}

bool BoardImpl::allShipsDestroyed() const	//returns true if a board has no surviving ship
{
	if (shipsLeft == 0)
		return true;

	return false;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
	m_impl = new BoardImpl(g);
}

Board::~Board()
{
	delete m_impl;
}

void Board::clear()
{
	m_impl->clear();
}

void Board::block()
{
	return m_impl->block();
}

void Board::unblock()
{
	return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
	m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
	return m_impl->allShipsDestroyed();
}

