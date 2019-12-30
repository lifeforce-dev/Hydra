//---------------------------------------------------------------
//
// DebugController.cpp
//

#include "DebugController.h"

#include "client/DebugEvents.h"
#include "client/Game.h"
#include "common/log.h"
#include "client/RenderEngine.h"

#include "client/imgui/imgui_impl_sdl.h"
#include "client/imgui/imgui_impl_opengl3.h"

namespace Client {

//===============================================================================

namespace {
// Logger for the render engine.
std::shared_ptr<spdlog::logger> s_logger;
} // anon namespace


DebugController::DebugController()
{
	REGISTER_LOGGER("DebugController");
	s_logger = Log::Logger("DebugController");
}

DebugController::~DebugController()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void DebugController::SubscribeEvents()
{
	auto& events = g_game->GetDebugEvents();

	using namespace std::chrono;
	events.GetFullFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetRenderFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetEventHandlerFrameCompletedEvent().subscribe(
		[this](const microseconds& frameTime)
	{
		// NYI
	});

	events.GetSimFrameCompletedEvent().subscribe([this](const microseconds& frameTime)
	{
		// NYI
	});
}

void DebugController::Initialize(SDL_Window* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, g_game->GetRenderEngine()->GetGLContext());
	ImGui_ImplOpenGL3_Init();
	m_window = window;
}

void DebugController::Render()
{
	if (!m_window)
	{
		SPDLOG_LOGGER_WARN(s_logger, "Warning: Attempting to render to a dead window.");
	}

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();
	bool showWindow = true;
	ImGui::ShowDemoWindow(&showWindow);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

bool DebugController::HandleKeyEvent(SDL_KeyboardEvent* event)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard)
	{
		return false;
	}

	// Always pass escape to someone else.
	if (event->keysym.sym == SDLK_ESCAPE)
	{
		return false;
	}

	int key = event->keysym.scancode;
	IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
	io.KeysDown[key] = (event->type == SDL_KEYDOWN);
	io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
	io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
	io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
	io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

	return true;
}

bool DebugController::HandleMouseButtonEvent(SDL_MouseButtonEvent* event)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		return false;
	}

	// NYI

	return true;
}

bool DebugController::HandleMouseWheelEvent(SDL_MouseWheelEvent* event)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		return false;
	}

	if (event->x > 0)
		io.MouseWheelH += 1;
	if (event->x < 0)
		io.MouseWheelH -= 1;
	if (event->y > 0)
		io.MouseWheel += 1;
	if (event->y < 0)
		io.MouseWheel -= 1;

	return true;
}

bool DebugController::HandleMouseMotionEvent(SDL_MouseMotionEvent* event)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		return false;
	}

	// NYI

	return true;
}

bool DebugController::HandleTextInputEvent(SDL_TextInputEvent* event)
{
	auto& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard)
	{
		return false;
	}

	io.AddInputCharactersUTF8(event->text);
	return true;
}

//===============================================================================

} //namespace Client
