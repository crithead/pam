/**
 * A PAM module to print a notice to syslog.
 */

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include <security/pam_appl.h>
#include <security/pam_ext.h>

#define BUFLEN 256

/**
 * Collect command line arguments into a single string.
 *
 * @param buffer Output buffer
 * @param buflen Length of output buffer
 * @param argc Argument count
 * @param argv Argument vector
 */
static void _collect(char *buffer, int buflen, int argc, const char **argv)
{
    int i = 0, n = 0;

    if (buffer == NULL || buflen <= 0) {
        return;
    }

    buffer[0] = 0;

    for (i = 0; i < argc && n < buflen; i++) {
        n += snprintf(buffer + n, buflen - n, "%s%c", argv[i],
                      (i < argc - 1) ? ' ' : '\n');
    }

    buffer[buflen - 1] = 0;
}

/**
 * Print arguments to syslog at NOTICE.
 *
 * @param argc Argument count
 * @param argv Argument vector
 */
static void _log_args(pam_handle_t *pamh, int argc, const char **argv)
{
    char args[BUFLEN];
    _collect(args, BUFLEN, argc, argv);
    pam_syslog(pamh, LOG_NOTICE, "%s [%s]", __func__, args);
}

/* ********** Account Management ****************************************** */

int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
                     int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE;
}

/* ********** Authentication ********************************************** */

int pam_sm_authenticate(pam_handle_t *pamh, int flags,
                        int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE; // or PAM_SUCCESS
}

int pam_sm_setcred(pam_handle_t *pamh, int flags,
                   int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE;
}

/* ********** Session Management ****************************************** */

int pam_sm_open_session(pam_handle_t *pamh, int flags,
                        int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE;
}

int pam_sm_close_session(pam_handle_t *pamh, int flags,
                         int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE;
}

/* ********** Password Management ***************************************** */

int pam_sm_chauthtok(pam_handle_t *pamh, int flags,
                     int argc, const char **argv)
{
    _log_args(pamh, argc, argv);
    return PAM_IGNORE;
}

