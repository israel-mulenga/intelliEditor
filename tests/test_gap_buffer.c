#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "editor/gap_buffer.h"

/* ============================================================
   SIMPLE ASSERTION FRAMEWORK FOR TESTING
   ============================================================ */

int test_count = 0;
int test_passed = 0;
int test_failed = 0;

#define ASSERT(condition, message) \
    do { \
        test_count++; \
        if (condition) { \
            test_passed++; \
            printf("✓ PASS: %s\n", message); \
        } else { \
            test_failed++; \
            printf("✗ FAIL: %s\n", message); \
        } \
    } while(0)

#define ASSERT_EQUAL(actual, expected, message) \
    ASSERT((actual) == (expected), message)

#define ASSERT_NOT_NULL(ptr, message) \
    ASSERT((ptr) != NULL, message)

#define ASSERT_NULL(ptr, message) \
    ASSERT((ptr) == NULL, message)

#define ASSERT_STRING_EQUAL(str1, str2, message) \
    ASSERT(strcmp(str1, str2) == 0, message)

#define TEST_SUITE(name) \
    printf("\n========================================\n"); \
    printf("TEST SUITE: %s\n", name); \
    printf("========================================\n")

#define TEST_END \
    printf("\n========================================\n"); \
    printf("SUMMARY: %d/%d tests passed\n", test_passed, test_count); \
    printf("========================================\n")

/* ============================================================
   TEST HELPER FUNCTIONS
   ============================================================ */

/**
 * Helper: Print buffer contents for debugging
 */
void print_buffer_state(GapBuffer *gb, const char *label) {
    if (!gb) {
        printf("[%s] Buffer is NULL\n", label);
        return;
    }
    printf("[%s] Gap: [%zu, %zu), Size: %zu, Content: '%s'\n",
           label, gb->gap_start, gb->gap_end, gb->size,
           gap_buffer_get_content(gb) ? gap_buffer_get_content(gb) : "NULL");
}

/**
 * Helper: Get the actual content length (excluding gap)
 */
size_t get_content_length(GapBuffer *gb) {
    char *content = gap_buffer_get_content(gb);
    if (!content) return 0;
    size_t len = strlen(content);
    free(content);
    return len;
}

/* ============================================================
   TEST 1: BUFFER CREATION AND INITIALIZATION
   ============================================================ */

void test_buffer_creation() {
    TEST_SUITE("Buffer Creation");

    // Test 1.1: Create buffer with positive capacity
    GapBuffer *gb = gap_buffer_create(100);
    ASSERT_NOT_NULL(gb, "Buffer should be created successfully");
    ASSERT_EQUAL(gb->size, 100, "Buffer size should match initial capacity");
    ASSERT_EQUAL(gb->gap_start, 0, "Gap start should be 0");
    ASSERT_EQUAL(gb->gap_end, 0, "Gap end should be 0");

    // Test 1.2: Buffer should have valid buffer pointer
    ASSERT_NOT_NULL(gb->buffer, "Internal buffer should be allocated");

    // Test 1.3: Create buffer with small capacity
    gap_buffer_destroy(gb);
    gb = gap_buffer_create(10);
    ASSERT_NOT_NULL(gb, "Buffer with small capacity should be created");
    ASSERT_EQUAL(gb->size, 10, "Small buffer size should be correct");

    gap_buffer_destroy(gb);

    // Test 1.4: Create buffer with large capacity
    gb = gap_buffer_create(10000);
    ASSERT_NOT_NULL(gb, "Buffer with large capacity should be created");
    ASSERT_EQUAL(gb->size, 10000, "Large buffer size should be correct");

    gap_buffer_destroy(gb);

    TEST_END;
}

/* ============================================================
   TEST 2: SINGLE CHARACTER INSERTION
   ============================================================ */

