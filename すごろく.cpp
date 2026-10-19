#include <iostream>
#include <vector>
#include <random>
#include <cstring> // strlen用
#include "DxLib.h"

using namespace std;

/* サイコロ設定 */
random_device rd;
mt19937 dice_num(rd());
uniform_int_distribution diceroll(1, 6);

/* ゲーム内カウント */
const int FRAME_RATE = 30;
const int FRAME_TIME = 1000 / FRAME_RATE;

/* 画面設定 */
const int WIDTH = 960;
const int HEIGHT = 640;
const int BSIZE = 80;

/* キャラクターサイズ */
const int CHARA_X = 0;
const int CHARA_Y = 285;

enum { TITLE, PLAY, DICE, MOVE, RESULT, PRE, DIS, SUP, JANKEN, SURROT };
enum { CHARAWAL, CHARAPRE, CHARADIS, CHARASUP };
enum { green, blue, red, yellow };
enum { JAN_ENUM, HEL, DEC, UP, DOW };

int CHARA1[4];
int CHARA2[4];
int MASS[4];
int JAN[3];
int NUM[6];
int scene = TITLE;

/* 色設定 */
struct Color {
	
	int RED, GREEN, BLUE, WHITE, BLACK, MAGENTA, EMERALDGREEN, GOLD, LIGHTGREEN;
	
	void Init() {
	
		RED = GetColor(255, 0, 0);
		GREEN = GetColor(0, 255, 255);
		BLUE = GetColor(0, 0, 255);
		WHITE = GetColor(255, 255, 255);
		BLACK = GetColor(0, 0, 0);
		MAGENTA = GetColor(255, 0, 224);
		EMERALDGREEN = GetColor(32, 192, 96);
		GOLD = GetColor(244, 192, 0);
		LIGHTGREEN = GetColor(128, 255, 128);
	
	}

};

Color colors;

class Character {

private:

	int x;
	int y;
	int target_x;
	int speed;
	int hp;
	int state;
	int images[4];

public:
	
	void init(int start_x, int start_y, int initial_hp, const int img_array[4]);
	void startMove(int move_distance);
	bool update();
	void setState(int new_state);
	int getx();
	int gety();
	int gethp();
	void draw();
	vector<int> rollDice(int num_dice);

};

void drawText(int x, int y, const char* txt, int val, int siz, int col) {

	SetFontSize(BSIZE * siz / 100);

	int strWidth = GetDrawStringWidth(txt, (int)strlen(txt));

	x = x - strWidth / 2;
	y = y - BSIZE * siz / 100 / 2;

	DrawFormatString(x + 1, y + 2, colors.BLACK, txt, val);
	DrawFormatString(x, y, col, txt, val);

}

