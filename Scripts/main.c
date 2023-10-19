/**
 * @file main.c
 * @author Vinicius Gonçalves (vgmineira@ucs.br)
 * @brief 
 * @version 0.1
 * @date 2023-10-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

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


void printAppInfo(struct reg app) {
    printf("App Name: %s\n", app.appName);
    printf("App ID: %s\n", app.appId);
    printf("Category: %s\n", app.category);
    printf("Rating: %.2f\n", app.rating);
    printf("Rating Count: %d\n", app.ratingCount);
    printf("Free: %s\n", app.free ? "Yes" : "No");
    printf("Price: %.2f\n", app.price);
    printf("Released: %s\n", app.released);
    printf("Last Updated: %s\n", app.lastUpdated);
    printf("Ad Supported: %s\n", app.adSupported ? "Yes" : "No");
    printf("In-App Purchases: %s\n", app.inAppPurchases ? "Yes" : "No");
}


int PesquisaBinariaArquivo(char x[], FILE *arq_aux, int e, int d, int *cmp_count, int flag, struct reg *buffer) {
    int meio = (e+d)/2;
    (*cmp_count)++;
    struct reg temp;
    fseek(arq_aux, sizeof(struct reg)*meio, SEEK_SET); //SEEK_CUR vs SEEK_SET
    fread(&temp, sizeof(struct reg), 1, arq_aux);
    if(flag == 1)
        printf("\tcomparando %s com %s\n", x, temp.appId);
    if(strcmp(temp.appId, x) == 0) { *buffer = temp; return meio; }
    if(e >= d) { return -1; }
    else {
        if(strcmp(temp.appId, x) < 0) { return PesquisaBinariaArquivo(x, arq_aux, meio+1, d, cmp_count, flag, buffer); }
        else { return PesquisaBinariaArquivo(x, arq_aux, e, meio-1, cmp_count, flag, buffer); }
    }
}


struct IndexRecord {
    char appChar[160];
    long dataFilePosition;
};


int PesquisaBinariaArquivoName(const char x[], FILE *arq_aux, int e, int d, int *meio) {
    if (e > d) {
        return -1;  // Nome não encontrado
    }

    *meio = (e + d) / 2;
    struct IndexRecord temp;

    fseek(arq_aux, sizeof(struct IndexRecord) * (*meio), SEEK_SET);
    fread(&temp, sizeof(struct IndexRecord), 1, arq_aux);

    if (strcmp(temp.appChar, x) == 0) {
        // Encontrou uma correspondência, verifique se é a primeira
        printf("%s\n", temp.appChar);
        while ((*meio) > e) {
            fseek(arq_aux, sizeof(struct IndexRecord) * ((*meio) - 1), SEEK_SET);
            fread(&temp, sizeof(struct IndexRecord), 1, arq_aux);
            if (strcmp(temp.appChar, x) != 0) {
                break;  // Encontrou o primeiro
            }
            (*meio)--;
        }

        // Retorna a dataFilePosition associada ao primeiro 'appName' encontrado
        fseek(arq_aux, sizeof(struct IndexRecord) * (*meio), SEEK_SET);
        fread(&temp, sizeof(struct IndexRecord), 1, arq_aux);
        return temp.dataFilePosition;  // Retornou a posição do primeiro 'appName' encontrado
    } else if (strcmp(temp.appChar, x) < 0) {
        return PesquisaBinariaArquivoName(x, arq_aux, (*meio) + 1, d, meio);
    } else {
        return PesquisaBinariaArquivoName(x, arq_aux, e, (*meio) - 1, meio);
    }
}


// Estrutura para um nó da árvore AVL
struct AVLNode {
    float price;
    struct LinkedListNode* positions;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
};

// Estrutura para um nó da lista encadeada de posições físicas
struct LinkedListNode {
    int position;
    struct LinkedListNode* next;
};

int max(int a, int b) {
    return (a > b)? a : b;
}

// Função utilitária para alocar um novo nó da árvore AVL
struct AVLNode* newNode(float price) {
    struct AVLNode* node = (struct AVLNode*)malloc(sizeof(struct AVLNode));
    node->price = price;
    node->positions = NULL;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    return node;
}

// Função utilitária para alocar um novo nó da lista encadeada
struct LinkedListNode* newLinkedListNode(int position) {
    struct LinkedListNode* node = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
    node->position = position;
    node->next = NULL;
    return node;
}

int height(struct AVLNode *N) {
    if (N == NULL)
        return 0;
    return 1+max(height(N->left), height(N->right));
}

// Função para obter o balanço de um nó da árvore AVL
int getBalance(struct AVLNode* N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Função para realizar uma rotação simples à direita
struct AVLNode* rightRotate(struct AVLNode* y) {
    struct AVLNode* x = y->left;
    struct AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));

    return x;
}

// Função para realizar uma rotação simples à esquerda
struct AVLNode* leftRotate(struct AVLNode* x) {
    struct AVLNode* y = x->right;
    struct AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));

    return y;
}

// Função para inserir um nó na árvore AVL
struct AVLNode* insert(struct AVLNode* node, float price, int position) {
    if (node == NULL) {
        struct AVLNode* newNode = (struct AVLNode*)malloc(sizeof(struct AVLNode));
        newNode->price = price;
        newNode->positions = newLinkedListNode(position);
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (price < node->price)
        node->left = insert(node->left, price, position);
    else if (price > node->price)
        node->right = insert(node->right, price, position);
    else {
        // Se o preço for o mesmo, adicione a posição à lista encadeada
        struct LinkedListNode* newNode = newLinkedListNode(position);
        newNode->next = node->positions;
        node->positions = newNode;
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // Casos de rotação
    if (balance > 1 && price < node->left->price)
        return rightRotate(node);

    if (balance < -1 && price > node->right->price)
        return leftRotate(node);

    if (balance > 1 && price > node->left->price) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && price < node->right->price) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Função para encontrar um nó na árvore AVL com um preço específico
struct AVLNode* search(struct AVLNode* root, float price) {
    if (root == NULL || root->price == price)
        return root;

    if (root->price > price)
        return search(root->left, price);

    return search(root->right, price);
}

// Função utilitária para imprimir a lista encadeada de posições
void printPositions(struct LinkedListNode *head, FILE *bin, char id[], int show) {
    struct reg temp;
    while (head != NULL) {
        fseek(bin, sizeof(struct reg)*(head->position), SEEK_SET);
        fread(&temp, sizeof(struct reg), 1, bin);
        if(show == 2) {
            printf("[%s]\t%s\n", temp.appId, temp.appName);
        } else if(strcmp(id, temp.appId) == 0) {
            printAppInfo(temp);
            break;
        }
        head = head->next;
    }
    printf("\n");
}


int PesquisaBinariaArquivoIndex(const char x[], FILE *indexFile, struct IndexRecord *result, int *count) {
    struct IndexRecord record;
    long left = 0, mid;
    fseek(indexFile, 0, SEEK_END);
    long right = (ftell(indexFile) / sizeof(struct IndexRecord)) - 1;

    int found = 0;  // Flag para indicar se a chave foi encontrada

    while (left <= right) {
        (*count)++;
        mid = left + (right - left) / 2;
        fseek(indexFile, mid * sizeof(struct IndexRecord), SEEK_SET);
        fread(&record, sizeof(struct IndexRecord), 1, indexFile);

        int cmp = strcmp(x, record.appChar);

        if (cmp == 0) {
            // Encontrou a chave no índice
            *result = record;
            found = 1;  // Define a flag para indicar que a chave foi encontrada
            right = mid - 1;  // Move para a esquerda para encontrar a chave anterior
        } else if (cmp < 0) {
            // Chave está à esquerda
            right = mid - 1;
        } else {
            // Chave está à direita
            left = mid + 1;
        }
    }

    if (!found) {
        // A chave não foi encontrada no índice, mas 'record' contém a chave mais próxima
        if (mid > 0) {  // Verifique se é maior que 0 para evitar underflow
            fseek(indexFile, (mid - 1) * sizeof(struct IndexRecord), SEEK_SET);
            fread(&record, sizeof(struct IndexRecord), 1, indexFile);
            *result = record;
        } else {
            // Não há chave anterior, então retorne um valor que indique que a chave não foi encontrada
            result->appChar[0] = '\0'; // Pode ser um valor inválido
            result->dataFilePosition = -1; // Por exemplo, -1 indica que a chave não foi encontrada
        }
    }


    return found;  // Retorna 1 se a chave foi encontrada, 0 se não
}



int main() {
    FILE *arq_bin;

    arq_bin = fopen("../sequential-db.bin", "rb");

    if(!arq_bin) {
        printf("File not found!\n");
        exit(1);
    }

    fseek(arq_bin, 0L, SEEK_END);
    long unsigned int bytes_len = ftell(arq_bin);
    long unsigned int qtd_registros = bytes_len/sizeof(struct reg);
    rewind(arq_bin);

    printf("\nFile Info:\n");
    printf("\t%li Bytes\n", bytes_len);
    printf("\t%li MBytes\n", (bytes_len/1024)/1024);
    printf("\t%li Registros\n", qtd_registros);
    printf("\t%d Bytes (struct sizeof)\n\n", sizeof(struct reg));  

    struct AVLNode* root = NULL; //AVL node price
    struct AVLNode* rootRating = NULL; //AVL node rating

    int resp = 1;
    while(resp) {
        printf("MENU:\n\n");
        printf("0 - sair\n");
        printf("1 - Pesquisa Binaria Direta no Arquivo de Dados sequencial-indexado [appId]\n");
        printf("2 - Criar Indices [...]\n");
        printf("3 - Consulta de Registros pelo campo [price] para apps PAGOS/NAO FREE (AVL em Memoria)\n"); //existem apps pagos com valor ZERO :)
        printf("4 - Consulta de Registros pelo campo [rating] (AVL em Memoria)\n");
        printf("5 - Consulta de Registros pelo campo [appId] (Indice Parcial em Arquivo)\n");
        printf("6 - Consulta de Registros pelo campo [appName] (Indice em Arquivo)\n");

        scanf("%d", &resp);

        char line[160];
        struct reg buffer;
        struct AVLNode* result;
        struct IndexRecord indexBuffer;
        FILE *indexFile;
        int request, showCMP, choice, cmp = 0, position = 0;

        rewind(arq_bin);

        switch (resp) {
            case 0:
                printf("Programa finalizado !");
                break;

            case 1:
                printf("\nPESQUISA BINARIA EM ARQUIVO\n");
                printf("Insira uma chave (appId): ");
                scanf(" %[^\n]s", line);
                printf("Deseja ver as comparacoes ? [1-sim / 2-nao]\n");
                scanf("%d", &showCMP);
                printf("\n");

                request = PesquisaBinariaArquivo(line, arq_bin, 0, qtd_registros, &cmp, showCMP, &buffer);

                if(request >= 0) {
                    printf("\nQuantidade de comparacoes: %d\n", cmp);
                    printAppInfo(buffer);
                }
                else {
                    printf("Registro nao encontrado !");
                }
                break;
            
            case 2:
                printf("\nCRIACAO DE INDICES\n\n");
                int resp2;
                printf("\t1 - (Arquivo) Partial Index [appId]\n");
                printf("\t2 - (Arquivo) Hash Index [appName]\n");
                printf("\t3 - (Memoria) AVL Index [rating]\n");
                printf("\t4 - (Memoria) AVL Index [price]\n");

                scanf("%d", &resp2);

                switch (resp2) {
                    case 1:
                        system("create-partial-idx-appId.exe");
                        break;
                    
                    case 2:
                        system("create-idx-appName.exe");
                        break;

                    case 3:
                        if(rootRating != NULL){
                            printf("AVL Index [rating] ja foi criado na Memoria!\n");
                            break; 
                        }
                        while (1) {
                            struct reg record;
                            int bytesRead = fread(&record, sizeof(struct reg), 1, arq_bin);
                            if (bytesRead != 1)
                                break; // Fim do arquivo

                            // Insira o registro na árvore AVL indexada por rating
                            rootRating = insert(rootRating, record.rating, position);

                            position++;
                        }
                        printf("AVL Index [rating] criado em Memoria com sucesso!\n");
                        break;

                    case 4:
                        if(root != NULL){
                            printf("AVL Index [price] ja foi criado na Memoria!\n");
                            break; 
                        }
                        while (1) {
                            struct reg record;
                            int bytesRead = fread(&record, sizeof(struct reg), 1, arq_bin);
                            if (bytesRead != 1)
                                break; // Fim do arquivo

                            // Insira o registro na árvore AVL indexada por preço (somente maiores que 0.0, não FREE)
                            if(!record.free)
                                root = insert(root, record.price, position);

                            position++;
                        }
                        printf("AVL Index [price] criado em Memoria com sucesso!\n");
                        break;
                    
                    default:
                        printf("none\n");
                        break;
                }
                break;

            case 3:
                printf("\nCONSULTA [price]\n\n");
                if(root == NULL) {
                    printf("Index AVL [price] nao foi criado!\n");
                    break;
                }
                printf("1 - Buscar por Id + Price\n2 - Buscar por Price (listar)\n");
                scanf("%d", &choice);
                if(choice == 1) {
                    printf("Insira uma chave (appId): ");
                    scanf(" %[^\n]s", line);
                }
                float priceToSearch; // 10.99
                printf("insira o [price] do app: ");
                scanf("%f", &priceToSearch);
                printf("\n\n");
                result = search(root, priceToSearch);
                if (result != NULL) {
                    printPositions(result->positions, arq_bin, line, choice);
                } else {
                    printf("Nenhum registro encontrado com para essa chave com esse preco\n");
                }
                break;

            case 4:
                printf("\nCONSULTA [rating] (0.0 ate 5.0)\n\n");
                if(rootRating == NULL) {
                    printf("Index AVL [rating] nao foi criado!\n");
                    break;
                }
                printf("1 - Buscar por Id + Rating\n2 - Buscar por Rating (listar)\n");
                scanf("%d", &choice);
                if(choice == 1) {
                    printf("Insira uma chave (appId): ");
                    scanf(" %[^\n]s", line);
                }
                float rating; // 0.5
                printf("insira o [rating] do app: ");
                scanf("%f", &rating);
                printf("\n\n");
                result = search(rootRating, rating);
                if (result != NULL) {
                    printPositions(result->positions, arq_bin, line, choice);
                } else {
                    printf("Nenhum registro encontrado com para essa chave com esse rating\n");
                }
                break;
            
            case 5:
                printf("\nCONSULTA [appId]\n\n");
                indexFile = fopen("Indices/indice-parcial-appId.idx", "rb");

                if(!indexFile) {
                    printf("File not found! Indice [appId] nao foi criado!\n");
                    break;
                }

                printf("Insira uma chave (appId): ");
                scanf(" %[^\n]s", line);

                PesquisaBinariaArquivoIndex(line, indexFile, &indexBuffer, &cmp);

                if(strcmp(indexBuffer.appChar, line) == 0) {
                    fseek(arq_bin, sizeof(struct reg) * indexBuffer.dataFilePosition, SEEK_SET); //SEEK_CUR vs SEEK_SET
                    fread(&buffer, sizeof(struct reg), 1, arq_bin);
                    printAppInfo(buffer);
                } else {
                    request = PesquisaBinariaArquivo(line, arq_bin, indexBuffer.dataFilePosition, qtd_registros, &cmp, 0, &buffer);
                    if(request >= 0) {
                        printf("\nQuantidade de comparacoes: %d\n", cmp);
                        printAppInfo(buffer); 
                    } else {
                        printf("Registro nao encontrado !");
                    }
                }
                fclose(indexFile);
                break;
            
            case 6:
                printf("\nCONSULTA [appName]\n\n");
                indexFile = fopen("Indices/indice-appName.idx", "rb");

                if(!indexFile) {
                    printf("File not found! Indice [appName] nao foi criado!\n");
                    break;
                }

                fseek(indexFile, 0L, SEEK_END);
                int bytesIdx = ftell(indexFile);
                int regIdx = bytesIdx/sizeof(struct IndexRecord);

                printf("Insira um nome (appName): ");
                scanf(" %[^\n]s", line);
                request = PesquisaBinariaArquivoName(line, indexFile, 0, regIdx, &position);

                if(request == -1) {fclose(indexFile); break;}

                fseek(indexFile, sizeof(struct IndexRecord) * position++, SEEK_SET); 
                fread(&indexBuffer, sizeof(struct IndexRecord), 1, indexFile);
                fseek(arq_bin, sizeof(struct reg) * indexBuffer.dataFilePosition, SEEK_SET); 
                fread(&buffer, sizeof(struct reg), 1, arq_bin);
                printAppInfo(buffer);

                while(1) {
                    printf("\n---\n");
                    fseek(indexFile, sizeof(struct IndexRecord) * position++, SEEK_SET); 
                    fread(&indexBuffer, sizeof(struct IndexRecord), 1, indexFile);

                    if(strcmp(indexBuffer.appChar, line) != 0) {break;}

                    fseek(arq_bin, sizeof(struct reg) * indexBuffer.dataFilePosition, SEEK_SET); 
                    fread(&buffer, sizeof(struct reg), 1, arq_bin);
                    printAppInfo(buffer);
                }

                fclose(indexFile);
                break;

            default:
                printf("Escolha uma opcao valida !");
                break;
        }
        printf("\n---------------------------------------------------------------------------\n\n");
    }

    fclose(arq_bin);

    return 0;
}