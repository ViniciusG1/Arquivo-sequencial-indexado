#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct reg {
    char appName[160]; 
    char appId[160];
    char category[26]; 
    float rating; 
    int ratingCount; 
    bool free; 
    float price; 
    char released[14]; 
    char lastUpdated[14]; 
    bool adSupported; 
    bool inAppPurchases;
};

struct IndexRecord {
    char appName[160];
    long dataFilePosition;
};

int compareAppName(const void *a, const void *b) {
    return strcmp(((struct IndexRecord *)a)->appName, ((struct IndexRecord *)b)->appName);
}

int main() {
    FILE *dataFile = fopen("../sequential-db.bin", "rb");
    FILE *indexFile = fopen("Indices/indice-appName.idx", "wb");

    if (dataFile == NULL || indexFile == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    long recordCount = 0;
    struct reg record;

    while (fread(&record, sizeof(struct reg), 1, dataFile) == 1) {
        struct IndexRecord indexRecord;

        strcpy(indexRecord.appName, record.appName);
        indexRecord.dataFilePosition = recordCount;

        fwrite(&indexRecord, sizeof(struct IndexRecord), 1, indexFile);

        recordCount++;
    }

    fclose(dataFile);
    fclose(indexFile);

    indexFile = fopen("Indices/indice-appName.idx", "rb");
    if (indexFile == NULL) {
        printf("Erro ao abrir o arquivo de indice.\n");
        return 1;
    }

    fseek(indexFile, 0, SEEK_END);
    long numRecords = ftell(indexFile) / sizeof(struct IndexRecord);
    rewind(indexFile);

    struct IndexRecord *indexArray = (struct IndexRecord *)malloc(numRecords * sizeof(struct IndexRecord));
    if (indexArray == NULL) {
        printf("Erro na alocação de memoria.\n");
        return 1;
    }

    fread(indexArray, sizeof(struct IndexRecord), numRecords, indexFile);

    qsort(indexArray, numRecords, sizeof(struct IndexRecord), compareAppName);

    fclose(indexFile);
    indexFile = fopen("Indices/indice-appName.idx", "wb");
    if (indexFile == NULL) {
        printf("Erro ao abrir o arquivo de indice.\n");
        return 1;
    }

    fwrite(indexArray, sizeof(struct IndexRecord), numRecords, indexFile);

    fclose(indexFile);
    free(indexArray);

    printf("Arquivo de indice [appName] criado e ordenado com sucesso.\n");

    return 0;
}
