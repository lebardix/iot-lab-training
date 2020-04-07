#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Add required includes here */
#include "fmt.h"
#include "shell.h"

#include "hashes/sha256.h"
#include "hashes/sha3.h"

/* Add here the buffers to store the SHA256 hash and its hexadecimal representation */
static uint8_t sha256_hash[SHA256_DIGEST_LENGTH];
static char sha256_hash_hex[SHA256_DIGEST_LENGTH * 2];

/* Add here the buffers to store the SHA3 hash and its hexadecimal representation */
static uint8_t sha3_hash[SHA3_256_DIGEST_LENGTH];
static char sha3_hash_hex[SHA3_256_DIGEST_LENGTH * 2];

/* Implement the SHA256 command handler here */
static int _sha256_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to hash>\n", argv[0]);
        return 1;
    }

    /* Compute the hash here */
    sha256_context_t sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, (uint8_t*)argv[1], strlen(argv[1]));
    sha256_final(&sha256, sha256_hash);

    /* Convert the hash array to a string of hex characters */
    fmt_bytes_hex(sha256_hash_hex, sha256_hash, SHA256_DIGEST_LENGTH);
    
    /* Print the hash */
    printf("SHA256: %s\n", sha256_hash_hex);

    return 0;
}


/* Implement the SHA3 command handler here */
static int _sha3_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to hash>\n", argv[0]);
        return 1;
    }

    /* Compute the hash here */
    keccak_state_t state;
    sha3_256_init(&state);
    sha3_update(&state, argv[1], strlen(argv[1]));
    sha3_256_final(&state, sha3_hash);
    
    /* Convert the hash array to a string in hex format */
    fmt_bytes_hex(sha3_hash_hex, sha3_hash, SHA3_256_DIGEST_LENGTH);
    
    /* Print the hash */
    printf("SHA3: %s\n", sha3_hash_hex);

    return 0;
}

/* Declare the list of shell commands */
static const shell_command_t shell_commands[] = {
    { "sha256", "Compute SHA256 hasg", _sha256_handler },
    { "sha3", "Compute SHA3 hash", _sha3_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Configure and start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
