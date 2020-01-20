#pragma once
#include "../../dependencies/common_includes.hpp"

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
	};
}

namespace visuals {
	
	void run();
	void player_esp(player_t* entity);
	void entity_esp(player_t* entity);
}