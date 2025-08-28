#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

#define HEIGHT 26
#define WIDTH 80

struct circle_t {
    double x, y, r;
    double direction_x, direction_y;
};

void clear_screen()
{
    printf("\x1b[2J\x1b[H");
}

void animation(struct circle_t *circle)
{
    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            double squared_distance = sqrt(pow((x - circle->x), 2) + pow((y - circle->y), 2));
            if(squared_distance < circle->r) {
                if( x % 2 == 1) {
                    printf("\x1b[38;2;0;140;0m#");
                    printf("\x1b[0m");
                } else {
                    printf("#");
                }
            } else {
                printf(".");
            }
        }

        putchar('\n');
    }
}

void on_sig(int signum)
{
    printf("\x1b[2J\x1b[H");
    printf("\x1b[?25h");

    printf("\nDetected signum: %d\n", signum);
    exit(0);
}

int main(void)
{
    /* On interrupt signal call this function */
    signal(SIGINT, on_sig);

    /* Circle attributes */
    struct circle_t circle;
    circle.y = HEIGHT / 2.f;
    circle.x = WIDTH / 2.f;
    circle.r = 4;
    circle.direction_y = .5f;
    circle.direction_x = .4f;
    int speed = 8;
    

    clear_screen();
    printf("\x1b[?25l");

    int stopper = 80; // While loop handler
    while(stopper > 0)
    {
        clear_screen();
        animation(&circle);
        
        circle.y += circle.direction_y * speed;
        circle.x += circle.direction_x * speed;

        if((circle.y - circle.r) <= 0 || circle.y >= HEIGHT - circle.r) { circle.direction_y *= -1; }
        else if(circle.x - circle.r <= 0 || circle.x >= WIDTH - circle.r) { circle.direction_x *= -1; }
        
        usleep(70000); /* make the thread sleep for 70000 micro seconds */
        stopper--; // Decrement so we don't rely on Interrupt signal to exit.
    }
    
    clear_screen();
    printf("\x1b[H");
    printf("\x1b[?25h");
    return 0;
}
