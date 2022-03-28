#define SDL_MAIN_HANDLED // for some reason SDL main is broken on my computer
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <stdio.h>
#include <vector>

struct KerrBlackHole
{
    int x = 0, y = 0;

    double M = 10.0f;       // mass
    double J = 1.0f;        // angular momentum
    double r = 0.0f;        // radial coordinate
    double theta = 0.0f;    // colatitude
    double phi = 0.0f;      // longitude

    void showEditor()
    {
        ImGui::TextUnformatted("KerrBlackHole");

        ImGui::DragInt("pos_x", &x);
        ImGui::DragInt("pos_y", &y);

        ImGui::InputDouble("mass", &M);
        ImGui::InputDouble("angular momentum", &J);
        ImGui::InputDouble("radial coordinate", &r);
    }

    double kerrParameter() const
    {
        return J / M;
    }
};

//https://en.wikipedia.org/w/index.php?title=Midpoint_circle_algorithm&oldid=889172082#C_example
void draw_circle(SDL_Renderer* renderer, int x0, int y0, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    std::vector<SDL_Point> points;
    points.reserve(4096);
    while (x >= y)
    {
        points.push_back({ x0 + x, y0 + y });
        points.push_back({x0 + y, y0 + x});
        points.push_back({x0 - y, y0 + x});
        points.push_back({x0 - x, y0 + y});
        points.push_back({x0 - x, y0 - y});
        points.push_back({x0 - y, y0 - x});
        points.push_back({x0 + y, y0 - x});
        points.push_back({ x0 + x, y0 - y });

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
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
        SDL_Log("Error creating SDL_Renderer!");
        return false;
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
    int grid_size = 35;

    KerrBlackHole blackHole;

    // Main loop
    bool done = false;
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
        static int steps = 100;
        static int scale = 100;
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("clear color", (float*)&clear_color); 
            ImGui::ColorEdit3("grid color", (float*)&grid_color);   
            ImGui::SliderInt("grid size", &grid_size, 10, 100);
           
            ImGui::DragInt("steps", &steps, 10, 100, 1000);
            ImGui::DragInt("scale", &scale, 10, 10, 10000);

        	blackHole.showEditor();

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

        for (int t = 0; t < steps; t++)
        {
            double const th = M_PI*t / steps;
            for (int p = 0; p < steps; p++)
            {
                double const ph = (2*M_PI) * p / steps;

                auto const a = blackHole.kerrParameter();
                double const x = sqrt(blackHole.r * blackHole.r + a * a) * sin(th) * cos(ph);
                double const y = sqrt(blackHole.r * blackHole.r + a * a) * sin(th) * sin(ph);
                double const z = blackHole.r * cos(th);
                SDL_SetRenderDrawColor(renderer, (Uint8)(grid_color.x * 255), (Uint8)(grid_color.y * 255), (Uint8)(grid_color.z * 255), (Uint8)(grid_color.w * 255));
                SDL_RenderDrawPoint(renderer, blackHole.x + x * scale, blackHole.y + y * scale);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawPoint(renderer, blackHole.x + z * scale, blackHole.y + y * scale);
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderDrawPoint(renderer, blackHole.x + x * scale, blackHole.y + z * scale);
            }

        }

        //for (int x = 0; x < width; x += grid_size)
        //{
        //    SDL_RenderDrawLine(renderer, x, 0, x, height);
        //}

        //for (int y = 0; y < height; y += grid_size)
        //{
        //    SDL_RenderDrawLine(renderer, 0, y, width, y);
        //}

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