#include "main.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

HWND overlay::game_window;
HWND overlay::overlay_window;

ID3D11Device* g_pd3d_device = nullptr;
ID3D11DeviceContext* g_pd3d_device_context = nullptr;
IDXGISwapChain* g_p_swap_chain = nullptr;
ID3D11RenderTargetView* g_main_render_target_view = nullptr;
LONG orginal;

void create_render_target( ) {
	ID3D11Texture2D* p_back_buffer;
	g_p_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &p_back_buffer ) );
	g_pd3d_device->CreateRenderTargetView( p_back_buffer, NULL, &g_main_render_target_view );
	p_back_buffer->Release( );
}

bool create_device_d3d( ) {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 144;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = overlay::overlay_window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if ( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain, &g_pd3d_device, &featureLevel, &g_pd3d_device_context ) != S_OK )
		return false;

	create_render_target( );
	return true;
}

void cleanup_render_target( ) {
	if ( g_main_render_target_view ) { g_main_render_target_view->Release( ); g_main_render_target_view = nullptr; }
}

void cleanup_device_d3d( ) {
	cleanup_render_target( );
	if ( g_p_swap_chain ) { g_p_swap_chain->Release( ); g_p_swap_chain = nullptr; }
	if ( g_pd3d_device_context ) { g_pd3d_device_context->Release( ); g_pd3d_device_context = nullptr; }
	if ( g_pd3d_device ) { g_pd3d_device->Release( ); g_pd3d_device = nullptr; }
}

std::string random_string( ) {
	std::string str( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );

	std::random_device rd;
	std::mt19937 generator( rd( ) );

	std::shuffle( str.begin( ), str.end( ), generator );

	return str.substr( 0, 32 );
}


LRESULT WINAPI wnd_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg ) {
		case WM_SIZE:
			if ( g_pd3d_device != NULL && wParam != SIZE_MINIMIZED ) {
				cleanup_render_target( );
				g_p_swap_chain->ResizeBuffers( 0, (UINT)LOWORD( lParam ), (UINT)HIWORD( lParam ), DXGI_FORMAT_UNKNOWN, 0 );
				create_render_target( );
			}
			return 0;
		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;
	}

	return ::DefWindowProc( hWnd, msg, wParam, lParam );
}

void overlay::init_window_create_device( ) {

	RECT game_pos, game_size;
	GetClientRect( game_window, &game_pos );
	GetWindowRect( game_window, &game_size );

	auto window_name = random_string( );

	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, wnd_proc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, window_name.c_str( ), NULL };
	::RegisterClassEx( &wc );

	overlay_window = ::CreateWindowEx( WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, window_name.c_str( ), WS_POPUP, game_size.left, game_size.top, game_pos.right, game_pos.bottom, NULL, NULL, wc.hInstance, NULL );

	orginal = GetWindowLong( overlay_window, GWL_EXSTYLE );

	SetLayeredWindowAttributes( overlay_window, RGB( 0, 0, 0 ), 255, LWA_ALPHA );

	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea( overlay_window, &margins );

	if ( !create_device_d3d( ) ) {
		cleanup_device_d3d( );
		::UnregisterClass( wc.lpszClassName, wc.hInstance );
		printf("Couldn't create the d3d device!");
		system("pause");
		return;
	}

	::ShowWindow( overlay_window, SW_SHOWDEFAULT );
	::UpdateWindow( overlay_window );

	SetForegroundWindow( game_window );
	SetFocus( game_window );

	IMGUI_CHECKVERSION( );
	ImGui::CreateContext( );
	ImGui::StyleColorsDark( );

	ImGui_ImplWin32_Init( overlay_window );
	ImGui_ImplDX11_Init( g_pd3d_device, g_pd3d_device_context );
}

void overlay::start_render_loop( ) {
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	ImVec4 clear_color = ImVec4( 0.f, 0.f, 0.f, 0.f );
	while ( msg.message != WM_QUIT ) {
		if ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			continue;
		}

		if (!IsWindow(overlay::game_window))
			break;

		if ( GetAsyncKeyState( VK_F7 ) ) {
			break;
		}
		
		static bool clicked = false;
		static bool clicked2 = false;
		bool change_menu_state = false;
		bool change_crosshair_state = false;

		std::this_thread::sleep_for(std::chrono::microseconds(50));

		if (GetForegroundWindow() == overlay::game_window || GetForegroundWindow() == overlay::overlay_window)
		{
			if (GetAsyncKeyState(VK_INSERT) && (GetForegroundWindow() == overlay::game_window || GetForegroundWindow() == overlay::overlay_window)) {
				if (!clicked) {
					clicked = true;
					change_menu_state = true;
				}
			}
			else
				clicked = false;

			if (GetAsyncKeyState(VK_F4) && (GetForegroundWindow() == overlay::game_window || GetForegroundWindow() == overlay::overlay_window)) {
				if (!clicked2) {
					clicked2 = true;
					change_crosshair_state = true;	
				}
			}
			else
				clicked2 = false;

			if(change_crosshair_state) {
				settings::cheat::use_crosshair = !settings::cheat::use_crosshair;
			}


			if (change_menu_state) {
				settings::menu::menu_is_open = !settings::menu::menu_is_open;
				auto cur_hwnd = GetForegroundWindow();
				if (settings::menu::menu_is_open) {
					if (cur_hwnd == overlay::game_window) {
						keybd_event(0, 0, 0, 0);
						SetForegroundWindow(overlay::overlay_window);
					}

					auto org = orginal;
					org &= ~(WS_EX_TRANSPARENT);
					SetWindowLong(overlay::overlay_window, GWL_EXSTYLE, org);

				}
				else {
					if (cur_hwnd == overlay::overlay_window) {
						keybd_event(0, 0, 0, 0);
						SetForegroundWindow(overlay::game_window);
					}
					SetWindowLong(overlay::overlay_window, GWL_EXSTYLE, orginal);
				}
			}

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			menu::draw_crosshair();

			if (settings::menu::menu_is_open) {
				menu::draw();
			}

			ImGui::Render();

			g_pd3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view, NULL);
			g_pd3d_device_context->ClearRenderTargetView(g_main_render_target_view, (float*)&clear_color);

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_p_swap_chain->Present(1, 0);
		}
	}
}