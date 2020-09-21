#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>


// Enum das intruções suportadas
enum instucoes {ADD=1 , ADDI, SUB, SUBI, BEQ, B, NOP};

FILE *f;
int linha = 0;
char str[25];
char mem_prog[100][25];  // Memória de programa 100 linhas com tamanho máximo de 20 caracteres

// Struct que representa a intrução
typedef struct{
    int Opcode;
    int  Op1;
    int  Op2;
    int  Op3;
    int  Valida;
    int  valorTemp1;
    int  valorTemp2;
    int  valorTemp3;
}instrucao;

// Program counter
unsigned int PC = 0;

//Banco de Registradores
unsigned int R[32];

//Tabela de Predição
unsigned char  predicao[32];

instrucao instBusca; // instrução no estágio de busca
instrucao instDecod; // instrução no estágio de decodificação
instrucao instExec;  // instrução no estágio de execução
instrucao instMem;   // instrução no estágio de memória
instrucao instWb;    // instrução no estágio de wb


void printBancoReg(){


    printf("Banco de Registradores ------------------------------------------------------------- \n\r");
    for(int i = 0; i< 8; i++)
        printf("|R%d - %d ", i, R[i]);
    printf("\n\r");
    for(int i = 8; i< 16; i++)
        printf("|R%d - %d ", i, R[i]);
    printf("\n\r");
    for(int i = 16; i< 24; i++)
        printf("|R%d - %d ", i, R[i]);
    printf("\n\r");
    for(int i = 24; i< 32; i++)
        printf("|R%d - %d ", i, R[i]);
    printf("\n\r");
    printf("----------------------------------------------------------------------------------- \n\r");

}

void busca(void){


    printf("Etapa de Busca --------------------------------------------------------------------- \n\r");

    instBusca.Opcode = NOP;

    if (strncmp(mem_prog[PC], "addi",4)== 0)
        instBusca.Opcode = ADDI;
    if (strncmp(mem_prog[PC], "add",3)== 0)
        instBusca.Opcode = ADD;
    if (strncmp(mem_prog[PC], "subi",4)== 0)
        instBusca.Opcode = SUBI;
    if (strncmp(mem_prog[PC], "sub",3)== 0)
        instBusca.Opcode = SUB;
    if (strncmp(mem_prog[PC], "b",1)== 0)
        instBusca.Opcode = B;
    if (strncmp(mem_prog[PC], "beq",3)== 0)
        instBusca.Opcode = BEQ;


    //addi $t1 , $t0 , 1
    strncpy(str, &mem_prog[PC][7], 2);
    instBusca.Op1 = atoi(str);

    strncpy(str, &mem_prog[PC][13], 2);
    instBusca.Op2 = atoi(str);

    strncpy(str, &mem_prog[PC][17], 2);
    instBusca.Op3 = atoi(str);

   printf("Memoria %s", mem_prog[PC]);
   printf("PC %d \n", PC);
   printf("Opcode %d \n", instBusca.Opcode);
   printf("Operando 1 %d \n", instBusca.Op1);
   printf("Operando 2 %d \n", instBusca.Op2);
   printf("Operando 3 %d \n", instBusca.Op3);


   PC++;

   printf("------------------------------------------------------------------------------------ \n\r");

}

void decodifica(void){

    printf("Etapa de Decodificação ------------------------------------------------------------- \n\r");

    instDecod = instBusca;

    if (instDecod.Opcode == B)
        instDecod.valorTemp1 = instDecod.Op1;
    else{
        instDecod.valorTemp2 = R[instDecod.Op2];
        if ((instDecod.Opcode == ADDI) || (instDecod.Opcode == SUBI))
            instDecod.valorTemp3 = instDecod.Op3;
        else
            instDecod.valorTemp3 = R[instDecod.Op3];
    }

   printf("------------------------------------------------------------------------------------ \n\r");
}

void executa(void){


    printf("Etapa de Execução ------------------------------------------------------------------ \n\r");


    instExec = instDecod;
    if ((instExec.Opcode == ADDI) || (instExec.Opcode == ADD))
        instExec.valorTemp1 = instExec.valorTemp2 + instExec.valorTemp3;

    if ((instExec.Opcode == SUBI) || (instExec.Opcode == SUB))
        instExec.valorTemp1 = instExec.valorTemp2 - instExec.valorTemp3;

    // Implemetar  instruções de desvio
    //if (instExec.Opcode == BEQ ){
    //    if(instExec.valorTemp1 == instExec.valorTemp2)
    //        PC = instExec.valorTemp3;
    //}
    //if (instExec.Opcode == B )
    //     PC = instExec.valorTemp1;

    printf("------------------------------------------------------------------------------------ \n\r");
}

void acessomem(void){

    printf("Etapa de Acesso Memória ------------------------------------------------------------ \n\r");



    // Como não  vamos simular sw e lw  este estágio não tem operações
    instMem = instExec;

    printf("------------------------------------------------------------------------------------ \n\r");
}

void escritawb(void){

    printf("Etapa de Escrita Resultados -------------------------------------------------------- \n\r");


    instWb = instMem;

    // apenas instruções que  escrevem o resultado no rgistrador
    if ((instWb.Opcode == ADDI) || (instWb.Opcode == ADD)|| (instWb.Opcode == SUBI) || (instWb.Opcode == SUB))
        R[instWb.Op1] = instWb.valorTemp1;


   printf("------------------------------------------------------------------------------------ \n\r");

}


int main()
{
    setlocale(LC_ALL,"Portuguese");
    char tecla;

    f= fopen("exemplo_codigo.txt", "r");

    if (!f) {
        printf("Erro ao acessar arquivo de programa \n\r");
        exit(1);
    }

    printf("Memória de Programa Lida ----------------------------------------------------------- \n\r");

    while (fgets(str, 25, f) != NULL){
        strcpy(mem_prog[linha], str);
        printf("%s", mem_prog[linha]);
        linha ++;  // vai limitar o tamanho na etapa de busca
    }
     printf("Código fonte com %d linhas \n", linha-1);

    printf("------------------------------------------------------------------------------------ \n\r");


    while(1){

        scanf("%c", &tecla);

        if (tecla == '\n') {
            escritawb();
            acessomem();
            executa();
            decodifica();
            busca();
            printBancoReg();
        }

    }

    return 0;
}
