/*__            ___                 ***************************************
/   \          /   \          Copyright (c) 1996-2020 Freeciv21 and Freeciv
\_   \        /  __/          contributors. This file is part of Freeciv21.
 _\   \      /  /__     Freeciv21 is free software: you can redistribute it
 \___  \____/   __/    and/or modify it under the terms of the GNU  General
     \_       _/          Public License  as published by the Free Software
       | @ @  \_               Foundation, either version 3 of the  License,
       |                              or (at your option) any later version.
     _/     /\                  You should have received  a copy of the GNU
    /o)  (o/\ \_                General Public License along with Freeciv21.
    \_____/ /                     If not, see https://www.gnu.org/licenses/.
      \____/        ********************************************************/

#ifdef HAVE_CONFIG_H
#include <fc_config.h>
#endif

// client
#include "client_main.h"
#include "editgui_g.h"
#include "options.h"

#include "chatline_g.h"
#include "citydlg_g.h"
#include "connectdlg_g.h"
#include "dialogs_g.h"
#include "editgui_g.h"
#include "graphics_g.h"
#include "gui_main_g.h"
#include "mapview_g.h"
#include "themes_g.h"

#include "gui_interface.h"

static struct gui_funcs funcs;

/**
   Return gui_funcs table. Used by gui side to get table for filling
   with function addresses.
 */
struct gui_funcs *get_gui_funcs() { return &funcs; }

/**
   Call ui_init callback
 */
void ui_init(void) { funcs.ui_init(); }

/**
   Call ui_main callback
 */
void ui_main() { funcs.ui_main(); }

/**
   Call ui_exit callback
 */
void ui_exit(void) { funcs.ui_exit(); }

/**
   Call version_message callback
 */
void version_message(const char *vertext) { funcs.version_message(vertext); }

/**
   Call real_output_window_append callback
 */
void real_output_window_append(const QString &astring,
                               const struct text_tag_list *tags, int conn_id)
{
  funcs.real_output_window_append(astring, tags, conn_id);
}

/**
   Call get_gui_type callback
 */
enum gui_type get_gui_type(void) { return funcs.get_gui_type(); }

/**
   Call insert_client_build_info callback
 */
void insert_client_build_info(char *outbuf, size_t outlen)
{
  funcs.insert_client_build_info(outbuf, outlen);
}

/**
   Call is_view_supported callback
 */
bool is_view_supported(enum ts_type type)
{
  return funcs.is_view_supported(type);
}

/**
   Call tileset_type_set callback
 */
void tileset_type_set(enum ts_type type) { funcs.tileset_type_set(type); }

/**
   Call free_intro_radar_sprites callback
 */
void free_intro_radar_sprites(void) { funcs.free_intro_radar_sprites(); }

/**
   Call load_gfxfile callback
 */
QPixmap *load_gfxfile(const char *filename)
{
  return funcs.load_gfxfile(filename);
}

/**
   Call create_sprite callback
 */
QPixmap *create_sprite(int width, int height, const QColor *pcolor)
{
  return funcs.create_sprite(width, height, pcolor);
}

/**
   Call get_sprite_dimensions callback
 */
void get_sprite_dimensions(const QPixmap *sprite, int *width, int *height)
{
  funcs.get_sprite_dimensions(sprite, width, height);
}

/**
   Call crop_sprite callback
 */
QPixmap *crop_sprite(const QPixmap *source, int x, int y, int width,
                     int height, const QPixmap *mask, int mask_offset_x,
                     int mask_offset_y, float scale, bool smooth)
{
  return funcs.crop_sprite(source, x, y, width, height, mask, mask_offset_x,
                           mask_offset_y, scale, smooth);
}

/**
   Call free_sprite callback
 */
void free_sprite(QPixmap *s) { funcs.free_sprite(s); }

/**
   Call color_alloc callback
 */
QColor *color_alloc(int r, int g, int b)
{
  return funcs.color_alloc(r, g, b);
}

/**
   Call color_free callback
 */
void color_free(QColor *pcolor) { return funcs.color_free(pcolor); }

/**
   Call canvas_create callback
 */
QPixmap *canvas_create(int width, int height)
{
  return funcs.canvas_create(width, height);
}

/**
   Call canvas_free callback
 */
void canvas_free(QPixmap *store) { funcs.canvas_free(store); }

/**
   Call canvas_copy callback
 */
void canvas_copy(QPixmap *dest, const QPixmap *src, int src_x, int src_y,
                 int dest_x, int dest_y, int width, int height)
{
  funcs.canvas_copy(dest, src, src_x, src_y, dest_x, dest_y, width, height);
}

