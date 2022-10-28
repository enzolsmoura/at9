/*
 * PREENCHA O NOME E RA DA DUPLA!!!!!!
 *
 * Nome: Enzo Leonardo Sabatelli de Moura
 * RA: 21.01535-0
 *
 * Nome: Enzo Lipari Saisi
 * RA: 21.00788-8
 *
 * ENTREGUE APENAS O ARQUIVO MAIN.C
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/sqlite3.h"
#include <string.h>
#define MAX_CHAR 500
#define MAX_PROD 18


int novoId(sqlite3 *db, sqlite3_stmt *stmt);
void lerString(char s[MAX_CHAR]);
void gerenciarEstoque (sqlite3 *db, sqlite3_stmt *stmt);
void fecharVenda(sqlite3 *db, sqlite3_stmt *stmt, int idVenda);
void novaCompra(sqlite3 *db, sqlite3_stmt *stmt,int idVenda);
void insereVenda(sqlite3 *db, sqlite3_stmt *stmt, int codigo, int quant, int idVenda, float preco);

int main() {
  sqlite3 *db = NULL;
  sqlite3_stmt *stmt = NULL;
  int conexao, op, idVenda, testador;
  int sessao = 0;
  system("clear");
  printf("Criando conexão com o banco: \n");
  conexao = sqlite3_open("Lojinha.db", &db);
  if (conexao != SQLITE_OK) {
    printf("Erro ao conectar ao banco.\n\n");
    exit(0);
  }
  printf("Sucesso na conexão!\n");
  printf("Escolha a operação a ser realizada:\n\n[1] - Nova venda\n[2] - Adicionar produtos ao carrinho\n[3] - Fechar venda\n[4] - Gerenciar o estoque\n[5] - Sair do programa\n");
  testador = scanf("%i",&op);
  if(testador!=1){
    printf("Você não digitou um número!\nSaindo do programa...\n");
    exit(0);
  }
  while(op!=5){
    switch (op) {
    default:
      printf("Opção não reconhecida. Por favor, tente outra opção. \n");
      break;
    case 1:
    if(sessao==1){
      printf("Já há uma venda em aberto. Para fechar uma venda, digite 3.\n");
    }
    else{
      system("clear");
      idVenda = novoId(db,stmt);
      sessao = 1;
      printf("\nO identificador da nova venda é %i.\n",idVenda);
    }
      break;
    case 2:
    if(sessao==0){
      printf("Ainda não foi iniciada uma nova venda. Por favor, digite 1 se quiser iniciar uma nova venda.\n");
    }
    else{
      novaCompra(db,stmt,idVenda);
    }
      break;
    case 3:
    if(sessao==0){
      printf("Ainda não foi iniciada uma nova venda. Por favor, digite 1 se quiser iniciar uma nova venda. \n");
    }
    else{
      fecharVenda(db,stmt,idVenda);
      sessao = 0;
    }
      break;
    case 4:
      gerenciarEstoque(db,stmt);
      break;
    } 
    printf("Escolha a operação a ser realizada:\n\n[1] - Nova venda\n[2] - Adicionar produtos ao carrinho\n[3] - Fechar venda\n[4] - Gerenciar o estoque\n[5] - Sair do programa\n");
    testador = scanf("%i",&op);
    if(testador!=1){
      printf("Você não digitou um número!\nSaindo do programa...\n");
      exit(0);
    }
  }
  printf("Saindo do programa...\n");

  return 0;
}

int novoId(sqlite3 *db, sqlite3_stmt *stmt){
  int novoId;
  sqlite3_prepare(db, "SELECT max(idvenda) FROM vendas", -1, &stmt, NULL);
  sqlite3_step(stmt);
  novoId = sqlite3_column_double(stmt,0)+1;
  sqlite3_finalize(stmt);
  return novoId;
}

void novaCompra(sqlite3 *db, sqlite3_stmt *stmt, int idVenda){
  int codigo, quant, testador;
  float precoTotal = 0;
  char query[MAX_CHAR];
  system("clear");
  sqlite3_prepare(db, "SELECT * FROM estoque", -1, &stmt, NULL);
  printf("Código\tNome\t\tQuantidade\tPreço\n");
  while (sqlite3_step(stmt) != SQLITE_DONE){
    for (int i = 0; i < sqlite3_column_count(stmt); i++)
      printf("%s\t", sqlite3_column_text(stmt, i));
    printf("\n");
  }
  sqlite3_finalize(stmt);
  while(1){
    printf("\nDigite o código do produto que deseja (0 para sair deste menu): ");
    testador = scanf("%i",&codigo);
    if(testador!=1){
      printf("Você não digitou um número!\nSaindo do programa...\n");
      exit(0);
    }
    if(codigo==0){
      break;
    }
    sprintf(query,"SELECT codigo FROM estoque WHERE codigo=%i",codigo);
    sqlite3_prepare(db,query, -1, &stmt, NULL);
    sqlite3_step(stmt);
    if((int)sqlite3_column_double(stmt,0)==0){
      sqlite3_finalize(stmt);
      printf("Código de produto não encontrado. Por favor, insira um código válido.\n");
    }
    else{
      sqlite3_finalize(stmt);
      sprintf(query,"SELECT quant FROM estoque WHERE codigo=%i",codigo);
      sqlite3_prepare(db,query, -1, &stmt, NULL);
      sqlite3_step(stmt);
      if((int)sqlite3_column_double(stmt,0)==0){
          printf("Este produto não está disponível no momento!\n");
          break;
      }
      printf("Digite a quantidade a ser adicionada: ");
      testador = scanf("%i",&quant);
      if(testador!=1){
        printf("Você não digitou um número!\nSaindo do programa...\n");
        exit(0);
      }
      if(quant>(int)sqlite3_column_double(stmt,0)||quant<=0){
        printf("Quantidade não disponível! (Mínimo: 1 / Máximo: %i)\n",(int)sqlite3_column_double(stmt,0));
        break;
      }
      else{
        sqlite3_finalize(stmt);
        sprintf(query,"SELECT preco FROM estoque WHERE codigo=%i",codigo);
        sqlite3_prepare(db,query, -1, &stmt, NULL);
        sqlite3_step(stmt);
        precoTotal = (float)quant*(float)sqlite3_column_double(stmt,0);
        insereVenda(db,stmt,codigo,quant,idVenda,precoTotal);
        sqlite3_finalize(stmt);
        sprintf(query, "SELECT vendas.codigo, estoque.nome, vendas.quant, vendas.preco FROM vendas INNER JOIN estoque ON vendas.codigo = estoque.codigo WHERE idVenda = %i",idVenda);
        sqlite3_prepare(db, query, -1, &stmt, NULL);
        printf("Código\tNome\t\tQuantidade\tPreço Total\n");
        while (sqlite3_step(stmt) != SQLITE_DONE){
          for (int i = 0; i < sqlite3_column_count(stmt); i++)
            printf("%s\t", sqlite3_column_text(stmt, i));
          printf("\n");
        }
        sqlite3_finalize(stmt);
        sprintf(query, "SELECT sum(preco) FROM (SELECT vendas.codigo, estoque.nome, vendas.quant, vendas.preco FROM vendas INNER JOIN estoque ON vendas.codigo = estoque.codigo WHERE idVenda = %i)",idVenda);
        sqlite3_prepare(db, query, -1, &stmt, NULL);
        sqlite3_step(stmt);
        printf("Subtotal da compra: R$%.2f\n\n",(float)sqlite3_column_double(stmt,0));
        sqlite3_finalize(stmt);
        printf("Produto(s) adicionado(s) ao carrinho.\n\n");
        sqlite3_prepare(db, "SELECT * FROM estoque", -1, &stmt, NULL);
        printf("Código\tNome\t\tQuantidade\tPreço\n");
        while (sqlite3_step(stmt) != SQLITE_DONE){
          for (int i = 0; i < sqlite3_column_count(stmt); i++)
            printf("%s\t", sqlite3_column_text(stmt, i));
          printf("\n");
        }
        break;
      }
    }
  }
}

void insereVenda(sqlite3 *db, sqlite3_stmt *stmt, int codigo, int quant, int idVenda, float preco){
  char query[MAX_CHAR];
  sprintf(query,"SELECT COUNT() FROM vendas WHERE codigo = %i AND idVenda = %i",codigo,idVenda);
  sqlite3_prepare(db,query, -1, &stmt, NULL);
  sqlite3_step(stmt);
  if((int)sqlite3_column_double(stmt,0)==0){
    sqlite3_finalize(stmt);
    sprintf(query,"INSERT INTO vendas (idvenda, codigo, quant, preco) VALUES (%i,%i,%i,%.1f)",idVenda,codigo,quant,preco);
    sqlite3_prepare(db,query, -1, &stmt, NULL);
    sqlite3_step(stmt);
  }
  else{
    sqlite3_finalize(stmt);
    sprintf(query,"UPDATE vendas SET quant=quant+%i,preco=preco+%.1f WHERE codigo = %i AND idVenda = %i",quant,preco,codigo,idVenda);
    sqlite3_prepare(db,query, -1, &stmt, NULL);
    sqlite3_step(stmt);
  }
  sqlite3_finalize(stmt);
  sprintf(query, "UPDATE estoque SET quant=quant-%i WHERE codigo=%i", quant, codigo);
  sqlite3_prepare(db, query, -1, &stmt, NULL);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void fecharVenda(sqlite3 *db, sqlite3_stmt *stmt, int idVenda){
  char query[MAX_CHAR];
  system("clear");
  sprintf(query, "SELECT vendas.codigo, estoque.nome, vendas.quant, vendas.preco FROM vendas INNER JOIN estoque ON vendas.codigo = estoque.codigo WHERE idVenda = %i",idVenda);
  sqlite3_prepare(db, query, -1, &stmt, NULL);
  printf("Código\tNome\t\tQuantidade\tPreço Total\n");
  while (sqlite3_step(stmt) != SQLITE_DONE){
    for (int i = 0; i < sqlite3_column_count(stmt); i++)
      printf("%s\t", sqlite3_column_text(stmt, i));
    printf("\n");
  }
  sqlite3_finalize(stmt);
  sprintf(query, "SELECT sum(preco) FROM (SELECT vendas.codigo, estoque.nome, vendas.quant, vendas.preco FROM vendas INNER JOIN estoque ON vendas.codigo = estoque.codigo WHERE idVenda = %i)",idVenda);
  sqlite3_prepare(db, query, -1, &stmt, NULL);
  sqlite3_step(stmt);
  printf("Preço total da compra: R$%.2f\n\n",(float)sqlite3_column_double(stmt,0));
}

void gerenciarEstoque(sqlite3 *db, sqlite3_stmt *stmt){
  char produto[MAX_PROD], query[MAX_CHAR];
  int codigo, quant, min, max, testador;
  float preco;
  system("clear");
  sqlite3_prepare(db, "SELECT * FROM estoque", -1, &stmt, NULL);
  printf("Código\tNome\t\tQuantidade\tPreço\n");
  while (sqlite3_step(stmt) != SQLITE_DONE){
    for (int i = 0; i < sqlite3_column_count(stmt); i++)
      printf("%s\t", sqlite3_column_text(stmt, i));
    printf("\n");
  }
  sqlite3_finalize(stmt);
  while(1){
    printf("\nInforme o código do produto que deseja adicionar (0 se quiser adicionar um novo produto): ");
    testador = scanf("%i",&codigo);
    if(testador!=1){
      printf("Você não digitou um número!\nSaindo do programa...\n");
      exit(0);
    }
    if(codigo<0){
      printf("Insira um valor maior que 0!\n");
      break;
    }
    else{
      sqlite3_prepare(db, "SELECT min(codigo) FROM estoque", -1, &stmt, NULL);
      sqlite3_step(stmt);
      min = (int)sqlite3_column_double(stmt,0);
      sqlite3_finalize(stmt);
      sqlite3_prepare(db, "SELECT max(codigo) FROM estoque", -1, &stmt, NULL);
      sqlite3_step(stmt);
      max = (int)sqlite3_column_double(stmt,0);
      sqlite3_finalize(stmt);
      if(codigo>=min&&codigo<=max){
        printf("Digite a quantidade que deseja adicionar ao estoque: ");
        testador = scanf("%i",&quant);
        if(testador!=1){
          printf("Você não digitou um número!\nSaindo do programa...\n");
          exit(0);
        }
        printf("Digite o preço do produto: ");
        testador = scanf("%f",&preco);
        if(testador!=1){
          printf("Você não digitou um número!\nSaindo do programa...\n");
          exit(0);
        }
        sprintf(query, "UPDATE estoque SET quant=quant+%i,preco=%.1f WHERE codigo=%i", quant,preco,codigo);
        sqlite3_prepare(db, query, -1, &stmt, NULL);
        sqlite3_step(stmt);
        printf("Produto(s) adicionado(s) ao estoque.\n\n");
        sqlite3_finalize(stmt);
        sqlite3_prepare(db, "SELECT * FROM estoque", -1, &stmt, NULL);
        printf("Código\tNome\t\tQuantidade\tPreço\n");
        while (sqlite3_step(stmt) != SQLITE_DONE){
          for (int i = 0; i < sqlite3_column_count(stmt); i++)
            printf("%s\t", sqlite3_column_text(stmt, i));
          printf("\n");
        }
        break;
      }
      else{
        if(codigo>max){
          printf("Código de produto não encontrado. Para cadastrar um produto novo, digite 0.\n");
          break;
        }
        if(codigo<min){
          printf("Digite o nome do produto a ser cadastrado: ");
          lerString(produto);
          printf("Digite a quantidade que deseja adicionar ao estoque: ");
          testador = scanf("%i",&quant);
          if(testador!=1){
            printf("Você não digitou um número!\nSaindo do programa...\n");
            exit(0);
          }
          printf("Digite o preço do produto: ");
          testador = scanf("%f",&preco);
          if(testador!=1){
            printf("Você não digitou um número!\nSaindo do programa...\n");
            exit(0);
          }
          sprintf(query, "INSERT INTO estoque (nome,quant,preco) VALUES (\"%-18s\",%i,%.1f)",produto,quant,preco);
          sqlite3_prepare(db, query, -1, &stmt, NULL);
          sqlite3_step(stmt);
          printf("Produto(s) adicionado(s) ao estoque.\n\n");
          sqlite3_finalize(stmt);
          sqlite3_prepare(db, "SELECT * FROM estoque", -1, &stmt, NULL);
          printf("Código\tNome\t\tQuantidade\tPreço\n");
          while (sqlite3_step(stmt) != SQLITE_DONE){
            for (int i = 0; i < sqlite3_column_count(stmt); i++)
              printf("%s\t", sqlite3_column_text(stmt, i));
            printf("\n");
          }
          break;
        }
      }
    }
  }
  
}

void lerString(char s[MAX_CHAR]) {
  setbuf(stdin, 0);
  scanf("\n");
  fgets(s, MAX_CHAR, stdin);
  if (s[strlen(s)-1] == '\n') {
    s[strlen(s)-1] = '\0';
  }
  
}