void janken() {

	static int state = 0;
	static int p_hand = -1;
	static int e_hand = -1;
	static int result = -1;

	if (state == 0) {

		drawText(WIDTH / 2, HEIGHT * 0.2, "先攻後攻じゃんけん", 0, 50, colors.GOLD);
		drawText(WIDTH / 2, HEIGHT * 0.35, "勝った方が先攻！キーを押してね", 0, 40, colors.GOLD);
		drawText(WIDTH / 2, HEIGHT * 0.6, "1: グー   2: チョキ   3: パー", 0, 50, colors.WHITE);

		if (CheckHitKey(KEY_INPUT_1) == 1) {

			p_hand = 0;
		
		}
		
		else if (CheckHitKey(KEY_INPUT_2) == 1) {

			p_hand = 1;

		}

		else if (CheckHitKey(KEY_INPUT_3) == 1) {

			p_hand = 2;
		
		}

		if (p_hand != -1) {
		
			e_hand = diceroll(dice_num) % 3;
			result = (p_hand - e_hand + 3) % 3;
			state = 1;
			WaitTimer(200);
		
		}
	
	}
	else if (state == 1) {
	
		DrawRotaGraph(WIDTH / 4, HEIGHT * 0.7, 0.6, 0.0, JAN[p_hand], TRUE);
		DrawRotaGraph(WIDTH * 3 / 4, HEIGHT * 0.7, 0.6, 0.0, JAN[e_hand], TRUE);

		drawText(WIDTH / 4, HEIGHT / 2 - 30, "あなた", 0, 30, colors.LIGHTGREEN);
		drawText(WIDTH * 3 / 4, HEIGHT / 2 - 30, "あいて", 0, 30, colors.RED);

		if (result == 0) {
		
			drawText(WIDTH / 2, HEIGHT - 130, "あいこ！", 0, 60, colors.WHITE);
			drawText(WIDTH / 2, HEIGHT - 180, "スペースキーでもう1回", 0, 40, colors.GOLD);

			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				state = 0;
				p_hand = -1;
				WaitTimer(200);
			
			}
		
		}
		else if (result == 2) {
		
			drawText(WIDTH / 2, HEIGHT * 0.2, "あなたの勝ち！先攻です", 0, 60, colors.LIGHTGREEN);
			drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーで戻る", 0, 40, colors.GOLD);

			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				state = 0;
				p_hand = -1;
				scene = PLAY;
				WaitTimer(200);
			
			}
		
		}
		
		else if (result == 1) {
		
			drawText(WIDTH / 2, HEIGHT * 0.2, "あなたの負け！後攻です", 0, 60, colors.RED);
			drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーで戻る", 0, 40, colors.GOLD);

			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				state = 0;
				p_hand = -1;
				scene = PLAY;
				WaitTimer(200);
			
			}
		
		}
	
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetWindowText("すごろく");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);

	if (DxLib_Init() == -1) {

		return -1;

	}

	CHARA1[0] = LoadGraph("CHARA1/CHARA1_WAL.png");
	CHARA1[1] = LoadGraph("CHARA1/CHARA1_PRE.png");
	CHARA1[2] = LoadGraph("CHARA1/CHARA1_DIS.png");
	CHARA1[3] = LoadGraph("CHARA1/CHARA1_SUP.png");

	CHARA2[0] = LoadGraph("CHARA2/CHARA2_WAL.png");
	CHARA2[1] = LoadGraph("CHARA2/CHARA2_PRE.png");
	CHARA2[2] = LoadGraph("CHARA2/CHARA2_DIS.png");
	CHARA2[3] = LoadGraph("CHARA2/CHARA2_SUP.png");

	MASS[0] = LoadGraph("MASS/GREEN.png");
	MASS[1] = LoadGraph("MASS/BLUE.png");
	MASS[2] = LoadGraph("MASS/RED.png");
	MASS[3] = LoadGraph("MASS/YELLOW.png");

	JAN[0] = LoadGraph("JANKEN/ROC.png");
	JAN[1] = LoadGraph("JANKEN/SCI.png");
	JAN[2] = LoadGraph("JANKEN/PEP.png");

	NUM[0] = LoadGraph("NUMBER/ONE.png");
	NUM[1] = LoadGraph("NUMBER/TOW.png");
	NUM[2] = LoadGraph("NUMBER/THR.png");
	NUM[3] = LoadGraph("NUMBER/FOU.png");
	NUM[4] = LoadGraph("NUMBER/FIV.png");
	NUM[5] = LoadGraph("NUMBER/SIX.png");

	int img[7] = {
		
		LoadGraph("BACK/TITLE_BACK.jpg"),
		LoadGraph("BACK/MOVE_BACK.jpg"),
		LoadGraph("BACK/PLAY_BACK.jpg"),
		LoadGraph("BACK/PRE_BACK.jpg"),
		LoadGraph("BACK/DIS_BACK.jpg"),
		LoadGraph("BACK/SUP_BACK.jpg"),
		LoadGraph("BACK/JANKEN_BACK.jpeg")
	
	};

	Character chara[2];
	
	chara[0].init(CHARA_X, CHARA_Y, 100, CHARA1);
	chara[1].init(CHARA_X, CHARA_Y + 100, 100, CHARA2);

	int BGM = LoadSoundMem("MUSIC/BGM.mp3");
	int CLICK = LoadSoundMem("MUSIC/CLICK.wav");
	int WALK = LoadSoundMem("MUSIC/WALK.wav");
	int PRESED = LoadSoundMem("MUSIC/PRE.wav");
	int DISAPOINTED = LoadSoundMem("MUSIC/DIS.wav");
	int SUPREASED = LoadSoundMem("MUSIC/SUP.wav");

	ChangeVolumeSoundMem(128, BGM);
	ChangeVolumeSoundMem(150, CLICK);
	ChangeVolumeSoundMem(100, WALK);
	PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);

	colors.Init();

	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int timer = 0;

	// サイコロ状態共有用変数
	int dice_state = 0;
	vector<int> current_dices;

	while (ProcessMessage() == 0) {
		
		++timer;
		ClearDrawScreen();

		switch (scene) {

		case TITLE:
			
			DrawGraph(0, 0, img[0], FALSE);
			drawText(WIDTH / 2, HEIGHT * 0.33, "すごろく", 0, 100, colors.LIGHTGREEN);

			if (timer % 30 < 15) {
			
				drawText(WIDTH / 2, HEIGHT * 0.66, "「SPACE」キーを押してゲームスタート", 0, 50, colors.GOLD);
			
			}

			if (CheckHitKey(KEY_INPUT_SPACE)) {
			
				PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
				scene = PLAY;
			
			}
			
			break;

		case PLAY:
			
			DrawGraph(0, 0, img[2], FALSE);
			drawText(WIDTH / 2, HEIGHT / 2, "PLAY GAME", 0, 80, colors.WHITE);

			if (CheckHitKey(KEY_INPUT_O) == 1) { 
				
				scene = SURROT;
			
			}
			
			if (CheckHitKey(KEY_INPUT_P) == 1) { 
				
				scene = PRE; 
				PlaySoundMem(PRESED, DX_PLAYTYPE_BACK);
			
			}
			
			if (CheckHitKey(KEY_INPUT_D) == 1) { 
				
				scene = DIS;
				PlaySoundMem(DISAPOINTED, DX_PLAYTYPE_BACK); 
			
			}
			
			if (CheckHitKey(KEY_INPUT_S) == 1) { 
				
				scene = SUP;
				PlaySoundMem(SUPREASED, DX_PLAYTYPE_BACK); 
			
			}
			
			if (CheckHitKey(KEY_INPUT_J) == 1) {

				scene = JANKEN;
			
			}
			
			if (CheckHitKey(KEY_INPUT_X) == 1) {
				
				dice_state = 0; 
				scene = DICE;
			
			}
			
			break;

		case MOVE: {
			
			DrawGraph(0, 0, img[1], FALSE);
			bool is_moving = chara[0].update();
			chara[0].draw();

			if (!is_moving) {
			
				StopSoundMem(WALK);
				chara[0].init(CHARA_X, CHARA_Y, 100, CHARA1);
				scene = PLAY;
			
			}
		
		}
		
			break;

		case PRE:
		
			DrawGraph(0, 0, img[3], TRUE);
			DrawGraph(0, 75, CHARA1[CHARAPRE], TRUE);
			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

				scene = PLAY;
			
			}
			
			break;

		case DIS:
			
			DrawGraph(0, 0, img[4], TRUE);
			DrawGraph(0, 75, CHARA1[CHARADIS], TRUE);
			
			
			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

				scene = PLAY;
			
			}
			
			break;

		case SUP:
			
			DrawGraph(0, 0, img[5], TRUE);
			DrawGraph(0, 75, CHARA1[CHARASUP], TRUE);
			
			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {

				scene = PLAY;
			
			}
			
			break;

		case JANKEN:

			DrawGraph(0, 0, img[6], TRUE);
			janken();
			
			break;

		case DICE:
			
			if (dice_state == 0) {
			
				current_dices = chara[0].rollDice(1);

				if (timer % 30 < 15) {
				
					drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでサイコロを止める！", 0, 40, colors.GOLD);
				
				}

				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					dice_state = 1;
					PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
					WaitTimer(200);
				
				}
			
			}
			
			else if (dice_state == 1) {
			
				DrawRotaGraph(WIDTH / 2, HEIGHT / 2, 0.5, 0.0, NUM[current_dices[0] - 1], TRUE);
				drawText(WIDTH / 2, HEIGHT * 0.2, "出目が決定！", 0, 50, colors.LIGHTGREEN);

				if (timer % 30 < 15) {
				
					drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーで移動開始！", 0, 40, colors.GOLD);
				
				}

				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					chara[0].startMove(current_dices[0] * 80);
					scene = MOVE;
					dice_state = 0;
					PlaySoundMem(WALK, DX_PLAYTYPE_BACK);
					WaitTimer(200);
				
				}
			
			}
			
			break;

		case SURROT:
			
			if (dice_state == 0) {

				current_dices = chara[0].rollDice(3);

				if (timer % 30 < 15) {
			
					drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでスロットを止める！", 0, 40, colors.GOLD);
				
				}

				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					dice_state = 1;
					PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
					WaitTimer(200);
				
				}
			
			}
			
			else if (dice_state == 1) {
			
				for (int i = 0; i < 3; i++) {
				
					int draw_x = WIDTH / 4 + (i * (WIDTH / 4));
					DrawRotaGraph(draw_x, HEIGHT / 2, 0.5, 0.0, NUM[current_dices[i] - 1], TRUE);
			
				}

				drawText(WIDTH / 2, HEIGHT * 0.2, "スロット結果確定！", 0, 50, colors.LIGHTGREEN);
				
				if (timer % 30 < 15) {
				
					drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでPLAYへ戻る", 0, 40, colors.GOLD);
				
				}

				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					scene = PLAY;
					dice_state = 0;
					WaitTimer(200);
				
				}
			
			}
			
			break;

		}

		ScreenFlip();
		WaitTimer(FRAME_TIME);

		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {

			break;
		
		}
	
}

	DxLib_End();
	return 0;

}

