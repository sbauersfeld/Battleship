# Battleship
A fully playable Battleship game featuring an intelligent computer opponent. In order to play the game, copy the source files into a project, compile, and run the program! There are three difficulty levels separated by the intelligence of the computer opponent. Additionally, you can choose to pit the computer players against each other and see how the various levels of computer intelligence perform against one another.

The following is a brief decsription of how the most intelligent computer player operates.
The good player uses a recursive algorithm to randomly place each ship in a vertical or horizontal direction beginning at a random point. It repeats the generation of random directions and starting coordinates until all ships can fit on the board. The good player recommends its shots based on what state it is in. It begins the game in state 1, where it randomly fires at any point on the board that is not next to another location that has already been targeted. If it hits a ship, it switches to state two and begins firing in a counterclockwise manner until it hits another ship. It then continues firing along that column or row until it destroys the ship or misses. If it misses, it begins firing along the reverse direction, beginning with the point that first set it to state two. If the ship has not been destroyed when the good player shoots along this column or row, then it determines that it must have hit two consecutive ships and begins firing from the start point along the untargeted direction. If this occurs, then the good player will set the second hit location as the starting point for its next target. The good player also has a third state, which is triggered if it does not have a target and has fired shots that cover more than half the board or if half of its ships have been destroyed. In state three, the good player randomly attacks points on the board regardless of whether nearby coordinates have already been attacked. This allows it to find undiscovered ships that are in between previously fired shots.
