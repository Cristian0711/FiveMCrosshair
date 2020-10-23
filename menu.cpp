#include "main.h"

namespace settings {
	namespace cheat {
		bool use_crosshair = false;
		int radius = 1;
		ImVec4 color{ 1.00f, 0.00f, 0.00f, 1.00f };
	};

	namespace menu
	{
		bool menu_is_open = false;
	};
};

namespace menu {
	bool cheat_tab_active = true;

	void draw_tab( const char* name, bool& active ) {
		ImGui::Text( name );
		ImGui::SameLine( 118 + 50 );

		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4 { 0.f, 92.f / 255.f, 196.f / 255.f, 1.f } );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4 { 0.f, 92.f / 255.f, 196.f / 255.f, 1.f } );
		if ( ImGui::Button( "", { 10,10 } ) )
			active = !active;
		ImGui::PopStyleColor( 2 );
	}

	std::once_flag init_style;
	void draw( ) {

		std::call_once( init_style, [ & ] ( ) {
			auto& style = ImGui::GetStyle( );
			style.WindowRounding = 0.f;
			style.FrameRounding = 0.f;
			style.ItemSpacing = ImVec2( 8, 2 );
			style.WindowPadding = ImVec2( 3.f, 3.f );
			style.Colors[ ImGuiCol_FrameBg ] = ImVec4 { 56.f / 255.f, 59.f / 255.f, 58.f / 255.f, 1.f };
			style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4 { 56.f / 255.f, 59.f / 255.f, 58.f / 255.f, 1.f };
			style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4 { 56.f / 255.f, 59.f / 255.f, 58.f / 255.f, 1.f };
			style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4 { 0.f, 92.f / 255.f, 196.f / 255.f, 1.f };
			style.Colors[ ImGuiCol_SliderGrab ] = ImVec4 { 0.f, 128.f / 255.f, 199.f / 255.f, 1.f };
			} );

		ImGui::Begin( "tab_crosshair", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
		ImGui::SetWindowSize( { 131 + 50,0 }, ImGuiCond_Always );
		draw_tab( "Make#1197", cheat_tab_active);
		if (cheat_tab_active) {
			ImGui::Checkbox("Crosshair", &settings::cheat::use_crosshair);
			ImGui::SliderInt("Radius", &settings::cheat::radius, 1, 20);
			ImGui::ColorEdit4("##Color", (float*)&settings::cheat::color, ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_AlphaBar);
		}
		ImGui::End( );
	}

	void draw_crosshair( ) {
		if (!settings::cheat::use_crosshair || (GetForegroundWindow() != overlay::game_window && !settings::menu::menu_is_open && GetForegroundWindow() != overlay::overlay_window)
			|| (!GetAsyncKeyState(VK_RBUTTON) && !settings::menu::menu_is_open))
		{
			return;
		}

		ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::cheat::radius, (ImColor)settings::cheat::color, 20);
	}
 
	struct cacat
	{
		int a, b, c;
	};

	cacat c;
};
