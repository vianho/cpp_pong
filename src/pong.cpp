#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;
const int BALL_WIDTH = 7;
const int BALL_HEIGHT = 7;
const int PADDLE_WIDTH = 7;
const int PADDLE_HEIGHT = 60;
const float BALL_SPEED = 0.5f;

enum class CollisionType
{
    None,
    Top,
    Middle,
    Bottom,
    Left,
    Right
};

struct Contact
{
    CollisionType type;
    float penetration;
};
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
        Ball(Vec2 position, Vec2 velocity)
            : position(position), velocity(velocity)
        {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = BALL_WIDTH;
            rect.h = BALL_HEIGHT;
        }

        void CollideWithPaddle(Contact const& contact)
        {
            position.x += contact.penetration;
            velocity.x = -velocity.x;

            if (contact.type == CollisionType::Top)
            {
                velocity.y = -0.75f * BALL_SPEED;
            }
            if (contact.type == CollisionType::Bottom)
            {
                velocity.y = 0.75f * BALL_SPEED;
            }
        }

        void CollideWithWall(Contact const& contact)
        {
            if (contact.type == CollisionType::Top || contact.type == CollisionType::Bottom)
            {
                position.y += contact.penetration;
                velocity.y = -velocity.y;
            }
            else if (contact.type == CollisionType::Left)
            {
                position.x = WINDOW_WIDTH / 2.0f;
                position.y = WINDOW_HEIGHT / 2.0f;
                velocity.x = -BALL_SPEED;
                velocity.y = BALL_SPEED;
            }
            else if (contact.type == CollisionType::Right)
            {
                position.x = WINDOW_WIDTH / 2.0f;
                position.y = WINDOW_HEIGHT / 2.0f;
                velocity.x = BALL_SPEED;
                velocity.y = BALL_SPEED;
            }

        }

        void Update(float dt)
        {
            position += velocity * dt;
        }

        void Draw(SDL_Renderer* renderer)
        {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);

            SDL_RenderFillRect(renderer, &rect);
        }

        Vec2 position;
        Vec2 velocity;
        SDL_Rect rect{};
};

class Paddle
{
    public:
        Paddle(Vec2 position, Vec2 velocity)
            : position(position), velocity(velocity)
        {
            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = PADDLE_WIDTH;
            rect.h = PADDLE_HEIGHT;
        }

        void Update(float dt)
        {
            position += velocity * dt;

            if (position.y < 0)
            {
                position.y = 0;
            }
            else if (position.y > (WINDOW_HEIGHT - PADDLE_HEIGHT))
            {
                position.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
            }
        }

        void Draw(SDL_Renderer* renderer)
        {
            rect.y = static_cast<int>(position.y);

            SDL_RenderFillRect(renderer, &rect);
        }

        Vec2 position;
        SDL_Rect rect{};
        Vec2 velocity;
};

class PlayerScore
{
    public:
        PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font)
            : renderer(renderer), font(font)
        {
            surface = TTF_RenderText_Solid(font, "0", {0xFF, 0xFF, 0xFF, 0xFF});
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            int width, height;
            SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

            rect.x = static_cast<int>(position.x);
            rect.y = static_cast<int>(position.y);
            rect.w = width;
            rect.h = height;
        }

        ~PlayerScore()
        {
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        void SetScore(int score)
        {
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        
            surface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), {0xFF, 0xFF, 0xFF, 0xFF});
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            int width, height;
            SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
            rect.w = width;
            rect.h = height;
        }

        void Draw()
        {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        }


        SDL_Renderer* renderer;
        TTF_Font* font;
        SDL_Surface* surface{};
        SDL_Texture* texture{};
        SDL_Rect rect{};

};

Contact checkPaddleCollision(Ball const& ball, Paddle const& paddle)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    float paddleLeft = paddle.position.x;
    float paddleRight = paddle.position.x + PADDLE_WIDTH;
    float paddleTop = paddle.position.y;
    float paddleBottom = paddle.position.y + PADDLE_HEIGHT;

    Contact contact{};

    if (ballLeft >= paddleRight)
    {
        return contact;
    }
    if (ballRight <= paddleLeft)
    {
        return contact;
    }
    if (ballTop >= paddleBottom)
    {
        return contact;
    }
    if (ballBottom <= paddleTop)
    {
        return contact;
    }

    // actually collide
    float paddleRangeUpper = paddleBottom - (PADDLE_HEIGHT * 2.0f / 3.0f);
    float paddleRangeMiddle = paddleBottom - (PADDLE_HEIGHT / 3.0f);

    if (ball.velocity.x < 0)
    {
        // Left paddle
        contact.penetration = paddleRight - ballLeft;
    }
    else if (ball.velocity.x > 0)
    {
        // Right paddle
        contact.penetration = paddleLeft - ballRight;
    }

    if ((ballBottom > paddleTop) && (ballBottom < paddleRangeUpper))
    {
        contact.type = CollisionType::Top;
    }
    else if ((ballBottom > paddleRangeUpper) && (ballBottom < paddleRangeMiddle))
    {
        contact.type = CollisionType::Middle;
    }
    else
    {
        contact.type = CollisionType::Bottom;
    }

    return contact;
}

