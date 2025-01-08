/**
 * BWGui
 *
 * Application
 */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "action.h"
#include "color.h"
#include "shape.h"

#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace bwgui
{
	
class MouseCorrection
{
public:
	MouseCorrection(float x, float y): x_{x}, y_{y} {}
	void toggle() { enabled_ = !enabled_; }
	float x() const { return enabled_ ? x_ : 1; }
	float y() const { return enabled_ ? y_ : 1; }
	operator bool() const { return enabled_; }

private:
	float x_, y_;
	bool enabled_{false};
};

class Application
{
public:
	explicit Application(std::string name, int delay = 1000);

	Size<int> GetSize() const { return size_; }
	void Resize(Size<int> const& size) { size_ = size; }
	SDL_Renderer* renderer() const { return renderer_; }
	
	void SetScale(float scale) { scale_ = scale; }
	void SetMouseCorrection(MouseCorrection correction) { correction_ = correction; }

	int Execute();
	int Loop();
	
	virtual void OnClick(Point<int> p) {}
	virtual void OnLoop() {};
	virtual void OnRender() = 0;
	
protected:
	Rectangle<int> background() { return {{0, 0}, size_}; }

private:
	friend void DoInLoop(void*);
	bool Init();
	void DoWork();
	void OnEvent(SDL_Event& event);
	void Cleanup();
	void UpdateViewport();
	void Delay() { SDL_Delay(delay_); }
	void Clear() { SDL_RenderClear(renderer_); }
	void Update() { SetBrushColor(renderer_, Black); SDL_RenderPresent(renderer_); }
	void DrawToolkit();

	std::string name_;
	Size<int> size_{2266, 1075};
	int delay_{1000};
	bool is_running_{true};
	float scale_{1};
	MouseCorrection correction_{1, 1};
	SDL_Window* window_{nullptr};
	SDL_Renderer* renderer_{nullptr};
};

Application::Application(std::string name, int delay)
	: name_{std::move(name)},
	  delay_{delay}
{
#ifdef __APPLE__
	SetMouseCorrection({0.9485, 1.04});
#endif
}

bool Application::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		LOG_ERROR();
		return false;
	}

	if (SDL_CreateWindowAndRenderer(size_.w, size_.h,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_FOCUS,
			&window_, &renderer_) < 0)
	{
		LOG_ERROR();
		return false;
	}

	SDL_SetWindowSize(window_, size_.w, size_.h);
	SDL_SetWindowTitle(window_, name_.c_str());
	SDL_ShowWindow(window_);

	return true;
}

void Application::UpdateViewport()
{	
	auto size = GetSize();
	SDL_Rect view{0, 0, size.w, size.h};
	if (SDL_RenderSetViewport(renderer_, &view) != 0) LOG_ERROR();
	if (SDL_RenderSetScale(renderer_, scale_, scale_) != 0) LOG_ERROR();
}

void Application::DrawToolkit()
{
	if (correction_)
	{
		SetBrushColor(renderer_, Red);
		DrawRectangle(renderer_, background());
	}
}

void Application::OnEvent(SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_QUIT: is_running_ = false; break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_m: correction_.toggle(); break;
				case SDLK_h: scale_ -= 0.05; break;
				case SDLK_j: scale_ += 0.05; break;
				case SDLK_r: scale_ = 1.; break;
			}; break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.clicks == 1)
			{
				const auto x = static_cast<int>(event.button.x * correction_.x() / scale_);
				const auto y = static_cast<int>(event.button.y * correction_.y() / scale_);
				OnClick({x, y});
			}
			break;
	}
}

void Application::Cleanup()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void Application::DoWork()
{
	OnLoop();
	Clear();
	UpdateViewport();
	OnRender();
	DrawToolkit();
	Update();
}

void DoInLoop(void* arg)
{
	auto* app = static_cast<Application*>(arg);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		app->OnEvent(event);
	}
	app->DoWork();
	app->Delay();
}

int Application::Loop()
{
	if (!Init()) return -1;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(DoInLoop, this, 0, 1);
#else
	while (is_running_) DoInLoop(this);
#endif

	Cleanup();
	return 0;
}

int Application::Execute()
{
	if (!Init()) return -1;
	DoWork();
	return 0;
}

} // namespace bwgui

#endif // _APPLICATION_H_
