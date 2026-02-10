#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passinput.h"

int main() {
    fprintf(stderr, "Log in to System\n");
    char *pw = get_password_input("Enter Secret:", 64);

    if (pw) {
        // Here you would hash/use the password
        printf("Success! Received password of length %zu\n", strlen(pw));
        
        secure_wipe(pw, 64);
        free(pw);
    } else {
        fprintf(stderr, "Action cancelled.\n");
        return 1;
    }
    return 0;
}
