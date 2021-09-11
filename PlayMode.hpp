#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//function to draw player and enemy
	void draw_character();

	//----- game state -----
	static constexpr uint8_t character_width = 32U;
	static constexpr uint8_t character_height = 48U;
	static constexpr uint8_t ground_pos = 96U;
	float timer = 60.0f;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//player status
	struct Player {
		uint8_t rmb_down = 0; //0:released, 1:pressed
		uint8_t guard = 0;	  //0:no guard, 1:guarded, 2:guard expired
		uint8_t status = 2;	  //0:still, 1:guard_pos, 2:deflect_pos, 3:hit_pos
		static constexpr uint8_t max_health = 3;
		uint8_t health = max_health;
		int score = 0;
		float guard_timer = 0.0f;
		glm::vec2 player_tile_pos = glm::vec2(character_width, ground_pos);
	} player_1;

	//enemy status
	struct Enemy {
		int status = 1; //0:still, 1:fire, 2:surrender
		glm::vec2 enemy_tile_pos = glm::vec2(PPU466::ScreenWidth - 3 * character_width / 2, ground_pos);
		glm::vec2 bullet_tile_pos = glm::vec2(PPU466::ScreenWidth/2, PPU466::ScreenHeight/2);
	} enemy_1;



	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};

struct SpriteTileIndex {

	//indicates the starting index in tile table
	static constexpr std::array<uint8_t, 4> ground = { 0,1,8,9 };
	static constexpr std::array<uint8_t, 4> player = { 16, 40, 64, 88 };
	static constexpr std::array<uint8_t, 3> enemy = { 112, 136, 160 };
	static constexpr uint8_t bullet = 184;

	static constexpr uint8_t health = 15;

	static constexpr std::array<uint8_t, 10> numerals{
		10, 11, 12, 13, 14, 2, 3, 4, 5, 6
	};

	static constexpr uint8_t ground_palette = 0;
	static constexpr uint8_t num_health_palette = 2;
	static constexpr uint8_t player_palette = 3;
	static constexpr uint8_t enemy_palette = 4;
	static constexpr uint8_t bullet_palette = 5;

};
