#include "scr_game_11m.h"

/*****************************************************************************/
/* Variable Declaration - Game 11m */
/*****************************************************************************/

static uint8_t em_game_state;

void em_game_frame_display()
{
    view_render.clear();

    // Khung thành ngoài
    view_render.fillRect(18, 0, 88, 3, WHITE);   // xà ngang ngoài
    view_render.fillRect(18, 0, 3, 30, WHITE);   // cột trái ngoài
    view_render.fillRect(103, 0, 3, 30, WHITE);  // cột phải ngoài

    // Vạch ngang sân
    view_render.fillRect(0, 28, LCD_WIDTH, 2, WHITE);

    // Vạch sân phối cảnh
    view_render.drawLine(14, 30, 2, LCD_HEIGHT - 22, WHITE);
    view_render.drawLine(110, 30, LCD_WIDTH - 3, LCD_HEIGHT - 22, WHITE);
    view_render.fillRect(2, LCD_HEIGHT - 22, LCD_WIDTH - 4, 2, WHITE);

    // Chấm pen
    view_render.fillRect(61, 45, 3, 3, WHITE);
}

/*****************************************************************************/
/* View - Game 11m */
/*****************************************************************************/

static void view_scr_game_11m();

view_dynamic_t dyn_view_11m = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_game_11m};

view_screen_t scr_game_11m = {
    &dyn_view_11m,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void view_scr_game_11m()
{
	if (em_game_state == GAME_PLAY)
	{
        em_game_frame_display();
        view_render.drawBitmap(61, 43, bitmap_ball, 7, 7, WHITE);
	}
	else if (em_game_state == GAME_OVER)
	{
		view_render.clear();
	}
}

/*****************************************************************************/
/* Handle - Game 11m */
/*****************************************************************************/

void scr_game_11m_handle(ak_msg_t* msg)
{
	switch (msg->sig)
	{
	case SCREEN_ENTRY:
	{
		APP_DBG_SIG("11M_GAME SCREEN_ENTRY\n");
        em_game_state = GAME_PLAY;
	}
	break;

	case EM_GAME_EXIT_GAME:
	{
		APP_DBG_SIG("AC_DISPLAY_GAME_11M_EXIT\n");
		em_game_state = GAME_OFF;
		//SCREEN_TRAN(scr_game_over_handle, &scr_game_over);
	}
	break;

	default:
		break;
	}
}
