// TicTacToe.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <cctype>

#include "IAGO2K14.h"

void play() {
	IAGO2K14 * b = new IAGO2K14();
	int humanPlayer = 1;
	int cpuPlayer = -1;

	cout << b->toString();
	int consecutivePasses = 0;
	while(!b->full_board() && consecutivePasses<2) {
		int row, col;
		//check if player must pass:
		if(!b->has_valid_move(humanPlayer)) {
			cout << "You must pass." << endl;
			consecutivePasses++;
		}
		else {
            consecutivePasses = 0;
			cout << "Your move row (1-8): ";
			cin >> row;
			if(cin.fail()){
                 std::cerr<< "Illegal move."<<endl;
                 cin.clear();
                 cin.ignore();
                 continue;
            }
			cout << "Your move col (1-8): ";
			cin >> col;
			if(cin.fail()){
                 std::cerr<< "Illegal move."<<endl;
                 cin.clear();
                 cin.ignore();
                 continue;
            }
			if(!b->play_square(row, col, humanPlayer)) {
                  cout << "Illegal move." << endl;
			      continue;
            }        
		}
		//move for computer:
		if(b->full_board())
			break;
		else {
			cout << b->toString() << "..." << endl;
			if(b->timed_move(b, cpuPlayer))
				consecutivePasses=0;
			else
				consecutivePasses++;
			cout << b->toString();
		}
	}
	int score = b->score();
	if(score==0)
		cout << "Tie game." << endl;
	else if(score>0)
		cout << "Human wins by " << score << endl;
	else if(score<0)
		cout << "Computer wins by " << -score << endl;
	char a;
	cin >> a;
}


int main(int argc, char * argv[])
{
    play();
	return 0;
}
