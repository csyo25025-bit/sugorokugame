#include<iostream>
#include<vector>
#include"DxLib.h"

using namespace std;

/*ゲーム内カウント*/
const int FRAME_RATE = 30;
const int FRAME_TIME = 1000 / FRAME_RATE;

/*画面設定*/
const int WIDTH = 960;
const int HEIGHT = 640;

const int BSIZE = 80;

/*キャラクターサイズ*/
const int CHARA_X = 0;
const int CHARA_Y = 285;

enum {TITLE,PLAY,DICE,MOVE,RESULT};

/*色設定*/
struct Color {
	
	int RED;
	int GREEN;
	int BLUE;
	int WHITE;
	int BLACK;
	int MAGENTA;
	int EMERALDGREEN;
	int GOLD;
	int LIGHTGREEN;

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

/*関数設定*/
void drawText(int x, int y, const char* txt, int val, int siz, int col) {

	SetFontSize(BSIZE * siz / 100);
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	
	x = x - strWidth / 2;
	y = y - BSIZE * siz / 100 / 2;

	DrawFormatString(x + 1, y + 2, colors.BLACK, txt, val);
	DrawFormatString(x, y, col, txt, val);

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetWindowText("すごろく");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);

	int dx = 0;

	if (DxLib_Init() == -1) {
	
		return -1;
	
	}

	int img[2] = {

		LoadGraph("BACK/TITLE_BACK.jpg"),
		LoadGraph("BACK/MOVE_BACK.jpg")

	};

	int BGM= LoadSoundMem("MUSIC/BGM.mp3");
	int CLICK = LoadSoundMem("MUSIC/CLICK.wav");
	int WALK = LoadSoundMem("MUSIC/WALK.wav");

	int CHARA[8] = {

		LoadGraph("CHARA1/CHARA1_WAL.png"),
		LoadGraph("CHARA1/CHARA1_PRE.png"),
		LoadGraph("CHARA1/CHARA1_DIS.png"),
		LoadGraph("CHARA1/CHARA1_SUP.png"),

		LoadGraph("CHARA2/CHARA2_WAL.png"),
		LoadGraph("CHARA2/CHARA2_PRE.png"),
		LoadGraph("CHARA2/CHARA2_DIS.png"),
		LoadGraph("CHARA2/CHARA2_SUP.png")

	};

	ChangeVolumeSoundMem(128, BGM);
	ChangeVolumeSoundMem(150, CLICK);
	ChangeVolumeSoundMem(100, WALK);
	PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);
	

	colors.Init();

	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int scene = TITLE;
	int timer = 0;

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
				dx = 0;
				scene = PLAY;
			
			}

			break;

		case PLAY:

			DrawBox(0, 0, WIDTH, HEIGHT, colors.BLACK, TRUE);
			drawText(WIDTH / 2, HEIGHT / 2, "PLAY GAME", 0, 80, colors.WHITE);

			if (CheckHitKey(KEY_INPUT_M) == 1) {
			
				scene = MOVE;
				PlaySoundMem(WALK, DX_PLAYTYPE_BACK);
			
			}

			break;

		case MOVE:
			
			DrawGraph(0, 0, img[1], FALSE);
			dx = dx + 10;
			DrawGraph(CHARA_X+dx, CHARA_Y, CHARA[4], TRUE);

			if (CHARA_X + dx > 800) {
			
				StopSoundMem(WALK);
				scene = PLAY;
			
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