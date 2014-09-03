//
//  IAGO2K14.h
//  Othello AI
//
//  Created by Nicholas Hall and Thadeus Duval on 6/10/14.
//  Copyright (c) 2014 Nicholas Hall and Thadeus Duval.
//  All rights reserved.
//

#ifndef IAGO2K14_h
#define IAGO2K14_h

#include <utility>
#include <limits>
#include <vector>

using namespace std;

class IAGO2K14 {
	int squares[8][8];
    
public:
	IAGO2K14();
    float inf = numeric_limits<float>::max();
    float ninf = -inf;
    struct stab_par {
        int wts;
        float stab;
        float par;
        int corners;
    };
    int cpu = -1; //white unless it is told to make first move
    int maxdepth;
    int fulldepth;
    int timeout;
    float timelimit = 12.5;
    bool whitemoved = false;
    bool firstmove = true;
    bool movesgenerated;
    vector<pair<int,int>> globalmoves;
    clock_t start;
    string toString();
	bool play_square(int, int, int);
	bool play_square(int&, int&);
	bool move_is_valid(int, int, int);
	bool check_or_flip_path(int, int, int, int, int, bool);
	int get_square(int, int);
	int score();
    stab_par stability_parity(int);
	bool full_board();
	bool has_valid_move(int);
    int minrow = 3;
    int maxrow = 6;
    int mincol = 3;
    int maxcol = 6;
    int weights[8][8] = {
        {15, -5, 8, 6, 6, 8, -5, 15},
        {-5, -7, -3, -1, -1, -3, -7, -5},
        {8, -3, 3, 0, 0, 3, -3, 8},
        {6, -1, 0, 1, 1, 0, -1, 6},
        {6, -1, 0, 1, 1, 0, -1, 6},
        {8, -3, 3, 0, 0, 3, -3, 8},
        {-5, -7, -3, -1, -1, -3, -7, -5},
        {15, -5, 8, 6, 6, 8, -5, 15}};
    int xs[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int ys[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    float negamax(IAGO2K14, float, float, int, int);
    vector<pair<int,int>> get_moves(IAGO2K14 *, int);
    float mobility(IAGO2K14 *, int);
    float evaluate(IAGO2K14 *, int);
    vector<pair<int,int>> order_moves(vector<pair<int,pair<int,int>>>);
    pair<int,int> make_move(IAGO2K14 *, int);
    pair<int,int> white_opener();
    vector<pair<int,int>> valid_corners();
    bool cornered;
    bool timed_move(IAGO2K14 *, int);
};

IAGO2K14::IAGO2K14() {
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			squares[i][j] = 0;
	squares[3][3]=-1;
	squares[4][4]=-1;
	squares[3][4]=1;
	squares[4][3]=1;
}

string IAGO2K14::toString() {
	stringstream s;
	char cforvalplusone[] = {'W', '_', 'B'};
	s << "  1 2 3 4 5 6 7 8" << endl;
	for(int i=0; i<8;i++) {
		s << i+1 << '|';
		for(int j=0; j<8; j++)
			s << cforvalplusone[squares[i][j]+1] << '|';
		s << endl;
	}
	return s.str();
}

//returns if player with val has some valid move in this configuration
bool IAGO2K14::has_valid_move(int val) {
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			if(move_is_valid(i+1, j+1, val))
				return true;
	return false;
}

//r and c zero indexed here
//checks whether path in direction rinc, cinc results in flips for val
//will actually flip the pieces along path when doFlips is true
bool IAGO2K14::check_or_flip_path(int r, int c, int rinc, int cinc, int val, bool doFlips) {
	int pathr = r + rinc;
	int pathc = c + cinc;
	if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]!=-1*val)
		return false;
	//check for some chip of val's along the path:
	while(true) {
		pathr += rinc;
		pathc += cinc;
		if(pathr < 0 || pathr > 7 || pathc < 0 || pathc > 7 || squares[pathr][pathc]==0)
			return false;
		if(squares[pathr][pathc]==val) {
			if(doFlips) {
				pathr=r+rinc;
				pathc=c+cinc;
				while(squares[pathr][pathc]!=val) {
					squares[pathr][pathc]=val;
					pathr += rinc;
					pathc += cinc;
				}
			}
			return true;
		}
	}
	return false;
}


//returns whether given move is valid in this configuration
bool IAGO2K14::move_is_valid(int row, int col, int val) {
	int r = row-1;
	int c = col-1;
	if(r < 0 || r > 7 || c < 0 || c > 7)
		return false;
	//check whether space is occupied:
	if(squares[r][c]!=0)
		return false;
	//check that there is at least one path resulting in flips:
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			if(check_or_flip_path(r, c, rinc, cinc, val, false))
				return true;
		}
	return false;
}

