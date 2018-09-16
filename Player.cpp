#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
	AwfulPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
	// Clustering ships is bad strategy
	for (int k = 0; k < game().nShips(); k++)
		if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
			return false;
	return true;
}

Point AwfulPlayer::recommendAttack()
{
	if (m_lastCellAttacked.c > 0)
		m_lastCellAttacked.c--;
	else
	{
		m_lastCellAttacked.c = game().cols() - 1;
		if (m_lastCellAttacked.r > 0)
			m_lastCellAttacked.r--;
		else
			m_lastCellAttacked.r = game().rows() - 1;
	}
	return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
	bool result(cin >> r >> c);
	if (!result)
		cin.clear();  // clear error state so can do more input operations
	cin.ignore(10000, '\n');
	return result;
}


class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	virtual bool isHuman() const { return true; }
};

HumanPlayer::HumanPlayer(string nm, const Game& g) : Player(nm, g) {}

bool HumanPlayer::placeShips(Board& b) {

	for (int k = 0; k < game().nShips(); k++) {
		Direction dir;
		b.display(false);
		for (;;) {				//infinitely loops until user inputs correct value
			cout << "Enter h or v for direction of " << game().shipName(k) << " " << "(length " << game().shipLength(k) << " ): ";
			char com;
			cin >> com;			//prompts for user input and updates desired direction accordingly
			if (com == 'h') {
				dir = HORIZONTAL;
				break;
			}
			else if (com == 'v') {
				dir = VERTICAL;
				break;
			}
			else cout << "Direction must be h or v." << endl;	//if input is invalid
		}

		if (dir == HORIZONTAL)
			for (;;) {				//loops infinitely until required input
				cout << "Enter row and column of leftmost cell (e.g. 3 5 ): ";
				int r, c;
				if (getLineWithTwoIntegers(r, c)) {						//prompts user input
					if (!b.placeShip(Point(r, c), k, HORIZONTAL))			//attempts to place ship at requested location
						cout << "The ship can not be placed there." << endl;	//if place ship is not possible
					else {
						break;
					}
				}
				else cout << "You must enter two integers." << endl;			//invalid input
			}

		else if (dir == VERTICAL) {					//same process as above but for vertical direction
			for (;;) {
				cout << "Enter row and column of topmost cell (e.g. 3 5 ): ";
				int r, c;
				if (getLineWithTwoIntegers(r, c)) {
					if (!b.placeShip(Point(r, c), k, VERTICAL))
						cout << "The ship can not be placed there." << endl;
					else {
						break;
					}
				}
				else cout << "You must enter two integers." << endl;
			}
		}
	}

	return true;
}

