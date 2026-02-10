## `passinput.h` — Secure Masked CLI Input

A lightweight, dependency-free POSIX C library for secure, interactive password entry in the terminal.

### Key Features

* **Interactive UI**: Masked input (`*`) with real-time `Ctrl+V` visibility toggling.
* **Full Navigation**: Support for Arrow keys, `Home`, `End`, `Backspace`, and `Delete`.
* **Security Focused**: Disables echo/canonical mode, provides `secure_wipe`, and ensures terminal restoration via `SIGINT`/`SIGTERM` handlers.
* **Zero Dependencies**: Standard POSIX headers only (`termios.h`, `unistd.h`).

---

### Usage

**Installation:** Drop `passinput.h` and `passinput.c` into your project.

```c
#include "passinput.h"

int main() {
    int max = 64;
    char *pass = get_password_input("Enter Secret: ", max);

    if (pass) {
        // ... process ...
        secure_wipe(pass, max);
        free(pass);
    }
    return 0;
}

```

*Compile with: `gcc main.c passinput.c -o app*`

---

### Controls & API

| Key | Action |
| --- | --- |
| **Ctrl + V** | Toggle Mask/Plaintext |
| **Enter** | Confirm input |
| **Esc / Ctrl + D** | Cancel (returns `NULL`) |
| **Arrows / Home / End** | Navigate cursor |

#### Functions

* **`char* get_password_input(const char *prompt, int max_len)`**: Returns a heap-allocated string or `NULL`.
* **`void secure_wipe(char *str, int len)`**: Uses `volatile` pointers to overwrite memory, bypassing compiler optimization.

---

### Technical Considerations

* **POSIX Only**: Requires Linux/macOS; no native Windows support.
* **ASCII Target**: Not optimized for multi-byte UTF-8 or emojis.
* **Terminal Support**: Requires ANSI escape sequence compatibility for rendering.
* **Robustness**: Uses `tcsetattr` for terminal control and `\33[2K\r` for clean line refreshes.

---

### License

MIT

