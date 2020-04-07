#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Add required includes here */
#include "shell.h"
#include "fmt.h"
#include "edsign.h"
#include "ed25519.h"
#include "random.h"

/* Declare here the buffers where the asymmetric keys are stored */
static uint8_t secret_key[EDSIGN_SECRET_KEY_SIZE] = { 0 };
static uint8_t public_key[EDSIGN_PUBLIC_KEY_SIZE] = { 0 };

/* Declare here the buffers where the signature and its hexadecimal representation are stored */
static uint8_t signature[EDSIGN_SIGNATURE_SIZE] = { 0 };
static char signature_hex[EDSIGN_SIGNATURE_SIZE * 2] = { 0 };


/* Implement the key command handler here */
static int _key_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Create the new keypair */
    random_bytes(secret_key, sizeof(secret_key));
    ed25519_prepare(secret_key);
    edsign_sec_to_pub(public_key, secret_key);


    /* Print the new keypair */
    puts("New keypair generated:");
    printf("  - Secret: ");
    for (uint8_t i = 0; i < EDSIGN_SECRET_KEY_SIZE; ++i) {
      printf("%02X", secret_key[i]);
    }

    printf("\n  - Public: ");
    for (uint8_t i = 0; i < EDSIGN_PUBLIC_KEY_SIZE; ++i) {
      printf("%02X", public_key[i]);
    }
    puts("");

    return 0;
}

/* Implement the sign command handler here */
static int _sign_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <message>\n", argv[0]);
        return 1;
    }

    /* Clear signature temporary buffer content */
    memset(signature, 0, sizeof(signature));

    /* Generate the signature here */
    edsign_sign(signature, public_key, secret_key, (uint8_t *)argv[1], strlen(argv[1]));


    /* Convert the signature byte array to a string of hex characters */
    fmt_bytes_hex(signature_hex, signature, EDSIGN_SIGNATURE_SIZE);

    /* Print the signature */
    printf("%s\n", signature_hex);

    return 0;
}

/* Implement the verify command handler here */
static int _verify_handler(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <message> <signature>\n", argv[0]);
        return 1;
    }

    /* Clear signature temporary buffer content */
    memset(signature, 0, sizeof(signature));

    /* Convert the input signature in hex to a byte array here */
    fmt_hex_bytes(signature, argv[2]);

    /* Verify the signature here */
    if (edsign_verify(signature, public_key, (uint8_t *)argv[1], strlen(argv[1]))) {
      puts("Message verified");
    }
    else {
      puts("Message not verified");
    }

    return 0;
}

/* Declare the list of shell commands */
static const shell_command_t shell_commands[] = {
    { "key", "Generate a new pair of keys", _key_handler },
    { "sign", "Compute the signature of a message", _sign_handler },
    { "verify", "Verify the signature of a message", _verify_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Initialize the random seed */
    random_init(0);

    /* Configure and start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE + 32];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE + 32);

    return 0;
}
