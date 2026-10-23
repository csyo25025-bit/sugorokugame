#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm>
#include "DxLib.h"

using namespace std;

random_device rd;
mt19937 dice_num(rd());
uniform_int_distribution diceroll(1, 6);
uniform_int_distribution mass_prob(1, 100);
uniform_int_distribution yellow_event(0, 1); // 0: じゃんけん / 1: スロット / 2:コイントス / 3:シューティングゲーム

const int FRAME_RATE = 30;
const int FRAME_TIME = 1000 / FRAME_RATE;
const int WIDTH = 960;
const int HEIGHT = 640;

const int BASE_FONT_SIZE = 80;
const int MASS_SIZE = 60;

const int TOTAL_MASS = 30; // マス数（変更可能）
const int COLS_PER_ROW = WIDTH / MASS_SIZE; // 16マス

const int CHARA_X = 0;
const int CHARA_Y = 285;

enum { TITLE, FIRST_JANKEN, PLAY, DICE, MOVE, RESULT, PRE, DIS, SUP, JANKEN, SURROT };
enum { CHARAWAL, CHARAPRE, CHARADIS, CHARASUP };
enum { green, blue, red, yellow };

enum MassType {
	
	MASS_START = -2,
	MASS_GOAL = -1,
	MASS_GREEN = 0,
	MASS_BLUE = 1,
	MASS_RED = 2,
	MASS_YELLOW = 3

};

int CHARA1[4];
int CHARA2[4];
int MASS[4];
int JAN[3];
int NUM[6];
int scene = TITLE;

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
	int max_hp;
	int state;
	int images[4];
	int current_mass;

public:
	
	void init(int start_x, int start_y, int initial_hp, const int img_array[4]);
	void startMoveScreen();
	void addMass(int count, int max_mass);
	void addHp(int amount);
	bool update();
	void setState(int new_state);
	int getx();
	int gety();
	int gethp();
	int getMaxHp();
	bool isLowHp();
	int getMassIndex();
	int getImage(int img_type);
	void draw();
	vector<int> rollDice(int num_dice);

};

void drawText(int x, int y, const char* txt, int val, int siz, int col) {
	
	SetFontSize(BASE_FONT_SIZE * siz / 100);
	
	int strWidth = GetDrawStringWidth(txt, (int)strlen(txt));
	
	x = x - strWidth / 2;
	y = y - BASE_FONT_SIZE * siz / 100 / 2;
	
	DrawFormatString(x + 1, y + 2, colors.BLACK, txt, val);
	DrawFormatString(x, y, col, txt, val);

}

void getMassPos(int index, int& out_x, int& out_y) {

	int row = index / COLS_PER_ROW;
	int col = index % COLS_PER_ROW;

	if (row % 2 == 1) {

		col = (COLS_PER_ROW - 1) - col;

	}

	out_x = col * MASS_SIZE;
	int base_y = CHARA_Y + 120;
	out_y = base_y - (row * MASS_SIZE);

}

vector<int> createMap(int num_mass, int start_type = MASS_START, int goal_type = MASS_GOAL) {

	vector<int> game_map;

	game_map.reserve(num_mass);

	for (int i = 0; i < num_mass; ++i) {

		int prob = mass_prob(dice_num);

		if (prob <= 60) {

			game_map.push_back(MASS_GREEN);

		}
　
		else if (prob <= 80) {

			game_map.push_back(MASS_BLUE);

		}

		else if (prob <= 90) {

			game_map.push_back(MASS_RED);

		}

		else {

			game_map.push_back(MASS_YELLOW);

		}

	}

	if (num_mass > 0) {

		game_map[0] = start_type;

	}
	
	if (num_mass > 1) {

		game_map[num_mass - 1] = goal_type;

	}
	
	return game_map;

}

