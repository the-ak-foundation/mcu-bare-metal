#include "scr_zomwar_game.h"

/*****************************************************************************/
/* Variable Declaration - Zomwar game screen */
/*****************************************************************************/
#define GAME_OFF  (0)
#define GAME_PLAY (1)
#define GAME_OVER (2)

#define NUM_BATS 2

static uint8_t zw_game_state;
zw_game_setting_t settingsetup;

static struct {
	int16_t x;
	uint8_t y;
	int8_t  dir;
	uint8_t frame;
	uint8_t frame_tick;
} bat_anim[NUM_BATS];

/*****************************************************************************/
/* View - Zomwar game screen*/
/*****************************************************************************/
void zw_game_frame_display() {
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(8,55);
	view_render.print("Wave:");
	view_render.print(wave_level);
	view_render.setCursor(64,55);
	view_render.print("Score:");
	view_render.print(zw_game_score);
	view_render.drawLine(0, LCD_HEIGHT, 	LCD_WIDTH, LCD_HEIGHT,		WHITE);
	view_render.drawLine(0, LCD_HEIGHT-11, 	LCD_WIDTH, LCD_HEIGHT-11,	WHITE);
	view_render.drawRect(0, 0, 128, 64, 1);
}

void zw_game_gunner_display() {
	if (gunner.visible == WHITE) {
		const unsigned char* frame = (gunner.action_image == 2) ? bitmap_gunner_II : bitmap_gunner_I;
		view_render.drawBitmap( gunner.x, \
								gunner.y - 10, \
								frame, \
								SIZE_BITMAP_GUNNER_X, \
								SIZE_BITMAP_GUNNER_Y, \
								WHITE);
	}
}

void zw_game_bullet_display() {
	for (uint8_t i = 0; i < MAX_NUM_BULLET; i++) {
		if (bullet[i].visible == WHITE) {
			view_render.drawBitmap(	bullet[i].x, \
									bullet[i].y, \
									bitmap_bullet, \
									SIZE_BITMAP_BULLET_X, \
									SIZE_BITMAP_BULLET_Y, \
									WHITE);
		}
	}
}

void zw_game_zombie_display() {
	for (uint8_t i = 0; i < NUM_ZOMBIES; i++) {
		if (zombie[i].visible == WHITE) {
			if (zombie[i].action_image == 1) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_I, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
			else if (zombie[i].action_image == 2) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_II, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
			else if (zombie[i].action_image == 3) {
				view_render.drawBitmap(	zombie[i].x, \
										zombie[i].y, \
										bitmap_zombie_III, \
										SIZE_BITMAP_ZOMBIES_X, \
										SIZE_BITMAP_ZOMBIES_Y, \
										WHITE);
			}
		}
	}
}

void zw_game_bang_display() {
	for (uint8_t i = 0; i < NUM_BANG; i++) {
		if (bang[i].visible == WHITE) {
			if (bang[i].action_image == 1) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_I, \
										SIZE_BITMAP_BANG_I_X, \
										SIZE_BITMAP_BANG_I_Y, \
										WHITE);
			}
			else if (bang[i].action_image == 2) {
				view_render.drawBitmap(	bang[i].x, \
										bang[i].y, \
										bitmap_bang_II, \
										SIZE_BITMAP_BANG_II_X, \
										SIZE_BITMAP_BANG_II_Y, \
										WHITE);
			}
			else if (bang[i].action_image == 3) {
				view_render.drawBitmap( bang[i].x + 2, \
										bang[i].y - 1, \
										bitmap_bang_III, \
										SIZE_BITMAP_BANG_II_X, \
										SIZE_BITMAP_BANG_II_Y, \
										WHITE);
			}
		}
	}
}

void zw_game_car_display() {
	for (uint8_t i = 0; i < NUM_LANES; i++) {
		if (car[i].visible) {
			if (car[i].action_image == 1) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_I, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
			else if (car[i].action_image == 2) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_II, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
			else if (car[i].action_image == 3) {
				view_render.drawBitmap( car[i].x, \
										car[i].y, \
										bitmap_car_III, \
										SIZE_BITMAP_CAR_X, \
										SIZE_BITMAP_CAR_Y, \
										WHITE);
			}
		}
	}
}

void zw_game_grass_display() {
	static const uint8_t ly[NUM_LANES] = LANE_Y;
	for (uint8_t l = 0; l < NUM_LANES; l++) {
		uint8_t gy = ly[l] + 9;
		for (uint8_t x = 0; x < 128; x += 5) {
			view_render.drawPixel(x,     gy,     WHITE);
			view_render.drawPixel(x + 1, gy,     WHITE);
			view_render.drawPixel(x + 2, gy,     WHITE);
			//view_render.drawPixel(x + 2, gy - 1, WHITE);
			//view_render.drawPixel(x + 3, gy - 1, WHITE);
		}
	}
}

