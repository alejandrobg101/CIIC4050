#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#define MAX_RUN 255

int64_t GenerateTestFile(const char* filename);
int64_t CompressRLE(const char* input_filename, const char* output_filename);
int64_t DecompressRLE(const char* input_filename, const char* output_filename);

int64_t GenerateTestFile(const char* filename) {
  FILE* file = fopen(filename, "wb");
  if (file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
    return -1;
  }

  int64_t total_size = 0;
  unsigned char byte;

  byte = 'A';
  for (int i = 0; i < 255; i++) {
    fwrite(&byte, 1, 1, file);
    total_size++;
  }

  byte = 'B';
  for (int i = 0; i < 10; i++) {
    fwrite(&byte, 1, 1, file);
    total_size++;
  }

  byte = 'Z';
  for (int i = 0; i < 150; i++) {
    fwrite(&byte, 1, 1, file);
    total_size++;
  }

  byte = 'C';
  for (int i = 0; i < 50; i++) {
    fwrite(&byte, 1, 1, file);
    total_size++;
  }

  fclose(file);
  return total_size;
}

int64_t CompressRLE(const char* input_filename, const char* output_filename) {
  FILE* input_file = fopen(input_filename, "rb");
  if (input_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s for reading\n", input_filename);
    return -1;
  }

  FILE* output_file = fopen(output_filename, "wb");
  if (output_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s for writing\n",
            output_filename);
    fclose(input_file);
    return -1;
  }

  unsigned char current_byte, next_byte;
  unsigned char counter = 1;
  int64_t compressed_size = 0;

  if (fread(&current_byte, 1, 1, input_file) != 1) {
    fclose(input_file);
    fclose(output_file);
    return 0;
  }

  while (fread(&next_byte, 1, 1, input_file) == 1) {
    if (next_byte == current_byte && counter < MAX_RUN) {
      counter++;
    } else {
      fwrite(&counter, 1, 1, output_file);
      fwrite(&current_byte, 1, 1, output_file);
      compressed_size += 2;

      current_byte = next_byte;
      counter = 1;
    }
  }

  fwrite(&counter, 1, 1, output_file);
  fwrite(&current_byte, 1, 1, output_file);
  compressed_size += 2;

  fclose(input_file);
  fclose(output_file);
  return compressed_size;
}

int64_t DecompressRLE(const char* input_filename, const char* output_filename) {
  FILE* input_file = fopen(input_filename, "rb");
  if (input_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s for reading\n", input_filename);
    return -1;
  }

  FILE* output_file = fopen(output_filename, "wb");
  if (output_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s for writing\n",
            output_filename);
    fclose(input_file);
    return -1;
  }

  unsigned char counter, data;
  int64_t decompressed_size = 0;

  while (fread(&counter, 1, 1, input_file) == 1) {
    if (fread(&data, 1, 1, input_file) != 1) {
      fprintf(stderr, "Error: Incomplete pair in compressed file\n");
      fclose(input_file);
      fclose(output_file);
      return -1;
    }

    for (int i = 0; i < counter; i++) {
      fwrite(&data, 1, 1, output_file);
      decompressed_size++;
    }
  }

  fclose(input_file);
  fclose(output_file);
  return decompressed_size;
}

int main() {
  const char* original_file = "original_data.bin";
  const char* compressed_file = "compressed_data.rle";
  const char* decompressed_file = "decompressed_data.bin";

  printf("=== RLE Compression/Decompression Program ===\n\n");

  printf("Phase 1: Generating test file...\n");
  int64_t original_size = GenerateTestFile(original_file);
  if (original_size < 0) {
    fprintf(stderr, "Error generating test file\n");
    return 1;
  }
  printf("Original file size: %" PRId64 " bytes\n\n", original_size);

  printf("Phase 2: Compressing file...\n");
  int64_t compressed_size = CompressRLE(original_file, compressed_file);
  if (compressed_size < 0) {
    fprintf(stderr, "Error compressing file\n");
    return 1;
  }
  printf("Compressed file size: %" PRId64 " bytes\n\n", compressed_size);

  printf("Phase 3: Decompressing file...\n");
  int64_t decompressed_size = DecompressRLE(compressed_file, decompressed_file);
  if (decompressed_size < 0) {
    fprintf(stderr, "Error decompressing file\n");
    return 1;
  }
  printf("Decompressed file size: %" PRId64 " bytes\n\n", decompressed_size);

  printf("=== Validation ===\n");
  if (original_size == decompressed_size) {
    printf("✓ SUCCESS: Original and decompressed sizes match!\n");
  } else {
    printf("✗ ERROR: Size mismatch!"
        " Original: %" PRId64 ", Decompressed: %" PRId64 "\n",
           original_size, decompressed_size);
    return 1;
  }

  printf("\n=== Compression Statistics ===\n");
  printf("Original size:      %" PRId64 " bytes\n", original_size);
  printf("Compressed size:    %" PRId64 " bytes\n", compressed_size);
  printf("Decompressed size:  %" PRId64 " bytes\n", decompressed_size);

  if (compressed_size > 0) {
    double compression_ratio = (double)original_size / compressed_size;
    printf("Compression ratio:  %.2f:1\n", compression_ratio);
  } else {
    printf("Compression ratio:  N/A (compressed size is 0)\n");
  }

  return 0;
}