void drawMap(const vector<int>& game_map, int p1_mass, int p2_mass) {

	for (size_t i = 0; i < game_map.size(); ++i) {

		int draw_x, draw_y;

		getMassPos((int)i, draw_x, draw_y);

		if (draw_y < 0) {

			continue;
		
		}

		int type = game_map[i];

		if (type >= 0 && type < 4) {
		
			DrawExtendGraph(draw_x, draw_y, draw_x + MASS_SIZE, draw_y + MASS_SIZE, MASS[type], TRUE);
		
		}
		
		else if (type == MASS_START) {
		
			DrawExtendGraph(draw_x, draw_y, draw_x + MASS_SIZE, draw_y + MASS_SIZE, MASS[green], TRUE);
		
		}
		
		else if (type == MASS_GOAL) {
		
			DrawExtendGraph(draw_x, draw_y, draw_x + MASS_SIZE, draw_y + MASS_SIZE, MASS[yellow], TRUE);
		
		}

		if ((int)i == p1_mass) {
		
			DrawBox(draw_x, draw_y, draw_x + MASS_SIZE, draw_y + MASS_SIZE, colors.GOLD, FALSE);
			DrawBox(draw_x + 1, draw_y + 1, draw_x + MASS_SIZE - 1, draw_y + MASS_SIZE - 1, colors.GOLD, FALSE);
		
		}

		if ((int)i == p2_mass) {
		
			DrawBox(draw_x + 3, draw_y + 3, draw_x + MASS_SIZE - 3, draw_y + MASS_SIZE - 3, colors.RED, FALSE);
			DrawBox(draw_x + 4, draw_y + 4, draw_x + MASS_SIZE - 4, draw_y + MASS_SIZE - 4, colors.RED, FALSE);
		
		}

		if (type == MASS_START) {

			drawText(draw_x + MASS_SIZE / 2, draw_y + MASS_SIZE / 2, "START", 0, 18, colors.WHITE);
		
		}
		
		else if (type == MASS_GOAL) {

			drawText(draw_x + MASS_SIZE / 2, draw_y + MASS_SIZE / 2, "GOAL", 0, 18, colors.RED);
		
		}
	
	}

}

void firstJanken(int& current_turn) {

	static int state = 0;
	static int p_hand = -1;
	static int e_hand = -1;
	static int result = -1;

	if (state == 0) {

		drawText(WIDTH / 2, HEIGHT * 0.2, "先攻後攻決定じゃんけん", 0, 50, colors.GOLD);
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
		
		else if (result == 2) { // 勝ち
		
			drawText(WIDTH / 2, HEIGHT * 0.2, "あなたの勝ち！先攻（あなた）です", 0, 50, colors.LIGHTGREEN);
			drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーでゲーム開始", 0, 40, colors.GOLD);
			
			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				state = 0;
				p_hand = -1;
				current_turn = 0; // あなたが先攻
				scene = PLAY;
				WaitTimer(200);
			
			}
		
		}
		
		else if (result == 1) { // 負け
		
			drawText(WIDTH / 2, HEIGHT * 0.2, "あなたの負け！先攻（あいて）です", 0, 50, colors.RED);
			drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーでゲーム開始", 0, 40, colors.GOLD);
			
			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				state = 0;
				p_hand = -1;
				current_turn = 1; // あいてが先攻
				scene = PLAY;
				
				WaitTimer(200);
			
			}
		
		}
	
	}

}

