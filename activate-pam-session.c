/**
 * Activate PAM Session.
 *
 * Authenticate the user, open a PAM session management, then close it.
 *
 * pam(3), pam_start(3), pam_end(3)
 * pam_authenticate(3), pam_open_session(3), pam_close_session(3)
 */

#include <getopt.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct options {
    bool verbose;
    bool print_usage;
    const char *program_name;
    const char *user_name;
};

static bool activate(struct options *);
static void get_options(int argc, char **, struct options *);
static char *get_program_name(char *);
static void print_usage(struct options *);

static struct pam_conv conv = {
    misc_conv,  /* Use the default conversation function from pam_misc */
    NULL
};

/** Default program name to use in printing usage info. */
static const char DEFAULT_PROGRAM_NAME[] = "activate-pam-session";

/**
 * main - Activate a PAM Session.
 * @param[in] argc Argument count
 * @param[in] Argument vector
 * @retval 0 Success
 * @retval 1 Failure
 */
int main(int argc, char **argv)
{
    struct options opts = {0};
    get_options(argc, argv, &opts);

    if (opts.print_usage) {
        print_usage(&opts);
        return 0;
    }

    if (activate(&opts)) {
        if (opts.verbose) {
            printf("PAM session activated for %s\n", opts.program_name);
        }
    } else {
        fprintf(stderr, "PAM session activation failed for %s\n", opts.program_name);
        return 1;
    }

    return 0;
}

/**
 * Open and close a PAM session, for an authenticated user.
 * @param[in] opts Options structure
 * @retval true Success
 * @retval false Failure
 */
static bool activate(struct options *opts)
{
    pam_handle_t *pam_handle = NULL;
    int pam_result = PAM_SUCCESS;

    if (opts->verbose) {
        printf("program_name: %s\n", opts->program_name ? opts->program_name : "NULL");
        printf("user_name: %s\n",  opts->user_name ? opts->user_name : "NULL" );
    }

    if (opts->program_name == NULL) {
        fprintf(stderr, "No service name");
        return false;
    }

    if (opts->user_name == NULL) {
        fprintf(stderr, "No user name");
        return false;
    }

    pam_result = pam_start(opts->program_name, opts->user_name, &conv, &pam_handle);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "pam_start: %s\n", pam_strerror(pam_handle, pam_result));
        return false;
    }
    if (opts->verbose) {
        puts("PAM started");
    }

    pam_result = pam_authenticate(pam_handle, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "pam_authenticate: %s\n", pam_strerror(pam_handle, pam_result));
        pam_end(pam_handle, pam_result);
        return false;
    }
    if (opts->verbose) {
        printf("User authenticated: %s\n", opts->user_name);
    }

    pam_result = pam_open_session(pam_handle, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "pam_open_session: %s\n", pam_strerror(pam_handle, pam_result));
        pam_end(pam_handle, pam_result);
        return false;
    }
    if (opts->verbose) {
        puts("Session opened");
    }

    pam_result = pam_close_session(pam_handle, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "pam_close_session: %s\n", pam_strerror(pam_handle, pam_result));
        pam_end(pam_handle, pam_result);
        return false;
    }
    if (opts->verbose) {
        puts("Session closed");
    }

    pam_end(pam_handle, pam_result);
    if (opts->verbose) {
        puts("PAM ended");
    }

    return true;
}

/**
 * Parse the program arguments to set options.
 * And get the program name from argv[0].
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 * @param[out] opts An intialized options structure
 */
static void get_options(int argc, char **argv, struct options *opts)
{
    extern char *optarg;

    if (argv == NULL || opts == NULL) {
        return;
    }

    /* Set defaults */
    opts->print_usage = false;
    opts->verbose = false;
    opts->program_name = get_program_name(argv[0]);
    opts->user_name = NULL;

    static const char SHORT_OPTIONS[] = "hu:v";
    static struct option LONG_OPTIONS[] = {
        { "help",        no_argument,        0,  'h' },
        { "user",        required_argument,  0,  'u' },
        { "verbose",     no_argument,        0,  'v' },
        { 0,             0,                  0,   0 }
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &option_index);

    while (c != -1) {
        switch (c) {
        case 'h':
            opts->print_usage = true;
            break;
        case 'u':
            opts->user_name = optarg;
            break;
        case 'v':
            opts->verbose = true;
            break;
        default:
            break;
        }
        c = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS, &option_index);
    }

    if (opts->user_name == NULL) {
        opts->user_name = getenv("USER");
    }
}

/**
 * Trim path from argv[0] to get the program name.
 * @param[in] argv0 The first program argument, the name used to launch the program.
 * @return Pointer to the start of the program name.
 */
static char *get_program_name(char *argv0)
{
    if (argv0 == NULL) {
        return "<program-name>";
    }

    char *p = strrchr(argv0, '/');
    if (p != NULL && strlen(p + 1) > 0) {
        return p + 1;
    }

    return argv0;
}

/**
 * Print a helpful message to the console.
 * @param[in] opts Program options structure
 */
static void print_usage(struct options *opts)
{
    static const char USAGE[] = "\
    %s - call PAM to open and close a session\n\
\n\
    %s [ -h | --help ] | [ --v | --verbose ] [ -u NAME | --user NAME ]\n\
\n\
        -h, --help          Print usage message and exit\n\
        -v, --verbose       Enable verbose output\n\
        -u, --user NAME     The user to activate the session for\n\
\n\
";

    const char *pn = DEFAULT_PROGRAM_NAME;
    if (opts != NULL && opts->program_name != NULL) {
        pn = opts->program_name;
    }

    printf(USAGE, pn, pn);
}

