/**
 * An example program that shows how to use PAM to authenticate a user.
 *
 * pam_start(3)
 * pam_end(3)
 */

#include <stdbool.h>
#include <pam.h>
#include <stdio.h>

static bool authenticate(void);


int main(int argc, const char **argv)
{

    if (authenticate()) {
        // Allow
    } else {
        // Log error and exit
    }

    return 0;
}

static bool authenticate(void)
{
    bool value = true;

    pam_start()

    pam_acct_mgmt();

    pam_open_session();
    pam_close_session();


    pam_end();

    return value;
}

