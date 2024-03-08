#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//############1###################
//dados do boot record: bytes por setor, setores reservados, setores por cluster, numero de fat, numero de setores por fat e número de entradas no diretório raiz.
//apresentar as entradas (somente 8.3) válidas do diretório raiz apresentando as seguintes informações: nome, extensão, tipo, first cluster e tamanho.
//mostrar o conteúdo de um arquivo (pode ser solicitado pelo usuário ou senão o primeiro arquivo encontrado no diretório raiz).

typedef struct {
    char nome[8];    
    char ext[3];              
    unsigned char attr;   
    unsigned char nada[14];      
    unsigned short int first_cluster; 
    unsigned int size; 
} __attribute__((packed)) Directory;//ignora o alinhameto de memoria de pares

int main(int argc, char const *argv[])
{
    // abre arquivo .img
    FILE *file;
    short int bytes_por_setores;
    unsigned char setores_por_cluster;
    uint8_t setores_reservados;
    uint8_t fat;
    unsigned char setores_por_fat;
    short int entradas_diretorio;

    
    file = fopen("./test.img", "rb");

    //setores  por cluster
    fseek(file, 13, SEEK_SET);
    fread(&setores_por_cluster,2,1, file);

    //setores  reservados
    fseek(file, 14, SEEK_SET);
    fread(&setores_reservados,1,1, file);

    //número de setores por fat
    fseek(file, 22, SEEK_SET);
    fread(&setores_por_fat,2,1, file);

    //número de FAT
    fseek(file, 16, SEEK_SET);
    fread(&fat,1,1, file);

    //número de entradas no diretório raiz
    fseek(file, 17, SEEK_SET);
    fread(&entradas_diretorio,2,1, file);

    //bytes por setor
    fseek(file, 11, SEEK_SET);
    fread(&bytes_por_setores,2,1, file);

    printf("Bytes por setor %hd \n", bytes_por_setores);
    printf("Setores por cluster %x \n", setores_por_cluster);
    printf("Setores reservados %hu \n", setores_reservados);
    printf("FAT  %hd \n", fat);
    printf("Setores por FAT %hu \n", setores_por_fat);
    printf("Numero de entradas no diretorio raiz %hu \n", entradas_diretorio);
    printf("-------------------------------------------------------\n");
    //#####################################################################################//
    //#############2###############
    //Entradas (somente 8.3) válidas do diretório raiz apresentando as seguintes informações: nome, extensão, tipo, first cluster e tamanho
    long int n_fat = (entradas_diretorio * setores_por_fat) + entradas_diretorio;
    long int pos_diretorio_raiz = (entradas_diretorio* setores_por_fat) + n_fat; 
    printf("A posicao inicial do diretorio raiz: %ld\n", pos_diretorio_raiz);

    // posicionar no início do diretório raiz e na posição 11 para ver se o atributo é valido para standard 8.3
    fseek(file, pos_diretorio_raiz, SEEK_SET);

    Directory entry;
    int inuteis = 0;
    int n_83 = 0;
    int s_83=0;
    int flag = 0;
    printf("size da estrutura: %d\n", sizeof(Directory));
    for(int i = 0; i < entradas_diretorio; i++){
        fread(&entry, sizeof(Directory), 1, file);
        //verifica se a entrada é vazia ou foi excluída, e ignora pois n tem o que ler, para a proxima entrada
        unsigned char sera_que_deletou_ou_ta_vaziu = entry.nome[0];
        if(sera_que_deletou_ou_ta_vaziu == 0x00 || sera_que_deletou_ou_ta_vaziu ==  0xE5){
            inuteis++;
           continue;            
        }
        //verifica o atributo do arquivo é no formato standard 8.3, se nao for vai para a proxima entrada
        if(entry.attr == 0x0F){
            n_83++;
            continue;
        }
        s_83++;
        printf("---------------------------------------------------------\n");
        printf("Nome: %s\n", entry.nome); // nome
        printf("Extensao: %s\n", entry.ext); //extensão
        printf("Tipo: %s\n", (entry.attr & 0x10) ? "Diretorio" : "Arquivo"); // Verifica o bit de atributo para determinar o tipo do arquivo
        printf("Tipo: %u\n",entry.attr);
        printf("Primeiro cluster: %x\n", entry.first_cluster);
        printf("Tamanho: %u bytes\n", entry.size);

        //############################3############################3
        //mostrar o conteúdo de um arquivo (pode ser solicitado pelo usuário ou senão o primeiro arquivo encontrado no diretório raiz).
        if(entry.attr == 0x20 && flag == 0){
            flag++;
            
        }
             
    }
    //printf("vazio ou excluidos: %d \n", inuteis);
    //printf("nao eh 8.3: %d \n", n_83);
    //printf("ss eh 8.3: %hd\n", s_83);
    //############################3############################3
    //mostrar o conteúdo de um arquivo (pode ser solicitado pelo usuário ou senão o primeiro arquivo encontrado no diretório raiz).
    long int tam_do_diretorio_raiz = entradas_diretorio * 32;
    long int pos_dados = pos_diretorio_raiz + tam_do_diretorio_raiz;
    int pos_fat = (setores_reservados * bytes_por_setores);
    printf("---------------------------------------------------------\n");
    printf("posicao dos dados: %ld\n", pos_dados);
    printf("posicao da fat: %d\n", pos_fat);
    le_conteudo_fat(pos_fat,pos_dados,entry.first_cluster, file);
    
    return 0;
}

void le_conteudo_fat(int pos_fat, long int pos_dados, unsigned short int first_cluster, FILE *file) {
    //subtrai 2 dos dados
    printf("ta lendo a fat\n");
    printf("posicao dos dados: %ld\n", pos_dados);
    printf("posicao da fat: %d\n", pos_fat);
    printf("Primeiro cluster: %x\n", first_cluster);
    char conteudo[300];

    unsigned char cluster[2];
    //enquanto o cluster de indice do tamanho de 2 bytes for diferente de 0xff 0xff continua procuranod e lendo o conteúdo
    while(cluster[0] != 0xff && cluster[1]!= 0xff){
        fseek(file, pos_fat, SEEK_SET);
        fread(&cluster,sizeof(cluster),1, file);
        printf("cluster: %s\n", cluster);
    }
}