Point HumanPlayer::recommendAttack() {
	cout << "Enter the row and column to attack (e.g. 3 5): ";
	int r, c;
	while (!getLineWithTwoIntegers(r, c)); //prompts for user input until it is valid

	return Point(r, c);
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player {
public:
	MediocrePlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	bool placeRec(Board & b, int r = 0, int c = 0, int k = 0);
private:
	char cellsAttacked[MAXROWS][MAXCOLS];
	Point lastAttacked;
	int state;
};
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : lastAttacked(-1, -1), Player(nm, g) {
	state = 1;													//begin in state 1

	for (int r = 0; r < game().rows(); r++)						//initialize attacked cells to empty
		for (int c = 0; c < game().cols(); c++)
			cellsAttacked[r][c] = '.';

}

bool MediocrePlayer::placeRec(Board & b, int r, int c, int k) {
	if (k == game().nShips())										//if all ships are on board return true
		return true;

	if (r == game().rows() && c == game().cols())					//case if its not possible for all ships to fit on board
		return false;

	if (b.placeShip(Point(r, c), k, HORIZONTAL))					//attempt to place ship horizontally
		if (placeRec(b, 0, 0, k + 1)) {
			return true;
		}
		else (b.unplaceShip(Point(r, c), k, HORIZONTAL));			//if this causes the next ships to be unplaceable, undo its placement


		if (b.placeShip(Point(r, c), k, VERTICAL))						//same process as above but for vertical placement
			if (placeRec(b, 0, 0, k + 1)) {
				return true;
			}
			else (b.unplaceShip(Point(r, c), k, VERTICAL));


			if (r < game().rows())									//if the ship did not fit in this point, move down one row
				return placeRec(b, r + 1, c, k);

			r = 0;													//if we are at the last row, move over to top of next column
			return placeRec(b, r, c + 1, k);

			return false;
}

bool MediocrePlayer::placeShips(Board & b) {
	for (int i = 0; i < 50; i++) {			//attempts to place ships 50 times
		b.block();							//blocks board
		if (placeRec(b)) {					//calls recursive function
			b.unblock();					//unblocks board
			return true;
		}
		b.clear();
	}
	return false;
}

void MediocrePlayer::recordAttackByOpponent(Point p) {
	//does nothing
}

Point MediocrePlayer::recommendAttack() {
	int rows = game().rows();
	int cols = game().cols();

	if (state == 2) {

		int r = lastAttacked.r;					//records last hit coordinate
		int c = lastAttacked.c;
		int leastr = r - 4;						//records possible attack range for rows and columns
		if (leastr < 0)
			leastr = 0;
		int greatr = r + 4;
		if (greatr > rows)
			greatr = rows;

		int leastc = c - 4;
		if (leastc < 0)
			leastc = 0;
		int greatc = c + 4;
		if (greatc > cols)
			greatc = cols;

		int ranger = greatr - leastr;
		int rangec = greatc - leastc;
		int max = greatr;
		if (max == rows)
			max--;
		state = 1;
		for (int i = leastr; i <= max; i++)			//returns to state 1 if all nearby cells have been attacked already
			if (cellsAttacked[i][c] == '.') {
				state = 2;
				break;
			}
			
		max = greatc;
		if (max == cols)
			max--;
		if (state == 1)
			for (int i = leastc; i <= max; i++)
				if (cellsAttacked[r][i] == '.') {
					state = 2;
					break;
				}
				

				if (state == 2) {
					for (;;) {				//infinite loop

						int gate = randInt(2);				//random number decides to attack horizontaly or vertically
						if (gate == 1) {					// generate random row point to attack within range
							r = randInt(ranger + 1);
							r = leastr + r;
						}
						else {
							c = randInt(rangec + 1);		//generate random column point to attack within range
							c = leastc + c;
						}

						if (cellsAttacked[r][c] == '.') {
							cellsAttacked[r][c] = '*';				//if the point has not been attacked, mark it and attack it
							return Point(r, c);
						}
						else {							//otherwise repeat the process
							r = lastAttacked.r;
							c = lastAttacked.c;
						}
					}
				}

	}

	if (state == 1) {
		for (;;) {
			int r = randInt(rows);				//attacks random point on the board
			int c = randInt(cols);
			if (cellsAttacked[r][c] == '.') {
				cellsAttacked[r][c] = '*';
				return Point(r, c);
			}
		}
	}

	return Point(0, 0);
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {

	if (!validShot)
		return;

	if (!shotHit)
		return;

	if (shotHit && shipDestroyed) {			//resets to state 1 if a ship is destroyed
		state = 1;
		lastAttacked = Point(-1, -1);
		return;
	}

	if (shotHit && !shipDestroyed) {
		if (state == 1) {
			state = 2;
			lastAttacked.r = p.r;				//records hit location if the player is not currently targeting a location and it hit something
			lastAttacked.c = p.c;
		}
		return;
	}

}
//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player {
public:
	GoodPlayer(string name, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	~GoodPlayer();
	Point findSpot();
	bool placeRec(Board& b, int counter, int k = 0, int r = 0, int c = 0);
	bool checkFit(Point p, int length, Direction dir);
private:
	char attackedCells[MAXROWS][MAXCOLS];
	int m_state, nShots, oppShots, target, totalHealth, currentHealth;
	struct Node {
		int shipId;
		bool shipDestroyed;
		vector<Point> pos;
		Node* next;
	};
	Node* head;
	char m_grid[MAXROWS][MAXCOLS];
	struct myShips {
		int Id, health;
		bool destroyed;
		char symbol;
		myShips* next;
	};
	myShips* headptr;
	struct shipsRemaining {
		int ID, length;
		shipsRemaining* next;
		bool destroyed;
	};
	shipsRemaining* shipPTR;
	int densityGrid[MAXROWS][MAXCOLS];
};

bool GoodPlayer::placeRec(Board & b, int counter, int k, int r, int c) {			//same process as mediocre player
	if (k == game().nShips())
		return true;
	counter++;

	if (counter > 1000)
		return false;

	r = randInt(game().rows());
	c = randInt(game().cols());

	int randDir = randInt(2);				//chooses to place ships randomly
	if (randDir == 0) {
		if (b.placeShip(Point(r, c), k, HORIZONTAL))
			if (placeRec(b, counter, k + 1)) {
				for (int i = 0; i < game().shipLength(k); i++) {			//records the placed ship location on its private grid
					m_grid[r][c + i] = game().shipSymbol(k);
				}
				return true;
			}
			else (b.unplaceShip(Point(r, c), k, HORIZONTAL));		//remove ship if it prevents other from fitting
	}
	else {
		if (b.placeShip(Point(r, c), k, VERTICAL))			//same process for vertical direction
			if (placeRec(b, counter, k + 1)) {
				return true;
				for (int i = 0; i < game().shipLength(k); i++) {
					m_grid[r + i][c] = game().shipSymbol(k);
				}
			}
			else (b.unplaceShip(Point(r, c), k, VERTICAL));
	}

	return placeRec(b, counter, k);		//repeat process until all ships placed
}

bool GoodPlayer::checkFit(Point p, int length, Direction dir) {
	int r = p.r;
	int c = p.c;
	if (dir == VERTICAL && r + length > game().rows())
		return false;

	if (dir == HORIZONTAL &&  c + length > game().cols())
		return false;

	if (dir == HORIZONTAL) {							//checks each point that the ship would occupy if placed at a given location and returns false if any point is not a dot
		for (int i = 0; i < length; i++)
			if (attackedCells[r][c + i] != '.')
				return false;
	}

	else if (dir == VERTICAL) {
		for (int i = 0; i < length; i++)
			if (attackedCells[r + i][c] != '.')
				return false;
	}

	return true;
}

Point GoodPlayer::findSpot() {
	int r, c;
	int rows = game().rows();
	int cols = game().cols();
	//if (m_state == 1 && (1.0 * nShots / (rows * cols) >= .5))	//if in state 1 and half the board has been attacked, switch to state 3
		//m_state = 3;

	if (m_state == 1) {
		for (int i = 0; i < rows; i++)
			for (int k = 0; k < cols; k++)
				densityGrid[i][k] = 0;

		for (shipsRemaining* ptr = shipPTR; ptr != nullptr; ptr = ptr->next) { //loops for each remaining ship
			if (!ptr->destroyed) {
				for (int i = 0; i < rows; i++)
					for (int k = 0; k < cols; k++) {
						if (checkFit(Point(i, k), ptr->length, HORIZONTAL)) {
							for (int startc = 0; startc < ptr->length; startc++) { //increments the value of each point where a ship could be
								densityGrid[i][k + startc]++;
							}
						}
						if (checkFit(Point(i, k), ptr->length, VERTICAL)) {
							for (int startr = 0; startr < ptr->length; startr++) {
								densityGrid[i + startr][k]++;
							}
						}
					}
			}
		}

		int countUp = 0;
		Point likely(0, 0);
		for (int i = 0; i < rows; i++)
			for (int k = 0; k < cols; k++) {
				if (densityGrid[i][k] > countUp) {			//attacks the most likely position
					countUp = densityGrid[i][k];
					likely.r = i;
					likely.c = k;
				}
			}
		attackedCells[likely.r][likely.c] = '*';
		return likely;

	}

	if (m_state == 2) {
		Node* ptr = head;

		for (; ptr != nullptr; ptr = ptr->next) {		//finds currently targeted ship
			if (ptr->shipId == target)
				break;
		}

		if (ptr == nullptr)
			return (Point(0, 0));

		int r1 = ptr->pos[0].r;
		int c1 = ptr->pos[0].c;
		if (ptr->pos.size() >= 2) {		//if the ship has been hit twice or more
			int r2 = ptr->pos[1].r;
			int c2 = ptr->pos[1].c;
			if (r1 == r2) {				//if the ship is oriented horizontally
				int cl = 0;
				for (int i = 0; i < ptr->pos.size() && ptr->pos[i].c != -1; i++) { //find the most recently hit point
					cl = ptr->pos[i].c;
				}
				if (cl < cols - 1 && attackedCells[r1][cl + 1] == '.') {	//attack up over one column if it has not been attacked yet
					attackedCells[r1][cl + 1] = '*';
					nShots++;
					return Point(r1, cl + 1);
				}
				else if (cl > 0 && attackedCells[r1][cl - 1] == '.') {		//attack one column left if it has not been attacked
					attackedCells[r1][cl - 1] = '*';
					nShots++;
					return Point(r1, cl - 1);
				}
				else if (c1 < cols - 1 && attackedCells[r1][c1 + 1] == '.') { //attack one column right from first hit location
					attackedCells[r1][c1 + 1] = '*';
					nShots++;
					return Point(r1, c1 + 1);
				}
				else if (c1 > 0 && attackedCells[r1][c1 - 1] == '.') {		//attack one column left from first hit location
					attackedCells[r1][c1 - 1] = '*';
					nShots++;
					return Point(r1, c1 - 1);
				}
			}
			if (c1 == c2) {						//same process for vertically placed ship
				int rl = 0;
				for (int i = 0; i < ptr->pos.size() && ptr->pos[i].r != -1; i++) {
					rl = ptr->pos[i].r;
				}
				if (rl < rows - 1 && attackedCells[rl + 1][c1] == '.') {
					attackedCells[rl + 1][c1] = '*';
					nShots++;
					return Point(rl + 1, c1);
				}
				else if (rl > 0 && attackedCells[rl - 1][c1] == '.') {
					attackedCells[rl - 1][c1] = '*';
					nShots++;
					return Point(rl - 1, c1);
				}
				else if (r1 > 0 && attackedCells[r1 - 1][c1] == '.') {
					attackedCells[r1 - 1][c1] = '*';
					nShots++;
					return Point(r1 - 1, c1);
				}
				else if (r1 < rows - 1 && attackedCells[r1 + 1][c1] == '.') {
					attackedCells[r1 + 1][c1] = '*';
					nShots++;
					return Point(r1 + 1, c1);
				}
			}
		}

		if (ptr->pos.size() >= 2) {		//if we reach this point, then our vector contains the location of multiple ships
			Node* ptr1 = new Node;		//creates new node to contain second ship
			ptr1->next = head;
			ptr1->shipDestroyed = false;
			ptr1->shipId = ptr->shipId + 1;
			head = ptr1;

			for (vector<Point>::iterator it = ptr->pos.begin() + 1; it != ptr->pos.end();) {
				int newr = it->r;
				int newc = it->c;
				ptr1->pos.push_back(Point(newr, newc));		//creates new node's vector to contain location of second ship
				it = ptr->pos.erase(it);
			}
		}

		if (r1 > 0 && attackedCells[r1 - 1][c1] == '.') {		//attacks counterclockwise around the originally hit location
			attackedCells[r1 - 1][c1] = '*';
			nShots++;
			return Point(r1 - 1, c1);
		}
		else if (c1 > 0 && attackedCells[r1][c1 - 1] == '.') {
			attackedCells[r1][c1 - 1] = '*';
			nShots++;
			return Point(r1, c1 - 1);
		}
		else if (r1 < rows - 1 && attackedCells[r1 + 1][c1] == '.') {
			attackedCells[r1 + 1][c1] = '*';
			nShots++;
			return Point(r1 + 1, c1);
		}

		else if (c1 < cols - 1 && attackedCells[r1][c1 + 1] == '.') {
			attackedCells[r1][c1 + 1] = '*';
			nShots++;
			return Point(r1, c1 + 1);
		}
	}

	for (;;) {			//attacks randomly if in state 3
		r = randInt(rows);
		c = randInt(cols);
		if (attackedCells[r][c] == '.') {
			attackedCells[r][c] = '*';
			nShots++;
			return Point(r, c);
		}
	}

	return Point(0, 0);
}

GoodPlayer::GoodPlayer(string name, const Game& g) : Player(name, g) {
	totalHealth = 0;			//initializes provate members
	currentHealth = 0;
	m_state = 1;
	oppShots = 0;
	nShots = 0;
	target = -1;

	for (int i = 0; i < game().rows(); i++)			//attacked cells grind initialized to empty
		for (int j = 0; j < game().cols(); j++) {
			attackedCells[i][j] = '.';
			m_grid[i][j] = '.';
			densityGrid[i][j] = 0;
		}

	head = nullptr;

	headptr = nullptr;
	for (int i = 0; i < game().nShips(); i++) {		//creates linked list for each of the good player's ships
		myShips* ptr = new myShips;
		ptr->destroyed = false;
		ptr->health = game().shipLength(i);
		totalHealth += ptr->health;
		currentHealth += ptr->health;
		ptr->Id = i;
		ptr->symbol = game().shipSymbol(i);
		ptr->next = headptr;
		headptr = ptr;
	}
	shipPTR = nullptr;
	for (int i = 0; i < game().nShips(); i++) {
		shipsRemaining* ptr = new shipsRemaining;
		ptr->ID = i;
		ptr->destroyed = false;
		ptr->length = game().shipLength(i);
		ptr->next = shipPTR;
		shipPTR = ptr;
	}
}

GoodPlayer::~GoodPlayer() {		//deletes all dynamically allocated linked list nodes
	while (head != nullptr) {
		Node* temp = head;
		head = head->next;
		delete temp;
	}

	while (headptr != nullptr) {
		myShips* temp = headptr;
		headptr = headptr->next;
		delete temp;
	}

	while (shipPTR != nullptr) {
		shipsRemaining* temp = shipPTR;
		shipPTR = shipPTR->next;
		delete temp;
	}

}

bool GoodPlayer::placeShips(Board& b) {

	return placeRec(b, 0);		//calls recursive function

}

Point GoodPlayer::recommendAttack() {

	Point p = findSpot();		//calls function to find best attack point

	return Point(p.r, p.c);
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
	if (!validShot)
		return;

	if (shotHit) {
		m_state = 2;
		Node* ptr = head;
		int temp = -1;
		for (; ptr != nullptr; ptr = ptr->next) {		//finds current target if the last attack hit
			temp = ptr->shipId;
			if (temp == target)
				break;
		}

		if (ptr == nullptr) {		//creates a new node to attack if there is no current target
			target = temp + 1;
			Node* ptr1 = new Node;
			ptr1->next = head;
			ptr1->pos.push_back(p);
			ptr1->shipDestroyed = false;
			ptr1->shipId = target;
			head = ptr1;
			return;
		}

		ptr->pos.push_back(p);	//pushes hit point onto node's vector


		if (shipDestroyed) {
			ptr->shipDestroyed = true;
			target = -1;
			m_state = 1;
			int length = game().shipLength(shipId);
			shipsRemaining* ptr2 = shipPTR;
			for (; ptr2 != nullptr; ptr2 = ptr2->next) {
				if (ptr2->length == length)
					break;
			}
			if (ptr2 != nullptr)
				ptr2->destroyed = true;

			if (ptr->pos.size() > length) {		//if a destroyed ship had a smaller langth than the number of times it was hit, we hit multiple ships
				vector<Point>::iterator it1 = ptr->pos.begin();
				vector<Point>::iterator it2 = ptr->pos.end() - 1;
				Direction dir;
				bool up;
				bool left;
				if (it1->c == it2->c) {
					dir = VERTICAL;
					if (it1->r < it2->r)
						 up = true;
					else up = false;
				}
				else { 
					dir = HORIZONTAL; 
					if (it1->c < it2->c) {
						left = true;
					}
					else left = false;
				}

				Node* ptr1 = new Node;					//creates new node containing hit locations of other ship in vector
				ptr1->next = head;
				if (dir == VERTICAL) {
					for (int i = 0; i < length; i++) {
						if (up) {

							for (vector<Point>::iterator it = ptr->pos.begin(); it != ptr->pos.end(); it++) {
								if (it->r == it2->r - 1) {
									it2->r = -10;
									it2 = it;
									break;
								}

							}
						}
						else {
							for (vector<Point>::iterator it = ptr->pos.begin(); it != ptr->pos.end(); it++) {
								if (it->r == it2->r + 1) {
									it2->r = -10;
									it2 = it;
									break;
								}
							}
						}
					}
				}
				else if (dir == HORIZONTAL) {
					for (int i = 0; i < length; i++) {
						if (left) {
							for (vector<Point>::iterator it = ptr->pos.begin(); it != ptr->pos.end(); it++) {
								if (it->c == it2->c - 1) {
									it2->c = -10;
									it2 = it;
									break;
								}

							}
						}
						else {
							for (vector<Point>::iterator it = ptr->pos.begin(); it != ptr->pos.end(); it++) {
								if (it->c == it2->c + 1) {
									it2->c = -10;
									it2 = it;
									break;
								}
							}
						}
					}
				}
				for (it1 = ptr->pos.begin(); it1 != ptr->pos.end(); it1++) {
					if (it1->r != -10 && it1->c != -10)
						ptr1->pos.push_back(*it1);
				}
				ptr1->shipDestroyed = false;
				ptr1->shipId = ptr->shipId + 1;
				head = ptr1;
				m_state = 2;
				target = ptr1->shipId;		//sets this ship as the current target
				return;
			}

			for (ptr = head; ptr != nullptr; ptr = ptr->next) {		//looks for any targets that are not already destroyed
				if (!ptr->shipDestroyed) {
					target = ptr->shipId;
					m_state = 2;
					break;
				}
			}
		}
	}

}

void GoodPlayer::recordAttackByOpponent(Point p) {
	/*oppShots++;
	int counter1 = 0;
	int counter2 = 0;
	if (game().isValid(p.r, p.c))
		char sym = m_grid[p.r][p.c];
	if (sym != '.') {					//if opponent shot hit
		currentHealth--;
		for (myShips* ptr = headptr; ptr != nullptr; ptr = ptr->next) {		//finds hit ship
			counter2++;
			if (ptr->symbol == sym)
			{
				ptr->health--;		//decreases health
				if (ptr->health == 0) {	//records if destroyed
					ptr->destroyed = true;
				}
			}
			if (ptr->destroyed)
				counter1++;
		}
	}*/

	/*if (m_state == 1) {						//if the remaining ships or total health is less than health and we are in state 3, switch to state 1
		if (1.0 * currentHealth / totalHealth < .5)
			m_state = 3;
		else if (counter2 != 0 && (1.0 * counter1 / counter2 >= .5)) {
			m_state = 3;
		}
	}*/


}
//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
	static string types[] = {
		"human", "awful", "mediocre", "good"
	};

	int pos;
	for (pos = 0; pos != sizeof(types) / sizeof(types[0]) &&
		type != types[pos]; pos++)
		;
	switch (pos)
	{
	case 0:  return new HumanPlayer(nm, g);
	case 1:  return new AwfulPlayer(nm, g);
	case 2:  return new MediocrePlayer(nm, g);
	case 3:  return new GoodPlayer(nm, g);
	default: return nullptr;
	}
}