void test_single_insertion() {
    TEST_SUITE("Single Character Insertion");

    GapBuffer *gb = gap_buffer_create(100);

    // Test 2.1: Insert single character
    gap_buffer_insert(gb, 'H');
    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "H",
                        "Buffer should contain 'H' after insert");
    free(content);

    // Test 2.2: Cursor position should advance
    ASSERT_EQUAL(gb->gap_start, 1, "Gap start should advance after insert");

    // Test 2.3: Insert another character
    gap_buffer_insert(gb, 'i');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Hi",
                        "Buffer should contain 'Hi'");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 3: MULTIPLE CHARACTER INSERTION
   ============================================================ */

void test_multiple_insertions() {
    TEST_SUITE("Multiple Character Insertion");

    GapBuffer *gb = gap_buffer_create(100);

    // Test 3.1: Insert multiple characters (simple string)
    const char *test_string = "Hello";
    for (size_t i = 0; i < strlen(test_string); i++) {
        gap_buffer_insert(gb, test_string[i]);
    }

    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Hello",
                        "Buffer should contain 'Hello'");
    free(content);

    // Test 3.2: Cursor should be at correct position
    ASSERT_EQUAL(gb->gap_start, 5, "Gap start should be 5 after 5 insertions");

    // Test 3.3: Insert longer string
    gap_buffer_destroy(gb);
    gb = gap_buffer_create(100);

    const char *long_string = "The quick brown fox";
    for (size_t i = 0; i < strlen(long_string); i++) {
        gap_buffer_insert(gb, long_string[i]);
    }

    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "The quick brown fox",
                        "Buffer should contain long string");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 4: CHARACTER DELETION
   ============================================================ */

void test_deletion() {
    TEST_SUITE("Character Deletion");

    GapBuffer *gb = gap_buffer_create(100);

    // Test 4.1: Insert and delete character
    gap_buffer_insert(gb, 'A');
    gap_buffer_insert(gb, 'B');
    gap_buffer_insert(gb, 'C');

    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "ABC",
                        "Buffer should contain 'ABC' before delete");
    free(content);

    // Test 4.2: Delete last character
    gap_buffer_delete(gb);
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "AB",
                        "Buffer should contain 'AB' after delete");
    free(content);

    // Test 4.3: Delete another character
    gap_buffer_delete(gb);
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "A",
                        "Buffer should contain 'A'");
    free(content);

    // Test 4.4: Delete remaining character
    gap_buffer_delete(gb);
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "",
                        "Buffer should be empty after deleting all");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 5: CURSOR MOVEMENT
   ============================================================ */

void test_cursor_movement() {
    TEST_SUITE("Cursor Movement");

    GapBuffer *gb = gap_buffer_create(100);

    // Test 5.1: Insert some text
    const char *text = "Hello World";
    for (size_t i = 0; i < strlen(text); i++) {
        gap_buffer_insert(gb, text[i]);
    }

    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Hello World",
                        "Initial text should be correct");
    free(content);

    // Test 5.2: Move cursor to beginning
    gap_buffer_move_cursor(gb, 0);
    ASSERT_EQUAL(gb->gap_start, 0, "Cursor should be at position 0");

    // Test 5.3: Insert at beginning should prepend
    gap_buffer_insert(gb, '[');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "[Hello World",
                        "Character should be inserted at beginning");
    free(content);

    // Test 5.4: Move cursor to middle
    gap_buffer_move_cursor(gb, 6); // Position after "[Hello"
    gap_buffer_insert(gb, '(');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "[Hello(World",
                        "Character should be inserted at middle position");
    free(content);

    // Test 5.5: Move cursor to end and insert
    gap_buffer_move_cursor(gb, 12); // End of string
    gap_buffer_insert(gb, ']');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "[Hello(World]",
                        "Character should be inserted at end");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 6: BUFFER RESIZING
   ============================================================ */

