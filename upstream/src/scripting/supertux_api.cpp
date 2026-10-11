//  SuperTux
//  Copyright (C) 2026 SuperTux2-Wii contributors
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "scripting/supertux_api.hpp"

#include <simplesquirrel/class.hpp>
#include <simplesquirrel/table.hpp>

#include "object/anchor_point.hpp"
#include "scripting/ambient_sound.hpp"
#include "scripting/background.hpp"
#include "scripting/camera.hpp"
#include "scripting/candle.hpp"
#include "scripting/display_effect.hpp"
#include "scripting/floating_image.hpp"
#include "scripting/functions.hpp"
#include "scripting/gradient.hpp"
#include "scripting/level.hpp"
#include "scripting/level_time.hpp"
#include "scripting/particlesystem.hpp"
#include "scripting/platform.hpp"
#include "scripting/player.hpp"
#include "scripting/scripted_object.hpp"
#include "scripting/sector.hpp"
#include "scripting/text.hpp"
#include "scripting/thunderstorm.hpp"
#include "scripting/tilemap.hpp"
#include "scripting/willowisp.hpp"
#include "scripting/wind.hpp"

namespace scripting {

namespace {

/* simplesquirrel registers into a table it holds; this hands it ours without
   giving it the VM, which stays with Scripting. */
ssq::Table root_table(HSQUIRRELVM v)
{
  ssq::Object root(v);
  sq_pushroottable(v);
  sq_getstackobj(v, -1, &root.getRaw());
  sq_addref(v, &root.getRaw());
  sq_pop(v, 1);
  return ssq::Table(root);
}

} // namespace

void
register_supertux_api(HSQUIRRELVM v)
{
  ssq::Table root = root_table(v);

  /* Constants */
  root.set<int>("ANCHOR_TOP", ANCHOR_TOP);
  root.set<int>("ANCHOR_BOTTOM", ANCHOR_BOTTOM);
  root.set<int>("ANCHOR_LEFT", ANCHOR_LEFT);
  root.set<int>("ANCHOR_RIGHT", ANCHOR_RIGHT);
  root.set<int>("ANCHOR_MIDDLE", ANCHOR_MIDDLE);
  root.set<int>("ANCHOR_TOP_LEFT", ANCHOR_TOP_LEFT);
  root.set<int>("ANCHOR_TOP_RIGHT", ANCHOR_TOP_RIGHT);
  root.set<int>("ANCHOR_BOTTOM_LEFT", ANCHOR_BOTTOM_LEFT);
  root.set<int>("ANCHOR_BOTTOM_RIGHT", ANCHOR_BOTTOM_RIGHT);

  /* Global functions */
  root.addFunc("display", &display);
  root.addFunc("print_stacktrace", &print_stacktrace);
  root.addFunc("get_current_thread", &get_current_thread);
  root.addFunc("is_christmas", &is_christmas);
  root.addFunc("display_text_file", &display_text_file);
  root.addFunc("load_worldmap", &load_worldmap);
  root.addFunc("load_level", &load_level);
  root.addFunc("wait", &wait);
  root.addFunc("wait_for_screenswitch", &wait_for_screenswitch);
  root.addFunc("exit_screen", &exit_screen);
  root.addFunc("fadeout_screen", &fadeout_screen);
  root.addFunc("shrink_screen", &shrink_screen);
  root.addFunc("abort_screenfade", &abort_screenfade);
  root.addFunc("import", &import);
  root.addFunc("save_state", &save_state);
  root.addFunc("load_state", &load_state);
  root.addFunc("debug_collrects", &debug_collrects);
  root.addFunc("debug_show_fps", &debug_show_fps);
  root.addFunc("debug_draw_solids_only", &debug_draw_solids_only);
  root.addFunc("debug_worldmap_ghost", &debug_worldmap_ghost);
  root.addFunc("play_music", &play_music);
  root.addFunc("play_sound", &play_sound);
  root.addFunc("set_game_speed", &set_game_speed);
  root.addFunc("grease", &grease);
  root.addFunc("invincible", &invincible);
  root.addFunc("ghost", &ghost);
  root.addFunc("mortal", &mortal);
  root.addFunc("restart", &restart);
  root.addFunc("whereami", &whereami);
  root.addFunc("gotoend", &gotoend);
  root.addFunc("warp", &warp);
  root.addFunc("camera", &camera);
  root.addFunc("set_gamma", &set_gamma);
  root.addFunc("quit", &quit);
  root.addFunc("rand", &rand);
  root.addFunc("record_demo", &record_demo);
  root.addFunc("play_demo", &play_demo);
  root.addFunc("Level_finish", &Level_finish);
  root.addFunc("Level_spawn", &Level_spawn);
  root.addFunc("Level_flip_vertically", &Level_flip_vertically);
  root.addFunc("Level_toggle_pause", &Level_toggle_pause);

  /* Classes */

  ssq::Class ambient_sound = root.addAbstractClass<AmbientSound>("AmbientSound");
  ambient_sound.addFunc("set_pos", &AmbientSound::set_pos);
  ambient_sound.addFunc("get_pos_x", &AmbientSound::get_pos_x);
  ambient_sound.addFunc("get_pos_y", &AmbientSound::get_pos_y);

  ssq::Class background = root.addAbstractClass<Background>("Background");
  background.addFunc("set_image", &Background::set_image);
  background.addFunc("set_images", &Background::set_images);
  background.addFunc("set_speed", &Background::set_speed);

  ssq::Class camera = root.addAbstractClass<Camera>("Camera");
  camera.addFunc("reload_config", &Camera::reload_config);
  camera.addFunc("shake", &Camera::shake);
  camera.addFunc("set_pos", &Camera::set_pos);
  camera.addFunc("set_mode", &Camera::set_mode);
  camera.addFunc("scroll_to", &Camera::scroll_to);

  ssq::Class candle = root.addAbstractClass<Candle>("Candle");
  candle.addFunc("get_burning", &Candle::get_burning);
  candle.addFunc("set_burning", &Candle::set_burning);

  ssq::Class display_effect = root.addAbstractClass<DisplayEffect>("DisplayEffect");
  display_effect.addFunc("fade_out", &DisplayEffect::fade_out);
  display_effect.addFunc("fade_in", &DisplayEffect::fade_in);
  display_effect.addFunc("set_black", &DisplayEffect::set_black);
  display_effect.addFunc("is_black", &DisplayEffect::is_black);
  display_effect.addFunc("sixteen_to_nine", &DisplayEffect::sixteen_to_nine);
  display_effect.addFunc("four_to_three", &DisplayEffect::four_to_three);

  ssq::Class floating_image = root.addClass<FloatingImage>("FloatingImage",
      ssq::Class::Ctor<FloatingImage(const std::string& spritefile)>());
  floating_image.addFunc("set_layer", &FloatingImage::set_layer);
  floating_image.addFunc("get_layer", &FloatingImage::get_layer);
  floating_image.addFunc("set_pos", &FloatingImage::set_pos);
  floating_image.addFunc("get_pos_x", &FloatingImage::get_pos_x);
  floating_image.addFunc("get_pos_y", &FloatingImage::get_pos_y);
  floating_image.addFunc("set_anchor_point", &FloatingImage::set_anchor_point);
  floating_image.addFunc("get_anchor_point", &FloatingImage::get_anchor_point);
  floating_image.addFunc("set_visible", &FloatingImage::set_visible);
  floating_image.addFunc("get_visible", &FloatingImage::get_visible);
  floating_image.addFunc("set_action", &FloatingImage::set_action);
  floating_image.addFunc("get_action", &FloatingImage::get_action);
  floating_image.addFunc("fade_in", &FloatingImage::fade_in);
  floating_image.addFunc("fade_out", &FloatingImage::fade_out);

  ssq::Class gradient = root.addAbstractClass<Gradient>("Gradient");
  gradient.addFunc("set_direction", &Gradient::set_direction);
  gradient.addFunc("get_direction", &Gradient::get_direction);
  gradient.addFunc("set_color1", &Gradient::set_color1);
  gradient.addFunc("set_color2", &Gradient::set_color2);
  gradient.addFunc("swap_colors", &Gradient::swap_colors);

  ssq::Class level_time = root.addAbstractClass<LevelTime>("LevelTime");
  level_time.addFunc("start", &LevelTime::start);
  level_time.addFunc("stop", &LevelTime::stop);
  level_time.addFunc("get_time", &LevelTime::get_time);
  level_time.addFunc("set_time", &LevelTime::set_time);

  ssq::Class particle_system = root.addAbstractClass<ParticleSystem>("ParticleSystem");
  particle_system.addFunc("set_enabled", &ParticleSystem::set_enabled);
  particle_system.addFunc("get_enabled", &ParticleSystem::get_enabled);

  ssq::Class platform = root.addAbstractClass<Platform>("Platform");
  platform.addFunc("goto_node", &Platform::goto_node);
  platform.addFunc("start_moving", &Platform::start_moving);
  platform.addFunc("stop_moving", &Platform::stop_moving);

  ssq::Class player = root.addAbstractClass<Player>("Player");
  player.addFunc("add_bonus", &Player::add_bonus);
  player.addFunc("set_bonus", &Player::set_bonus);
  player.addFunc("add_coins", &Player::add_coins);
  player.addFunc("make_invincible", &Player::make_invincible);
  player.addFunc("deactivate", &Player::deactivate);
  player.addFunc("activate", &Player::activate);
  player.addFunc("walk", &Player::walk);
  player.addFunc("set_dir", &Player::set_dir);
  player.addFunc("set_visible", &Player::set_visible);
  player.addFunc("get_visible", &Player::get_visible);
  player.addFunc("kill", &Player::kill);
  player.addFunc("set_ghost_mode", &Player::set_ghost_mode);
  player.addFunc("get_ghost_mode", &Player::get_ghost_mode);
  player.addFunc("kick", &Player::kick);
  player.addFunc("do_cheer", &Player::do_cheer);
  player.addFunc("do_duck", &Player::do_duck);
  player.addFunc("do_standup", &Player::do_standup);
  player.addFunc("do_backflip", &Player::do_backflip);
  player.addFunc("do_jump", &Player::do_jump);
  player.addFunc("trigger_sequence", &Player::trigger_sequence);
  player.addFunc("use_scripting_controller", &Player::use_scripting_controller);
  player.addFunc("do_scripting_controller", &Player::do_scripting_controller);
  player.addFunc("get_velocity_x", &Player::get_velocity_x);
  player.addFunc("get_velocity_y", &Player::get_velocity_y);

  ssq::Class scripted_object = root.addAbstractClass<ScriptedObject>("ScriptedObject");
  scripted_object.addFunc("set_action", &ScriptedObject::set_action);
  scripted_object.addFunc("get_action", &ScriptedObject::get_action);
  scripted_object.addFunc("move", &ScriptedObject::move);
  scripted_object.addFunc("set_pos", &ScriptedObject::set_pos);
  scripted_object.addFunc("get_pos_x", &ScriptedObject::get_pos_x);
  scripted_object.addFunc("get_pos_y", &ScriptedObject::get_pos_y);
  scripted_object.addFunc("set_velocity", &ScriptedObject::set_velocity);
  scripted_object.addFunc("get_velocity_x", &ScriptedObject::get_velocity_x);
  scripted_object.addFunc("get_velocity_y", &ScriptedObject::get_velocity_y);
  scripted_object.addFunc("enable_gravity", &ScriptedObject::enable_gravity);
  scripted_object.addFunc("gravity_enabled", &ScriptedObject::gravity_enabled);
  scripted_object.addFunc("set_visible", &ScriptedObject::set_visible);
  scripted_object.addFunc("is_visible", &ScriptedObject::is_visible);
  scripted_object.addFunc("set_solid", &ScriptedObject::set_solid);
  scripted_object.addFunc("is_solid", &ScriptedObject::is_solid);
  scripted_object.addFunc("get_name", &ScriptedObject::get_name);

  ssq::Class sector = root.addAbstractClass<Sector>("Sector");
  sector.addFunc("set_ambient_light", &Sector::set_ambient_light);
  sector.addFunc("get_ambient_red", &Sector::get_ambient_red);
  sector.addFunc("get_ambient_green", &Sector::get_ambient_green);
  sector.addFunc("get_ambient_blue", &Sector::get_ambient_blue);
  sector.addFunc("set_gravity", &Sector::set_gravity);
  sector.addFunc("set_music", &Sector::set_music);

  ssq::Class text = root.addAbstractClass<Text>("Text");
  text.addFunc("set_text", &Text::set_text);
  text.addFunc("set_font", &Text::set_font);
  text.addFunc("fade_in", &Text::fade_in);
  text.addFunc("fade_out", &Text::fade_out);
  text.addFunc("set_visible", &Text::set_visible);
  text.addFunc("set_centered", &Text::set_centered);
  text.addFunc("set_pos", &Text::set_pos);
  text.addFunc("get_pos_x", &Text::get_pos_x);
  text.addFunc("get_pos_y", &Text::get_pos_y);
  text.addFunc("set_anchor_point", &Text::set_anchor_point);
  text.addFunc("get_anchor_point", &Text::get_anchor_point);

  ssq::Class thunderstorm = root.addAbstractClass<Thunderstorm>("Thunderstorm");
  thunderstorm.addFunc("start", &Thunderstorm::start);
  thunderstorm.addFunc("stop", &Thunderstorm::stop);
  thunderstorm.addFunc("thunder", &Thunderstorm::thunder);
  thunderstorm.addFunc("lightning", &Thunderstorm::lightning);
  thunderstorm.addFunc("flash", &Thunderstorm::flash);
  thunderstorm.addFunc("electrify", &Thunderstorm::electrify);

  ssq::Class tile_map = root.addAbstractClass<TileMap>("TileMap");
  tile_map.addFunc("goto_node", &TileMap::goto_node);
  tile_map.addFunc("start_moving", &TileMap::start_moving);
  tile_map.addFunc("stop_moving", &TileMap::stop_moving);
  tile_map.addFunc("get_tile_id", &TileMap::get_tile_id);
  tile_map.addFunc("get_tile_id_at", &TileMap::get_tile_id_at);
  tile_map.addFunc("change", &TileMap::change);
  tile_map.addFunc("change_at", &TileMap::change_at);
  tile_map.addFunc("fade", &TileMap::fade);
  tile_map.addFunc("tint_fade", &TileMap::tint_fade);
  tile_map.addFunc("set_alpha", &TileMap::set_alpha);
  tile_map.addFunc("get_alpha", &TileMap::get_alpha);

  ssq::Class will_o_wisp = root.addAbstractClass<WillOWisp>("WillOWisp");
  will_o_wisp.addFunc("goto_node", &WillOWisp::goto_node);
  will_o_wisp.addFunc("set_state", &WillOWisp::set_state);
  will_o_wisp.addFunc("start_moving", &WillOWisp::start_moving);
  will_o_wisp.addFunc("stop_moving", &WillOWisp::stop_moving);

  ssq::Class wind = root.addAbstractClass<Wind>("Wind");
  wind.addFunc("start", &Wind::start);
  wind.addFunc("stop", &Wind::stop);
}

} // namespace scripting

/* EOF */
