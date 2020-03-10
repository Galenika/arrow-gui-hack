#include "menu.hpp"
config cfg;
std::vector<menuitems_t> menuitems;
cmenu menu;
int x = 50, y = 100, w = 200, h = 15;
float idk = 0;
float idk2 = 0;
float cat_0;
float cat_1;
float cat_2;
float cat_3;
float cat_4;
int currentselecteditem = 0;
int menu_alpha;
int menu_pos;
void cmenu::run()
{
	static bool _pressed = true;

	if (!_pressed && GetAsyncKeyState(VK_INSERT))
		_pressed = true;
	else if (_pressed && !GetAsyncKeyState(VK_INSERT))
	{
		_pressed = false;
		cfg.menuopened = !cfg.menuopened;
	}

	if (cfg.menuopened)
	{
		menu_alpha = min(menu_alpha + 7, 255);
		if (cfg.menu_anim)
			menu_pos = min(menu_alpha + 4, 255);
		else
			menu_pos = 255;
	}
	else
	{
		menu_alpha = max(menu_alpha - 7, 0);
		if (cfg.menu_anim)
			menu_pos = max(menu_alpha - 4, 0);
		else
			menu_pos = 255;
	}

	static vec2_t oldpos;
	static vec2_t mousepos;
	static bool _dragging = false;
	bool _click = false;
	static bool _resizing = false;
	static int _drag_x = 300;
	static int _drag_y = 350;
	int finalsize = 200;
	vec2_t _mouse_pos = interfaces::surface->GetMousePosition();
	if (GetAsyncKeyState(VK_LBUTTON))
		_click = true;

	if (_dragging && !_click)
		_dragging = false;

	if (_resizing && !_click)
		_resizing = false;

	if (_dragging && _click)
	{
		x = _mouse_pos.x - _drag_x;
		y = _mouse_pos.y - _drag_y;
	}

	if (interfaces::surface->MouseInRegion(x + menu_pos, y, finalsize, finalsize) && !(interfaces::surface->MouseInRegion(
		x + 10, y + 20, finalsize - 10, finalsize - 10)))
	{
		_dragging = true;
		_drag_x = _mouse_pos.x - x;
		_drag_y = _mouse_pos.y - y;
	}

	this->addchildren();
	this->handleinput();
	this->renderbackground();
	this->handlechildren();

	menuitems.clear();
	h = 15;
}

void cmenu::renderbackground()
{

	render::draw_filled_rect(x + menu_pos, y, w, h, color(30, 30, 30, menu_alpha));
	render::draw_outline(x + menu_pos, y, w, h, color(0, 0, 0, menu_alpha));
	render::draw_filled_rect(x + menu_pos, y, w, 15, color(25, 25, 25, menu_alpha));
	render::draw_outline(x + menu_pos, y, w, 15, color(0, 0, 0, menu_alpha));
	render::draw_text_string(x + menu_pos + (w / 2), y + 1, render::fonts::watermark_font, "obelus", true, color(220, 220, 220, menu_alpha));

	//render::draw_text_string(0, 0, render::fonts::watermark_font, std::to_string(currentselecteditem), false, color(220, 220, 220, 255));


	render::draw_filled_rect(x + menu_pos + 1, y + 15 + (15 * currentselecteditem), w - 1 - 1, 15 - 1, color(50, 50, 50, menu_alpha / 2));
	//render::draw_outline(x + menu_pos, y + 15 + (15 * currentselecteditem), w, 15, color(0, 0, 0, 255));
}
void cmenu::addchildren()
{

	menuitems.push_back(menuitems_t("legit", &cat_0, menuitemtype::section));

	if (cat_0)
	{
		menuitems.push_back(menuitems_t("backtrack", &cfg.backtrack));
	}
	menuitems.push_back(menuitems_t("visuals", &cat_1, menuitemtype::section));

	if (cat_1)
	{
		menuitems.push_back(menuitems_t("show teammates", &cfg.team_check));
		menuitems.push_back(menuitems_t("box esp", &cfg.box_esp));
		menuitems.push_back(menuitems_t("name esp", &cfg.name_esp));
		menuitems.push_back(menuitems_t("weapon esp", &cfg.weapon_esp));
		menuitems.push_back(menuitems_t("health bar", &cfg.health_bar));
		menuitems.push_back(menuitems_t("opacity", &cfg.esp_opacity, menuitemtype::slider, 0, 255));
	}
	menuitems.push_back(menuitems_t("chams", &cat_2, menuitemtype::section));

	if (cat_2)
	{
		menuitems.push_back(menuitems_t("enemy", &cfg.chams_enemy));
		menuitems.push_back(menuitems_t("enemy xqz", &cfg.chams_enemy_xqz));
		menuitems.push_back(menuitems_t("teammate", &cfg.chams_team));
		menuitems.push_back(menuitems_t("teammate xqz", &cfg.chams_team_xqz));
		//menuitems.push_back(menuitems_t("slider", &idk2, menuitemtype::slider, 0, 100));
	}

	menuitems.push_back(menuitems_t("misc", &cat_3, menuitemtype::section));

	if (cat_3)
	{
		menuitems.push_back(menuitems_t("bunnyhop", &cfg.bunny_hop));
		menuitems.push_back(menuitems_t("engine radar", &cfg.radar));
		menuitems.push_back(menuitems_t("rank revealer", &cfg.rank_revealer));
		menuitems.push_back(menuitems_t("menu animation", &cfg.menu_anim));
		menuitems.push_back(menuitems_t("lock cursor", &cfg.lock_cursor));
		//menuitems.push_back(menuitems_t("slider", &idk2, menuitemtype::slider, 0, 100));
	}

	h += 15 * menuitems.size();
}

