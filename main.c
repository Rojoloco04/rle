// Runlength compression in C
// Usage: ./rle <input file> <output file> <runlength> <mode>
// mode=0 is compress and mode=1 is decompress

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void error(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void compress(char* inputFile, char* outputFile, int k) {
    // opening input file and opening/creating output file
    int in_fd = open(inputFile, O_RDONLY);
    if (in_fd == -1) error("Error with input file");
    int out_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out_fd == -1) error("Error with output file");
    
    // variable declaration
    int count = 1;
    char* buffer[k], next[k];
    ssize_t bytesRead = read(in_fd, buffer, k);

    while (bytesRead > 0) {
        // read in next byte, if at end, exit
        ssize_t nextRead = read(in_fd, next, k);
        if (nextRead == 0) return;

        // increment count until pattern no longer repeated
        while (memcmp(buffer, next, k) == 0 && count < 255) {
            count++;
            nextRead = read(in_fd, next, k);
        }
        
        // write characters and count to output
        write(out_fd, &count, 1);
        write(out_fd, buffer, k);

        // move next pattern into buffer for comparison, reset count
        memcpy(buffer, next, k);
        bytesRead = nextRead;
        count = 1;
    }

    close(in_fd);
    close(out_fd);
}

void decompress(char* inputFile, char* outputFile, int k) {
    // opening input file and opening/creating output file
    int in_fd = open(inputFile, O_RDONLY);
    if (in_fd == -1) error("Error with input file");
    int out_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out_fd == -1) error("Error with output file");

    // variable declaration
    int count = 0;
    char* buffer[k];
    read(in_fd, &count, 1);
    ssize_t bytesRead = read(in_fd, buffer, k);

    // outputting decompressed data to output file
    while (bytesRead > 0) {
        for (int i = 0; i < count; i++) {
            write(out_fd, buffer, k);
        }
        read(in_fd, &count, 1);
        bytesRead = read(in_fd, buffer, k);
    }
}

int main(int argc, char *argv[]) {
    // argument error checking
    if (argc != 5) {
        fprintf(stderr, "Usage: <inputFile> <outputFile> <runLength> <mode>\n");
        return EXIT_FAILURE;
    }

    // run length error checking
    int runLength = atoi(argv[3]);
    if (runLength < 1) {
        fprintf(stderr, "ERROR: Run length cannot be less than 1\n");
        return EXIT_FAILURE;
    }

    // mode error cheking, function calls
    int mode = atoi(argv[4]);
    if (mode == 0) {
        compress(argv[1], argv[2], runLength);
    }
    else if (mode == 1) {
        decompress(argv[1], argv[2], runLength);
    }
    else {
        fprintf(stderr, "ERROR: Mode must be 0 (compress) or 1 (decompress)\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
