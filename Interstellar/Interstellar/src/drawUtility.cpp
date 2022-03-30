#include <SDL2/SDL_render.h>
#include <vector>

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
        points.push_back({ x0 + y, y0 + x });
        points.push_back({ x0 - y, y0 + x });
        points.push_back({ x0 - x, y0 + y });
        points.push_back({ x0 - x, y0 - y });
        points.push_back({ x0 - y, y0 - x });
        points.push_back({ x0 + y, y0 - x });
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