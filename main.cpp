#include <iostream>
#include <raylib.h>

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 960
#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 100
#define BALL_RADIUS 10.0
#define FPS 60

class Player {
public:
        int x;
        int y;
        int move_speed;
        int score;

        Player(int initial_x) {
                x = initial_x;
                y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2;
                move_speed = 10;
                score = 0;
        }

        void reset() {
                y = WINDOW_HEIGHT / 2 - PLAYER_HEIGHT / 2;
        }

        void move(int *game_running, int key_up, int key_down) {
                if (*game_running) {
                        if (IsKeyDown(key_up) && y > 0) {
                                y -= move_speed;
                        } else if (IsKeyDown(key_down) && y + PLAYER_HEIGHT < WINDOW_HEIGHT)
                                y += move_speed;
                }
        }

        void draw() {
                DrawRectangle(x, y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
        }

        void increaseScore() {
                score += 1; 
        }
};

class Ball {
public:
        int x;
        int y;
        int x_speed;
        int y_speed;

        Ball() {
                x = WINDOW_WIDTH / 2;
                y = WINDOW_HEIGHT / 2;
                x_speed = 0;
                y_speed = 0;
        }

        void reset(int *game_running) {
                x = WINDOW_WIDTH / 2;
                y = WINDOW_HEIGHT/ 2;
                x_speed = 0;
                y_speed = 0;
                *game_running = 0;
        }

        void setSpeed(int speed) {
                if (x_speed > 0) {
                        x_speed = speed;
                } else {
                        x_speed = -speed;
                }
                if (y_speed > 0) {
                        y_speed = speed;
                } else {
                        y_speed = -speed;
                }
        }

        void move() {
                x += x_speed;
                y += y_speed;
        }
        void draw() {
                DrawCircle(x, y, BALL_RADIUS, RED);
        }

        void checkStartMoving(int *game_running) {
                //if (*game_running == 0 && IsKeyPressed(KEY_SPACE)) {
                if (*game_running == 0) {
                        setSpeed(6);
                        //x_speed *= -1;
                        *game_running = 1;
                }
        }

        int checkWallCollision() {
                if (y <= 0 || y >= WINDOW_HEIGHT) {
                        std::cout << "floor/ceiling collision at " << x << ":" << y << "\n";
                        y_speed *= -1;
                }
                if (x <= 0) {
                        //x_speed *= -1;
                        return 1;
                } else if (x >= WINDOW_WIDTH) {
                        //x_speed *= -1;
                        return 2;
                }
                
                return 0;
        }
        void checkPlayerCollision(Player player_1, Player player_2) {
               // Ball is going to the right
                int x_expected, y_expected;
                if (x_speed > 0) {
                        if (x > player_2.x - BALL_RADIUS &&
                            y < player_2.y + PLAYER_HEIGHT &&
                            y > player_2.y) {
                                x_speed *= -1;
                                setSpeed(12);
                                std::cout << "right collision at " << x << ":" << y << "\n";
                        }
                } else {
                        if (x < player_1.x + PLAYER_WIDTH + BALL_RADIUS &&
                            y < player_1.y + PLAYER_HEIGHT &&
                            y > player_1.y) {
                                x_speed *= -1;
                                setSpeed(12);
                                std::cout << "left collision at " << x << ":" << y << "\n";

                                x_expected = WINDOW_HEIGHT - y + x;
                                y_expected = WINDOW_WIDTH + x_expected - 1262;
                                std::cout << "x:" << x_expected << " y:" << y_expected << "\n";
                        }

                }
        }

        
};

        
enum MainMenuOptions {Singleplayer, Multiplayer, Options};
enum GameState {Menu, Running_Multiplayer, Running_Singleplayer, Paused};

class MainMenu {
public:
        const char *main_menu_names[3] = {"Singleplayer", "Multiplayer", "Options"};

        MainMenuOptions selected = Singleplayer;

        void menuDown() {
                if (selected == Options) {
                        selected = Singleplayer;
                } else {
                        selected = static_cast<MainMenuOptions>(selected + 1);
                }
        }

        void menuUp() {
                if (selected == Singleplayer) {
                        selected = Options;
                } else {
                        selected = static_cast<MainMenuOptions>(selected - 1);
                }
        }

        MainMenuOptions menuSelect() {
                return selected;
        }

        void draw() {
                for (int i = Singleplayer; i <= Options; i++) {
                        if (selected == i)
                                DrawCircle(WINDOW_WIDTH/2 - 97,  (i+1)*35 + 15 + WINDOW_HEIGHT/3, 5, RED);
                        DrawText(TextFormat("%s", main_menu_names[i]), WINDOW_WIDTH/2 - 90, (i+1)*35 + WINDOW_HEIGHT/3, 35, WHITE);
                }
        }

};


//void pauseGame(gameState *game_state) {
//        if (IsKeyPressed(KEY_SPACE)) {
//                if (*game_state == Running) {
//                        *game_state = Paused; 
//                }
//        }
//}


int main () {

        int game_running = 0;
        int collision = 0;
        Player player_1(PLAYER_WIDTH / 2);
        Player player_2(WINDOW_WIDTH - 1.5 * PLAYER_WIDTH);

        GameState game_state = Menu;
        MainMenu main_menu;
        MainMenuOptions menu_selected;

        Ball ball;

        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game");
        SetTargetFPS(60);

        // Game Loop
        while (WindowShouldClose() == false) {

                // 1. Event handling
                //
                if (game_state == Menu) {
                        if (IsKeyPressed(KEY_UP)) {
                                main_menu.menuUp();
                        } else if (IsKeyPressed(KEY_DOWN)) {
                                main_menu.menuDown();
                        } else if (IsKeyPressed(KEY_ENTER)) {
                                menu_selected = main_menu.menuSelect();
                                switch (menu_selected) {
                                case Singleplayer:
                                        break;
                                case Multiplayer:
                                        game_state = Running_Multiplayer;
                                        break;
                                case Options:
                                        break;

                                }
                        }
                }

                if (game_state == Running_Multiplayer) {
                        collision = ball.checkWallCollision();
                        if (collision == 1) {
                                player_2.increaseScore();
                                ball.reset(&game_running);
                        } else if (collision == 2) {
                                player_1.increaseScore();
                                ball.reset(&game_running);
                        }

                        ball.checkPlayerCollision(player_1, player_2);
                        ball.checkStartMoving(&game_running);

                        if (!game_running) {
                                player_1.reset();
                                player_2.reset();
                        }
                }
                // 2. Updating positions

                if (game_state == Running_Multiplayer) {
                        player_1.move(&game_running, KEY_W, KEY_S);
                        player_2.move(&game_running, KEY_UP, KEY_DOWN);
                        ball.move();
                }

                // 3. Drawing
                BeginDrawing();
                ClearBackground(BLACK);

                if (game_state == Running_Multiplayer) {
                        player_1.draw();
                        player_2.draw();
                        ball.draw();
                        DrawText(TextFormat("%i", player_1.score), WINDOW_WIDTH/4, 30, 50, WHITE);
                        DrawText(TextFormat("%i", player_2.score), 3*WINDOW_WIDTH/4, 20, 50, WHITE);
                }

                if (game_state == Menu) {
                        main_menu.draw();
                        //std::cout << "Menu\n";
                }

                EndDrawing();
        }

        CloseWindow();

        return 0;
}