void zw_game_tombstone_display() {
	static const uint8_t ly[NUM_LANES] = LANE_Y;
	for (uint8_t i = 0; i < NUM_TOMBSTONES; i++) {
		if (!tombstones[i].active) continue;
		view_render.drawBitmap(
			tombstones[i].x,
			ly[tombstones[i].lane],
			bitmap_tombstone,
			SIZE_BITMAP_TOMBSTONE_X,
			SIZE_BITMAP_TOMBSTONE_Y,
			WHITE);
	}
}

static void zw_game_bat_init() {
	bat_anim[0].dir        = (zw_game_score & 1) ? 1 : -1;
	bat_anim[0].y          = 4 + (uint8_t)(zw_game_score % 18);
	bat_anim[0].x          = (bat_anim[0].dir == 1) ? -16 : 128;
	bat_anim[0].frame      = 1;
	bat_anim[0].frame_tick = 0;

	bat_anim[1].dir        = -bat_anim[0].dir;
	bat_anim[1].y          = bat_anim[0].y + 20;
	bat_anim[1].x          = (bat_anim[1].dir == 1) ? -16 : 128;
	bat_anim[1].frame      = 2;
	bat_anim[1].frame_tick = 1;
}

static void zw_game_bat_update() {
	for (uint8_t i = 0; i < NUM_BATS; i++) {
		bat_anim[i].x += bat_anim[i].dir * 2;
		if (++bat_anim[i].frame_tick >= 3) {
			bat_anim[i].frame_tick = 0;
			bat_anim[i].frame      = (bat_anim[i].frame % 3) + 1;
		}
		if (bat_anim[i].dir == 1 && bat_anim[i].x >= 128) {
			bat_anim[i].x = -16;
		} else if (bat_anim[i].dir == -1 && bat_anim[i].x <= -16) {
			bat_anim[i].x = 128;
		}
	}
}

static void zw_game_bat_display() {
	const unsigned char* frames[3] = {bitmap_bat_I, bitmap_bat_II, bitmap_bat_III};
	for (uint8_t i = 0; i < NUM_BATS; i++) {
		view_render.drawBitmap(bat_anim[i].x, bat_anim[i].y,
							   frames[bat_anim[i].frame - 1],
							   16, 16, WHITE);
	}
}

void zw_game_warning_display() {
	if (wave_warning_active) {

		if ((wave_warning_timer / WARNING_BLINK_RATE) % 2 == 0) {

			view_render.drawBitmap(
				(LCD_WIDTH - SIZE_BITMAP_WARNING_X) / 2,
				(54 - SIZE_BITMAP_WARNING_Y) / 2,
				bitmap_warning,
				SIZE_BITMAP_WARNING_X,
				SIZE_BITMAP_WARNING_Y,
				WHITE);
		}
	}
}

static void view_scr_zomwar_game();

view_dynamic_t dyn_view_item_zomwar_game = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_zomwar_game
};

view_screen_t scr_zomwar_game = {
	&dyn_view_item_zomwar_game,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_zomwar_game() {
	if (zw_game_state == GAME_PLAY) {
		zw_game_frame_display();
		zw_game_grass_display();
		zw_game_tombstone_display();
		zw_game_gunner_display();
		zw_game_bullet_display();
		zw_game_zombie_display();
		zw_game_bang_display();
		zw_game_car_display();
		zw_game_warning_display();
	}
	else if (zw_game_state == GAME_OVER) {
		view_render.clear();
		view_render.drawBitmap(15, 4, bitmap_rip, 100, 54, WHITE);
		view_render.drawBitmap(0, 48, bitmap_spiderweb_BL, 16, 16, WHITE);
		view_render.drawBitmap(110, 48, bitmap_spiderweb_BR, 16, 16, WHITE);
		view_render.drawBitmap(85, 8, bitmap_branch_R, 50, 17, WHITE);
		view_render.drawBitmap(-2, 4, bitmap_branch_L, 50, 17, WHITE);

		zw_game_bat_display();
	}
}

/*****************************************************************************/
/* Handle - Zomwar game screen */
/*****************************************************************************/
static void zw_game_input_handle(uint8_t sig) {
	switch (sig) {
	case ZW_GAME_BTN_UP_PRESSED:
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_UP);
		break;
	case ZW_GAME_BTN_UP_RELEASED:
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
		break;
	case ZW_GAME_BTN_DOWN_PRESSED:
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_DOWN);
		break;
	case ZW_GAME_BTN_DOWN_RELEASED:
		task_post_pure_msg(ZW_GAME_GUNNER_ID, ZW_GAME_GUNNER_STOP);
		break;
	case ZW_GAME_BTN_MODE_RELEASED:
		task_post_pure_msg(ZW_GAME_BULLET_ID, ZW_GAME_BULLET_SHOOT);
		break;
	default:
		break;
	}
}

