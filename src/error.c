/**
 * @file error.c
 * @brief Implementation of error handling and context management
 *
 * Thread-local error contexts are cached for the lifetime of each thread.
 * The memory is intentionally not freed on thread exit to avoid complexity
 * with thread cleanup handlers. The OS will reclaim the memory when the
 * thread terminates.
 */

#include "metagraph/base.h"
#include "metagraph/result.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// C23 thread-local storage for error context
// Note: This memory is cached per-thread and not freed until thread exit
static _Thread_local metagraph_error_context_t *thread_error_context = NULL;

// Get thread-local error context, creating if necessary
static metagraph_error_context_t *metagraph_get_thread_error_context(void) {
    if (!thread_error_context) {
        thread_error_context = calloc(1, sizeof(metagraph_error_context_t));
        // If allocation fails, we can't store error context but that's OK
        // The functions will handle NULL context gracefully
    }
    return thread_error_context;
}

// Error string lookup table
typedef struct {
    metagraph_result_t code;
    const char *message;
} error_string_entry_t;

static const error_string_entry_t METAGRAPH_ERROR_STRINGS[] = {
    // Success codes
    {METAGRAPH_SUCCESS, "Success"},
    {METAGRAPH_SUCCESS_PARTIAL, "Partial success"},
    // Memory errors
    {METAGRAPH_ERROR_OUT_OF_MEMORY, "Out of memory"},
    {METAGRAPH_ERROR_INVALID_ALIGNMENT, "Invalid alignment"},
    {METAGRAPH_ERROR_POOL_EXHAUSTED, "Memory pool exhausted"},
    {METAGRAPH_ERROR_FRAGMENTATION, "Memory too fragmented"},
    // Parameter errors
    {METAGRAPH_ERROR_INVALID_ARGUMENT, "Invalid argument"},
    {METAGRAPH_ERROR_NULL_POINTER, "Null pointer"},
    {METAGRAPH_ERROR_INVALID_SIZE, "Invalid size"},
    {METAGRAPH_ERROR_INVALID_ALIGNMENT_VALUE, "Invalid alignment value"},
    {METAGRAPH_ERROR_BUFFER_TOO_SMALL, "Buffer too small"},
    // Graph structure errors
    {METAGRAPH_ERROR_NODE_NOT_FOUND, "Node not found"},
    {METAGRAPH_ERROR_EDGE_NOT_FOUND, "Edge not found"},
    {METAGRAPH_ERROR_NODE_EXISTS, "Node already exists"},
    {METAGRAPH_ERROR_EDGE_EXISTS, "Edge already exists"},
    {METAGRAPH_ERROR_CIRCULAR_DEPENDENCY, "Circular dependency detected"},
    {METAGRAPH_ERROR_GRAPH_CORRUPTED, "Graph corrupted"},
    {METAGRAPH_ERROR_MAX_NODES_EXCEEDED, "Maximum nodes exceeded"},
    {METAGRAPH_ERROR_MAX_EDGES_EXCEEDED, "Maximum edges exceeded"},
    // I/O and bundle errors
    {METAGRAPH_ERROR_IO_FAILURE, "I/O failure"},
    {METAGRAPH_ERROR_FILE_NOT_FOUND, "File not found"},
    {METAGRAPH_ERROR_FILE_ACCESS_DENIED, "File access denied"},
    {METAGRAPH_ERROR_BUNDLE_CORRUPTED, "Bundle corrupted"},
    {METAGRAPH_ERROR_BUNDLE_VERSION_MISMATCH, "Bundle version mismatch"},
    {METAGRAPH_ERROR_CHECKSUM_MISMATCH, "Checksum mismatch"},
    {METAGRAPH_ERROR_COMPRESSION_FAILED, "Compression failed"},
    {METAGRAPH_ERROR_MMAP_FAILED, "Memory mapping failed"},
    // Concurrency errors
    {METAGRAPH_ERROR_LOCK_TIMEOUT, "Lock timeout"},
    {METAGRAPH_ERROR_DEADLOCK_DETECTED, "Deadlock detected"},
    {METAGRAPH_ERROR_CONCURRENT_MODIFICATION, "Concurrent modification"},
    {METAGRAPH_ERROR_THREAD_CREATION_FAILED, "Thread creation failed"},
    {METAGRAPH_ERROR_ATOMIC_OPERATION_FAILED, "Atomic operation failed"},
    // Algorithm errors
    {METAGRAPH_ERROR_TRAVERSAL_LIMIT_EXCEEDED, "Traversal limit exceeded"},
    {METAGRAPH_ERROR_INFINITE_LOOP_DETECTED, "Infinite loop detected"},
    {METAGRAPH_ERROR_DEPENDENCY_CYCLE, "Dependency cycle"},
    {METAGRAPH_ERROR_TOPOLOGICAL_SORT_FAILED, "Topological sort failed"},
    // System errors
    {METAGRAPH_ERROR_PLATFORM_NOT_SUPPORTED, "Platform not supported"},
    {METAGRAPH_ERROR_FEATURE_NOT_AVAILABLE, "Feature not available"},
    {METAGRAPH_ERROR_RESOURCE_EXHAUSTED, "Resource exhausted"},
    {METAGRAPH_ERROR_PERMISSION_DENIED, "Permission denied"},
    // Internal errors
    {METAGRAPH_ERROR_INTERNAL_STATE, "Internal state error"},
    {METAGRAPH_ERROR_ASSERTION_FAILED, "Assertion failed"},
    {METAGRAPH_ERROR_NOT_IMPLEMENTED, "Not implemented"},
    {METAGRAPH_ERROR_VERSION_MISMATCH, "Version mismatch"},
};

