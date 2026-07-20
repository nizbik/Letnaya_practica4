/*
 * Точка входа приложения — консольный интерфейс к библиотеке
 * Зубехин Никита
 * МК-101
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "large_blocks.h"

static void PrintUsage(const char *prog) {
    printf("Usage:\n");
    printf("  %s generate <output_file>\n", prog);
    printf("  %s sort <input_file> [output_file] [cmp:1-5]\n", prog);
    printf("\n");
    printf("Comparators:\n");
    printf("  1 - lexicographic, case-sensitive   (default)\n");
    printf("  2 - lexicographic, case-insensitive\n");
    printf("  3 - by string length\n");
    printf("  4 - by last character\n");
    printf("  5 - by count of digit characters\n");
}


#define GEN_MAX_LEN_LIMIT   (1000000u)   
#define GEN_MAX_COUNT_LIMIT (10000000u)  
#define GEN_MAX_TOTAL_CHARS (500000000ULL) 


static int ReadBoundedUInt(const char *prompt, unsigned int maxAllowed, unsigned int *outValue) {
    char line[256];

    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        char *end = NULL;
        errno = 0;
        long value = strtol(line, &end, 10);

        
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') {
            end++;
        }

        int parsedWholeToken = (end != line) && (*end == '\0');

        if (!parsedWholeToken || errno == ERANGE || value < 0 || (unsigned long) value > maxAllowed) {
            printf("Please enter an integer between 0 and %u.\n", maxAllowed);
            continue;
        }

        *outValue = (unsigned int) value;
        return 1;
    }
}

static void RunGenerate(const char *outFile) {
    unsigned int maxLen, count;

    if (!ReadBoundedUInt("Maximum string length: ", GEN_MAX_LEN_LIMIT, &maxLen)) {
        fprintf(stderr, "Invalid input\n");
        return;
    }
    if (!ReadBoundedUInt("Number of strings: ", GEN_MAX_COUNT_LIMIT, &count)) {
        fprintf(stderr, "Invalid input\n");
        return;
    }

   
    if ((unsigned long long) maxLen * (unsigned long long) count > GEN_MAX_TOTAL_CHARS) {
        fprintf(stderr,
                "maxLen * count is too large (limit is %llu characters total). "
                "Reduce the length or the number of strings.\n",
                GEN_MAX_TOTAL_CHARS);
        return;
    }

    srand((unsigned int) time(NULL));
    GenerateRandomStringsFile(outFile, count, maxLen);

    printf("Generated %u random strings (max length %u) into \"%s\"\n", count, maxLen, outFile);
}

static CompareFn PickComparator(int cmpId) {
    switch (cmpId) {
        case 2: return CompareLexNoCase;
        case 3: return CompareByLength;
        case 4: return CompareLastCharThenLex;
        case 5: return CompareDigitCount;
        default: return CompareLexCase;
    }
}

static void RunSort(const char *inFile, const char *outFile, int cmpId) {
    int total = 0, groupCount = 0;
    LargeBlock **strings = LoadStringsFromFile(inFile, &total, &groupCount);

    if (strings == NULL || total == 0) {
        printf("No strings loaded from \"%s\"\n", inFile);
        return;
    }

    printf("Loaded %d strings from \"%s\"\n\n", total, inFile);

    printf("Before sorting:\n");
    PrintStrings(strings, total);

    SortStrings(strings, total, PickComparator(cmpId));

    printf("\nAfter sorting:\n");
    PrintStrings(strings, total);

    if (outFile != NULL) {
        SaveStringsToFile(strings, total, outFile);
        printf("\nSaved sorted strings to \"%s\"\n", outFile);
    }

    FreeStrings(strings, groupCount);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "generate") == 0) {
        if (argc < 3) {
            PrintUsage(argv[0]);
            return 1;
        }
        RunGenerate(argv[2]);
        return 0;
    }

    if (strcmp(argv[1], "sort") == 0) {
        if (argc < 3) {
            PrintUsage(argv[0]);
            return 1;
        }
        const char *inFile = argv[2];
        const char *outFile = (argc >= 4) ? argv[3] : NULL;
        int cmpId = (argc >= 5) ? atoi(argv[4]) : 1;
        RunSort(inFile, outFile, cmpId);
        return 0;
    }

    PrintUsage(argv[0]);
    return 1;
}
