#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;
const int BALL_WIDTH = 7;
const int BALL_HEIGHT = 7;

class Vec2
{
    public:
        // constructors
        Vec2()
            : x(0.0f), y(0.0f)
        {}

        Vec2(float x, float y)
            : x(x), y(y)
        {}

        // overloads
        Vec2 operator+ (Vec2 const& rhs)
        {
            return Vec2(x + rhs.x, y + rhs.y);
        }

        Vec2& operator+= (Vec2 const& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vec2 operator* (float rhs)
        {
            return Vec2(x * rhs, y * rhs);
        }

        // data
        float x, y;
};

class Ball
{
    public:
        Ball(Vec2 position)
            : position(position)
        {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = BALL_WIDTH;
            rect.h = BALL_HEIGHT;
        }

        void Draw(SDL_Renderer* renderer)
        {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);

            SDL_RenderFillRect(renderer, &rect);
        }

        Vec2 position;
        SDL_Rect rect{};
};

int main()
{
    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Pong", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // init ball
    Ball ball(
        Vec2(
            (WINDOW_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f), 
            (WINDOW_HEIGHT / 2.0f) - (BALL_HEIGHT / 2.0f)
            ));

    // Game logic
    {
        bool running = true;

        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                    }
                }
            }

            // Clear window
            // RGBA
            SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
            SDL_RenderClear(renderer);

            
            // Rendering is done below
            
            // net
            // Set the draw color to be white
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            for (int y=0; y < WINDOW_HEIGHT; y++) {
                if (y % 5 == 0)
                {
                    SDL_RenderDrawPoint(renderer, WINDOW_WIDTH/2, y);
                }
            }

            // ball
            ball.Draw(renderer);

            // Present the backbuffer
            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QUIT;

    return 0;
}