/**
   Call canvas_put_sprite callback
 */
void canvas_put_sprite(QPixmap *pcanvas, int canvas_x, int canvas_y,
                       const QPixmap *psprite, int offset_x, int offset_y,
                       int width, int height)
{
  funcs.canvas_put_sprite(pcanvas, canvas_x, canvas_y, psprite, offset_x,
                          offset_y, width, height);
}

/**
   Call canvas_put_sprite_full callback
 */
void canvas_put_sprite_full(QPixmap *pcanvas, int canvas_x, int canvas_y,
                            const QPixmap *psprite)
{
  funcs.canvas_put_sprite_full(pcanvas, canvas_x, canvas_y, psprite);
}

/**
   Call canvas_put_sprite_fogged callback
 */
void canvas_put_sprite_fogged(QPixmap *pcanvas, int canvas_x, int canvas_y,
                              const QPixmap *psprite, bool fog, int fog_x,
                              int fog_y)
{
  funcs.canvas_put_sprite_fogged(pcanvas, canvas_x, canvas_y, psprite, fog,
                                 fog_x, fog_y);
}

void canvas_put_sprite_citymode(QPixmap *pcanvas, int canvas_x, int canvas_y,
                                const QPixmap *psprite, bool fog, int fog_x,
                                int fog_y)
{
  funcs.canvas_put_sprite_citymode(pcanvas, canvas_x, canvas_y, psprite, fog,
                                   fog_x, fog_y);
}

/**
   Call canvas_put_rectangle callback
 */
void canvas_put_rectangle(QPixmap *pcanvas, const QColor *pcolor,
                          int canvas_x, int canvas_y, int width, int height)
{
  funcs.canvas_put_rectangle(pcanvas, pcolor, canvas_x, canvas_y, width,
                             height);
}

/**
   Call canvas_fill_sprite_area callback
 */
void canvas_fill_sprite_area(QPixmap *pcanvas, QPixmap *psprite,
                             const QColor *pcolor, int canvas_x,
                             int canvas_y)
{
  funcs.canvas_fill_sprite_area(pcanvas, psprite, pcolor, canvas_x,
                                canvas_y);
}

/**
   Call canvas_put_line callback
 */
void canvas_put_line(QPixmap *pcanvas, const QColor *pcolor,
                     enum line_type ltype, int start_x, int start_y, int dx,
                     int dy)
{
  funcs.canvas_put_line(pcanvas, pcolor, ltype, start_x, start_y, dx, dy);
}

/**
   Call canvas_put_curved_line callback
 */
void canvas_put_curved_line(QPixmap *pcanvas, const QColor *pcolor,
                            enum line_type ltype, int start_x, int start_y,
                            int dx, int dy)
{
  funcs.canvas_put_curved_line(pcanvas, pcolor, ltype, start_x, start_y, dx,
                               dy);
}

/**
   Call get_text_size callback
 */
void get_text_size(int *width, int *height, enum client_font font,
                   const QString &text)
{
  funcs.get_text_size(width, height, font, text);
}

/**
   Call canvas_put_text callback
 */
void canvas_put_text(QPixmap *pcanvas, int canvas_x, int canvas_y,
                     enum client_font font, const QColor *pcolor,
                     const QString &text)
{
  funcs.canvas_put_text(pcanvas, canvas_x, canvas_y, font, pcolor, text);
}

/**
   Call set_rulesets callback
 */
void set_rulesets(int num_rulesets, QStringList rulesets)
{
  funcs.set_rulesets(num_rulesets, rulesets);
}

/**
   Call options_extra_init callback
 */
void options_extra_init(void) { funcs.options_extra_init(); }

/**
   Call add_net_input callback
 */
void add_net_input(QTcpSocket *sock) { funcs.add_net_input(sock); }

/**
   Call remove_net_input callback
 */
void remove_net_input(void) { funcs.remove_net_input(); }

/**
   Call real_conn_list_dialog_update callback
 */
void real_conn_list_dialog_update(void *unused)
{
  funcs.real_conn_list_dialog_update(NULL);
}

/**
   Call close_connection_dialog callback
 */
void close_connection_dialog(void) { funcs.close_connection_dialog(); }

/**
   Call add_idle_callback callback
 */
void add_idle_callback(void(callback)(void *), void *data)
{
  funcs.add_idle_callback(callback, data);
}

/**
   Call sound_bell callback
 */
void sound_bell(void) { funcs.sound_bell(); }

/**
   Call real_set_client_page callback
 */