void zw_game_level_setup() {
	zw_game_setting_read(&settingdata);
	settingsetup = settingdata;
}

void zw_game_time_tick_setup() {
	timer_set(	AC_TASK_DISPLAY_ID, \
				ZW_GAME_TIME_TICK, \
				ZW_GAME_TIME_TICK_INTERVAL, \
				TIMER_PERIODIC);
}

void zw_game_save_and_reset_score() {
	eeprom_write(	EEPROM_SCORE_PLAY_ADDR, \
					(uint8_t*)&zw_game_score, \
					sizeof(zw_game_score));
}

void scr_zw_game_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");

		zw_game_level_setup();

		task_post_pure_msg(ZW_GAME_GUNNER_ID, 		ZW_GAME_GUNNER_SETUP);
		task_post_pure_msg(ZW_GAME_BULLET_ID, 	 	ZW_GAME_BULLET_SETUP);
		task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_SETUP);
		task_post_pure_msg(ZW_GAME_CAR_ID, 			ZW_GAME_CAR_SETUP);
		task_post_pure_msg(ZW_GAME_BANG_ID, 	 	ZW_GAME_BANG_SETUP);
		task_post_pure_msg(ZW_GAME_BORDER_ID, 	 	ZW_GAME_BORDER_SETUP);

		zw_game_time_tick_setup();

		zw_game_state = GAME_PLAY;
		zw_game_register_input(zw_game_input_handle);
	}
		break;

	case ZW_GAME_TIME_TICK: {
		APP_DBG_SIG("ZW_GAME_TIME_TICK\n");
		if (zw_game_state == GAME_PLAY) {
			task_post_pure_msg(ZW_GAME_GUNNER_ID, 		ZW_GAME_GUNNER_UPDATE);
			task_post_pure_msg(ZW_GAME_BULLET_ID, 		ZW_GAME_BULLET_RUN);
			task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_RUN);
			task_post_pure_msg(ZW_GAME_ZOMBIE_ID, 		ZW_GAME_ZOMBIE_DETONATOR);
			task_post_pure_msg(ZW_GAME_CAR_ID,  		ZW_GAME_CAR_RUN);
			task_post_pure_msg(ZW_GAME_BANG_ID, 		ZW_GAME_BANG_UPDATE);
			task_post_pure_msg(ZW_GAME_BORDER_ID, 		ZW_GAME_CHECK_GAME_OVER);
		}
		else if (zw_game_state == GAME_OVER) {
			zw_game_bat_update();
		}
	}
		break;

	/* BTN signals chỉ đến đây khi handler không được đăng ký (GAME_OFF) */
	case ZW_GAME_BTN_MODE_RELEASED:
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_MODE_RELEASED);
		break;

	case ZW_GAME_BTN_UP_RELEASED:
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_UP_RELEASED);
		break;

	case ZW_GAME_BTN_DOWN_RELEASED:
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_BUTTON_DOWN_RELEASED);
		break;

	case ZW_GAME_RESET: {
		APP_DBG_SIG("ZW_GAME_RESET\n");
		if (zw_game_state != GAME_PLAY) break;

		task_post_pure_msg(ZW_GAME_GUNNER_ID, 		ZW_GAME_GUNNER_RESET);
		task_post_pure_msg(ZW_GAME_BULLET_ID, 		ZW_GAME_BULLET_RESET);
		task_post_pure_msg(ZW_GAME_ZOMBIE_ID,		ZW_GAME_ZOMBIE_RESET);
		task_post_pure_msg(ZW_GAME_CAR_ID,  		ZW_GAME_CAR_RESET);
		task_post_pure_msg(ZW_GAME_BANG_ID, 		ZW_GAME_BANG_RESET);
		task_post_pure_msg(ZW_GAME_BORDER_ID, 		ZW_GAME_BORDER_RESET);

		timer_set(	AC_TASK_DISPLAY_ID, \
					ZW_GAME_EXIT_GAME, \
					ZW_GAME_TIME_EXIT_INTERVAL, \
					TIMER_ONE_SHOT);

		zw_game_save_and_reset_score();

		zw_game_bat_init();

		zw_game_state = GAME_OVER;
	}
		BUZZER_PlayTones(tones_3beep);
		break;

	case ZW_GAME_EXIT_GAME: {
		APP_DBG_SIG("ZW_GAME_EXIT_GAME\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, ZW_GAME_TIME_TICK);
		zw_game_unregister_input();
		zw_game_state = GAME_OFF;
		SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
	}
		break;

	default:
		break;
	}
}
