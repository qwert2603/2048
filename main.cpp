#include "stdafx.h"
#include <conio.h>
#include "Field2048.h"
#include <iostream>
using namespace std;

template<unsigned N> inline void show(const array<array<unsigned, N>, N> &_arr) {
	system("cls");
	for (const auto &i : _arr) {
		for (const auto &j : i) {
			cout << j;
		}
		cout << endl;
	}
	cout << endl;
}

int main() {
	Field2048<4> f;	// поле
	char act;	// нажатая клавиша
	bool good = true;	// есть ли доступные ходы
	for (;;) {
		show(f.get_tiles());
		if (!good) {
			cout << endl;
			cout << "it's all over." << endl << endl;
			cout << "program by Alex Zhdanov. 2015." << endl;
			break;
		}
		act = _getch();
		switch (act) {
		case -94:
		case 100: { good = f.move_right(); break; }
		case -28:
		case 97: { good = f.move_left(); break; }
		case -26:
		case 119: { good = f.move_up(); break; }
		case -21:
		case 115: { good = f.move_down(); break; }
		default: break;
		}
	}


	char z1;
	cin >> z1;
	return 0;
}

