//###    ##                # 
//  #  # ##  Tetris!   #   #
//    ##               #   #
//     #       ####   ##   #

#define AC_SC_APPLICATION
#include "ACScreenController.h"

//how the peices and colors are defined
enum blocktype { Z, S, T, O, I, L, J, X };

//global vars
int score = 0;
int lvl = 0;
int lines = 0;
bool game = true;

//blocks
struct block {
	blocktype type;
	block() {
		type = X;
	}
};

block board[10][20];
block nextPieceBoard[4][4];
int cols;
int rows;

//piece moving
struct piece {
	blocktype shape = X;
	AC::vec2dInt blocks[4];
	int x = 0;
	int y = 0;
	int rot = 0;

	piece(blocktype setShape, int start_x, int start_y) {
		shape = setShape;
		x = start_x;
		y = start_y;
		rot = 0;

		//load blocks for the shape
		switch (shape)
		{
		case Z:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(1, 0);
			blocks[2] = AC::vec2dInt(1, 1);
			blocks[3] = AC::vec2dInt(2, 1);
			break;
		case S:
			blocks[0] = AC::vec2dInt(1, 0);
			blocks[1] = AC::vec2dInt(0, 1);
			blocks[2] = AC::vec2dInt(1, 1);
			blocks[3] = AC::vec2dInt(2, 0);
			break;
		case T:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(1, 0);
			blocks[2] = AC::vec2dInt(1, 1);
			blocks[3] = AC::vec2dInt(2, 0);
			break;
		case O:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(0, 1);
			blocks[2] = AC::vec2dInt(1, 1);
			blocks[3] = AC::vec2dInt(1, 0);
			break;
		case I:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(1, 0);
			blocks[2] = AC::vec2dInt(2, 0);
			blocks[3] = AC::vec2dInt(3, 0);
			break;
		case L:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(1, 0);
			blocks[2] = AC::vec2dInt(2, 0);
			blocks[3] = AC::vec2dInt(0, 1);
			break;
		case J:
			blocks[0] = AC::vec2dInt(0, 0);
			blocks[1] = AC::vec2dInt(1, 0);
			blocks[2] = AC::vec2dInt(2, 0);
			blocks[3] = AC::vec2dInt(2, 1);
			break;
		case X:
			break;
		default:
			break;
		}

	};

	piece() {
		shape = X;
		x = 0;
		y = 0;
	};

