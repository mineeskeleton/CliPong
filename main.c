#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int program_should_close = 0;

struct object {
    int x;
    int y;
    int size_x;
    int size_y;
};

char screen[10][20];

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int put_object_in_screen(struct object obj, char visual) {
    for (int i = obj.y; i < obj.y + obj.size_y && i < 10; i++) {
        for (int j = obj.x; j < obj.x + obj.size_x && j < 20; j++) {
            screen[i][j] = visual;
        }
    }
    return 0;
}

int screen_draw() {
    printf("\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int clear_screen() {
    system("clear");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            screen[i][j] = ' ';
        }
    }
    return 0;
}

int main() {
    struct object paddle = {0, 1, 1, 3};
    struct object paddle2 = {19, 1, 1, 3};
    struct object ball = {10, 5, 1, 1};
    int ball_velocity_x = 1;
    int ball_velocity_y = 1;
    int points = 0;
    
    while (!program_should_close) {
        paddle2.y = ball.y - paddle2.size_y / 2;
        if (paddle2.y < 0) paddle2.y = 0;
        if (paddle2.y > 10 - paddle2.size_y) paddle2.y = 10 - paddle2.size_y;
        
        ball.x += ball_velocity_x;
        ball.y += ball_velocity_y;
        
        if (ball.y < 0 || ball.y >= 10) {
            ball_velocity_y = -ball_velocity_y;
            ball.y += ball_velocity_y;
        }
        
        if (ball.x == paddle.x + paddle.size_x) {
            if (ball.y >= paddle.y && ball.y < paddle.y + paddle.size_y) {
                points++;
                ball_velocity_x = -ball_velocity_x;
                ball.x = paddle.x + paddle.size_x;
            }
        }
        
        if (ball.x == paddle2.x) {
            if (ball.y >= paddle2.y && ball.y < paddle2.y + paddle2.size_y) {
                ball_velocity_x = -ball_velocity_x;
                ball.x = paddle2.x;
            } else {
                ball.x = 10;
                ball.y = 5;
                points++;
                ball_velocity_x = -ball_velocity_x;
            }
        }
        
        if (ball.x < 0) {
            ball.x = 10;
            ball.y = 5;
            if (points > 0) {
                points--;
            }
            ball_velocity_x = -ball_velocity_x;
        }
        
        if (kbhit()) {
            char c = getchar();
            if (c == 'w' && paddle.y > 0) {
                paddle.y--;
            }
            if (c == 's' && paddle.y < 10 - paddle.size_y) {
                paddle.y++;
            }
        }
        
        clear_screen();
        put_object_in_screen(ball, 'O');
        put_object_in_screen(paddle, 'I');
        put_object_in_screen(paddle2, 'I');
        printf("\nPoints: %d", points);
        screen_draw();
        
        usleep(75000);
    }
    
    return 0;
}
