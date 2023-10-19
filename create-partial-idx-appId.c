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
    char appId[160];
    long dataFilePosition;
};

int main() {
    FILE *dataFile = fopen("../sequential-db.bin", "rb");
    FILE *indexFile = fopen("Indices/indice-parcial-appId.idx", "wb");

    if (dataFile == NULL || indexFile == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    int blockSize = 5000;  // Tamanho do bloco
    long recordCount = 0;

    fseek(dataFile, 0L, SEEK_END);
    long int bytes_len = ftell(dataFile);
    long int qtd_registros = bytes_len/sizeof(struct reg);
    rewind(dataFile);    

    while (recordCount < qtd_registros) {
        struct IndexRecord indexRecord;
        struct reg temp;

        fread(&temp, sizeof(struct reg), 1, dataFile);
        strcpy(indexRecord.appId, temp.appId); indexRecord.dataFilePosition = recordCount;
        fwrite(&indexRecord, sizeof(struct IndexRecord), 1, indexFile);

        recordCount+=blockSize;
        fseek(dataFile, sizeof(struct reg)*recordCount, SEEK_SET); 
    }

    printf("Indice Parcial [appId] criado com sucesso!\n");
    fclose(dataFile);
    fclose(indexFile);
    return 0;
}
