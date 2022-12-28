// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: audio_mixer

#ifndef _AUDIO_MIXER_UI_H
#define _AUDIO_MIXER_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void Fadeoff_Animation(lv_obj_t * TargetObject, int delay);
void Fadeon_Animation(lv_obj_t * TargetObject, int delay);
extern lv_obj_t * ui_Audio_Player;
extern lv_obj_t * ui_Audio_Player_Panel3;
void ui_event_Audio_Player_Slider1(lv_event_t * e);
extern lv_obj_t * ui_Audio_Player_Slider1;

void SliderMove(lv_event_t * e);



LV_FONT_DECLARE(ui_font_14_bold);
LV_FONT_DECLARE(ui_font_mos_semibold_16);
LV_FONT_DECLARE(ui_font_number);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