void Character::init(int start_x, int start_y, int initial_hp, const int img_array[4]) {

	x = start_x;
	y = start_y;
	target_x = start_x;
	hp = initial_hp;
	speed = 10;
	state = CHARAWAL;

	for (int i = 0; i < 4; i++) {

		images[i] = img_array[i];
	
	}

}

void Character::startMove(int move_distance) {

	target_x = x + move_distance;
	state = CHARAWAL;

}

bool Character::update() {

	if (x < target_x) {

		x += speed;
		if (x >= WIDTH - BSIZE) {

			x = WIDTH - BSIZE;
			target_x = x;
			return false;

		}

		if (x >= target_x) {

			x = target_x;
			return false;

		}

		return true;

	}

	return false;

}

void Character::setState(int new_state) { 
	
	state = new_state; 

}

void Character::draw() {
	
	DrawGraph(x, y, images[state], TRUE);

}

int Character::getx() { 
	
	return x;

}

int Character::gety() { 
	
	return y; 

}

int Character::gethp() {
	
	return hp; 

}

vector<int> Character::rollDice(int num_dice) {

	vector<int> results;

	for (int i = 0; i < num_dice; i++) {

		int roll = diceroll(dice_num);
		results.push_back(roll);

		int draw_x = WIDTH / (num_dice + 1) * (i + 1);
		DrawRotaGraph(draw_x, HEIGHT / 2, 0.5, 0.0, NUM[roll - 1], TRUE);

	}

	return results;

}