#define METAGRAPH_ERROR_STRING_COUNT                                           \
    (sizeof(METAGRAPH_ERROR_STRINGS) / sizeof(METAGRAPH_ERROR_STRINGS[0]))
// Ensure table stays in sync with enum
static_assert(METAGRAPH_ERROR_STRING_COUNT == 44,
              "Add new error codes to error_strings table when extending "
              "metagraph_result_t");

#ifdef __has_attribute
#if __has_attribute(cold)
#define METAGRAPH_ATTR_COLD __attribute__((cold))
#endif
#endif
#ifndef METAGRAPH_ATTR_COLD
#define METAGRAPH_ATTR_COLD
#endif

#ifdef __has_attribute
#if __has_attribute(const)
#define METAGRAPH_ATTR_CONST __attribute__((const))
#endif
#endif
#ifndef METAGRAPH_ATTR_CONST
#define METAGRAPH_ATTR_CONST
#endif

#ifndef METAGRAPH_ATTR_COLD_CONST
#define METAGRAPH_ATTR_COLD_CONST METAGRAPH_ATTR_COLD METAGRAPH_ATTR_CONST
#endif

METAGRAPH_ATTR_COLD_CONST
const char *metagraph_result_to_string(metagraph_result_t result) {
    // Linear search through the table (fine for ~50 entries)
    // If table grows beyond ~200 entries, consider binary search
    const size_t count = METAGRAPH_ERROR_STRING_COUNT;
    for (size_t i = 0; i < count; i++) {
        if (METAGRAPH_ERROR_STRINGS[i].code == result) {
            return METAGRAPH_ERROR_STRINGS[i].message;
        }
    }

    // Handle user-defined range
    if (result >= METAGRAPH_ERROR_USER_DEFINED_START &&
        result <= METAGRAPH_ERROR_USER_DEFINED_END) {
        return "User-defined error";
    }

    return "Unknown error";
}

#ifdef __has_attribute
#if __has_attribute(format)
#define METAGRAPH_ATTR_PRINTF(fmt_index, arg_index)                            \
    __attribute__((format(printf, fmt_index, arg_index)))
#endif
#endif
#ifndef METAGRAPH_ATTR_PRINTF
#define METAGRAPH_ATTR_PRINTF(fmt_index, arg_index)
#endif

static void metagraph_write_message(metagraph_error_context_t *context,
                                    const char *format, va_list args)
    METAGRAPH_ATTR_PRINTF(2, 0);

typedef struct {
    char *buffer;
    size_t capacity;
    size_t position;
    bool truncated;
} metagraph_message_builder_t;

static void metagraph_builder_init(metagraph_message_builder_t *builder,
                                   char *buffer, size_t capacity) {
    builder->buffer = buffer;
    builder->capacity = capacity;
    builder->position = 0U;
    builder->truncated = false;
    mg_zero_buffer(buffer, capacity);
}

static void metagraph_builder_append_char(metagraph_message_builder_t *builder,
                                          char character) {
    if (builder->truncated) {
        return;
    }
    if (builder->capacity == 0U) {
        builder->truncated = true;
        return;
    }
    if (builder->position + 1U >= builder->capacity) {
        builder->buffer[builder->capacity - 1U] = '\0';
        builder->truncated = true;
        return;
    }
    builder->buffer[builder->position++] = character;
    builder->buffer[builder->position] = '\0';
}

static void
metagraph_builder_append_string(metagraph_message_builder_t *builder,
                                const char *text) {
    const char *source = text ? text : "(null)";
    while (*source != '\0' && !builder->truncated) {
        metagraph_builder_append_char(builder, *source);
        ++source;
    }
}

