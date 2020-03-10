#pragma once
#pragma once
#include "../..//dependencies/common_includes.hpp"


class cmenu
{
public:
	void run();
	void addchildren();
	void renderbackground();
	void handlechildren();
	void handleinput();
	void renderchildren();
}; extern cmenu menu;

enum menuitemtype
{
	SWITCH = 0,
	slider,
	section
};
struct menuitems_t
{
	menuitems_t(std::string name, float* value = 0, int itemtype = menuitemtype::SWITCH, int minimumvalue = 0, int maximumvalue = 1)
	{
		this->name = name;
		this->value = value;
		this->itemtype = itemtype;
		this->minimumvalue = minimumvalue;
		this->maximumvalue = maximumvalue;
	}

	std::string name;
	float* value;
	int itemtype;
	int minimumvalue;
	int maximumvalue;
};
extern cmenu menu;
namespace gui {
	void initialize();
}

struct config {
	bool menuopened;
	float backtrack;
	float radar;
	float team_check;
	float box_esp;
	float name_esp;
	float health_bar;
	float weapon_esp;
	float bunny_hop;
	float rank_revealer;
	float menu_anim = true;
	float lock_cursor = true;
	float esp_opacity = 255;

	float chams_enemy;
	float chams_enemy_xqz;
	float chams_team;
	float chams_team_xqz;
	float chams_material;
};
extern config cfg;