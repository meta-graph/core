/**
 * @file mg-cli.c
 * @brief MetaGraph command-line interface placeholder
 */

#include <stdio.h>
#include <string.h>

// Function with local buffer to trigger stack protection
static void metagraph_process_input(const char *input) {
    char buffer[64]; // Stack buffer that should trigger protection
    if (input) {
        size_t index = 0U;
        while (input[index] != '\0' && index + 1U < sizeof(buffer)) {
            buffer[index] = input[index];
            ++index;
        }
        buffer[index] = '\0';
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
    const char prefix[] = "Version: ";
    const char version[] = "0.1.0";
    size_t offset = 0U;
    while (prefix[offset] != '\0' && offset + 1U < sizeof(local_buffer)) {
        local_buffer[offset] = prefix[offset];
        ++offset;
    }
    size_t version_index = 0U;
    while (version[version_index] != '\0' &&
           offset + 1U < sizeof(local_buffer)) {
        local_buffer[offset++] = version[version_index++];
    }
    local_buffer[offset] = '\0';
    (void)printf("%s\n", local_buffer);

    return 0;
}
