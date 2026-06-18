#ifndef __SCR_GAME_11M_H__
#define __SCR_GAME_11M_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_bsp.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "eeprom.h"
#include "app_eeprom.h"

#include "buzzer.h"

#include "scr_idle.h"
#include "screens_bitmap.h"
#include "screens.h"

// Game state values for zw_game_state
#define GAME_OFF  (0)
#define GAME_PLAY (1)
#define GAME_OVER (2)

extern view_dynamic_t dyn_view_11m;
extern view_screen_t scr_game_11m;
extern void scr_game_11m_handle(ak_msg_t* msg);

#endif //__SCR_GAME_11M_H__