//executes move if it is valid.  Returns false and does not update board otherwise
bool IAGO2K14::play_square(int row, int col, int val) {
	if(!move_is_valid(row, col, val))
		return false;
	squares[row-1][col-1] = val;
	for(int rinc = -1; rinc <= 1; rinc++)
		for(int cinc = -1; cinc <= 1; cinc++) {
			check_or_flip_path(row-1, col-1, rinc, cinc, val, true);
		}
    minrow = min(minrow,(row-1));
    maxrow = max(maxrow,(row+1));
    mincol = min(mincol,(col-1));
    maxcol = max(maxcol,(col+1));
	return true;
}

//executes move if it is valid. Returns false and does not update board otherwise
//Makes computer make its move and returns the computer's move in row, and col
bool IAGO2K14::play_square(int &row, int &col){
    if (!firstmove) {
        bool check = play_square(row,col, -cpu);
        if (check) {
            cout << "Opponent's move:" << endl;
            cout << toString() << endl;
        }
        if (full_board()) {
            return false;
        }
        if (!check) {
            if (row == -1 and col == -1)
                cout << "Opponent passes." << endl;
            else
                cout << "Opponent's move was invalid!" << endl;
        }
    }
    else {
        firstmove = false;
        if (row==0 and col==0)
            cpu = 1;
        else {
            bool check = play_square(row,col, -cpu);
            if (check) {
                cout << "Opponent's move:" << endl;
                cout << toString() << endl;
            }
            else
                cout << "Opponent's move was invalid!" << endl;
        }
    }
    if (!whitemoved and cpu == -1) {
        pair<int,int> coords = white_opener();
        row = coords.first;
        col = coords.second;
        cout << "IAGO2K14's move:" << endl;
        cout << toString() << endl;
        return true;
    }
    movesgenerated = false;
    maxdepth = 2;
    fulldepth = 64;
    timeout = false;
    pair<int,int> temp_move;
    pair<int,int> best_move;
    best_move.first = -1;
    start = clock();
    while ((((clock()-start)/CLOCKS_PER_SEC) < timelimit) and (maxdepth < fulldepth)) {
        temp_move = make_move(this,cpu);
        if (!timeout and (temp_move.first != -1))
            best_move = temp_move;
        maxdepth++;
    }
    if (best_move.first == -1) {
        cout << "IAGO2K14 passes." << endl;
        row = -1;
        col = -1;
        return false;
    }
    row = best_move.first;
    col = best_move.second;
    bool check = play_square(row,col,cpu);
    cout << "IAGO2K14's move:" << endl;
    cout << toString() << endl;
    return check;
}

vector<pair<int,int>> IAGO2K14::valid_corners() {
    vector<pair<int,int>> corns;
    if (move_is_valid(1,1,cpu))
        corns.push_back(make_pair(1,1));
    if (move_is_valid(1,8,cpu))
        corns.push_back(make_pair(1,8));
    if (move_is_valid(8,1,cpu))
        corns.push_back(make_pair(8,1));
    if (move_is_valid(8,8,cpu))
        corns.push_back(make_pair(8,8));
    cornered = corns.size();
    return corns;
}

bool IAGO2K14::full_board() {
    if(maxcol != 9 or maxrow != 9 or minrow != 0 or minrow != 0)
        return false;
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			if(squares[i][j]==0)
				return false;
	return true;
}

//returns score, positive for X player's advantage
int IAGO2K14::score() {
	int sum =0;
	for(int i=0; i<8;i++)
		for(int j=0; j<8; j++)
			sum+=squares[i][j];
	return sum;
}

IAGO2K14::stab_par IAGO2K14::stability_parity(int cpuval) {
	int sum = 0;
    int tiles = 0;
    int weighted_score = 0;
    int stability = 0;
    int stab_div = 0;
    int cornerc = 0;
    int x, y;
	for(int i=0; i<8;i++) {
		for(int j=0; j<8; j++) {
            if (squares[i][j] != 0) {
                sum += squares[i][j];
                tiles++;
                weighted_score += cpuval * squares[i][j] * weights[i][j];
                for(int k=0; k<8; k++) {
                    x = i + xs[k];
                    y = j + ys[k];
                    if(x >= 0 and x < 8 and y >= 0 and y < 8 and squares[x][y] == 0) {
                        stability += cpuval * squares[i][j];
                        stab_div++;
                        break;
                    }
                }
            }
        }
    }
    if (squares[0][0]==0)
        cornerc += squares[1][0] + squares[0][1] + squares[1][1];
    if (squares[7][0]==0)
        cornerc += squares[6][0] + squares[7][1] + squares[6][1];
    if (squares[0][7]==0)
        cornerc += squares[0][6] + squares[7][1] + squares[1][6];
    if (squares[7][7]==0)
        cornerc += squares[6][7] + squares[7][6] + squares[6][6];
    stab_par vals;
    vals.wts = weighted_score;
    int mystab = (stability + stab_div) / 2;
    int oppstab = stab_div - mystab;
    if(mystab > oppstab)
        vals.stab = (100.0 * mystab)/stab_div;
    else if(mystab < oppstab)
        vals.stab = -(100.0 * oppstab)/stab_div;
    if (tiles)
        vals.par = cpuval * 100 * sum / tiles;
    vals.corners = -100*cornerc*cpuval;
    return vals;
}

