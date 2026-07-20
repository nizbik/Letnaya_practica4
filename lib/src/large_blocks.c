/*
 * Реализация библиотеки хранения, загрузки, вывода и сортировки строк в структуре LargeBlock.
 * Зубехин Никита
 * МК-101
 */



#include "large_blocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



char GetChar(LargeBlock *arr, int slot, int pos) {
    int k   = pos / CHARS_PER_LARGE;
    int off = pos % CHARS_PER_LARGE;
    int p = slot / LARGE_BLOCK_HEIGHT;
    int q = slot % LARGE_BLOCK_HEIGHT;
    int r = off / SMALL_BLOCK_HEIGHT;
    int c = off % SMALL_BLOCK_HEIGHT;
    return arr[k][p][q][r][c];
}

void SetChar(LargeBlock *arr, int slot, int pos, char value) {
    int k   = pos / CHARS_PER_LARGE;
    int off = pos % CHARS_PER_LARGE;
    int p = slot / LARGE_BLOCK_HEIGHT;
    int q = slot % LARGE_BLOCK_HEIGHT;
    int r = off / SMALL_BLOCK_HEIGHT;
    int c = off % SMALL_BLOCK_HEIGHT;
    arr[k][p][q][r][c] = value;
}

int StringLengthAt(LargeBlock *arr, int slot) {
    int pos = 0;
    while (GetChar(arr, slot, pos) != '\0') {
        pos++;
    }
    return pos;
}



static int GroupCapacity(LargeBlock *arr) {
    if (arr == NULL) {
        return 0;
    }
    int maxLen = 0;
    for (int s = 0; s < SLOTS_PER_GROUP; s++) {
        int len = StringLengthAt(arr, s);
        if (len > maxLen) {
            maxLen = len;
        }
    }
    int neededChars = maxLen + 1; /* с учётом завершающего нуля */
    return (neededChars + CHARS_PER_LARGE - 1) / CHARS_PER_LARGE;
}


static void EnsureCapacity(LargeBlock **arrPtr, int neededLen) {
    int neededChars = neededLen + 1;
    int neededK = (neededChars + CHARS_PER_LARGE - 1) / CHARS_PER_LARGE;
    int currentK = GroupCapacity(*arrPtr);

    if (neededK <= currentK) {
        return;
    }

    LargeBlock *newArr = malloc(sizeof(LargeBlock) * (size_t) neededK);

    if (currentK > 0) {
        memcpy(newArr, *arrPtr, sizeof(LargeBlock) * (size_t) currentK);
    }
    memset((char *) newArr + sizeof(LargeBlock) * (size_t) currentK, 0,
           sizeof(LargeBlock) * (size_t) (neededK - currentK));

    free(*arrPtr);
    *arrPtr = newArr;
}

static void WriteString(LargeBlock *arr, int slot, const char *s, int len) {
    for (int i = 0; i < len; i++) {
        SetChar(arr, slot, i, s[i]);
    }
    SetChar(arr, slot, len, '\0');
}

static void ReadString(LargeBlock *arr, int slot, char *buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] = GetChar(arr, slot, i);
    }
    buf[len] = '\0';
}


void SwapStrings(LargeBlock **strings, int idxA, int idxB) {
    if (idxA == idxB) {
        return;
    }

    int i1 = idxA / SLOTS_PER_GROUP, j1 = idxA % SLOTS_PER_GROUP;
    int i2 = idxB / SLOTS_PER_GROUP, j2 = idxB % SLOTS_PER_GROUP;

    int len1 = StringLengthAt(strings[i1], j1);
    int len2 = StringLengthAt(strings[i2], j2);

    char *buf1 = malloc((size_t) len1 + 1);
    char *buf2 = malloc((size_t) len2 + 1);
    ReadString(strings[i1], j1, buf1, len1);
    ReadString(strings[i2], j2, buf2, len2);

    if (i1 != i2) {
        EnsureCapacity(&strings[i2], len1);
        EnsureCapacity(&strings[i1], len2);
    }

    WriteString(strings[i2], j2, buf1, len1);
    WriteString(strings[i1], j1, buf2, len2);

    free(buf1);
    free(buf2);
}



static StringRef RefAt(LargeBlock **strings, int idx) {
    StringRef r;
    r.arr = strings[idx / SLOTS_PER_GROUP];
    r.slot = idx % SLOTS_PER_GROUP;
    return r;
}

void SortStrings(LargeBlock **strings, int totalStrings, CompareFn cmp) {
    for (int i = 1; i < totalStrings; i++) {
        int j = i;
        while (j > 0 && cmp(RefAt(strings, j - 1), RefAt(strings, j)) > 0) {
            SwapStrings(strings, j - 1, j);
            j--;
        }
    }
}



int CompareLexCase(StringRef a, StringRef b) {
    int i = 0;
    for (;;) {
        char ca = GetChar(a.arr, a.slot, i);
        char cb = GetChar(b.arr, b.slot, i);
        if (ca != cb) {
            return (unsigned char) ca - (unsigned char) cb;
        }
        if (ca == '\0') {
            return 0;
        }
        i++;
    }
}

