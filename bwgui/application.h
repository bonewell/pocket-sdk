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

class Application
{
public:
	explicit Application(std::string name, int delay = 1000);

	Size<int> GetSize() const { return {width, height}; }
	SDL_Renderer* renderer() const { return renderer_; }
	
	void SetScale(double scale) { scale_ = scale; }

	int Execute();
	int Loop();
	
	virtual void OnClick(Point<int> p) {}
	virtual void OnLoop() {};
	virtual void OnRender() = 0;
	
protected:
	constexpr static Rectangle<int> background() { return {0, 0, width, height}; }

private:
	constexpr static int width = 2266;
	constexpr static int height = 1075;

	friend void DoInLoop(void*);
	bool OnInit();
	void DoWork();
	void OnEvent(SDL_Event& event);
	void OnCleanup();
	void DrawWindow();
	void Delay() { SDL_Delay(delay_); }
	void Clear() { SDL_RenderClear(renderer_); }
	void Update() { SetBrushColor(renderer_, Black); SDL_RenderPresent(renderer_); }

	std::string name_;
	int delay_{1000};
	bool is_running_{true};
	float scale_{1.0};
	SDL_Window* window_{nullptr};
	SDL_Renderer* renderer_{nullptr};
};

Application::Application(std::string name, int delay)
	: name_{std::move(name)},
	  delay_{delay}
{}

bool Application::OnInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		LOG_ERROR();
		return false;
	}

	if (SDL_CreateWindowAndRenderer(width, height,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_FOCUS,
			&window_, &renderer_) < 0)
	{
		LOG_ERROR();
		return false;
	}

	SDL_SetWindowSize(window_, width, height);
	SDL_SetWindowTitle(window_, name_.c_str());
	SDL_ShowWindow(window_);

	return true;
}

void Application::DrawWindow()
{	
	auto size = GetSize();
	SDL_Rect view{0, 0, size.w, size.h};
	if (SDL_RenderSetViewport(renderer_, &view) != 0) LOG_ERROR();
	if (SDL_RenderSetScale(renderer_, scale_, scale_) != 0) LOG_ERROR();
}

void Application::OnEvent(SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_QUIT: is_running_ = false; break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_h: scale_ -= 0.1; break;
				case SDLK_j: scale_ += 0.1; break;
			}; break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (event.button.clicks == 1)
				OnClick({event.button.x, event.button.y});
			break;
	}
}

void Application::OnCleanup()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

void Application::DoWork()
{
	OnLoop();
	Clear();
	DrawWindow();
	OnRender();
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
	if (OnInit() == false) return -1;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(DoInLoop, this, 0, 1);
#else
	while (is_running_) DoInLoop(this);
#endif

	OnCleanup();
	return 0;
}

int Application::Execute()
{
	if (OnInit() == false) return -1;
	DoWork();
	return 0;
}

} // namespace bwgui

#endif // _APPLICATION_H_
