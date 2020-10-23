#pragma once

namespace settings {
	namespace cheat {
		extern bool use_crosshair;
		extern int radius;
	};

	namespace menu
	{
		extern bool menu_is_open;
	};
};

namespace menu {
	void draw( );
	void draw_crosshair( );
};