	int update(int add_x, int add_y) {
		//erase previous shape
		drawShape(x, y, X);
		//collisoin check
		int collide = doescollide(x + add_x, y + add_y, add_y);
		if (collide == 2) {
			drawShape(x, y, shape);
			shape = X;
			lines += checkRowClear();
			return 0;  //return 0 to make it stop updating
		}
		else if(collide == 1){
			drawShape(x, y, shape);
			return 1;
		}
		else {
			x += add_x;
			y += add_y;
			//place new shape
			drawShape(x, y, shape);
		}
		return 1;
	}
	int drawShape(int base_x, int base_y, blocktype draw) {
		for (AC::vec2dInt b : blocks) {
			board[base_x + b.x][base_y + b.y].type = draw;
		}
		return 0;
	}
	int doescollide(int check_x, int check_y, bool is_y) { //0 = does not collide at cords | 1 = colides with walls | 2 = collides down
		for (AC::vec2dInt b : blocks) {
			if (board[check_x+b.x][check_y+b.y].type != X || check_y+b.y >= rows || check_x + b.x < 0 || check_x + b.x >= cols) {
				return 1+is_y;
			}
		}
		return 0;
	}
	int rotate(int rotation) {
		//erase previous shape

		//scroll
		if (rotation > 0) {
			drawShape(x, y, X);
			if (rot < 3) {
				rot += 1;
			}
			else {
				rot = 0;
			}
		} else
		if (rotation < 0) {
			if (rot > 0) {
				rot -= 1;
			}
			else {
				rot = 3;
			}
		}

		//load different shape for each block
		switch (shape)
		{
		case Z:
			if (rot % 2) {
				blocks[0] = AC::vec2dInt(1, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(0, 2);
			}
			else {
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(2, 1);
			}
			break;
		case S:
			if (rot % 2) {
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(1, 2);
			}
			else {
				blocks[0] = AC::vec2dInt(1, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(2, 0);
			}
			break;
		case T:
			switch (rot)
			{
			case 0:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(2, 0);
				break;
			case 1:
				blocks[0] = AC::vec2dInt(1, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(1, 2);
				break;
			case 2:
				blocks[0] = AC::vec2dInt(1, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(2, 1);
				break;
			case 3:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(0, 2);
				break;
			default:
				break;
			}
			break;
		case O:
			break;
		case I:
			if (rot % 2) {
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(0, 2);
				blocks[3] = AC::vec2dInt(0, 3);
			}
			else {
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(2, 0);
				blocks[3] = AC::vec2dInt(3, 0);
			}
			break;
		case L:
			switch (rot)
			{
			case 0:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(2, 0);
				blocks[3] = AC::vec2dInt(0, 1);
				break;
			case 1:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(1, 2);
				break;
			case 2:
				blocks[0] = AC::vec2dInt(0, 1);
				blocks[1] = AC::vec2dInt(1, 1);
				blocks[2] = AC::vec2dInt(2, 1);
				blocks[3] = AC::vec2dInt(2, 0);
				break;
			case 3:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(0, 2);
				blocks[3] = AC::vec2dInt(1, 2);
				break;
			default:
				break;
			}
			break;
		case J:
			switch (rot)
			{
			case 0:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(2, 0);
				blocks[3] = AC::vec2dInt(2, 1);
				break;
			case 1:
				blocks[0] = AC::vec2dInt(1, 0);
				blocks[1] = AC::vec2dInt(1, 1);
				blocks[2] = AC::vec2dInt(1, 2);
				blocks[3] = AC::vec2dInt(0, 2);
				break;
			case 2:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(0, 1);
				blocks[2] = AC::vec2dInt(1, 1);
				blocks[3] = AC::vec2dInt(2, 1);
				break;
			case 3:
				blocks[0] = AC::vec2dInt(0, 0);
				blocks[1] = AC::vec2dInt(1, 0);
				blocks[2] = AC::vec2dInt(0, 1);
				blocks[3] = AC::vec2dInt(0, 2);
				break;
			default:
				break;
			}
			break;
		case X:
			break;
		default:
			break;
		}


		if (doescollide(x, y, 0)) {
			//go back
			rotate(-1);
		}
		else {
			//place new shape
			drawShape(x, y, shape);
		}
		return 0;
	}
	int checkRowClear() {
		//I know this fuction dosen't really belong here but oh well
		int cleared[4] = { NULL,NULL,NULL,NULL };
		int rowsclear = 0;
		for (int y = rows; y > 0; y--)//check though each row
		{
			bool clear = true;
			for (int x = 0; x < cols; x++)//check each block
			{
				if (board[x][y].type == X) {
					clear = false;
				}
			}
			if (clear) {
				cleared[rowsclear] = y;
				rowsclear++;
			}
		}
		//add points
		switch (rowsclear)
		{
		case 1:
			score += 40 * (lvl + 1);
			break;
		case 2:
			score += 100 * (lvl + 1);
			break;
		case 3:
			score += 300 * (lvl + 1);
			break;
		case 4:
			score += 1200 * (lvl + 1);
			break;
		default:
			break;
		}

		//remove rows and bring rest down
		for (int i = 3; i >= 0; i--)
		{
			if (cleared[i] != NULL) {//for each non-null row to remove
				for (int y_off = 0; y_off+cleared[i] != 0; y_off--)//go though each row above it
				{
					for (int x = 0; x < cols; x++)
					{
						if (cleared[i] + y_off == 0){
							board[x][cleared[i] + y_off].type = X;
						}
						else {
							board[x][cleared[i] + y_off].type = board[x][cleared[i] + y_off - 1].type;
						}
					}
				}
			}
		}


		return rowsclear;
	}
};

piece currentPiece;
piece nextPiece;

//timing
float tick = 1;
float frameTime = 0;


class Example : public AC::ScreenController {
public:
	Example()
	{
		// Name your application
		sAppName = "Tetris";
	}
public:
	bool OnUserCreate() override
	{
		// Called once at the start

		cols = sizeof(board) / sizeof(board[0]);
		rows = sizeof(board[0]) / sizeof(board[0][0]);
		std::srand(int(std::time(nullptr)));
		nextPiece = piece(blocktype(rand() % 7), 4, 0);


		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{

		frameTime += fElapsedTime;
		if (frameTime > tick && game) {
			frameTime = 0;
			//std::cout << "Tick" << std::endl;

			//per tick logic
			if (currentPiece.shape == X) {//first round logic
				currentPiece = nextPiece;
				//make next piece
				nextPiece = piece(blocktype(rand()%7), 4, 0);
				//clear next piece board
				for (int x = 0; x < 4; x++) {
					for (int y = 0; y < 4; y++) {
						nextPieceBoard[x][y].type = X;
					}
				}
				//draw next piece
				for (AC::vec2dInt b : nextPiece.blocks) {
					nextPieceBoard[b.x][b.y].type = nextPiece.shape;
				}
				currentPiece.drawShape(4, 0, currentPiece.shape);
			}
			else {
				if (!currentPiece.update(0, 1)) {
					//if it fails to update then kill the current piece
					currentPiece = nextPiece;
					//make next piece
					nextPiece = piece(blocktype(rand() % 7), 4, 0);
					//clear next piece board
					for (int x = 0; x < 4; x++) {
						for (int y = 0; y < 4; y++) {
							nextPieceBoard[x][y].type = X;
						}
					}
					//draw next piece
					for (AC::vec2dInt b : nextPiece.blocks) {
						nextPieceBoard[b.x][b.y].type = nextPiece.shape;
					}

					if (currentPiece.doescollide(4, 0, 0)) {//checks gameover
						game = false;
						for (int x = 0; x < cols; x++) {
							for (int y = 0; y < rows; y++) {
								board[x][y].type = X;
							}
						}
					}
					else {
						currentPiece.drawShape(4, 0, currentPiece.shape);
					}
				}
			}
			if ((GetKey(AC::DOWN).bHeld)) {
				score += 1;
			}
			else {
				tick = float(1 - (lvl * .07));
			}
		}

		//updates for scoring
		lvl = lines / 10;


		//Drawing sequince
		Clear(AC::Pixel(0,0,0));
		DrawRect(0,0, 83,163, AC::Pixel(0, 0, 255));
		DrawRect(AC::vec2dInt(1,1), AC::vec2dInt(81,161), AC::Pixel(0, 0, 255));

		//drawboard
		for (int x = 0; x < cols; x++) {
			for (int y = 0; y < rows; y++) {
				block currentblock = board[x][y];
				if (currentblock.type != X) {
					AC::Pixel color;

					switch (currentblock.type)
					{
					case Z:
						color = AC::Pixel(255, 0, 0);
						break;
					case S:
						color = AC::Pixel(0, 255, 0);
						break;
					case T:
						color = AC::Pixel(128, 0, 128);
						break;
					case O:
						color = AC::Pixel(255, 255, 0);
						break;
					case I:
						color = AC::Pixel(0, 255, 255);
						break;
					case L:
						color = AC::Pixel(255, 127, 0);
						break;
					case J:
						color = AC::Pixel(0, 0, 255);
						break;
					case X:
						color = AC::Pixel(225, 200, 200);
						break;
					default:
						break;
					}

					DrawRect((x * 8 + 2), (y * 8 + 2), 7, 7, color);
					DrawRect((x * 8 + 3), (y * 8 + 3), 5, 5, color-AC::Pixel(70,70,70));
					DrawRect((x * 8 + 4), (y * 8 + 4), 3, 3, color);
				}
			}
		}
		//draw next piece
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				block currentblock = nextPieceBoard[x][y];
				if (currentblock.type != X) {
					AC::Pixel color;

					switch (currentblock.type)
					{
					case Z:
						color = AC::Pixel(255, 0, 0);
						break;
					case S:
						color = AC::Pixel(0, 255, 0);
						break;
					case T:
						color = AC::Pixel(128, 0, 128);
						break;
					case O:
						color = AC::Pixel(255, 255, 0);
						break;
					case I:
						color = AC::Pixel(0, 255, 255);
						break;
					case L:
						color = AC::Pixel(255, 127, 0);
						break;
					case J:
						color = AC::Pixel(0, 0, 255);
						break;
					case X:
						color = AC::Pixel(225, 200, 200);
						break;
					default:
						break;
					}

					DrawRect((x * 8 + 110), (y * 8 + 90), 7, 7, color);
					DrawRect((x * 8 + 111), (y * 8 + 91), 5, 5, color - AC::Pixel(70, 70, 70));
					DrawRect((x * 8 + 112), (y * 8 + 92), 3, 3, color);
				}
			}
		}

		//scoreboard
		DrawString(95, 10, "Score: " + std::to_string(score));
		DrawString(95, 30, "Lvl: " + std::to_string(lvl));
		DrawString(95, 50, "Lines: " + std::to_string(lines));
		DrawString(95, 70, "Next Piece:");

		if (!game) {
			DrawString(6, 50, "GAME OVER");
		}

		//inputs
		if (GetKey(AC::RIGHT).bPressed) {
			currentPiece.update(1, 0);
		}
		else if (GetKey(AC::LEFT).bPressed){
			currentPiece.update(-1, 0);
		}

		if ((GetKey(AC::DOWN).bPressed)) {
			tick = float(.07);
		}else
		if ((GetKey(AC::DOWN).bReleased)) {
			tick = float(1 - (lvl * .05));
		}

		if ((GetKey(AC::UP).bPressed)) {
			currentPiece.rotate(1);
		}

		return true;

	}
};
int main()
{
	Example demo;
	if (demo.Construct(200, 164, 3, 3, false, true))
		demo.Start();
	return 0;
}