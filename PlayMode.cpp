#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "SDL_events.h"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "load_save_png.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <random>

inline void load_bullet(PPU466& ppu) {
	glm::uvec2 size;
	std::vector<glm::u8vec4> data;
	auto path = data_path("bullet.png");

	load_png(path, &size, &data, OriginLocation::LowerLeftOrigin);

	for (size_t yy = 0; yy < 8; yy++) {
		uint8_t bit0 = 0;
		uint8_t bit1 = 0;
		for (int xx = 7; xx >= 0; xx--) {
			size_t image_index = yy * size.x + xx;
			uint8_t brightness = data[image_index].x;

			bit0 <<= 1;
			bit1 <<= 1;

			if (data[image_index].w) {
				if (brightness == 0) {
					bit0 += 1;
				}
				else if (brightness < 255) {
					bit1 += 1;
				}
				else {
					bit0 += 1;
					bit1 += 1;
				}
			}
		}
		ppu.tile_table[SpriteTileIndex::bullet].bit0[yy] = bit0;
		ppu.tile_table[SpriteTileIndex::bullet].bit1[yy] = bit1;
	}

}

inline void load_other_tiles(PPU466& ppu, std::string file, uint8_t starting_index) {
	glm::uvec2 size;
	std::vector<glm::u8vec4> data;
	auto path = data_path(file);

	load_png(path, &size, &data, OriginLocation::LowerLeftOrigin);

	for (size_t y = 0; y < size.y / 8; y++) {
		for (size_t x = 0; x < size.x / 8; x++) {
			size_t tile_index = y * size.x / 8 + x + starting_index;
			for (size_t yy = 0; yy < 8; yy++) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (int xx = 7; xx >= 0; xx--) {
					size_t image_index = (y * 8 + yy) * size.x + (x * 8 + xx);
					uint8_t brightness = data[image_index].x;

					bit0 <<= 1;
					bit1 <<= 1;

					if (data[image_index].w) {
						if (brightness == 0) {
							bit0 += 1;
						}
						else if (brightness < 255) {
							bit1 += 1;
						}
						else {
							bit0 += 1;
							bit1 += 1;
						}
					}
				}
				ppu.tile_table[tile_index].bit0[yy] = bit0;
				ppu.tile_table[tile_index].bit1[yy] = bit1;
			}

		}

	}

}

PlayMode::PlayMode() {

	{
		load_bullet(ppu);
		load_other_tiles(ppu, "map.png", 0);
		load_other_tiles(ppu, "player.png", SpriteTileIndex::player[0]);
		load_other_tiles(ppu, "enemy.png", SpriteTileIndex::enemy[0]);
	}


	//ground palette
	ppu.palette_table[SpriteTileIndex::ground_palette] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0xdd, 0x99, 0x11, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//number and health palette
	ppu.palette_table[SpriteTileIndex::num_health_palette] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0xe3, 0x26, 0x36, 0xff),
		glm::u8vec4(0xff, 0xff, 0xff, 0xff),
	};
	//player palette
	ppu.palette_table[SpriteTileIndex::player_palette] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x04, 0xfb, 0xff),
		glm::u8vec4(0xe3, 0x26, 0x36, 0xff),
	};
	//enemy palette
	ppu.palette_table[SpriteTileIndex::enemy_palette] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0xfd, 0x86, 0x24, 0xff),
		glm::u8vec4(0xfe, 0xf7, 0x39, 0xff),
	};
	//bullet palette
	ppu.palette_table[SpriteTileIndex::bullet_palette] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0xdd, 0x99, 0x11, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (timer == 0.0f || player_1.health == 0) {
		return false;
	}

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_RIGHT) {
		player_1.rmb_down = 1;

	}
	else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_RIGHT) {
		if (player_1.rmb_down == 1) {
			player_1.jiggle_timer = 0.3f;
		}
		player_1.rmb_down = 0;
		
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//check if game is over
	if ((timer <= 0 || player_1.health <= 0) && enemy_1.bullet_tile_pos.y >= PPU466::ScreenHeight) {
		if (player_1.health > 0) {
			player_1.status = 0;
			enemy_1.status = 2;
		}
		else {
			player_1.status = 3;
		}
		return;
	}

	timer = glm::max(0.0f, timer - elapsed);
	static std::mt19937 mt;
	//update player status

	if (player_1.attacked == 1) {
		if (player_1.guard == 1) {
			player_1.status = 2;
			player_1.score += 1;
			player_1.attacked = 0;
			enemy_1.bullet_speed = 0.0f;
			enemy_1.bullet_tile_pos.y = PPU466::ScreenHeight;
		}
		else {
			if (player_1.attacked_timer >= 0.0f) {
				player_1.attacked_timer -= elapsed;
				player_1.status = 3;
				enemy_1.bullet_speed = 0.0f;
				enemy_1.bullet_tile_pos.y = PPU466::ScreenHeight;
			}
			else {
				player_1.attacked_timer = 0.1f;
				player_1.attacked = 0;
				player_1.health -= 1;
			}
		}
	}
	else {
		if (player_1.next_motion_timer > 0.0f) {
			player_1.next_motion_timer -= elapsed;
		}
		else if (player_1.rmb_down == 1) {
			player_1.status = 1;
			player_1.next_motion_timer = 0.1f;
			if (player_1.guard_timer > 0.0f && player_1.jiggle_timer <= FLT_EPSILON) {
				player_1.guard = 1;
			}
			else {
				player_1.guard = 0;
			}
		}
		else{
			player_1.status = 0;
			player_1.guard = 0;
			player_1.guard_timer = 0.4f;
		}
	}
	if (player_1.rmb_down == 1) {
		player_1.guard_timer  = glm::max(0.0f, player_1.guard_timer - elapsed);
	}
	if (player_1.jiggle_timer > 0.0f) {
		player_1.jiggle_timer = glm::max(0.0f, player_1.jiggle_timer - elapsed);
	}

	//update enemy status
	if (enemy_1.fire_timer > 0.0f) {
		enemy_1.fire_timer -= elapsed;
	}
	if (enemy_1.next_motion_timer > 0.0f) {
		enemy_1.next_motion_timer -= elapsed;
	}
	else {
		enemy_1.status = 0;

		if (enemy_1.fire_timer <= 0.0f && enemy_1.bullet_tile_pos.y >= PPU466::ScreenHeight) {
			enemy_1.status = 1;
			enemy_1.next_motion_timer = 1.0f;
			enemy_1.fire_timer = (mt() / float(mt.max())) * 3.0f + 0.3f;
			enemy_1.bullet_speed = (mt() / float(mt.max())) * 30.0f  + 10.0f;
			enemy_1.bullet_tile_pos.y = ground_pos + 22;
			enemy_1.bullet_tile_pos.x = enemy_1.enemy_tile_pos.x - 8;
		}
	}


	if (enemy_1.bullet_tile_pos.y < PPU466::ScreenHeight) {
		enemy_1.bullet_tile_pos.x -= 8 * enemy_1.bullet_speed * elapsed;
		if (enemy_1.bullet_tile_pos.x - player_1.player_tile_pos.x - character_width/2 < FLT_EPSILON) {
			player_1.attacked = 1;
		}
	}
}

