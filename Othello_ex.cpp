#include <bangtal>

using namespace bangtal;

ObjectPtr board[8][8];

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8]; // 보드의 처음 상태를 저장

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;


void setState(int x, int y, State state) {
	switch (state) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	board_state[y][x] = state; // 보드 상태를 저장
}

bool checkPossible(int x, int y, int dx, int dy) { // x,y는 현재 위치 dx,dy는 방향의 위치
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else {
			return false;
		}
	}

	return false;
}

//x,y에 놓여질 수 있는지 체크하고 놓여질수 있으면 true, 놓여질 수 없으면 false
bool checkPossible(int x, int y) {
	if (board_state[y][x] == State::BLACK) return false; // 이미 검은돌이 있으면 놓여질 수 없음
	if (board_state[y][x] == State::WHITE) return false; // 이미 흰돌이 있으면 놓여질 수 없음
	setState(x, y, State::BLANK);

	int delta[8][2] = { // 방향을 배열로 선언->코드줄수 줄이기
		{0, 1},
		{1, 1},
		{1, 0},
		{1, -1},
		{0, -1},
		{-1, -1},
		{-1, 0},
		{-1, 1},
	};
	/*
	for (int i = 0; i < 8; i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);
	}
	*/
	bool possible = false;
	for (auto d : delta) { // 위와 같은 표현
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}
}

		void reverse(int x, int y) {
			int delta[8][2] = { // 방향을 배열로 선언->코드줄수 줄이기
				{0, 1},
				{1, 1},
				{1, 0},
				{1, -1},
				{0, -1},
				{-1, -1},
				{-1, 0},
				{-1, 1},
			};

			bool possible = false;
			for (auto d : delta) {
				reverse(x, y, d[0], d[1]);
			}

		}

bool setPossible() {
	// 8x8의 모든 위치에 대해서 놓여질 수 있으면 상태를 POSSIBLE로 바꾼다.
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for(int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

int main() 
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	auto scene = Scene::create("Othello", "Images/background.png");

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction action)-> bool { // 이떄, 마우스콜백함수에서 x,y 로 정의하면 겹치고 여기서 이 변수들은 안쓰므로 아예 생략 
				if (board_state[y][x] == State::POSSIBLE) {
					if (turn == Turn::BLACK) { 
						setState(x, y, State::BLACK);
						reverse(x, y);
						turn = Turn::WHITE;
					}
					else {
						setState(x, y, State::WHITE);
						reverse(x, y);
						turn = Turn::BLACK;
					}
					if (!setPossible()) { // false일때
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK; // 턴이 블랙이면 턴은 화이트 아니면 블랙 
						
						if (!setPossible()) {
							showMessage("End Game");
						}
					}
				}
				
				return true;
				});
			board_state[y][x] = State::BLANK; // 보드 상태를 저장
		}
	}

	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();

	startGame(scene);

}