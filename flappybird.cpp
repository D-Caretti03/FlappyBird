/*
 * Simple flappy bird game using only SDL_Rect to draw both the player and the obstacles
 * Press space to start the game, esc to quit
 * If esc is pressed during the game you return to the main menu
 *
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define HEIGHT 800
#define WIDTH 1500
#define WHITE 0xffffffff
#define BLACK 0x00000000
#define BLUE 0x0000ffff
#define GREEN 0x00ff00
#define RED 0xff0000
#define PLAYER_H 30
#define PLAYER_W 50
#define PLAYER_X (WIDTH / 4)
#define PLAYER_Y (HEIGHT / 2)
#define BLOCK_W 30
#define BLOCK_X (WIDTH-BLOCK_W) 
#define BLOCK_H (HEIGHT/2)

const float G = 0.5f;
const float JUMP = -9.0f;
int high_score = 0;
SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Renderer *renderer; 
TTF_Font *font;
TTF_Font *font_num;

class Player
{
    public:
        float x;
        float y;
        float w;
        float h;
        double vel_y;

        void jump()
        {
            this->vel_y = JUMP;
        }

        void update()
        {
            this->vel_y += G;
            this->y += this->vel_y;

            if( this->y < 0 )
            {
                this->y = 0;
                this->vel_y = -this->vel_y/2;
            }
        }

        bool ground_collision()
        {
            if( this->y + this->h/2 > HEIGHT )
            {
                return true;
            }

            return false;
        }

        void draw()
        {
            SDL_Rect rect = (SDL_Rect) {(int)this->x, (int)this->y, (int)this->w, (int)this->h};
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            SDL_RenderFillRect( renderer, &rect );
        }
        
    Player()
    {
        this->x = PLAYER_X;
        this->y = PLAYER_Y;
        this->w = PLAYER_W;
        this->h = PLAYER_H;
        this->vel_y = 0;
    }
};

class Block
{
    public:
        int x;
        int y;
        int w;
        int h;
        int vel_x;

        void draw()
        {
            SDL_Rect block = (SDL_Rect) {(int)this->x, (int)this->y, (int)this->w, (int)this->h};
            SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255);
            SDL_RenderFillRect( renderer, &block );
        }

        void update()
        {
            this->x -= this->vel_x;
        }

        bool isOffScreen() const
        {
            if( this->x + this->w < 0 )
                return true;
            return false;
        }
    
        Block(int y, int h)
            : x(BLOCK_X), y(y), w(BLOCK_W), h(h), vel_x(6) {}
};

bool init();
bool init_ttf();
bool init_renderer();
bool create_window();
void clear();
void game_loop();
void menu();
void game_over(Player player, int score );


bool init()
{
    bool success = true;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        success = false;
    }

    return success;
}

bool init_ttf()
{
    bool success = true;

    if( TTF_Init() == -1 )
    {
        success = false;
    }
    return success;
}

bool create_window()
{
    bool success = true;

    window = SDL_CreateWindow( "FLAPPY BIRD", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );

    if( window == NULL )
    {
        success = false;
    }

    return success;
}

bool init_renderer()
{
    bool success = true;

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if( !renderer )
    {
        success = false;
    }
    return success;
}

SDL_Texture *loadText( const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if( textSurface == NULL)
    {
        std::cerr << "Text Rendering Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
    SDL_FreeSurface( textSurface );
    
    return textTexture;
}

void renderText( SDL_Texture *texture, SDL_Renderer *renderer, int x, int y )
{
    int textW = 0;
    int textH = 0;

    SDL_QueryTexture( texture, NULL, NULL, &textW, &textH);

    SDL_Rect dstRect = { x, y, textW, textH };

    SDL_RenderCopy( renderer, texture, NULL, &dstRect );
}


void game_over(Player player, int score )
{
    SDL_Color color = {255, 255, 255, 255};

    std::string high_string;
    const char *c_high_string;

    std::string string = std::to_string(score);
    const char *c_string = string.c_str();

    if( score > high_score )
    {
        high_score = score;
    }
    
    high_string = std::to_string(high_score);
    c_high_string = high_string.c_str();

    SDL_Texture *gameOverText = loadText("Game Over", renderer, font, color );
    SDL_Texture *scoreText = loadText("Points", renderer, font, color );
    SDL_Texture *pointsText = loadText(c_string, renderer, font_num, color );
    SDL_Texture *highScoreText = loadText("High Score", renderer, font, color );
    SDL_Texture *highPointsText = loadText(c_high_string, renderer, font_num, color );
    SDL_Texture *restartText = loadText("Press SPACE to restart the game", renderer, font, color);
    SDL_Texture *quitText = loadText("Press ESC to exit the game", renderer, font, color);

    SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255 );
    SDL_RenderClear( renderer );

    renderText( gameOverText, renderer, 400, 200 );
    renderText( scoreText, renderer, 400, 280 );
    renderText( pointsText, renderer, 580, 296 );
    renderText( highScoreText, renderer, 400, 360 );
    renderText( highPointsText, renderer, 680, 376 );
    renderText( restartText, renderer, 400, 480 );
    renderText( quitText, renderer, 400, 580 );

    SDL_RenderPresent( renderer );

    SDL_DestroyTexture( gameOverText );
    SDL_DestroyTexture( pointsText );
    SDL_DestroyTexture( scoreText );
    SDL_DestroyTexture( restartText );
    SDL_DestroyTexture( quitText );
    SDL_DestroyTexture( highScoreText );
    SDL_DestroyTexture( highPointsText );

    SDL_Event e;

    while( true )
    {
        if( SDL_PollEvent( &e ) )
        {
            if( e.type == SDL_QUIT )
            {
                clear();
                exit(EXIT_SUCCESS);
            }
            else if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_SPACE:    game_loop();
                                        break;
                    
                    case SDLK_ESCAPE:   clear();
                                        exit(EXIT_SUCCESS);
                                        break;
                }
            }
        }
    }
}

void menu()
{
    SDL_Event e;

    bool play = false;

    while( !play )
    {
        if( SDL_PollEvent( &e ) )
        {
            if( e.type == SDL_QUIT )
            {
                play = true;
            }
            else if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_SPACE:    game_loop();
                                        break;
                    
                    case SDLK_ESCAPE:   clear();
                                        exit(EXIT_SUCCESS);

                    default:            break;
                }
            }
    
            SDL_Color color = {255, 255, 255, 255};
            SDL_Texture *gameText = loadText("Flappy Bird 0", renderer, font, color );
            SDL_Texture *startText = loadText("Press SPACE to start game", renderer, font, color );
            SDL_Texture *exitText = loadText("Press ESC to exit game", renderer, font, color);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear( renderer );

            renderText( gameText, renderer, 550, 300);
            renderText( startText, renderer, 400, 400);
            renderText( exitText, renderer, 400, 500); 

            SDL_RenderPresent( renderer );

            SDL_DestroyTexture( gameText );
            SDL_DestroyTexture( startText );
            SDL_DestroyTexture( exitText );
        }
    }
}

void game_loop()
{
    std::vector<Block> blocks;

    Player player;

    int frame = 0;

    SDL_Event e;

    int score = 0;

    while( true )
    {
        if( SDL_PollEvent( &e ) )
        {
            if( e.type == SDL_QUIT )
            {
                clear();
                exit(EXIT_SUCCESS);
            }
            else if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_SPACE:    player.jump();
                                        break;
                    
                    case SDLK_ESCAPE:   menu();
                                        break;
                    default:            break;
                }
            }
        }

        std::string sc = std::to_string(score);
        const char *s_c = sc.c_str();

        SDL_Color color = {255, 255, 255, 255};
        SDL_Texture *scoreText = loadText(s_c, renderer, font_num, color );

        SDL_SetRenderDrawColor( renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
 
        renderText( scoreText, renderer, 0, 0 );

        if( frame % 60 == 0 )
        {
            int h_rand = rand() % (HEIGHT / 2) + 50;
            blocks.emplace_back(0, h_rand);

            int h_inf = HEIGHT - h_rand - 150;
            blocks.emplace_back(h_rand+150, h_inf);

            if( frame > 120 )
                score ++;
        }


        player.draw();
        for( auto& obj: blocks)
        {
            obj.draw();
        }

        player.update();
        for( auto& obj: blocks)
        {
            obj.update();
        }

        blocks.erase(
            std::remove_if(blocks.begin(), blocks.end(),
                           [](const Block& b) { return b.isOffScreen(); }),
            blocks.end()
        );

        for( auto& block: blocks )
        {
            if( (player.ground_collision()) || ((player.x + player.w >= block.x && player.x <= block.x + block.w) && (player.y <= block.y + block.h && player.y + player.h >= block.y)))
            {
                game_over(player, score);
            }
        }
        
        SDL_RenderPresent( renderer );

        SDL_DestroyTexture( scoreText );

        SDL_Delay(16);      
        
        frame++;
    }
}

void clear()
{
    TTF_CloseFont( font );
    TTF_CloseFont( font_num );

    SDL_DestroyRenderer( renderer );
    renderer = NULL;

    SDL_FreeSurface( surface );
    surface = NULL;

    SDL_DestroyWindow( window );
    window = NULL;

    TTF_Quit();

    SDL_Quit();
}

int main( int argc, char *argv[] )
{
    srand(time(NULL));

    if( !init() )
    {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    if( !init_ttf() )
    {
        std::cerr << "TTF could not be initialized! Error: " << SDL_GetError() << std::endl;
        clear();
        exit(EXIT_FAILURE);
    }

    if( !create_window() )
    {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        clear();
        exit(EXIT_FAILURE);
    }

    if( !init_renderer() )
    {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        clear();
        exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont("flappybird.ttf", 96);

    if( !font )
    {
        std::cerr << "Errore nel caricamento del font: " << TTF_GetError() << std::endl;
    }

    font_num = TTF_OpenFont("numbers.ttf", 48);

    if( !font_num )
    {
        std::cerr<< "Errore nel caricamento del font :" << TTF_GetError() << std::endl;
    }

    menu();

    clear();
    
    return 0;
}