int IAGO2K14::get_square(int row, int col) {
	return squares[row-1][col-1];
}

vector<pair<int,int>> IAGO2K14::get_moves(IAGO2K14 * b, int cpuval) {
    vector<pair<int,int>> moves;
    for(int i=b->minrow; i<(b->maxrow +1); i++) {
        for(int j=b->mincol; j<(b->maxcol +1); j++) {
            if(b->move_is_valid(i,j,cpuval))
                moves.push_back(make_pair(i,j));
        }
    }
    return moves;
}

float IAGO2K14::mobility(IAGO2K14 * b, int cpuval){
    vector<pair<int,int>> cpumoves = get_moves(b,cpuval);
    vector<pair<int,int>> oppmoves = get_moves(b,(-1*cpuval));
    float cpucount = cpumoves.size();
    float oppcount = oppmoves.size();
    if ((cpucount+oppcount) == 0)
        return 0;
    return (100 * (cpucount-oppcount) / (cpucount+oppcount));
}

float IAGO2K14::evaluate(IAGO2K14 * b, int cpuval) {
    float m = mobility(b,cpuval);
    stab_par v = b->stability_parity(cpuval);
    if (b->full_board())
        v.par = 1000 * v.par;
    return (12*m + 3*v.wts + 2*v.par + 20*v.corners + 5*v.stab);
}

vector<pair<int,int>> IAGO2K14::order_moves(vector<pair<int,pair<int,int>>> valued_moves) {
    sort(valued_moves.begin(), valued_moves.end());
    vector<pair<int,int>> tempmoves;
    for(int i=0; i<valued_moves.size(); i++)
        tempmoves.push_back(valued_moves[i].second);
    return tempmoves;
}

pair<int,int> IAGO2K14::make_move(IAGO2K14 * b, int cpuval) {
    vector<pair<int,int>> moves;
    if (movesgenerated)
        moves = globalmoves;
    else {
        moves = valid_corners();
        if (!cornered)
            moves = get_moves(b,cpuval);
        movesgenerated = true;
    }
    vector<pair<int,pair<int,int>>> valued_moves;
    pair<int,int> move;
    move.first = -1;
    for(int i=(moves.size() - 1.0); i>=0; i--) {
        if (timeout)
            break;
        IAGO2K14 temp_board = *b;
        pair<int,int> coords = moves[i];
        temp_board.play_square(coords.first,coords.second,cpuval);
        valued_moves.push_back(make_pair(-negamax(temp_board, ninf, inf, -cpuval, 1), coords));
    }
    globalmoves = order_moves(valued_moves);
    return globalmoves.back();
}

pair<int,int> IAGO2K14::white_opener() {
    for(int x = 3; x<=6; x=x+3) {
        for(int y = 3; y<=6; y=y+3) {
            play_square(x,y,-1);
            if (squares[x-1][y-1]==-1) {
                whitemoved = true;
                return make_pair(x,y);
            }
        }
    }
    return make_pair(-1,-1);
}

bool IAGO2K14::timed_move(IAGO2K14 * b, int cpuval) {
    if (!whitemoved and cpuval == -1) {
        white_opener();
        return true;
    }
    movesgenerated = false;
    maxdepth = 3;
    fulldepth = 64;
    timeout = false;
    pair<int,int> temp_move;
    pair<int,int> best_move;
    best_move.first = -1;
    start = clock();
    while ((((clock()-start)/CLOCKS_PER_SEC) < timelimit) and (maxdepth < fulldepth)) {
        temp_move = make_move(b,cpuval);
        if (!timeout and (temp_move.first != -1))
            best_move = temp_move;
        maxdepth++;
    }
    if (best_move.first == -1) {
        cout << "Computer passes." << endl;
        return false;
    }
    b->play_square(best_move.first,best_move.second,cpuval);
    return true;
}

float IAGO2K14::negamax(IAGO2K14 state, float a, float b, int cpuval, int depth) {
    if (timeout)
        return 0;
    if (((clock() - start)/CLOCKS_PER_SEC) > timelimit) {
        timeout = true;
        return 0;
    }
    if (depth >= maxdepth)
        return evaluate(&state, cpuval);
    if (state.full_board()) {
        maxdepth = fulldepth = depth;
        return evaluate(&state, cpuval);
    }
    float best = ninf;
    vector<pair<int,int>> moves = get_moves(&state,cpuval);
    for(int i=0; i<moves.size(); i++) {
        IAGO2K14 temp_board = state;
        pair<int,int> coords = moves[i];
        temp_board.play_square(coords.first,coords.second,cpuval);
        float v = -negamax(temp_board, -b, -a, -cpuval, (depth+1));
        best = max(best, v);
        a = max(a, v);
        if (a >= b)
            break;
    }
    return best;
}

#endif
