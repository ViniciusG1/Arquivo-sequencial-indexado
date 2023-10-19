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


struct reg parseCSVLine(const char *line) {
    struct reg temp;
    const char *delim = ";";
    char field[160];
    int field_index = 0, count = 0;

    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == delim[0] || line[i] == '\n') {
            field[field_index] = '\0'; // Termina a string
            switch (count) {
                case 0:
                    strcpy(temp.appName, field);
                    break;
                case 1:
                    strcpy(temp.appId, field);
                    break;
                case 2:
                    strcpy(temp.category, field);
                    break;
                case 3:
                    temp.rating = atof(field);
                    break;
                case 4:
                    temp.ratingCount = atoi(field);
                    break;
                case 5:
                    temp.free = (strcmp("True", field) == 0);
                    break;
                case 6:
                    temp.price = atof(field);
                    break;
                case 7:
                    strcpy(temp.released, field);
                    break;
                case 8:
                    strcpy(temp.lastUpdated, field);
                    break;
                case 9:
                    temp.adSupported = (strcmp("True", field) == 0);
                    break;
                case 10:
                    temp.inAppPurchases = (strcmp("True", field) == 0);
                    break;
                default:
                    break;
            }
            count++;
            field_index = 0; // Reinicia o índice do campo
        } else {
            field[field_index++] = line[i]; // Adiciona o caractere ao campo
        }
    }

    return temp;
}


int main() {
    FILE *arq_csv, *arq_bin;
    
    char line[280], header[110];

    arq_csv = fopen("../Google-Playstore-Sequential.csv", "r");
    arq_bin = fopen("../sequential-db.bin", "w+b");

    if(!arq_csv) {
        printf("File not found!\n");
        exit(1);
    }

    fgets(line, 320, arq_csv);
    strcpy(header, line);

    struct reg temp;
    while(fgets(line, 320, arq_csv)) {
        
        temp = parseCSVLine(line);
        fwrite(&temp, sizeof(struct reg), 1, arq_bin);

    }
    printf("\nDados carregados para o Arquivo Binario com Sucesso !\n");
    long unsigned int bytes_len = ftell(arq_bin);
    long unsigned int qtd_registros = bytes_len/sizeof(struct reg);
    printf("%li total bytes\n%li numero de registros\n%i bytes por struct\n\n", bytes_len, qtd_registros, sizeof(struct reg));

    fclose(arq_csv);
    fclose(arq_bin);

    return 0;
}