static void
metagraph_builder_append_unsigned(metagraph_message_builder_t *builder,
                                  unsigned long long value, unsigned base,
                                  bool uppercase) {
    if (base < 2U) {
        base = 10U;
    } else if (base > 16U) {
        base = 16U;
    }
    char digits[64];
    _Static_assert((_Bool)(sizeof(digits) >= 64U),
                   "digits buffer must be at least 64 bytes");
    const char *alphabet = "0123456789abcdef";
    if (uppercase) {
        alphabet = "0123456789ABCDEF";
    }
    size_t length = 0U;
    do {
        digits[length++] = alphabet[value % base];
        value /= base;
    } while (value != 0U && length < sizeof(digits));

    while (length > 0U && !builder->truncated) {
        metagraph_builder_append_char(builder, digits[--length]);
    }
}

static void
metagraph_builder_append_signed(metagraph_message_builder_t *builder,
                                long long value) {
    unsigned long long magnitude;
    if (value < 0) {
        metagraph_builder_append_char(builder, '-');
        magnitude = (unsigned long long)(-(value + 1)) + 1U;
    } else {
        magnitude = (unsigned long long)value;
    }
    metagraph_builder_append_unsigned(builder, magnitude, 10U, false);
}

static void
metagraph_builder_append_pointer(metagraph_message_builder_t *builder,
                                 const void *ptr) {
    metagraph_builder_append_string(builder, "0x");
    if (builder->truncated) {
        return;
    }
    uintptr_t value = (uintptr_t)ptr;
    metagraph_builder_append_unsigned(builder, value, 16U, false);
}

static void
metagraph_builder_append_ellipsis(metagraph_message_builder_t *builder) {
    if (!builder->truncated || builder->capacity <= 4U) {
        return;
    }
    builder->buffer[builder->capacity - 4U] = '.';
    builder->buffer[builder->capacity - 3U] = '.';
    builder->buffer[builder->capacity - 2U] = '.';
    builder->buffer[builder->capacity - 1U] = '\0';
}

typedef enum {
    METAGRAPH_LENGTH_NONE,
    METAGRAPH_LENGTH_LONG,
    METAGRAPH_LENGTH_LONG_LONG
} metagraph_length_modifier_t;

static metagraph_length_modifier_t
metagraph_parse_length(const char **cursor_ptr) {
    const char *cursor = *cursor_ptr;
    metagraph_length_modifier_t length = METAGRAPH_LENGTH_NONE;
    if (*cursor == 'l') {
        ++cursor;
        if (*cursor == 'l') {
            length = METAGRAPH_LENGTH_LONG_LONG;
            ++cursor;
        } else {
            length = METAGRAPH_LENGTH_LONG;
        }
    }
    *cursor_ptr = cursor;
    return length;
}

static unsigned long long
metagraph_extract_unsigned(va_list *args, metagraph_length_modifier_t length) {
    switch (length) {
    case METAGRAPH_LENGTH_LONG_LONG:
        return va_arg(*args, unsigned long long);
    case METAGRAPH_LENGTH_LONG:
        return va_arg(*args, unsigned long);
    case METAGRAPH_LENGTH_NONE:
    default:
        return (unsigned long long)va_arg(*args, unsigned int);
    }
}

static long long metagraph_extract_signed(va_list *args,
                                          metagraph_length_modifier_t length) {
    switch (length) {
    case METAGRAPH_LENGTH_LONG_LONG:
        return va_arg(*args, long long);
    case METAGRAPH_LENGTH_LONG:
        return va_arg(*args, long);
    case METAGRAPH_LENGTH_NONE:
    default:
        return (long long)va_arg(*args, int);
    }
}

static bool
metagraph_builder_append_format(metagraph_message_builder_t *builder,
                                const char **cursor_ptr, va_list *args) {
    const char *cursor = *cursor_ptr;
    metagraph_length_modifier_t length = metagraph_parse_length(&cursor);
    const char specifier = *cursor;
    if (specifier == '\0') {
        *cursor_ptr = cursor;
        return true;
    }
    ++cursor;

    bool error = false;
    switch (specifier) {
    case 's':
        metagraph_builder_append_string(builder, va_arg(*args, const char *));
        break;
    case 'd':
    case 'i':
        metagraph_builder_append_signed(builder,
                                        metagraph_extract_signed(args, length));
        break;
    case 'u':
        metagraph_builder_append_unsigned(
            builder, metagraph_extract_unsigned(args, length), 10U, false);
        break;
    case 'x':
    case 'X': {
        const bool uppercase = (bool)(specifier == 'X');
        metagraph_builder_append_unsigned(
            builder, metagraph_extract_unsigned(args, length), 16U, uppercase);
        break;
    }
    case 'p':
        metagraph_builder_append_pointer(builder, va_arg(*args, const void *));
        break;
    default:
        error = true;
        break;
    }

    *cursor_ptr = cursor;
    return error;
}