int CompareLexNoCase(StringRef a, StringRef b) {
    int i = 0;
    for (;;) {
        char ca = (char) tolower((unsigned char) GetChar(a.arr, a.slot, i));
        char cb = (char) tolower((unsigned char) GetChar(b.arr, b.slot, i));
        if (ca != cb) {
            return (unsigned char) ca - (unsigned char) cb;
        }
        if (ca == '\0') {
            return 0;
        }
        i++;
    }
}

int CompareByLength(StringRef a, StringRef b) {
    int la = StringLengthAt(a.arr, a.slot);
    int lb = StringLengthAt(b.arr, b.slot);
    if (la != lb) {
        return la - lb;
    }
    return CompareLexCase(a, b);
}

int CompareLastCharThenLex(StringRef a, StringRef b) {
    int la = StringLengthAt(a.arr, a.slot);
    int lb = StringLengthAt(b.arr, b.slot);
    char lastA = (la > 0) ? GetChar(a.arr, a.slot, la - 1) : 0;
    char lastB = (lb > 0) ? GetChar(b.arr, b.slot, lb - 1) : 0;
    if (lastA != lastB) {
        return (unsigned char) lastA - (unsigned char) lastB;
    }
    return CompareLexCase(a, b);
}

int CompareDigitCount(StringRef a, StringRef b) {
    int da = 0, db = 0;

    for (int i = 0; ; i++) {
        char c = GetChar(a.arr, a.slot, i);
        if (c == '\0') {
            break;
        }
        if (isdigit((unsigned char) c)) {
            da++;
        }
    }
    for (int i = 0; ; i++) {
        char c = GetChar(b.arr, b.slot, i);
        if (c == '\0') {
            break;
        }
        if (isdigit((unsigned char) c)) {
            db++;
        }
    }

    if (da != db) {
        return da - db;
    }
    return CompareLexCase(a, b);
}



LargeBlock **LoadStringsFromFile(const char *filename, int *outTotal, int *outGroupCount) {
    *outTotal = 0;
    *outGroupCount = 0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        return NULL;
    }


    int total = 0;
    int sawChar = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') {
            total++;
            sawChar = 0;
        } else {
            sawChar = 1;
        }
    }
    if (sawChar) {
        total++;
    }

    if (total == 0) {
        fclose(f);
        return NULL;
    }


    int *lengths = malloc(sizeof(int) * (size_t) total);
    rewind(f);
    for (int idx = 0; idx < total; idx++) {
        int len = 0;
        while ((ch = fgetc(f)) != EOF && ch != '\n') {
            len++;
        }
        lengths[idx] = len;
    }

    int groupCount = (total + SLOTS_PER_GROUP - 1) / SLOTS_PER_GROUP;
    LargeBlock **strings = malloc(sizeof(LargeBlock *) * (size_t) groupCount);
    for (int i = 0; i < groupCount; i++) {
        strings[i] = NULL;
    }

    
    rewind(f);
    for (int idx = 0; idx < total; idx++) {
        int group = idx / SLOTS_PER_GROUP;
        int slot = idx % SLOTS_PER_GROUP;

        EnsureCapacity(&strings[group], lengths[idx]);

        int pos = 0;
        while ((ch = fgetc(f)) != EOF && ch != '\n') {
            SetChar(strings[group], slot, pos, (char) ch);
            pos++;
        }
        SetChar(strings[group], slot, pos, '\0');
    }

    free(lengths);
    fclose(f);

    *outTotal = total;
    *outGroupCount = groupCount;
    return strings;
}

void FreeStrings(LargeBlock **strings, int groupCount) {
    if (strings == NULL) {
        return;
    }
    for (int i = 0; i < groupCount; i++) {
        free(strings[i]);
    }
    free(strings);
}



void PrintStrings(LargeBlock **strings, int totalStrings) {
    for (int idx = 0; idx < totalStrings; idx++) {
        StringRef r = RefAt(strings, idx);
        int pos = 0;
        char c;
        while ((c = GetChar(r.arr, r.slot, pos)) != '\0') {
            putchar(c);
            pos++;
        }
        putchar('\n');
    }
}

void SaveStringsToFile(LargeBlock **strings, int totalStrings, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        return;
    }
    for (int idx = 0; idx < totalStrings; idx++) {
        StringRef r = RefAt(strings, idx);
        int pos = 0;
        char c;
        while ((c = GetChar(r.arr, r.slot, pos)) != '\0') {
            fputc(c, f);
            pos++;
        }
        fputc('\n', f);
    }
    fclose(f);
}



void GenerateRandomStringsFile(const char *filename, unsigned int count, unsigned int maxLen) {
    static const char alphabet[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int alphabetSize = (int) (sizeof(alphabet) - 1);

    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        return;
    }

    for (unsigned int i = 0; i < count; i++) {
        unsigned int len = (maxLen == 0) ? 0 : ((unsigned int) rand() % (maxLen + 1));
        for (unsigned int j = 0; j < len; j++) {
            fputc(alphabet[rand() % alphabetSize], f);
        }
        fputc('\n', f);
    }

    fclose(f);
}
