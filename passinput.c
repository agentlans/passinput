#include "passinput.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

#define CLEAR_LINE "\33[2K\r"
#define CTRL_V 22
#define CTRL_D 4

static struct termios oldt;
static bool term_setup = false;

static void restore_terminal(void) {
    if (term_setup) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        term_setup = false;
    }
}

static void handle_signal(int sig) {
    restore_terminal();
    //const char *msg = "\n[Input Interrupted]\n";
	//fprintf(stderr, "%s", msg);
    _exit(sig);
}

void secure_wipe(char *str, int len) {
    if (!str) return;
    volatile char *p = (volatile char *)str;
    while (len--) *p++ = 0;
}

static void render(const char *prompt, const char *buffer, int len, int cursor_pos, bool visible) {
    fprintf(stderr, CLEAR_LINE "%s ", prompt);
    if (visible) {
        fprintf(stderr, "%s", buffer);
    } else {
        for (int i = 0; i < len; i++) fputc('*', stderr);
    }
    // Positioning: prompt length + 1 space + cursor position + 1 (ANSI is 1-based)
    fprintf(stderr, "\33[%zuG", strlen(prompt) + 2 + cursor_pos);
    fflush(stderr);
}

char* get_password_input(const char *prompt, int max_len) {
    char *buffer = calloc(max_len + 1, sizeof(char));
    if (!buffer) return NULL;

    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        free(buffer);
        return NULL;
    }
    term_setup = true;

    // Set up signal handling
    struct sigaction sa = { .sa_handler = handle_signal, .sa_flags = SA_RESTART };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int cursor_pos = 0, current_len = 0;
    int status = 0; // 0: input, 1: success, 2: cancel/error
    bool visible = false;

    while (status == 0) {
        render(prompt, buffer, current_len, cursor_pos, visible);
        
        int c = getchar();
        if (c == EOF) {
            if (errno == EINTR) continue;
            status = 2;
            break;
        }

        switch (c) {
            case CTRL_V: visible = !visible; break;
            case CTRL_D: status = 2; break;
            case 10: case 13: status = 1; break; 
            case 127: case 8: // Backspace
                if (cursor_pos > 0) {
                    memmove(&buffer[cursor_pos - 1], &buffer[cursor_pos], current_len - cursor_pos + 1);
                    cursor_pos--; current_len--;
                }
                break;
            case 27: { // Escape sequence
                newt.c_cc[VMIN] = 0;
                newt.c_cc[VTIME] = 1; // 100ms timeout
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                
                int next = getchar();
                if (next == '[') {
                    int code = getchar();
                    if (code == 'D' && cursor_pos > 0) cursor_pos--;
                    else if (code == 'C' && cursor_pos < current_len) cursor_pos++;
                    else if (code == 'H') cursor_pos = 0;
                    else if (code == 'F') cursor_pos = current_len;
                    else if (code == '3' && getchar() == '~' && cursor_pos < current_len) {
                        memmove(&buffer[cursor_pos], &buffer[cursor_pos+1], current_len - cursor_pos);
                        current_len--;
                    }
                } else if (next == -1 || next == 27) {
                    status = 2; // ESC twice or timeout
                }

                newt.c_cc[VMIN] = 1;
                newt.c_cc[VTIME] = 0;
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                break;
            }
            default:
                if (c >= 32 && c <= 126 && current_len < max_len) {
                    memmove(&buffer[cursor_pos + 1], &buffer[cursor_pos], current_len - cursor_pos);
                    buffer[cursor_pos++] = (char)c;
                    current_len++;
                    buffer[current_len] = '\0';
                }
                break;
        }
    }

    restore_terminal();
    fputc('\n', stderr);

    if (status == 1) return buffer;
    
    secure_wipe(buffer, max_len);
    free(buffer);
    return NULL;
}
