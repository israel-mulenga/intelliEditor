#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "editor/gap_buffer.h"

/* ============================================================
   ENGINEER'S ASSERTION FRAMEWORK
   ============================================================ */

int test_count = 0;
int test_passed = 0;
int test_failed = 0;

#define ASSERT(condition, message) \
    do { \
        test_count++; \
        if (condition) { \
            test_passed++; \
            printf("  ✓ [PASS] %s\n", message); \
        } else { \
            test_failed++; \
            printf("  ✗ [FAIL] %s\n", message); \
        } \
    } while(0)

#define ASSERT_EQUAL(actual, expected, message) \
    ASSERT((actual) == (expected), message)

#define ASSERT_STRING_EQUAL(str1, str2, message) \
    do { \
        const char* s1 = (str1) ? (str1) : ""; \
        const char* s2 = (str2) ? (str2) : ""; \
        ASSERT(strcmp(s1, s2) == 0, message); \
    } while(0)

#define TEST_SUITE(name) printf("\n🚀 RUNNING SUITE: %s\n%s\n", name, "------------------------------------------------")

/* ============================================================
   UTILITIES
   ============================================================ */

void insert_string(GapBuffer *gb, const char *str) {
    while (*str) gap_buffer_insert(gb, *str++);
}

/* ============================================================
   TEST SUITES
   ============================================================ */

void test_initialization() {
    TEST_SUITE("Buffer Initialization");
    size_t cap = 128;
    GapBuffer *gb = gap_buffer_create(cap);
    
    ASSERT(gb != NULL, "Buffer creation returns pointer");
    ASSERT_EQUAL(gb->size, cap, "Size matches initial capacity");
    ASSERT_EQUAL(gb->gap_start, 0, "Gap start initialized at 0");
    ASSERT_EQUAL(gb->gap_end, cap, "Gap end initialized at capacity (full gap)");
    
    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content, "", "New buffer is empty string");
    
    free(content);
    gap_buffer_destroy(gb);
}

void test_insertion_logic() {
    TEST_SUITE("Insertion & Sequential Growth");
    GapBuffer *gb = gap_buffer_create(10);
    
    insert_string(gb, "Hello");
    
    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content, "Hello", "Content matches inserted string");
    ASSERT_EQUAL(gb->gap_start, 5, "gap_start advanced by 5");
    ASSERT_EQUAL(gb->gap_end, 10, "gap_end remained static");
    
    free(content);
    gap_buffer_destroy(gb);
}

void test_backspace_vs_delete() {
    TEST_SUITE("Deletion Logic (Backspace vs Delete)");
    GapBuffer *gb = gap_buffer_create(20);
    
    insert_string(gb, "IntelliEditor"); // Cursor is after 'r'
    
    // Test Backspace (Delete from left)
    gap_buffer_backspace(gb); // Removes 'r'
    char *c1 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c1, "IntelliEdito", "Backspace removed last character");
    
    // Test Delete (Delete from right)
    gap_buffer_move_cursor(gb, 7); // Cursor between 'i' and 'E'
    // Buffer state: "Intelli" [GAP] "Edito"
    gap_buffer_delete(gb); // Should remove 'E'
    char *c2 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c2, "Intellidito", "Delete removed character at right of cursor");
    
    free(c1); free(c2);
    gap_buffer_destroy(gb);
}

void test_cursor_movement_and_editing() {
    TEST_SUITE("Complex Cursor Navigation");
    GapBuffer *gb = gap_buffer_create(20);
    
    insert_string(gb, "AC");
    gap_buffer_move_cursor(gb, 1); // Move between A and C
    gap_buffer_insert(gb, 'B');    // Insert B
    
    char *c1 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c1, "ABC", "Insert in middle works");
    
    gap_buffer_move_cursor(gb, 0); // Move to start
    gap_buffer_insert(gb, '>');
    char *c2 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c2, ">ABC", "Prepend works");
    
    gap_buffer_move_cursor(gb, 4); // Move to end
    gap_buffer_insert(gb, '!');
    char *c3 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c3, ">ABC!", "Append works");
    
    free(c1); free(c2); free(c3);
    gap_buffer_destroy(gb);
}

void test_resizing_durability() {
    TEST_SUITE("Memory Integrity (Resize)");
    GapBuffer *gb = gap_buffer_create(4); // Very small
    
    insert_string(gb, "Lubumbashi"); // Triggers resizes
    
    ASSERT(gb->size > 4, "Buffer size increased");
    char *content = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(content, "Lubumbashi", "Content preserved after multiple resizes");
    
    // Verify cursor still works after resize
    gap_buffer_move_cursor(gb, 5);
    gap_buffer_insert(gb, '-');
    char *c2 = gap_buffer_get_content(gb);
    ASSERT_STRING_EQUAL(c2, "Lubum-bashi", "Editing still works after resize");
    
    free(content); free(c2);
    gap_buffer_destroy(gb);
}

void test_edge_cases() {
    TEST_SUITE("Security & Edge Cases");
    GapBuffer *gb = gap_buffer_create(10);
    
    // Deleting from empty
    gap_buffer_backspace(gb);
    gap_buffer_delete(gb);
    ASSERT_EQUAL(gb->gap_start, 0, "Backspace on empty buffer is safe");
    
    // Overflow protection
    gap_buffer_move_cursor(gb, 500); // Should be caught by your overflow check
    ASSERT(gb->gap_start <= 10, "Cursor cannot overflow text length");
    
    gap_buffer_destroy(gb);
}

/* ============================================================
   MAIN RUNNER
   ============================================================ */

int main() {
    printf("\n================================================\n");
    printf("   INTELLIEDITOR - CORE ENGINE UNIT TESTS\n");
    printf("================================================\n");

    test_initialization();
    test_insertion_logic();
    test_backspace_vs_delete();
    test_cursor_movement_and_editing();
    test_resizing_durability();
    test_edge_cases();

    printf("\n\n================================================\n");
    printf("FINAL REPORT\n");
    printf("Tests Run:    %d\n", test_count);
    printf("Tests Passed: %d\n", test_passed);
    printf("Tests Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n", (test_passed * 100.0 / test_count));
    printf("================================================\n\n");

    return (test_failed > 0);
}