void janken(Character& current_chara, int current_turn, int& next_turn, int& enemy_timer) {

	static int state = 0;
	static int p_hand = -1;
	static int e_hand = -1;
	static int result = -1;
	static bool hp_recovered = false;

	if (state == 0) {

		hp_recovered = false;

		if (current_turn == 0) {

			drawText(WIDTH / 2, HEIGHT * 0.2, "イベントじゃんけん！", 0, 50, colors.GOLD);
			drawText(WIDTH / 2, HEIGHT * 0.35, "勝ったらHP20回復！キーを押してね", 0, 40, colors.GOLD);
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
		
		else {
		
			p_hand = diceroll(dice_num) % 3;
			e_hand = diceroll(dice_num) % 3;
			result = (p_hand - e_hand + 3) % 3;
			state = 1;
		
		}
	
	}
	
	else if (state == 1) {
	
		DrawRotaGraph(WIDTH / 4, HEIGHT * 0.7, 0.6, 0.0, JAN[p_hand], TRUE);
		DrawRotaGraph(WIDTH * 3 / 4, HEIGHT * 0.7, 0.6, 0.0, JAN[e_hand], TRUE);

		const char* p1_name = (current_turn == 0) ? "あなた" : "あいて";
		const char* p2_name = (current_turn == 0) ? "あいて" : "あなた";

		drawText(WIDTH / 4, HEIGHT / 2 - 30, p1_name, 0, 30, colors.LIGHTGREEN);
		drawText(WIDTH * 3 / 4, HEIGHT / 2 - 30, p2_name, 0, 30, colors.RED);

		if (result == 0) {
		
			drawText(WIDTH / 2, HEIGHT - 130, "あいこ！", 0, 60, colors.WHITE);
			
			if (current_turn == 0) {
			
				drawText(WIDTH / 2, HEIGHT - 180, "スペースキーでもう1回", 0, 40, colors.GOLD);
				
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					state = 0;
					p_hand = -1;
					WaitTimer(200);
				
				}
			
			}
			
			else {
			
				++enemy_timer;
				
				if (enemy_timer > 30) {
				
					enemy_timer = 0;
					state = 0;
					p_hand = -1;
				
				}
			
			}
		
		}
		
		else if (result == 2) {
		
			if (!hp_recovered) {
			
				current_chara.addHp(20);
				hp_recovered = true;
			
			}
			
			drawText(WIDTH / 2, HEIGHT * 0.2, "勝利！ (HP+20回復)", 0, 50, colors.LIGHTGREEN);

			if (current_turn == 0) {
			
				drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーで戻る", 0, 40, colors.GOLD);
				
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					state = 0;
					p_hand = -1;
					next_turn = 1 - current_turn;
					scene = PLAY;
					WaitTimer(200);
				
				}
			
			}
			
			else {
			
				++enemy_timer;
				
				if (enemy_timer > 40) {
				
					enemy_timer = 0;
					state = 0;
					p_hand = -1;
					next_turn = 1 - current_turn;
					scene = PLAY;
				
				}
			
			}
		
		}
		
		else if (result == 1) {
		
			drawText(WIDTH / 2, HEIGHT * 0.2, "敗北... (回復なし)", 0, 50, colors.RED);

			if (current_turn == 0) {
			
				drawText(WIDTH / 2, HEIGHT * 0.8, "スペースキーで戻る", 0, 40, colors.GOLD);
				
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					state = 0;
					p_hand = -1;
					next_turn = 1 - current_turn;
					scene = PLAY;
					WaitTimer(200);
				
				}
			
			}
			
			else {
			
				++enemy_timer;
				
				if (enemy_timer > 40) {
		
					enemy_timer = 0;
					state = 0;
					p_hand = -1;
					next_turn = 1 - current_turn;
					scene = PLAY;
				
				}
			
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
	int dice_state = 0;
	vector<int> current_dices;

	int current_turn = 0;
	int enemy_timer = 0;
	bool slot_hp_applied = false;
	int goal_winner = -1;

	vector<int> game_map;

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
				game_map = createMap(TOTAL_MASS, MASS_START, MASS_GOAL);
				chara[0].init(CHARA_X, CHARA_Y, 100, CHARA1);
				chara[1].init(CHARA_X, CHARA_Y + 100, 100, CHARA2);
				goal_winner = -1;

				scene = FIRST_JANKEN;
				WaitTimer(200);

			}

			break;

		case FIRST_JANKEN:

			DrawGraph(0, 0, img[6], TRUE);
			firstJanken(current_turn);

			break;

		case PLAY: {

			DrawGraph(0, 0, img[2], FALSE);

			drawMap(game_map, chara[0].getMassIndex(), chara[1].getMassIndex());
			chara[0].draw();
			chara[1].draw();

			int p1_rem = TOTAL_MASS - 1 - chara[0].getMassIndex();
			int p2_rem = TOTAL_MASS - 1 - chara[1].getMassIndex();
			if (p1_rem < 0) {

				p1_rem = 0;
			
			}
			
			if (p2_rem < 0) {

				p2_rem = 0;
			
			}

			char pos_info[64];
			sprintf_s(pos_info, "あなた: 残り%dマス  |  あいて: 残り%dマス", p1_rem, p2_rem);
			drawText(WIDTH / 2, HEIGHT * 0.08, pos_info, 0, 36, colors.GOLD);

			int p1_color = chara[0].isLowHp() ? colors.RED : colors.LIGHTGREEN;
			int p2_color = chara[1].isLowHp() ? colors.RED : colors.LIGHTGREEN;

			char p1_hp_txt[32], p2_hp_txt[32];
			sprintf_s(p1_hp_txt, "HP あなた: %d", chara[0].gethp());
			sprintf_s(p2_hp_txt, "HP あいて: %d", chara[1].gethp());

			drawText(WIDTH / 3, HEIGHT * 0.15, p1_hp_txt, 0, 32, p1_color);
			drawText(WIDTH * 2 / 3, HEIGHT * 0.15, p2_hp_txt, 0, 32, p2_color);

			if (current_turn == 0) {
				
				if (chara[0].isLowHp()) {
				
					drawText(WIDTH / 2, HEIGHT * 0.23, "【あなたのターン】ピンチ！(出目半減中) 「X」で振る", 0, 32, colors.RED);
				
				}
				
				else {
				
					drawText(WIDTH / 2, HEIGHT * 0.23, "【あなたのターン】「X」キーでサイコロ！", 0, 35, colors.WHITE);
				
				}

				
				if (CheckHitKey(KEY_INPUT_X) == 1) {
				
					dice_state = 0;
					scene = DICE;
			
				}
			
			}
			
			else {
			
				if (chara[1].isLowHp()) {
				
					drawText(WIDTH / 2, HEIGHT * 0.23, "【あいてのターン】ピンチ！(出目半減中) 自動進行中...", 0, 32, colors.RED);
				
				}
				
				else {
				
					drawText(WIDTH / 2, HEIGHT * 0.23, "【あいてのターン】自動進行中...", 0, 35, colors.RED);
				
				}

				++enemy_timer;
				
				if (enemy_timer > 30) {
				
					enemy_timer = 0;
					dice_state = 0;
					scene = DICE;
				
				}
			
			}

			//デバッグ用
			/*
			if (CheckHitKey(KEY_INPUT_P) == 1) {
			
				scene = PRE;
			
			}

			if (CheckHitKey(KEY_INPUT_S) == 1) {

				scene = SUP;

			}

			if (CheckHitKey(KEY_INPUT_D) == 1) {

				scene = DIS;

			}

			if (CheckHitKey(KEY_INPUT_O) == 1) {

				scene = SURROT;
			
			}
			
			if (CheckHitKey(KEY_INPUT_J) == 1) {

				scene = JANKEN;
			
			}

			*/
			
			break;
		
		}

		case MOVE: {
			
			DrawGraph(0, 0, img[1], FALSE);

			bool is_moving = chara[current_turn].update();
			chara[current_turn].draw();

			if (!is_moving) {
			
				StopSoundMem(WALK);

				int move_steps = current_dices[0];
				chara[current_turn].addMass(move_steps, TOTAL_MASS);

				int landed_mass = chara[current_turn].getMassIndex();

				if (landed_mass >= TOTAL_MASS - 1) {
					
					chara[current_turn].addHp(30);
					goal_winner = current_turn;
					scene = RESULT;
				
					break;
				
				}

				int mass_type = game_map[landed_mass];

				if (mass_type == MASS_BLUE) {
				
					chara[current_turn].addHp(10);
					PlaySoundMem(PRESED, DX_PLAYTYPE_BACK);
					
					scene = PRE;
				
				}
				
				else if (mass_type == MASS_RED) {
				
					chara[current_turn].addHp(-10);
					PlaySoundMem(DISAPOINTED, DX_PLAYTYPE_BACK);
					
					scene = DIS;
				
				}
				
				else if (mass_type == MASS_YELLOW) {
				
					PlaySoundMem(SUPREASED, DX_PLAYTYPE_BACK);
					scene = SUP;
				
				}
				
				else {
				
					current_turn = 1 - current_turn;
					
					scene = PLAY;
				
				}
		
			}
		
		}
		
				 break;

		case PRE:
			
			DrawGraph(0, 0, img[3], TRUE);
			DrawGraph(0, 75, chara[current_turn].getImage(CHARAPRE), TRUE);
			drawText(WIDTH / 2, HEIGHT * 0.85, "青マス！HP+10回復！ (SPACEで戻る)", 0, 36, colors.GOLD);

			if (current_turn == 0) {
			
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					current_turn = 1 - current_turn;
					scene = PLAY;
					
					WaitTimer(200);
				
				}
			
			}
			
			else {
			
				++enemy_timer;
				
				if (enemy_timer > 30) {
				
					enemy_timer = 0;
					current_turn = 1 - current_turn;
					scene = PLAY;
				
				}
			
			}
			
			break;

		case DIS:
			
			DrawGraph(0, 0, img[4], TRUE);
			DrawGraph(0, 75, chara[current_turn].getImage(CHARADIS), TRUE);
			drawText(WIDTH / 2, HEIGHT * 0.85, "赤マス！HP-10！ (SPACEで戻る)", 0, 36, colors.RED);

			if (current_turn == 0) {
			
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					current_turn = 1 - current_turn;
					scene = PLAY;
					
					WaitTimer(200);
				
				}
			
			}
			
			else {
			
				++enemy_timer;
			
				if (enemy_timer > 30) {
				
					enemy_timer = 0;
					current_turn = 1 - current_turn;
					
					scene = PLAY;
				
				}
			
			}
			
			break;

		case SUP:
			
			DrawGraph(0, 0, img[5], TRUE);
			DrawGraph(0, 75, chara[current_turn].getImage(CHARASUP), TRUE);
			drawText(WIDTH / 2, HEIGHT * 0.85, "黄色マス！イベント画面へ (SPACEで進む)", 0, 36, colors.GOLD);

			if (current_turn == 0) {
			
				if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
					int ev = yellow_event(dice_num);
					slot_hp_applied = false;
					
					if (ev == 0) {

						scene = JANKEN;
					
					}
					
					else if {

						scene = SURROT;
					
					}
					
					WaitTimer(200);
				
				}
			
			}

			else {
			
				++enemy_timer;
				
				if (enemy_timer > 30) {
				
					enemy_timer = 0;
					int ev = yellow_event(dice_num);
					slot_hp_applied = false;
					
					if (ev == 0) {

						scene = JANKEN;
					
					}
					
					else if{

						scene = SURROT;
				
					}
				
				}
			
			}
			break;

		case JANKEN:
			
			DrawGraph(0, 0, img[6], TRUE);
			janken(chara[current_turn], current_turn, current_turn, enemy_timer);
			
			break;

		case DICE:
			
			if (dice_state == 0) {
			
				current_dices = chara[current_turn].rollDice(1);

				if (chara[current_turn].isLowHp()) {
				
					current_dices[0] = max(1, current_dices[0] / 2);
				
				}

				if (current_turn == 0) {
				
					if (timer % 30 < 15) {
					
						drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでサイコロを止める！", 0, 40, colors.GOLD);
					
					}
					
					if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
					
						dice_state = 1;
						PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
						WaitTimer(200);
					
					}
				
				}
				
				else {
				
					++enemy_timer;
					
					if (enemy_timer > 20) {
					
						enemy_timer = 0;
						dice_state = 1;
						PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
					
					}
				
				}
			
			}
			
			else if (dice_state == 1) {
			
				DrawRotaGraph(WIDTH / 2, HEIGHT / 2, 0.5, 0.0, NUM[current_dices[0] - 1], TRUE);

				if (current_turn == 0) {
				
					if (chara[0].isLowHp()) {
					
						drawText(WIDTH / 2, HEIGHT * 0.2, "ピンチ！出目が半分になりました", 0, 45, colors.RED);
					
					}
					
					else {
					
						drawText(WIDTH / 2, HEIGHT * 0.2, "あなたの出目が決定！", 0, 50, colors.LIGHTGREEN);
					
					}

					if (timer % 30 < 15) {
					
						drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーで移動開始！", 0, 40, colors.GOLD);
					
					}
					
					if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
					
						chara[current_turn].startMoveScreen();
						scene = MOVE;
						dice_state = 0;
						PlaySoundMem(WALK, DX_PLAYTYPE_BACK);
						
						WaitTimer(200);
					
					}
				
				}
				
				else {
				
					if (chara[1].isLowHp()) {
					
						drawText(WIDTH / 2, HEIGHT * 0.2, "あいてはピンチ！出目が半分に", 0, 45, colors.RED);
					
					}
					
					else {
					
						drawText(WIDTH / 2, HEIGHT * 0.2, "あいての出目が決定！", 0, 50, colors.RED);
					
					}

					++enemy_timer;
					
					if (enemy_timer > 20) {
					
						enemy_timer = 0;
						chara[current_turn].startMoveScreen();
						scene = MOVE;
						dice_state = 0;
						PlaySoundMem(WALK, DX_PLAYTYPE_BACK);
					
					}
				
				}
			
			}
			
			break;

		case SURROT:
			
			if (dice_state == 0) {
			
				current_dices = chara[current_turn].rollDice(3);

				if (current_turn == 0) {
				
					if (timer % 30 < 15) {
					
						drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでスロットを止める！", 0, 40, colors.GOLD);
					
					}
					
					if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
				
						dice_state = 1;
						PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
						WaitTimer(200);
					
					}
				
				}
				
				else {
				
					++enemy_timer;
					
					if (enemy_timer > 20) {
					
						enemy_timer = 0;
						dice_state = 1;
						PlaySoundMem(CLICK, DX_PLAYTYPE_BACK);
					
					}
				
				}

			}
			
			else if (dice_state == 1) {
			
				for (int i = 0; i < 3; i++) {
				
					int draw_x = WIDTH / 4 + (i * (WIDTH / 4));
					DrawRotaGraph(draw_x, HEIGHT / 2, 0.5, 0.0, NUM[current_dices[i] - 1], TRUE);
				
				}

				bool is_aligned = (current_dices[0] == current_dices[1] && current_dices[1] == current_dices[2]);

				if (is_aligned) {
				
					if (!slot_hp_applied) {
					
						chara[current_turn].addHp(20);
						slot_hp_applied = true;
					
					}
				
					drawText(WIDTH / 2, HEIGHT * 0.2, "大当り！数字が全揃い！ (HP+20回復)", 0, 45, colors.GOLD);
				
				}
				
				else {
				
					drawText(WIDTH / 2, HEIGHT * 0.2, "残念！揃わなかった...", 0, 45, colors.WHITE);
				
				}

				if (current_turn == 0) {
				
					if (timer % 30 < 15) {
					
						drawText(WIDTH / 2, HEIGHT * 0.75, "「SPACE」キーでPLAYへ戻る", 0, 40, colors.GOLD);
					
					}

					if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
					
						current_turn = 1 - current_turn;
						scene = PLAY;
						dice_state = 0;
						WaitTimer(200);
					
					}
				
				}
				
				else {
				
					++enemy_timer;
					
					if (enemy_timer > 40) {
					
						enemy_timer = 0;
						current_turn = 1 - current_turn;
						scene = PLAY;
						dice_state = 0;
					
					}
				
				}
			
			}
			
			break;

		case RESULT: {
			
			DrawGraph(0, 0, img[0], FALSE);

			drawText(WIDTH / 2, HEIGHT * 0.2, "GAME OVER - 結果発表", 0, 60, colors.GOLD);

			char hp_res[64];
			sprintf_s(hp_res, "最終HP - あなた: %d  |  あいて: %d", chara[0].gethp(), chara[1].gethp());
			drawText(WIDTH / 2, HEIGHT * 0.4, hp_res, 0, 40, colors.WHITE);

			if (chara[0].gethp() > chara[1].gethp()) {
			
				drawText(WIDTH / 2, HEIGHT * 0.6, "あなたの勝利！！", 0, 70, colors.LIGHTGREEN);
			
			}
			
			else if (chara[0].gethp() < chara[1].gethp()) {
			
				drawText(WIDTH / 2, HEIGHT * 0.6, "あいての勝利...", 0, 70, colors.RED);
			
			}
			
			else {
			
				drawText(WIDTH / 2, HEIGHT * 0.6, "引き分け！", 0, 70, colors.WHITE);
			
			}

			if (timer % 30 < 15) {
			
				drawText(WIDTH / 2, HEIGHT * 0.85, "「SPACE」キーでタイトルへ戻る", 0, 40, colors.GOLD);
			
			}

			if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
			
				scene = TITLE;
				WaitTimer(200);
			
			}
			
			break;
		
		}

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
	max_hp = initial_hp;
	speed = 12;
	state = CHARAWAL;
	current_mass = 0;

	for (int i = 0; i < 4; i++) {

		images[i] = img_array[i];

	}

}