//helper function to draw player, enemy, and bullet
void PlayMode::draw_character() {

	//player sprite:
	{
		int player_start = (int)SpriteTileIndex::player[player_1.status];
		for (int i = 0; i < 24; i++) {
			ppu.sprites[i].x = int32_t(player_1.player_tile_pos.x + (i % 4) * 8);
			ppu.sprites[i].y = int32_t(player_1.player_tile_pos.y + (i / 4) * 8);
			ppu.sprites[i].index = player_start + i;
			ppu.sprites[i].attributes = SpriteTileIndex::player_palette;
		}
	}

	//enemy sprite:
	{
		int enemy_start = (int)SpriteTileIndex::enemy[enemy_1.status];
		for (int i = 0; i < 24; i++) {
			ppu.sprites[i + 24].x = int32_t(enemy_1.enemy_tile_pos.x + (i % 4) * 8);
			ppu.sprites[i + 24].y = int32_t(enemy_1.enemy_tile_pos.y + (i / 4) * 8);
			ppu.sprites[i + 24].index = enemy_start + i;
			ppu.sprites[i + 24].attributes = SpriteTileIndex::enemy_palette;
		}
	}

	//bullet sprite:
	{
		ppu.sprites[48].x = int32_t(enemy_1.bullet_tile_pos.x);
		ppu.sprites[48].y = int32_t(enemy_1.bullet_tile_pos.y);
		ppu.sprites[48].index = SpriteTileIndex::bullet;
		ppu.sprites[48].attributes = SpriteTileIndex::bullet_palette;
	}

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be sky blue
	ppu.background_color = glm::u8vec4(
		0x1f,
		0xd2,
		0xf9,
		0xf0
	);

	
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			if (y < ground_pos/8) {
				uint8_t tile_index = SpriteTileIndex::ground[2 * (y % 2) + (x % 2)];
				ppu.background[x + PPU466::BackgroundWidth * y] = (SpriteTileIndex::ground_palette << 8) | (tile_index);
			}
			else {
				ppu.background[x + PPU466::BackgroundWidth * y] = 7;
			}
		}
	}

	//draw characters
	draw_character();

	//health
	for (int i = 0; i < player_1.max_health; i++) {
		ppu.sprites[49 + i].x = int32_t(i * 8);
		ppu.sprites[49 + i].y = int32_t(PPU466::ScreenHeight - 8);
		ppu.sprites[49 + i].index = (i < player_1.health) ? SpriteTileIndex::health : 7;
		ppu.sprites[49 + i].attributes = SpriteTileIndex::num_health_palette;
	}

	//timer
	ppu.sprites[52].attributes = SpriteTileIndex::num_health_palette;
	ppu.sprites[52].index = SpriteTileIndex::numerals[((int)timer) / 10];
	ppu.sprites[52].x = PPU466::ScreenWidth / 2;
	ppu.sprites[52].y = PPU466::ScreenHeight - 8;
	ppu.sprites[53].attributes = SpriteTileIndex::num_health_palette;
	ppu.sprites[53].index = SpriteTileIndex::numerals[((int)timer) % 10];
	ppu.sprites[53].x = PPU466::ScreenWidth / 2 + 8;
	ppu.sprites[53].y = PPU466::ScreenHeight - 8;

	//scoreboard
	ppu.sprites[54].attributes = SpriteTileIndex::num_health_palette;
	ppu.sprites[54].index = SpriteTileIndex::numerals[player_1.score / 10];
	ppu.sprites[54].x = 32;
	ppu.sprites[54].y = PPU466::ScreenHeight - 8;
	ppu.sprites[55].attributes = SpriteTileIndex::num_health_palette;
	ppu.sprites[55].index = SpriteTileIndex::numerals[player_1.score % 10];
	ppu.sprites[55].x = 40;
	ppu.sprites[55].y = PPU466::ScreenHeight - 8;

	//--- actually draw ---
	ppu.draw(drawable_size);
}
