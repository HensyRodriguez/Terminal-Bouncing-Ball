#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define HEIGHT 26
#define WIDTH 80
#define CIRCLES_SIZE 3
#define ANSI_COLORS_SIZE 3

#define MY_ANSI_RED   "\x1b[38;2;140;0;0m"
#define MY_ANSI_GREEN "\x1b[38;2;0;140;0m"
#define MY_ANSI_BLUE  "\x1b[38;2;0;0;140m"
#define ANSI_RESET_ATTRIB "\x1b[0m"


struct circle_t {
    double x, y, r;
    double direction_x, direction_y;
};

void clear_screen_and_topleft_cursor(void)
{
    printf("\x1b[J\x1b[H");
}

void circle_logic(struct circle_t circles[CIRCLES_SIZE])
{
    for(int i = 0; i < CIRCLES_SIZE; i++)
    {
        struct circle_t* circle = &circles[i];
        if(circle->y - circle->r < 0)
        {
            circle->direction_y *= -1;
            circle->y = circle->r;
        }

        if(circle->y + circle->r > HEIGHT) {
            circle->direction_y *= -1;
            circle->y = HEIGHT - circle->r;
        }

        if (circle->x - circle->r < 0) {
            circle->direction_x *= -1;
            circle->x = circle->r;
        }

        if (circle->x + circle->r > WIDTH) {
            circle->direction_x *= -1;
            circle->x = WIDTH - circle->r;
        }
        circle->y += circle->direction_y;
        circle->x += circle->direction_x;
    }
}

void animation(char screen[HEIGHT][WIDTH],const struct circle_t circles[CIRCLES_SIZE], const char* ANSI_COLORS[ANSI_COLORS_SIZE])
{
    const char* color_screen[HEIGHT][WIDTH];
    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0;x < WIDTH; x++)
        {
            screen[y][x] = '.';
            color_screen[y][x] = NULL;
        }
    }
    
    for(int i = 0; i < CIRCLES_SIZE; i++)
    {
        for(int y = 0; y < HEIGHT; y++)
        {
            for(int x = 0; x < WIDTH; x++)
            {
                struct circle_t circle = circles[i];
                double squared_distance = (x - circle.x) * (x - circle.x) +
                    (y - circle.y) * (y - circle.y);
                if(squared_distance < circle.r * circle.r)
                {
                    screen[y][x] = '#';
                    color_screen[y][x] = ANSI_COLORS[i];
                }
            }
        }
    }

    for(int y = 0; y < HEIGHT; y++)
    {
            
        for(int x = 0;x < WIDTH; x++)
        {
            if(color_screen[y][x]) {
                printf("%s%c%s", color_screen[y][x], screen[y][x], ANSI_RESET_ATTRIB);
            } else {
                printf("%c", screen[y][x]);
            }
        }
        putchar('\n');
    }
    
}

void on_sig(int signum)
{
    printf("\x1b[H");
    printf("\x1b[?25h");

    printf("\nDetected signum: %d\n", signum);
    exit(0);
}

int main(void)
{
    /* On interrupt signal call this function */
    signal(SIGINT, on_sig);

    struct circle_t Circle1 = {.y=HEIGHT/2.f, .x=WIDTH/2.f, .r=4, .direction_x=1,.direction_y=1};
    struct circle_t Circle2 = {.y=HEIGHT/2.f, .x=WIDTH/2.f, .r=2, .direction_x=-1,.direction_y=-1};
    struct circle_t Circle3 = {.y=HEIGHT/2.f, .x=WIDTH/2.f, .r=6, .direction_x=-3,.direction_y=-1};
    struct circle_t Circles[CIRCLES_SIZE] = {Circle1, Circle2, Circle3};

    const char* ANSI_COLORS[ANSI_COLORS_SIZE] = {MY_ANSI_RED, MY_ANSI_GREEN, MY_ANSI_BLUE};

    char screen[HEIGHT][WIDTH] ={};

    printf("Would you like it to be an infinite loop?(Y/N): ");
    char c = '\0';
    scanf("%c", &c);

    int infinite_loop_flag = 0;
    if(c == 'Y' || c == 'y') {
        infinite_loop_flag = 1;
    }

    clear_screen_and_topleft_cursor();
    printf("\x1b[?25l");

    if(infinite_loop_flag)
    {
        while(1)
        {
            clear_screen_and_topleft_cursor();
            animation(screen, Circles, ANSI_COLORS);
            circle_logic(Circles);

            fflush(stdout);
            usleep(70000); /* make the thread sleep for 70000 micro seconds */
        }
    } else {
        int stopper = 80;
        while (stopper > 0)
        {
            clear_screen_and_topleft_cursor();
            animation(screen, Circles, ANSI_COLORS);
            circle_logic(Circles);

            fflush(stdout);
            usleep(70000); /* make the thread sleep for 70000 micro seconds */
            --stopper;
        }
    }
    
    putchar('\n');
    clear_screen_and_topleft_cursor();
    printf("\x1b[H");
    printf("\x1b[?25h");
    return 0;
}
