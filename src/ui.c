// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: audio_mixer

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
void Fadeoff_Animation(lv_obj_t * TargetObject, int delay);
void Fadeon_Animation(lv_obj_t * TargetObject, int delay);
lv_obj_t * ui_Audio_Player;
lv_obj_t * ui_Audio_Player_Panel3;
void ui_event_Audio_Player_Slider1(lv_event_t * e);
lv_obj_t * ui_Audio_Player_Slider1;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void Fadeoff_Animation(lv_obj_t * TargetObject, int delay)
{
 
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 100);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_start(&PropertyAnimation_0);

}
void Fadeon_Animation(lv_obj_t * TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 100);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_start(&PropertyAnimation_0);

}

 

///////////////////// SCREENS ////////////////////
void ui_Audio_Player_screen_init(void)
{
    ui_Audio_Player = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Audio_Player, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Audio_Player, lv_color_hex(0x2D2F36), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Audio_Player, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Audio_Player_Panel3 = lv_obj_create(ui_Audio_Player);
    lv_obj_set_width(ui_Audio_Player_Panel3, lv_pct(100));
    lv_obj_set_height(ui_Audio_Player_Panel3, lv_pct(100));
    lv_obj_set_x(ui_Audio_Player_Panel3, 1);
    lv_obj_set_y(ui_Audio_Player_Panel3, -1);
    lv_obj_set_align(ui_Audio_Player_Panel3, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Audio_Player_Panel3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Audio_Player_Panel3, lv_color_hex(0x2551B7), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Audio_Player_Panel3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Audio_Player_Slider1 = lv_slider_create(ui_Audio_Player);
    lv_obj_set_width(ui_Audio_Player_Slider1, 79);
    lv_obj_set_height(ui_Audio_Player_Slider1, 367);
    lv_obj_set_align(ui_Audio_Player_Slider1, LV_ALIGN_CENTER);
    

    lv_obj_add_event_cb(ui_Audio_Player_Slider1, ui_event_Audio_Player_Slider1, LV_EVENT_ALL, NULL);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Audio_Player_screen_init();
    lv_disp_load_scr(ui_Audio_Player);
}
