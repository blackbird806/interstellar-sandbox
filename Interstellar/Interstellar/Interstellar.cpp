#define SDL_MAIN_HANDLED // for some reason SDL main is broken on my computer
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <Eigen/Dense>
#define AINI_IMPLEMENTATION
#include "aini.hpp"
#include "KerrBlackHole.hpp"
#include "physicalConstants.hpp"
#include "drawUtility.hpp"

using vec = Eigen::Vector2<double>;

static std::string readText(std::ifstream const& settingsFile)
{
	std::ostringstream sstream;
	sstream << settingsFile.rdbuf();
	return sstream.str();
}

int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	SDL_WindowFlags const window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Interstellar-sandbox-2d", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

	// Setup SDL_Renderer instance
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	// Our state
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 grid_color = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);

	vec initialPos{};

	int grid_size = 20;
	int n_rays = 360;
	double pixel_scale = 1;
	double constexpr G = 1;
	double constexpr C = 1;
	double M = 1000;
	SchwarzschildBlackHole blackHole;
	{
		std::ifstream saveFile("save.ini");
		if (saveFile.is_open())
		{
			aini::Reader reader(readText(saveFile));
			blackHole.deserialize(reader);
		}
	}

	// Main loop
	bool done = false;
	int steps = 400;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			ImGui::Begin("Settings");
			ImGui::ColorEdit3("clear color", (float*)&clear_color); 
			ImGui::ColorEdit3("grid color", (float*)&grid_color);   
			ImGui::SliderInt("grid size", &grid_size, 10, 100);

			ImGui::DragInt("steps", &steps, 100, 1, 0);
			ImGui::DragInt("n rays", &n_rays, 1, 1, 0);
			ImGui::DragDouble("pixel scale", &pixel_scale);
			ImGui::DragDouble("M", &M);

			ImGui::DragDouble("initial pos x", &initialPos[0]);
			ImGui::DragDouble("initial pos y", &initialPos[1]);

			blackHole.showEditor();

			if (ImGui::Button("save"))
			{
				aini::Writer writer;
				blackHole.serialize(writer);
				std::ofstream saveFile("save.ini");
				saveFile << writer.write();
				saveFile.flush();
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, (Uint8)(grid_color.x * 255), (Uint8)(grid_color.y * 255), (Uint8)(grid_color.z * 255), (Uint8)(grid_color.w * 255));
		int width, height;
		SDL_GetWindowSize(window, &width, &height);

		// newton approximation
		// see: https://fr.wikipedia.org/wiki/Tests_exp%C3%A9rimentaux_de_la_relativit%C3%A9_g%C3%A9n%C3%A9rale
		// https://fr.wikipedia.org/wiki/Loi_universelle_de_la_gravitation

		auto draw_ray = [&](vec pos, vec dir)
		{
			for (int i = 0; i < steps; i++)
			{
				vec const d = blackHole.pos - pos;
				auto const F = (G * M) / (d.squaredNorm());
				auto const gravity = d.normalized() * F;
				pos += gravity + dir * C;
				auto const phi = (2.0 * G * M) / (C * C * d.norm());
				dir += vec{ cos(phi), sin(phi) };
				SDL_RenderDrawPoint(renderer, (int)(pos[0] / pixel_scale), (int)(pos[1] / pixel_scale));
			}
		};

		double const angleStep = (M_PI * 2.0) / n_rays;
		double angle = 0;
		for (int n = 0; n < n_rays; n++)
		{
			draw_ray(initialPos, vec{cos(angle), sin(angle)});
			angle += angleStep;
		}

		// draw black hole center
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_Rect brect{ (int)(blackHole.pos[0] / pixel_scale), (int)(blackHole.pos[1] / pixel_scale), 4, 4};
		SDL_RenderFillRect(renderer, &brect);
		//draw_circle(renderer, blackHole.pos[0] / pixel_scale, blackHole.pos[1] / pixel_scale, blackHole.SchwarzschildRadius() / pixel_scale);

		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}