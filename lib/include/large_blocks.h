/*
 * Определяет структуру хранения строк и интерфейс для работы с ней.
 * Зубехин Никита
 * МК-101
 */



#ifndef LARGE_BLOCKS_H
#define LARGE_BLOCKS_H



#define SMALL_BLOCK_WIDTH  2
#define SMALL_BLOCK_HEIGHT 4
#define LARGE_BLOCK_WIDTH  3
#define LARGE_BLOCK_HEIGHT 2

typedef char SmallBlock[SMALL_BLOCK_WIDTH][SMALL_BLOCK_HEIGHT];
typedef SmallBlock LargeBlock[LARGE_BLOCK_WIDTH][LARGE_BLOCK_HEIGHT];


#define SLOTS_PER_GROUP (LARGE_BLOCK_WIDTH * LARGE_BLOCK_HEIGHT)

#define CHARS_PER_LARGE (SMALL_BLOCK_WIDTH * SMALL_BLOCK_HEIGHT)


typedef struct {
    LargeBlock *arr;
    int slot;
} StringRef;

typedef int (*CompareFn)(StringRef a, StringRef b);


char GetChar(LargeBlock *arr, int slot, int pos);
void SetChar(LargeBlock *arr, int slot, int pos, char value);
int  StringLengthAt(LargeBlock *arr, int slot);


LargeBlock **LoadStringsFromFile(const char *filename, int *outTotal, int *outGroupCount);
void FreeStrings(LargeBlock **strings, int groupCount);


void PrintStrings(LargeBlock **strings, int totalStrings);
void SaveStringsToFile(LargeBlock **strings, int totalStrings, const char *filename);


void SwapStrings(LargeBlock **strings, int idxA, int idxB);
void SortStrings(LargeBlock **strings, int totalStrings, CompareFn cmp);


int CompareLexCase(StringRef a, StringRef b);         
int CompareLexNoCase(StringRef a, StringRef b);         
int CompareByLength(StringRef a, StringRef b);          
int CompareLastCharThenLex(StringRef a, StringRef b);   
int CompareDigitCount(StringRef a, StringRef b);       


void GenerateRandomStringsFile(const char *filename, unsigned int count, unsigned int maxLen);

#endif 
