/*
 * Custom Allocator Example
 * Demonstrates how to use custom memory allocator with PPHC
 *
 * Use cases:
 * - Bare-metal/embedded systems without standard malloc
 * - Memory-constrained environments with fixed pools
 * - Custom debugging/tracking allocators
 * - WebAssembly with custom memory management
 */

#include <pph/pph_calculator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================
   Example 1: Tracking Allocator
   Wraps standard malloc/free to track usage
   ============================================ */

static size_t total_allocated = 0;
static size_t total_freed = 0;
static size_t current_allocated = 0;
static size_t peak_allocated = 0;
static int allocation_count = 0;
static int free_count = 0;

void* tracking_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr != NULL) {
        total_allocated += size;
        current_allocated += size;
        allocation_count++;

        if (current_allocated > peak_allocated) {
            peak_allocated = current_allocated;
        }

        printf("[ALLOC] %p: %zu bytes (current: %zu, peak: %zu)\n",
               ptr, size, current_allocated, peak_allocated);
    }
    return ptr;
}

void* tracking_realloc(void* ptr, size_t size) {
    /* Note: This is simplified - real implementation should track old size */
    void *new_ptr = realloc(ptr, size);
    if (new_ptr != NULL) {
        printf("[REALLOC] %p -> %p: %zu bytes\n", ptr, new_ptr, size);
    }
    return new_ptr;
}

void tracking_free(void* ptr) {
    if (ptr != NULL) {
        free(ptr);
        free_count++;
        printf("[FREE] %p (freed %d times)\n", ptr, free_count);
    }
}

void print_allocation_stats(void) {
    printf("\n=== Allocation Statistics ===\n");
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Total freed: %zu bytes\n", total_freed);
    printf("Current allocated: %zu bytes\n", current_allocated);
    printf("Peak allocated: %zu bytes\n", peak_allocated);
    printf("Allocation count: %d\n", allocation_count);
    printf("Free count: %d\n", free_count);
    printf("============================\n\n");
}

/* ============================================
   Example 2: Simple Fixed Pool Allocator
   Pre-allocates fixed buffer for all allocations
   ============================================ */

#define POOL_SIZE (64 * 1024)  /* 64 KB pool */

static unsigned char memory_pool[POOL_SIZE];
static size_t pool_offset = 0;

void* pool_malloc(size_t size) {
    /* Simple bump allocator - no freeing */
    /* Align to 8 bytes */
    size = (size + 7) & ~7;

    if (pool_offset + size > POOL_SIZE) {
        fprintf(stderr, "Pool exhausted! Requested: %zu, Available: %zu\n",
                size, POOL_SIZE - pool_offset);
        return NULL;
    }

    void *ptr = &memory_pool[pool_offset];
    pool_offset += size;

    return ptr;
}

void* pool_realloc(void* ptr, size_t size) {
    /* Simple realloc - just allocate new and copy */
    void *new_ptr = pool_malloc(size);
    if (new_ptr != NULL && ptr != NULL) {
        /* In real implementation, should track old size */
        memcpy(new_ptr, ptr, size);
    }
    return new_ptr;
}

void pool_free(void* ptr) {
    /* Bump allocator doesn't free individual blocks */
    (void)ptr;  /* Unused */
}

void pool_reset(void) {
    pool_offset = 0;
    printf("Memory pool reset (freed %zu KB)\n", (size_t)(POOL_SIZE / 1024));
}

/* ============================================
   Main Example
   ============================================ */

int main(void) {
    pph21_input_t input = {0};
    pph_result_t *result;

    printf("=== PPHC Custom Allocator Examples ===\n\n");

    /* Example 1: Tracking Allocator */
    printf("Example 1: Tracking Allocator\n");
    printf("------------------------------\n");

    pph_init();
    pph_set_custom_allocator(tracking_malloc, tracking_realloc, tracking_free);

    /* Configure PPh21 calculation */
    input.subject_type = PPH21_PEGAWAI_TETAP;
    input.bruto_monthly.value = PPH_INT64_C(100000000000);  /* 10,000,000 */
    input.months_paid = 12;
    input.ptkp_status = PPH_PTKP_TK0;
    input.scheme = PPH21_SCHEME_TER;
    input.ter_category = PPH21_TER_CATEGORY_B;
    input.bonuses = NULL;
    input.bonus_count = 0;

    printf("\nCalculating PPh21 for 10M IDR/month...\n");
    result = pph21_calculate(&input);

    if (result != NULL) {
        char buffer[64];
        pph_money_to_string_formatted(result->total_tax, buffer, sizeof(buffer));
        printf("\nTotal PPh21: %s IDR\n", buffer);
        printf("Breakdown rows: %zu\n", (size_t)result->breakdown_count);

        pph_result_free(result);
    }

    print_allocation_stats();

    /* Reset to default allocator */
    pph_set_custom_allocator(NULL, NULL, NULL);

    printf("\n");

    /* Example 2: Pool Allocator */
    printf("Example 2: Fixed Pool Allocator\n");
    printf("--------------------------------\n");
    printf("Pool size: %d KB\n", POOL_SIZE / 1024);

    pph_set_custom_allocator(pool_malloc, pool_realloc, pool_free);

    printf("\nCalculating PPh21 with pool allocator...\n");
    result = pph21_calculate(&input);

    if (result != NULL) {
        char buffer[64];
        pph_money_to_string_formatted(result->total_tax, buffer, sizeof(buffer));
        printf("Total PPh21: %s IDR\n", buffer);
        printf("Pool used: %zu bytes (%.1f%%)\n",
               pool_offset, (pool_offset * 100.0) / POOL_SIZE);

        /* Note: pool allocator doesn't actually free memory */
        pph_result_free(result);
    }

    pool_reset();

    /* Reset to default allocator */
    pph_set_custom_allocator(NULL, NULL, NULL);

    printf("\n");

    /* Example 3: Multiple Calculations with Default Allocator */
    printf("Example 3: Back to Default Allocator\n");
    printf("-------------------------------------\n");

    /* Calculate with different salary */
    input.bruto_monthly.value = PPH_INT64_C(500000000000);  /* 50,000,000 */
    printf("\nCalculating PPh21 for 50M IDR/month...\n");

    result = pph21_calculate(&input);

    if (result != NULL) {
        char buffer[64];
        pph_money_to_string_formatted(result->total_tax, buffer, sizeof(buffer));
        printf("Total PPh21: %s IDR\n", buffer);

        pph_result_free(result);
    }

    printf("\n=== Examples Complete ===\n");

    return 0;
}
