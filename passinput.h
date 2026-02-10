#ifndef PASSINPUT_H
#define PASSINPUT_H

/**
 * @brief Prompts user for a password with hidden/masked input.
 * * Controls: 
 * - Ctrl+V: Toggle visibility
 * - Ctrl+D / Esc: Cancel
 * - Enter: Confirm
 * * @param prompt The string to display to the user.
 * @param max_len Maximum characters allowed.
 * @return Allocated string (must be freed by caller) or NULL on cancel/error.
 */
char* get_password_input(const char *prompt, int max_len);

/**
 * @brief Securely wipes a string from memory.
 */
void secure_wipe(char *str, int len);

#endif
