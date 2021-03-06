#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

//deklarasi float
float player_1_p, player_1_dp, player_2_p, player_2_dp;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 130, ball_dp_y, ball_half_size = 1;

//deklarasi integer skor
int player_1_score, player_2_score;

//Physics player
internal void
simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp = 0;
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp = 0;
	}
}
//Physics tembok
internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y);
}

//deklarasi game mode
enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
	GM_PAUSED,
	GM_PLAYER1WIN,
	GM_PLAYER2WIN,
};
//event game mode
Gamemode current_gamemode;
//tombol konfirmasi
int hot_button;
//deklarasi ai
bool enemy_is_ai;

//Gameplay
internal void
simulate_game(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0x000000);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);

	//Gameplay dimulai
	if (current_gamemode == GM_GAMEPLAY) {
		// tekan tombol escape untuk keluar game
		if (pressed(BUTTON_ESCAPE)) {
			WM_DESTROY;
			{
				running = false;
			}
		}
		//tekan tombol p untuk pause game
		else if (pressed(BUTTON_P)) {
			current_gamemode = GM_PAUSED;
		}
	
		//kontrol posisi player 1
		float player_1_ddp = 0.f;
		if (!enemy_is_ai) {
			if (is_down(BUTTON_UP)) player_1_ddp += 2000;
			if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
		}
		//Alogritma A.I
		else {
			//if (ball_p_y > player_1_p+2.f) player_1_ddp += 1300;
			//if (ball_p_y < player_1_p-2.f) player_1_ddp -= 1300;
			player_1_ddp = (ball_p_y - player_1_p) * 100;
			if (player_1_ddp > 1300) player_1_ddp = 1300;
			if (player_1_ddp < -1300) player_1_ddp = -1300;
		}
		//kontrol posisi player 2
		float player_2_ddp = 0.f;
		if (is_down(BUTTON_W)) player_2_ddp += 2000;
		if (is_down(BUTTON_S)) player_2_ddp -= 2000;

		//physics player 
		simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
		simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);


		// Physics Bola
		{
			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;
			//collision tembok
			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player_1_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = 80 - player_half_size_x - ball_half_size;
				ball_dp_x *= -1;
				ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
			}
			else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player_2_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = -80 + player_half_size_x + ball_half_size;
				ball_dp_x *= -1;
				ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
			}
			//colision arena
			if (ball_p_y + ball_half_size > arena_half_size_y) {
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;
			}
			else if (ball_p_y - ball_half_size < -arena_half_size_y) {
				ball_p_y = -arena_half_size_y + ball_half_size;
				ball_dp_y *= -1;
			}
			//tambah skor player 1
			if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_1_score++;
			}
			//tambah skor player 2
			else if (ball_p_x - ball_half_size < -arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;
				player_2_score++;
			}
		}
		//jika skor 10 maka game berakhir

		if (player_2_score == 10)
		{
			WM_DESTROY;
			{
				running = false;
			}
		}
		//jika skor 10 maka game berakhir
		if (player_1_score == 10)
		{
			WM_DESTROY;
			{
				running = false;
			}
		}


	///	if (player_1_score == 10)
		// {
			//current_gamemode == GM_PLAYER1WIN; {
			//		running = true;
				//}
		//	}
			
		//}

		///	if (player_2_score == 10)
		// {
			//current_gamemode == GM_PLAYER1WIN; {
			//		running = true;
				//}
		//	}

		//}

		//menampilkan skor

		draw_number(player_1_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player_2_score, 10, 40, 1.f, 0x00FFFF);

		// meampilkan pemain dan bola
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xffffff);

		draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0x00FFFF);
		draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xbbffbb);

	}
	//jika player 1 menang  (UNUSED)
	else if (current_gamemode == GM_PLAYER1WIN) {

		if (pressed(BUTTON_ESCAPE)) {
			WM_DESTROY;
			{
				running = false;
			}
		}

		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY; {
				running = true;
			}
		}
		draw_text("PLAYER 1 WIN", -50, 20, 2, 0xffffff);
		draw_text("ENTER TO", 15, -10, 1, 0xaaaaaa);
		draw_text("RESTART", 15, -20, 1, 0xaaaaaa);
		draw_text("ESC TO EXIT", -80, -10, 1, 0xff0000);
		
	}
	//jika player 2 menang  (UNUSED)
	else if (current_gamemode == GM_PLAYER2WIN) {

	if (pressed(BUTTON_ESCAPE)) {
		WM_DESTROY;
		{
			running = false;
		}
	}

	if (pressed(BUTTON_ENTER)) {
		current_gamemode = GM_GAMEPLAY; {
			running = true;
		}
	}
	draw_text("PLAYER 2 WIN", -50, 20, 2, 0xffffff);
	draw_text("ENTER TO", 15, -10, 1, 0xaaaaaa);
	draw_text("RESTART", 15, -20, 1, 0xaaaaaa);
	draw_text("ESC TO EXIT", -80, -10, 1, 0xff0000);

	}
	//jika game di paused
	else if (current_gamemode == GM_PAUSED) {
	if (pressed(BUTTON_ESCAPE)) 
	{
		WM_DESTROY;
	{
		running = false;
	}
		}

	if (pressed(BUTTON_ENTER)) {
		current_gamemode = GM_GAMEPLAY;
	}
	draw_text("PAUSED", -30, 20, 2, 0xffffff);
	draw_text("ENTER TO", 20, -10, 1, 0xff0000);
	draw_text("CONTINUE", 20, -20, 1, 0xff0000);
	draw_text("ESC TO EXIT", -80, -10, 1, 0xff0000);
}
//main menu
	else if (current_gamemode == GM_MENU) {

		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}

		if (pressed(BUTTON_ESCAPE)) {
			WM_DESTROY;
			{
			running = false;
			}
		}

		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}

		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			draw_text("MULTIPLAYER", 15, -10, 1, 0xaaaaaa);
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 15, -10, 1, 0xff0000);
		}

		draw_text("PING PONG", -50, 20, 2, 0xffffff);
	}
}