Contact checkWallCollision(Ball const& ball)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + BALL_WIDTH;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + BALL_HEIGHT;

    Contact contact {};

    if (ballTop < 0.0f)
    {
        contact.type = CollisionType::Top;
        contact.penetration = -ballTop;
    }
    else if (ballBottom > WINDOW_HEIGHT)
    {
        contact.type = CollisionType::Bottom;
        contact.penetration = WINDOW_HEIGHT - ballBottom;
    }
    else if (ballLeft < 0.0f)
    {
        contact.type = CollisionType::Left;
    }
    else if (ballRight > WINDOW_WIDTH)
    {
        contact.type = CollisionType::Right;
    }

    return contact;
}


int main()
{
    // Init SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *window = SDL_CreateWindow("Pong", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Init Font
    TTF_Font* scoreFont = TTF_OpenFont("bin/DejaVuSansMono.ttf", 20);

    // Init Sound
    Mix_Chunk* paddleHitSound = Mix_LoadWAV("bin/paddleHit.wav");
    Mix_Chunk* wallHitSound = Mix_LoadWAV("bin/wallHit.wav");

    // init ball
    Ball ball(
        Vec2((WINDOW_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f), 
            (WINDOW_HEIGHT / 2.0f) - (BALL_HEIGHT / 2.0f)),
        Vec2(BALL_SPEED, 0.0f));

    // create paddles
    Paddle paddleLeft(
        Vec2(20.0f, (WINDOW_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 4.0f)), 
        Vec2(0.0f, 0.0f));
    Paddle paddleRight(
        Vec2(WINDOW_WIDTH - 20.0f, (WINDOW_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 4.0f)),
        Vec2(0.0f, 0.0f));

    //create score
    PlayerScore playerLeftScoreText(Vec2(WINDOW_WIDTH / 4, 20), renderer, scoreFont);
    PlayerScore playerRightScoreText(Vec2(WINDOW_WIDTH * 3 / 4, 20), renderer, scoreFont);

    // Game logic
    {
        enum Buttons
        {
            paddleLeftUp = 0,
            paddleLeftDown,
            paddleRightUp,
            paddleRightDown,
        };

        const float PADDLE_SPEED = 1.0f;

        int playerLeftScore = 0;
        int playerRightScore = 0;

        bool running = true;
        bool buttons[4] = {};

        float dt = 0.0f;

        while (running)
        {
            auto startTime = std::chrono::high_resolution_clock::now();
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
                    else if (event.key.keysym.sym == SDLK_w)
                    {
                        buttons[Buttons::paddleLeftUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_s)
                    {
                        buttons[Buttons::paddleLeftDown] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_UP)
                    {
                        buttons[Buttons::paddleRightUp] = true;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        buttons[Buttons::paddleRightDown] = true;
                    }
                }
                else if (event.type == SDL_KEYUP)
                {
                    if (event.key.keysym.sym == SDLK_w)
                    {
                        buttons[Buttons::paddleLeftUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_s)
                    {
                        buttons[Buttons::paddleLeftDown] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_UP)
                    {
                        buttons[Buttons::paddleRightUp] = false;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        buttons[Buttons::paddleRightDown] = false;
                    }
                }
            }

            if (buttons[Buttons::paddleLeftUp])
            {
                paddleLeft.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::paddleLeftDown])
            {
                paddleLeft.velocity.y = PADDLE_SPEED;
            }
            else
            {
                paddleLeft.velocity.y = 0;
            }

            if (buttons[Buttons::paddleRightUp])
            {
                paddleRight.velocity.y = -PADDLE_SPEED;
            }
            else if (buttons[Buttons::paddleRightDown])
            {
                paddleRight.velocity.y = PADDLE_SPEED;
            }
            else
            {
                paddleRight.velocity.y = 0;
            }

            paddleLeft.Update(dt);
            paddleRight.Update(dt);

            ball.Update(dt);

            // check collision
            if (Contact contact = checkPaddleCollision(ball, paddleLeft);
                contact.type != CollisionType::None)
            {
                Mix_PlayChannel(-1, paddleHitSound, 0);
                ball.CollideWithPaddle(contact);
            }
            if (Contact contact = checkPaddleCollision(ball, paddleRight);
                contact.type != CollisionType::None)
            {
                Mix_PlayChannel(-1, paddleHitSound, 0);
                ball.CollideWithPaddle(contact);
            }
            if (Contact contact = checkWallCollision(ball); contact.type != CollisionType::None)
            {
                Mix_PlayChannel(-1, wallHitSound, 0);
                ball.CollideWithWall(contact);
                if (contact.type == CollisionType::Left)
                {
                    playerRightScore += 1;
                    playerRightScoreText.SetScore(playerRightScore);
                }
                if (contact.type == CollisionType::Right)
                {
                    playerLeftScore += 1;
                    playerLeftScoreText.SetScore(playerLeftScore);
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

            // render paddles
            paddleLeft.Draw(renderer);
            paddleRight.Draw(renderer);

            // render score text
            playerLeftScoreText.Draw();
            playerRightScoreText.Draw();

            // Present the backbuffer
            SDL_RenderPresent(renderer);

            auto stopTime = std::chrono::high_resolution_clock::now();
            dt = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();

        }
    }

    // Cleanup
    Mix_FreeChunk(wallHitSound);
    Mix_FreeChunk(paddleHitSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(scoreFont);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
