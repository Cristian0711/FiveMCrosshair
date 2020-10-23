#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <d3d11.h>
#include <dwmapi.h>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <mutex>

#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "overlay.hpp"
#include "process.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "Dwmapi.lib")
