#pragma once

namespace overlay {
	extern HWND game_window;
	extern HWND overlay_window;

	void init_window_create_device( );
	void start_render_loop( );
}