void real_set_client_page(enum client_pages page)
{
  funcs.real_set_client_page(page);
}

/**
   Call get_current_client_page callback
 */
enum client_pages get_current_client_page(void)
{
  return funcs.get_current_client_page();
}

/**
   Call set_unit_icon callback
 */
void set_unit_icon(int idx, struct unit *punit)
{
  funcs.set_unit_icon(idx, punit);
}

/**
   Call real_focus_units_changed callback
 */
void real_focus_units_changed(void) { funcs.real_focus_units_changed(); }

/**
   Call set_unit_icons_more_arrow callback
 */
void set_unit_icons_more_arrow(bool onoff)
{
  funcs.set_unit_icons_more_arrow(onoff);
}

/**
   Call gui_update_font callback
 */
void gui_update_font(const QString &font_name, const QString &font_value)
{
  funcs.gui_update_font(font_name, font_value);
}

/**
   Call editgui_refresh callback
 */
void editgui_refresh(void) { funcs.editgui_refresh(); }

/**
   Call editgui_notify_object_created callback
 */
void editgui_notify_object_created(int tag, int id)
{
  funcs.editgui_notify_object_created(tag, id);
}

/**
   Call editgui_notify_object_changed callback
 */
void editgui_notify_object_changed(int objtype, int object_id, bool removal)
{
  funcs.editgui_notify_object_changed(objtype, object_id, removal);
}

/**
   Call editgui_popup_properties callback
 */
void editgui_popup_properties(const struct tile_list *tiles, int objtype)
{
  funcs.editgui_popup_properties(tiles, objtype);
}

/**
   Call editgui_tileset_changed callback
 */
void editgui_tileset_changed(void) { funcs.editgui_tileset_changed(); }

/**
   Call editgui_popdown_all callback
 */
void editgui_popdown_all(void) { funcs.editgui_popdown_all(); }

/**
   Call popup_combat_info callback
 */
void popup_combat_info(int attacker_unit_id, int defender_unit_id,
                       int attacker_hp, int defender_hp,
                       bool make_att_veteran, bool make_def_veteran)
{
  funcs.popup_combat_info(attacker_unit_id, defender_unit_id, attacker_hp,
                          defender_hp, make_att_veteran, make_def_veteran);
}

/**
   Call update_timeout_label callback
 */
void update_timeout_label(void) { funcs.update_timeout_label(); }

/**
   Call start_turn callback
 */
void start_turn(void) { funcs.start_turn(); }

/**
   Call real_city_dialog_popup callback
 */
void real_city_dialog_popup(struct city *pcity)
{
  funcs.real_city_dialog_popup(pcity);
}

/**
   Call real_city_dialog_refresh callback
 */
void real_city_dialog_refresh(struct city *pcity)
{
  funcs.real_city_dialog_refresh(pcity);
}

/**
   Call popdown_city_dialog callback
 */
void popdown_city_dialog(struct city *pcity)
{
  funcs.popdown_city_dialog(pcity);
}

/**
   Call popdown_all_city_dialogs callback
 */
void popdown_all_city_dialogs(void) { funcs.popdown_all_city_dialogs(); }

/**
   Call handmade_scenario_warning callback
 */
bool handmade_scenario_warning(void)
{
  return funcs.handmade_scenario_warning();
}

/**
   Call refresh_unit_city_dialogs callback
 */
void refresh_unit_city_dialogs(struct unit *punit)
{
  funcs.refresh_unit_city_dialogs(punit);
}

/**
   Call city_dialog_is_open callback
 */
bool city_dialog_is_open(struct city *pcity)
{
  return funcs.city_dialog_is_open(pcity);
}

/**
   Call request_transport callback
 */
bool request_transport(struct unit *pcargo, struct tile *ptile)
{
  return funcs.request_transport(pcargo, ptile);
}

/**
   Call update_infra_dialog callback
 */
void update_infra_dialog(void) { funcs.update_infra_dialog(); }

/**
   Call gui_load_theme callback
 */
void gui_load_theme(QString &directory, QString &theme_name)
{
  funcs.gui_load_theme(directory, theme_name);
}

/**
   Call gui_clear_theme callback
 */
void gui_clear_theme(void) { funcs.gui_clear_theme(); }

/**
   Call get_gui_specific_themes_directories callback
 */
QStringList get_gui_specific_themes_directories(int *count)
{
  return funcs.get_gui_specific_themes_directories(count);
}

/**
   Call get_useable_themes_in_directory callback
 */
QStringList get_useable_themes_in_directory(QString &directory, int *count)
{
  return funcs.get_useable_themes_in_directory(directory, count);
}
