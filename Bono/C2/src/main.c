#include <curses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int x;
    int y;
} Pos;

static Pos ball_pos;
static Pos x_pos;
static int max_rows, max_cols;

static pthread_mutex_t pos_mtx = PTHREAD_MUTEX_INITIALIZER;
static volatile bool running = true;
static volatile bool won = false;

static void clamp_ball(Pos *p) {
    if (p->x < 1) p->x = 1;
    if (p->y < 1) p->y = 1;
    if (p->x > max_cols - 2) p->x = max_cols - 2;
    if (p->y > max_rows - 2) p->y = max_rows - 2;
}

static void *keyboard_thread(void *arg) {
    (void)arg;
    nodelay(stdscr, TRUE);  // non-blocking getch
    int ch;
    while (running) {
        ch = getch();
        if (ch == ERR) {
            usleep(20000);
            continue;
        }
        pthread_mutex_lock(&pos_mtx);
        switch (ch) {
            case KEY_LEFT:  ball_pos.x -= 1; break;
            case KEY_RIGHT: ball_pos.x += 1; break;
            case KEY_UP:    ball_pos.y -= 1; break;
            case KEY_DOWN:  ball_pos.y += 1; break;
            case 'q':
            case 'Q':
                running = false;
                break;
            default:
                break;
        }
        clamp_ball(&ball_pos);
        pthread_mutex_unlock(&pos_mtx);
    }
    return NULL;
}

static void random_step(Pos *p) {
    int dx = (rand() % 3) - 1;  // -1, 0, 1
    int dy = (rand() % 3) - 1;
    p->x += dx;
    p->y += dy;
    if (p->x < 1) p->x = 1;
    if (p->y < 1) p->y = 1;
    if (p->x > max_cols - 2) p->x = max_cols - 2;
    if (p->y > max_rows - 2) p->y = max_rows - 2;
}

static void *x_mover_thread(void *arg) {
    (void)arg;
    while (running) {
        usleep(800000);  // 800 ms
        pthread_mutex_lock(&pos_mtx);
        random_step(&x_pos);
        pthread_mutex_unlock(&pos_mtx);
    }
    return NULL;
}

static void *collision_thread(void *arg) {
    (void)arg;
    while (running) {
        pthread_mutex_lock(&pos_mtx);
        if (ball_pos.x == x_pos.x && ball_pos.y == x_pos.y) {
            won = true;
            running = false;
        }
        pthread_mutex_unlock(&pos_mtx);
        usleep(50000);  // check ~20 fps
    }
    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    getmaxyx(stdscr, max_rows, max_cols);

    pthread_mutex_lock(&pos_mtx);
    ball_pos.x = max_cols / 4;
    ball_pos.y = max_rows / 2;
    x_pos.x = (3 * max_cols) / 4;
    x_pos.y = max_rows / 2;
    pthread_mutex_unlock(&pos_mtx);

    pthread_t th_keys, th_x, th_col;
    pthread_create(&th_keys, NULL, keyboard_thread, NULL);
    pthread_create(&th_x, NULL, x_mover_thread, NULL);
    pthread_create(&th_col, NULL, collision_thread, NULL);

    while (running) {
        pthread_mutex_lock(&pos_mtx);
        Pos b = ball_pos;
        Pos x = x_pos;
        pthread_mutex_unlock(&pos_mtx);

        clear();
        mvprintw(0, 0, "Move with arrows, q to quit");
        mvaddch(b.y, b.x, 'o');
        mvaddch(x.y, x.x, 'X');
        refresh();
        usleep(50000);  // ~20 fps
    }

    // Show result before exiting
    clear();
    if (won) {
        mvprintw(max_rows / 2, (max_cols - 12) / 2, "You win! :)");
    } else {
        mvprintw(max_rows / 2, (max_cols - 12) / 2, "Bye!");
    }
    refresh();
    usleep(1000000);

    pthread_join(th_keys, NULL);
    pthread_join(th_x, NULL);
    pthread_join(th_col, NULL);

    endwin();
    return 0;
}