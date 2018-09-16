#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
	GameImpl(int nRows, int nCols);
	~GameImpl();
	int rows() const;
	int cols() const;
	bool isValid(Point p) const;
	Point randomPoint() const;
	bool addShip(int length, char symbol, string name);
	int nShips() const;
	int shipLength(int shipId) const;
	char shipSymbol(int shipId) const;
	string shipName(int shipId) const;
	Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:
	int m_Rows, m_Cols, m_Ships;
	struct Node {
		int mLength;
		char mSymbol;
		string mName;
		int ID;
		Node* next;
	};
	Node* head;
};

void waitForEnter()
{
	cout << "Press enter to continue: ";
	cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : m_Rows(nRows), m_Cols(nCols), m_Ships(0)
{
	if (nRows > MAXROWS || nCols > MAXCOLS)
		exit(1);
	head = nullptr; //intialize head of linked list to null

}

GameImpl::~GameImpl() {
	while (head != nullptr) { //deletes all dynamically allocated linked list members
		Node* ptr = head;
		head = head->next;
		delete ptr;
	}
}

int GameImpl::rows() const
{
	return m_Rows;
}

int GameImpl::cols() const
{
	return m_Cols;
}

bool GameImpl::isValid(Point p) const
{
	return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
	return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	if (length <= 0 || (length > m_Rows && length > m_Cols) || symbol == 'o' || symbol == 'X' || symbol == '.') //checks for bad conditions
		return false;

	for (Node*ptr = head; ptr != nullptr; ptr = ptr->next) //checks for duplicate ships
		if (ptr->mSymbol == symbol || ptr->mName == name)
			return false;

	Node* p = new Node; //inserts ship as a new node in the front of the linked list
	p->mLength = length;
	p->mSymbol = symbol;
	p->mName = name;
	p->ID = m_Ships;
	p->next = head;
	head = p;

	m_Ships++;
	return true;
}

int GameImpl::nShips() const
{
	return m_Ships;
}

int GameImpl::shipLength(int shipId) const
{
	for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) //finds corresponding ship and returns its length
		if (ptr->ID == shipId)
			return ptr->mLength;

	return -1;
}

char GameImpl::shipSymbol(int shipId) const
{
	for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) //finds corresponding ship and returns its symbol
		if (ptr->ID == shipId)
			return ptr->mSymbol;

	return '?';
}

string GameImpl::shipName(int shipId) const
{
	for (Node* ptr = head; ptr != nullptr; ptr = ptr->next) //finds corresponding ship and returns its name
		if (ptr->ID == shipId)
			return ptr->mName;

	return "";
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
	b1.clear();
	b2.clear();
	if (!p1->placeShips(b1) || !p2->placeShips(b2)) //returns null if either player cannot place ships
		return nullptr;

	Point p;
	bool hit, destroy, gate;
	int Id;

	for (;;) { //infinite loop

		cout << p1->name() << "'s turn.  Board for " << p2->name() << ": " << endl;

		if (p1->isHuman())
			b2.display(true);		//switches between display settings based on human player or not
		else b2.display(false);

		p = p1->recommendAttack();				//first player recommends attack
		if (b2.attack(p, hit, destroy, Id))		//second player updates board to reflect target coordinate
			gate = true;
		else gate = false;
		p1->recordAttackResult(p, gate, hit, destroy, Id);	//first player records result of attack
		p2->recordAttackByOpponent(p);						//second player records opponents atack

		if (!gate)											//prints out statement based on result of attack
			cout << p1->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
		else {
			cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and";
			if (destroy)
				cout << " destroyed the " << shipName(Id);
			else if (hit)
				cout << " hit something";
			else cout << " missed";
			cout << ", resulting in: " << endl;
		}

		if (p1->isHuman())							//once again displayssecond player's board after attack has been made
			b2.display(true);
		else b2.display(false);

		if (b2.allShipsDestroyed()) {			//if all of the second player's ships are destroyed, first player won
			if (p2->isHuman()) {
				b1.display(false);
			}

			cout << p1->name() << " won the game!" << endl;

			return p1;
		}
		//repeats whole process with player one and two switched
		if (shouldPause)
			waitForEnter();

		cout << p2->name() << "'s turn.  Board for " << p1->name() << ": " << endl;

		if (p2->isHuman())
			b1.display(true);
		else b1.display(false);

		p = p2->recommendAttack();
		if (b1.attack(p, hit, destroy, Id))
			gate = true;
		else gate = false;
		p2->recordAttackResult(p, gate, hit, destroy, Id);
		p1->recordAttackByOpponent(p);

		if (!gate)
			cout << p2->name() << " wasted a shot at (" << p.r << "," << p.c << ")." << endl;
		else {
			cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and";
			if (destroy)
				cout << " destroyed the " << shipName(Id);
			else if (hit)
				cout << " hit something";
			else cout << " missed";
			cout << ", resulting in: " << endl;
		}

		if (p2->isHuman())
			b1.display(true);
		else b1.display(false);

		if (b1.allShipsDestroyed()) {
			if (p1->isHuman()) {
				b2.display(false);
			}

			cout << p2->name() << " won the game!" << endl;

			return p2;
		}

		if (shouldPause)
			waitForEnter();

	}

	return nullptr;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
	if (nRows < 1 || nRows > MAXROWS)
	{
		cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
		exit(1);
	}
	if (nCols < 1 || nCols > MAXCOLS)
	{
		cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
		exit(1);
	}
	m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
	delete m_impl;
}

int Game::rows() const
{
	return m_impl->rows();
}

int Game::cols() const
{
	return m_impl->cols();
}

bool Game::isValid(Point p) const
{
	return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
	return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
	if (length < 1)
	{
		cout << "Bad ship length " << length << "; it must be >= 1" << endl;
		return false;
	}
	if (length > rows() && length > cols())
	{
		cout << "Bad ship length " << length << "; it won't fit on the board"
			<< endl;
		return false;
	}
	if (!isascii(symbol) || !isprint(symbol))
	{
		cout << "Unprintable character with decimal value " << symbol
			<< " must not be used as a ship symbol" << endl;
		return false;
	}
	if (symbol == 'X' || symbol == '.' || symbol == 'o')
	{
		cout << "Character " << symbol << " must not be used as a ship symbol"
			<< endl;
		return false;
	}
	int totalOfLengths = 0;
	for (int s = 0; s < nShips(); s++)
	{
		totalOfLengths += shipLength(s);
		if (shipSymbol(s) == symbol)
		{
			cout << "Ship symbol " << symbol
				<< " must not be used for more than one ship" << endl;
			return false;
		}
	}
	if (totalOfLengths + length > rows() * cols())
	{
		cout << "Board is too small to fit all ships" << endl;
		return false;
	}
	return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
	return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
	if (p1 == nullptr || p2 == nullptr || nShips() == 0)
		return nullptr;
	Board b1(*this);
	Board b2(*this);
	return m_impl->play(p1, p2, b1, b2, shouldPause);
}