void test_buffer_resizing() {
    TEST_SUITE("Buffer Resizing");

    // Test 6.1: Create small buffer and fill it
    GapBuffer *gb = gap_buffer_create(10);
    ASSERT_NOT_NULL(gb, "Small buffer should be created");

    // Insert many characters to trigger resize
    for (int i = 0; i < 20; i++) {
        gap_buffer_insert(gb, 'A' + (i % 26));
    }

    // Test 6.2: Buffer should resize automatically
    ASSERT(gb->size >= 20, "Buffer should have resized to hold 20 characters");

    // Test 6.3: Content should still be intact
    char *content = gap_buffer_get_content(gb);
    ASSERT_NOT_NULL(content, "Content should be retrievable after resize");
    ASSERT_EQUAL(strlen(content ? content : ""), 20,
                 "Content length should be 20 after 20 insertions");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 7: EDGE CASES
   ============================================================ */

void test_edge_cases() {
    TEST_SUITE("Edge Cases");

    // Test 7.1: Delete from empty buffer (should not crash)
    GapBuffer *gb = gap_buffer_create(100);
    gap_buffer_delete(gb); // Should handle gracefully
    ASSERT(gb != NULL, "Buffer should still be valid after delete on empty");

    // Test 7.2: Move cursor to position 0 on empty buffer
    gap_buffer_move_cursor(gb, 0);
    ASSERT_EQUAL(gb->gap_start, 0, "Cursor at 0 should be valid");

    // Test 7.3: Insert special characters
    gap_buffer_insert(gb, '\t');
    gap_buffer_insert(gb, '\n');
    gap_buffer_insert(gb, ' ');
    char *content = gap_buffer_get_content(gb);
    ASSERT_NOT_NULL(content, "Special characters should be handled");
    free(content);

    // Test 7.4: Insert at various positions in small buffer
    gap_buffer_destroy(gb);
    gb = gap_buffer_create(20);

    gap_buffer_insert(gb, 'A');
    gap_buffer_insert(gb, 'C');
    gap_buffer_move_cursor(gb, 1);
    gap_buffer_insert(gb, 'B');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "ABC",
                        "Insertion at middle position should work");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   TEST 8: REPEATED OPERATIONS
   ============================================================ */

void test_repeated_operations() {
    TEST_SUITE("Repeated Operations");

    GapBuffer *gb = gap_buffer_create(100);

    // Test 8.1: Insert, move, insert pattern
    gap_buffer_insert(gb, 'a');
    gap_buffer_insert(gb, 'b');
    gap_buffer_move_cursor(gb, 0);
    gap_buffer_insert(gb, 'X');

    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Xab",
                        "Insert-move-insert pattern should work");
    free(content);

    // Test 8.2: Delete and reinsert
    gap_buffer_move_cursor(gb, 3);
    gap_buffer_delete(gb);
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Xa",
                        "Delete should remove last char");
    free(content);

    gap_buffer_insert(gb, 'c');
    content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content ? content : "", "Xac",
                        "Reinsert after delete should work");
    free(content);

    gap_buffer_destroy(gb);
    TEST_END;
}

/* ============================================================
   MAIN TEST RUNNER
   ============================================================ */

int main() {
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║        GAP BUFFER TEST SUITE             ║\n");
    printf("║  Comprehensive Unit Tests for Editor     ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    // Run all test suites
    test_buffer_creation();
    test_single_insertion();
    test_multiple_insertions();
    test_deletion();
    test_cursor_movement();
    test_buffer_resizing();
    test_edge_cases();
    test_repeated_operations();

    // Print final summary
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║           FINAL TEST SUMMARY             ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ Total Tests:  %d                         ║\n", test_count);
    printf("║ Passed:       %d  ✓                      ║\n", test_passed);
    printf("║ Failed:       %d  ✗                      ║\n", test_failed);
    printf("║ Success Rate: %.1f%%                      ║\n",
           test_count > 0 ? (test_passed * 100.0 / test_count) : 0.0);
    printf("╚══════════════════════════════════════════╝\n\n");

    return test_failed > 0 ? 1 : 0;
}