void cmenu::renderchildren()
{

	if (!(menuitems.size() > 0))
		return;

	for (int i = 0; i < menuitems.size(); i++)
	{
		if (menuitems[i].itemtype == menuitemtype::section)
			render::draw_text_string(x + menu_pos + 3, y + 16 + (15 * i), render::fonts::watermark_font, menuitems[i].name, false, *menuitems[i].value ? color(255, 255, 255, menu_alpha) : color(255, 255, 255, menu_alpha));
		else if (menuitems[i].itemtype == menuitemtype::SWITCH)
		{
			render::draw_text_string(x + menu_pos + 8, y + 16 + (15 * i), render::fonts::watermark_font, menuitems[i].name, false, color(170, 170, 170, menu_alpha));
			render::draw_text_string_alternative(x + menu_pos + 8 + 189, y + 16 + (15 * i), render::fonts::watermark_font, *menuitems[i].value ? "on" : "off", true, *menuitems[i].value ? color(10, 220, 10, menu_alpha) : color(220, 10, 10, menu_alpha));


		}
		else
		{

			render::draw_text_string(x + menu_pos + 8, y + 16 + (15 * i), render::fonts::watermark_font, menuitems[i].name, false, color(170, 170, 170, menu_alpha));
			render::draw_text_string_alternative(x + menu_pos + 8 + 189, y + 16 + (15 * i), render::fonts::watermark_font, std::to_string((int)*menuitems[i].value), true, color(10, 220, 10, menu_alpha));
			//render::text(x + menu_pos + 8, y + 16 + (15 * i), fontlist::mainfont, display, color(255, 255, 255, 255), false, true, color(0, 0, 0, 255));
		}

	}
}

void cmenu::handlechildren()
{

	if (!(menuitems.size() > 0))
		return;

	for (int i = 0; i < menuitems.size(); i++)
	{
		if (i == currentselecteditem)
		{
			switch (menuitems[i].itemtype)
			{
			case menuitemtype::SWITCH:
			{
				if (GetAsyncKeyState(0x0d) & 1 || GetAsyncKeyState(0x0d) & 1 && cfg.menuopened)
					*menuitems[i].value = !*menuitems[i].value;



			} break;
			case menuitemtype::slider:
			{
				if ((GetAsyncKeyState(VK_RIGHT) & 1) && *menuitems[i].value < menuitems[i].maximumvalue && cfg.menuopened)
					*menuitems[i].value += 1;

				if ((GetAsyncKeyState(VK_LEFT) & 1) && *menuitems[i].value > menuitems[i].minimumvalue&& cfg.menuopened)
					*menuitems[i].value -= 1;
			} break;
			case menuitemtype::section:
			{
				if (GetAsyncKeyState(0x0d) & 1 || GetAsyncKeyState(0x0d) & 1 && cfg.menuopened)
					*menuitems[i].value = !*menuitems[i].value;

			} break;
			}
		}
	}

	this->renderchildren();
}

void cmenu::handleinput()
{
	if (!cfg.menuopened)
		return;

	if (GetAsyncKeyState(VK_DOWN) & 1)
	{
		if (currentselecteditem > (menuitems.size() - 2))
			currentselecteditem = 0;
		else
			currentselecteditem++;
	}


	if (GetAsyncKeyState(VK_UP) & 1)
	{
		if (currentselecteditem < 1)
			currentselecteditem = menuitems.size() - 1;
		else
			currentselecteditem--;
	}

}

