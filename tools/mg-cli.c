/**
 * @file mg-cli.c
 * @brief MetaGraph command-line interface placeholder
 */

#include <stdio.h>

#include "metagraph/base.h"

// Function with local buffer to trigger stack protection
static void metagraph_process_input(const char *input) {
    char buffer[64]; // Stack buffer that should trigger protection
    if (input) {
        size_t input_length = 0U;
        while (input[input_length] != '\0' &&
               input_length < (sizeof(buffer) - 1U)) {
            ++input_length;
        }
        const size_t copied = mg_copy_bytes(buffer, sizeof(buffer), input,
                                            input_length, input_length);
        (void)copied;
        buffer[input_length] = '\0';
        (void)printf("Processing: %s\n", buffer);
    }
}

int main(int argc, char *argv[]) {
    (void)printf("MetaGraph CLI - placeholder implementation\n");

    // Use argc/argv to ensure they're not optimized away
    if (argc > 1) {
        metagraph_process_input(argv[1]);
    }

    // Create another stack buffer
    char local_buffer[128];
    const char version_string[] = "Version: 0.1.0";
    const size_t literal_length = sizeof(version_string) - 1U;
    (void)mg_copy_bytes(local_buffer, sizeof(local_buffer), version_string,
                        literal_length, literal_length);
    local_buffer[(literal_length < sizeof(local_buffer))
                     ? literal_length
                     : (sizeof(local_buffer) - 1U)] = '\0';
    (void)printf("%s\n", local_buffer);

    return 0;
}
