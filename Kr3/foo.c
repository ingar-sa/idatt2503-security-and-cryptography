#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

static inline __attribute__((always_inline)) uint8_t gf_mul_SANIC(uint8_t a,
                                                                  uint8_t b) {
  register uint8_t res = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    res ^= ((-(a & (1U << i))) & b);
    b = (((uint16_t)b << 1) ^ ((-(b >> 7)) & 0x1B)) & 0xFF;
  }

  return res;
}

static inline __attribute__((always_inline)) uint8_t gf_add(uint8_t a,
                                                            uint8_t b) {
  return a ^ b;
}

static uint8_t gf_mul(uint8_t a, uint8_t b) {
  uint8_t result = 0;
  uint8_t current_b = b;
  const uint8_t OVERFLOW_REPLACE =
      0x1B; // 27 in decimal, polynomial x^4 + x^3 + x + 1

  // Process each bit of 'a', from least significant to most significant
  for (uint8_t bit = 0; bit < 8; bit++) {
    // If current bit of 'a' is set, XOR the result with the current value of b
    if (a & (1 << bit)) {
      result ^= current_b;
    }

    // Shift b left by 1 for next iteration
    current_b <<= 1;

    // If we overflow (bit shifted out), XOR with the reduction polynomial
    if (current_b & 0x100) {         // Check if bit 8 is set
      current_b &= 0xFF;             // Clear the overflow bit
      current_b ^= OVERFLOW_REPLACE; // Apply the reduction
    }
  }

  return result;
}

void write_multiplication_table(const char *filename) {
  FILE *f = fopen(filename, "w");
  if (!f) {
    perror("Failed to open file");
    return;
  }

  // Buffer for a single row (3 chars per number + space + newline + null
  // terminator) Max size: 256 * 4 + 1 = 1025 bytes per row
  char buf[1025];
  int pos;

  for (int i = 0; i < 256; i++) {
    pos = 0;
    for (int j = 0; j < 256; j++) {
      int product = gf_mul(i, j);
      if (j == 255) {
        pos += sprintf(buf + pos, "%d", product);
      } else {
        pos += sprintf(buf + pos, "%d ", product);
      }
    }
    buf[pos] = '\n';
    pos++;
    fwrite(buf, 1, pos, f);
  }

  fclose(f);
}

// Returns time in seconds as a double
static double get_time_diff(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
  struct timespec start, end;
  double precompute_time, verify_time;
  bool properties_hold = true;

  // Time the precomputation of the multiplication table
  clock_gettime(CLOCK_MONOTONIC, &start);

  static uint8_t mul_table[256][256];
  for (int a = 0; a < 256; a++) {
    for (int b = 0; b < 256; b++) {
      mul_table[a][b] = gf_mul(a, b);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  precompute_time = get_time_diff(start, end);

  // Time the property verification
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int a = 1; a < 256 && properties_hold; a++) {
    for (int b = 1; b < 256 && properties_hold; b++) {
      // Check commutativity
      if (mul_table[a][b] != mul_table[b][a]) {
        printf("%d * %d is not commutative\n", a, b);
        properties_hold = false;
        break;
      }

      for (int c = 1; c < 256 && properties_hold; c++) {
        // Check associativity
        uint8_t left = mul_table[mul_table[a][b]][c];
        uint8_t right = mul_table[a][mul_table[b][c]];
        if (left != right) {
          printf("%d, %d, and %d are not associative\n", a, b, c);
          properties_hold = false;
          break;
        }

        // Check distributivity
        uint8_t add_bc = gf_add(b, c);
        uint8_t left_dist = mul_table[a][add_bc];
        uint8_t right_dist = gf_add(mul_table[a][b], mul_table[a][c]);
        if (left_dist != right_dist) {
          printf("%d, %d, and %d are not distributive\n", a, b, c);
          properties_hold = false;
          break;
        }
      }
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  verify_time = get_time_diff(start, end);

  // Print results
  if (properties_hold) {
    printf("The commutative, associative, and distributive properties hold\n");
  } else {
    printf("The commutative, associative, and distributive properties do not "
           "hold\n");
  }

  printf("\nTiming Results:\n");
  printf("Multiplication table precomputation: %.6f seconds\n",
         precompute_time);
  printf("Property verification: %.6f seconds\n", verify_time);
  printf("Total time: %.6f seconds\n", precompute_time + verify_time);

  return 0;
}