static void metagraph_write_message(metagraph_error_context_t *context,
                                    const char *format, va_list args) {
    if (!context) {
        return;
    }
    metagraph_message_builder_t builder;
    metagraph_builder_init(&builder, context->message,
                           sizeof(context->message));

    if (!format) {
        return;
    }

    va_list local_args;
    va_copy(local_args, args);

    const char *cursor = format;
    while (*cursor != '\0' && !builder.truncated) {
        if (*cursor != '%') {
            metagraph_builder_append_char(&builder, *cursor);
            ++cursor;
            continue;
        }
        ++cursor;
        if (*cursor == '%') {
            metagraph_builder_append_char(&builder, '%');
            ++cursor;
            continue;
        }

        if (metagraph_builder_append_format(&builder, &cursor, &local_args)) {
            static const char error_msg[] = "<format error>";
            metagraph_builder_init(&builder, context->message,
                                   sizeof(context->message));
            metagraph_builder_append_string(&builder, error_msg);
            break;
        }
    }

    va_end(local_args);
    metagraph_builder_append_ellipsis(&builder);
}

static metagraph_result_t metagraph_set_error_context_v(
    metagraph_result_t code, const char *file, int line,
    const char *function, // NOLINT(bugprone-easily-swappable-parameters)
    const char *format,   // NOLINT(bugprone-easily-swappable-parameters)
    va_list args) METAGRAPH_ATTR_PRINTF(5, 0);

static metagraph_result_t metagraph_set_error_context_v(
    metagraph_result_t code, const char *file, int line,
    const char *function, // NOLINT(bugprone-easily-swappable-parameters)
    const char *format,   // NOLINT(bugprone-easily-swappable-parameters)
    va_list args) {
    metagraph_error_context_t *context = metagraph_get_thread_error_context();
    if (!context) {
        return code;
    }

    context->code = code;
    context->file = file;
    context->line = line;
    context->function = function;

    metagraph_write_message(context, format, args);

    context->detail = NULL;
    context->detail_size = 0;

    return code;
}

METAGRAPH_ATTR_COLD
METAGRAPH_ATTR_PRINTF(5, 6)
metagraph_result_t metagraph_set_error_context(
    metagraph_result_t code, const char *file, int line,
    const char *function, // NOLINT(bugprone-easily-swappable-parameters)
    const char *format, ...) {
    va_list args;
    va_start(args, format);
    metagraph_result_t result =
        metagraph_set_error_context_v(code, file, line, function, format, args);
    va_end(args);
    return result;
}

metagraph_result_t
metagraph_get_error_context(metagraph_error_context_t *context) {
    if (!context) {
        return METAGRAPH_ERROR_NULL_POINTER;
    }

    metagraph_error_context_t *thread_context =
        metagraph_get_thread_error_context();
    if (!thread_context) {
        // No context available (allocation failed), return success with empty
        // context
        mg_zero_buffer(context, sizeof(*context));
        context->code = METAGRAPH_SUCCESS;
        return METAGRAPH_SUCCESS;
    }

    // If no error has been set, return success with clear context
    if (thread_context->code == METAGRAPH_SUCCESS) {
        mg_zero_buffer(context, sizeof(*context));
        context->code = METAGRAPH_SUCCESS;
        return METAGRAPH_SUCCESS;
    }

    // Copy the error context
    *context = *thread_context;
    return METAGRAPH_SUCCESS;
}

void metagraph_clear_error_context(void) {
    metagraph_error_context_t *context = thread_error_context;
    if (context) {
        mg_zero_buffer(context, sizeof(metagraph_error_context_t));
        context->code = METAGRAPH_SUCCESS;
        // Note: We intentionally keep the allocated memory for reuse
        // rather than freeing it. This avoids repeated allocations
        // and the complexity of thread cleanup handlers.
    }
}

// Optional: Call this before thread exit to free the thread-local storage
// Not required as the OS will reclaim memory on thread termination
#ifdef METAGRAPH_EXPOSE_THREAD_CLEANUP
void metagraph_thread_cleanup(void) {
    if (thread_error_context) {
        free(thread_error_context);
        thread_error_context = NULL;
    }
}
#endif
