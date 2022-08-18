#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <algorithm>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
struct Window {
    int width;
    int height;
    SDL_Renderer *renderer;
    SDL_Window *win;
    TTF_Font *score_font;
};

struct Inputs {
    bool UP;
    bool DOWN;
    bool PAUSE;
};

struct {
    int width;
    int height;
} Resolutions[] = {
        {640, 480},
        {800, 600},
        {1280, 720}
};

struct Vec2f {
    float x;
    float y;
};

struct Ball {
    Vec2f pos;
    Vec2f dir;
    float radius;
    float speed;
};

bool init_sdl_win (SDL_Renderer *&renderer, SDL_Window *&win, int screen_width, int screen_height/*, TTF_Font *&score_font*/) {
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Couldn't init SDL", SDL_GetError());
        return false;
    }
/*
    TTF_Init();
    //create font
    score_font = TTF_OpenFont("DejaVuSans.ttf",40);
    if (TTF_Init() == -1) {
        printf("Couldn't initialize TTF");
        return false;
    }
    */
    win = SDL_CreateWindow("Pong",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, win_flags);
    if(!win) {
        printf("Failed to open %d x %d window: %s\n", screen_width, screen_height, SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(win, -1, render_flags);

    if(!renderer) {
        printf("Failed to create Renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

int random_number(int high, int low) {
    return rand() % high + low;
}
bool intersection (int x, int y, SDL_Rect player) {
    if(x > player.x &&
       y > player.y &&
       x < player.x + player.w &&
       y < player.y + player.h) {
        return true;
    }
    return false;
}

void check_ball_collisions(Ball &ball, int screen_height, SDL_Rect &paddle_p1, SDL_Rect &paddle_p2) {

    //wall collisions
    if (ball.pos.y < ball.radius) {
        ball.pos.y = ball.radius;
        ball.dir.y = -ball.dir.y;
    } else if (ball.pos.y > screen_height - ball.radius) {
        ball.pos.y = screen_height - ball.radius;
        ball.dir.y = -ball.dir.y;
    }
    //player collisions
    /*
    if(intersection(ball.pos.x, ball.pos.y, paddle_p1) == true ||
            intersection(ball.pos.x +))
*/
}
void move_ball(Ball &ball, float dt, int screen_width) {
    ball.pos.x += ball.dir.x * dt * ball.speed;
    ball.pos.y += ball.dir.y * dt * ball.speed;

}

void move_player(Inputs key_press, float *player_y_pos, float player_speed, float delta_time) {
    char dir = 0;
    if (key_press.UP)   dir -= 1;
    if (key_press.DOWN) dir += 1;
    *player_y_pos += dir * (player_speed * delta_time);
}

void handle_input(bool *is_running, Inputs *key_press) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:   key_press->UP   = (event.type == SDL_KEYDOWN); break;
                case SDLK_DOWN: key_press->DOWN = (event.type == SDL_KEYDOWN); break;
            }
        }

        if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            *is_running = false;
        }
    }
}

void destroy_window(SDL_Renderer *renderer, SDL_Window *win/*, TTF_Font *score_font*/) {
    SDL_DestroyRenderer(renderer);
    //TTF_CloseFont(score_font);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

SDL_Rect init_player(int paddle_x_pos, int paddle_y_pos, int paddle_height, int paddle_width) {
    SDL_Rect player;
    player.h = paddle_height;
    player.w = paddle_width;
    player.x = paddle_x_pos;
    player.y = paddle_y_pos;

    return player;
}
void draw_player(SDL_Renderer *renderer, SDL_Rect *player) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, player);
}
void draw_ball(SDL_Renderer *renderer, const Ball &ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect ball_rect = {
            (int)(ball.pos.x - ball.radius),
            (int)(ball.pos.y - ball.radius),
            (int)(ball.radius * 2),
            (int)(ball.radius * 2)
    };

    SDL_RenderFillRect(renderer, &ball_rect);
}

float clamp(float value, float min, float max) {
    return std::min(std::max(value, min), max);
}

void player_wall_collision(int screen_height, float &player_y, int paddle_height) {
    player_y = clamp(player_y, 0.0f, screen_height - paddle_height);
}


int main() {
    const SDL_MessageBoxButtonData buttons[] = {
            { /* .flags, .button id, .text */        0, 0, "640x480" },
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "800x600" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "1280x720" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
            { /* .colors (.r, .g, .b) */
                    /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                    { 255,   0,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                    {   0, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                    { 255, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                    {   0,   0, 255 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                    { 255,   0, 255 }
            }
    };
    const SDL_MessageBoxData message_box_data = {
            SDL_MESSAGEBOX_INFORMATION, /* .flags */
            nullptr, /* window */
            "Set screen resolution", /*  title of menu box */
            "Pick a Resolution of your choice :", /* Display message */
            SDL_arraysize(buttons), /* Number of Buttons */
            buttons, /* .buttons */
            &colorScheme /* .colorScheme */
    };
    int button_id;

    if (SDL_ShowMessageBox(&message_box_data, &button_id) < 0) {
        SDL_Log("error displaying message box");
    }
    /*
    else
    {
        printf("resolution is %d x %d", Resolutions[button_id].width, Resolutions[button_id].height);
    }
    if (button_id == -1) {
        SDL_Log("no resolution was picked");
    }
*/


    Window application;
    application.width = Resolutions[button_id].width;
    application.height = Resolutions[button_id].height;
    bool is_running =  init_sdl_win(application.renderer, application.win, application.width, application.height/*, application.score_font*/);

    //player params
    const int player_1_start_x_pos = 20;
    const int player_start_y_pos = application.height / 2;
    const int paddle_height = 75;
    const int paddle_width = 15;
    const int player_2_start_x_pos = application.width - player_1_start_x_pos;

    //ball params
    const int ball_start_x_pos = application.width / 2;
    const int ball_start_y_pos = application.height / 2;

    SDL_Rect p1 = init_player(player_1_start_x_pos, player_start_y_pos - (paddle_height / 2), paddle_height, paddle_width);
    SDL_Rect p2 = init_player(player_2_start_x_pos, player_start_y_pos - (paddle_height / 2), paddle_height, paddle_width);



    const float PADDLE_SPEED = 500.0f;
    float BALL_SPEED = PADDLE_SPEED / 6.0f;
    Inputs keys = {};

    float player_y = player_start_y_pos;

    Ball ball = { {(float)ball_start_x_pos, (float)ball_start_y_pos}, {-1.0f, 0.0f}, 5.0f, BALL_SPEED };;

    Uint32 now = SDL_GetTicks();
    srand(time(nullptr));
    while (is_running) {
        Uint32 last = now;
        now = SDL_GetTicks();
        const float dt = (now - last) / 1000.0f;

        handle_input(&is_running, &keys);
        p1.y = player_y;
        move_player(keys, &player_y, PADDLE_SPEED, dt);
        move_ball(ball, dt, application.width);
        check_ball_collisions(ball, application.height, p1, p2);

        player_wall_collision(application.height, player_y, p1.h);

        SDL_SetRenderDrawColor(application.renderer, 0, 0, 0, 255);
        SDL_RenderClear(application.renderer);
        draw_player(application.renderer, &p1);
        draw_player(application.renderer, &p2);
        draw_ball(application.renderer, ball);

        SDL_RenderPresent(application.renderer);
    }
    destroy_window(application.renderer, application.win/*, application.score_font*/);

    return 0;
}