void Character::startMoveScreen() {

	x = 0;
	target_x = WIDTH - MASS_SIZE;
	state = CHARAWAL;

}

void Character::addMass(int count, int max_mass) {

	current_mass += count;
	if (current_mass < 0) {

		current_mass = 0;
	
	}
	
	if (current_mass >= max_mass - 1) {

		current_mass = max_mass - 1;
	
	}

}

void Character::addHp(int amount) {

	hp += amount;
	if (hp < 0) {

		hp = 0;
	
	}

}

bool Character::isLowHp() {

	return hp <= (max_hp * 3 / 10);

}

bool Character::update() {

	if (x < target_x) {

		x += speed;

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

int Character::getMaxHp() { 
	
	return max_hp; 

}

int Character::getMassIndex() { 
	
	return current_mass; 

}

int Character::getImage(int img_type) { 
	
	return images[img_type]; 

}

vector<int> Character::rollDice(int num_dice) {

	vector<int> results;

	for (int i = 0; i < num_dice; ++i) {

		int roll = diceroll(dice_num);
		results.push_back(roll);

		int draw_x = WIDTH / (num_dice + 1) * (i + 1);
		DrawRotaGraph(draw_x, HEIGHT / 2, 0.5, 0.0, NUM[roll - 1], TRUE);

	}

	return results;

}