#include <stdio.h>
#include <string.h>

#define SYM_LIST_LENGTH 80
#define FST_LIST_LENGTH 20
#define MAX_OF_STATES 80

typedef struct {
  int amount;
  int list[FST_LIST_LENGTH];
} FinalSt;

typedef struct {
  int amount;
  char list[SYM_LIST_LENGTH];
} Sym;

typedef int (*TrTablePointer)[SYM_LIST_LENGTH];
typedef int (TrTable)[MAX_OF_STATES][SYM_LIST_LENGTH];

int get_opt(void);
void get_sym(Sym *);
void get_fst(FinalSt *);
void get_st_amount(int *);
void get_initial_st(int *);
void get_transition_table(TrTablePointer, Sym *, int);
void get_program_name(char *);
void debug_info(Sym *, int, FinalSt *, int, TrTablePointer, char *);
void gen_fn(TrTablePointer, Sym *, FinalSt *, int, int, char *);
void gen_goto(TrTablePointer, Sym *, FinalSt *, int, int, char *);

int main(void) {
  Sym sym;
  FinalSt finalSt;
  int st_amount;
  int initial_st;
  TrTable trTable;
  char c;
  char program_name[80];

  while ((c = get_opt()) != 'q') {
    switch (c) {
    case '0':
      get_sym(&sym);
      break;
    case '1':
      get_st_amount(&st_amount);
      break;
    case '2':
      get_fst(&finalSt);
      break;
    case '3':
      get_initial_st(&initial_st);
      break;
    case '4':
      get_transition_table(trTable, &sym, st_amount);
      break;
    case '5':
      get_program_name(program_name);
      break;
    case '6':
      gen_fn(trTable, &sym, &finalSt, initial_st, st_amount, program_name);
      break;
    case '7':
      gen_goto(trTable, &sym, &finalSt, initial_st, st_amount, program_name);
      break;
    case '8':
      debug_info(&sym, st_amount, &finalSt, initial_st, trTable, program_name);
      break;
    }
  }
  
  return 0;
}

int get_opt(void)
{
  int ret;
  for (int i = 0; i < 40; i++) printf("*");
  printf("\n");
  printf("0 ..... Entrar com conjunto de símbolos.\n");
  printf("1 ..... Entrar com número de estados.\n");
  printf("2 ..... Entrar com conjunto de estados finais.\n");
  printf("3 ..... Entrar com estado inicial.\n");
  printf("4 ..... Entrar com a tabela de transições.\n");
  printf("5 ..... Entrar com nome do programa.\n");
  printf("6 ..... Gerar programa por função.\n");
  printf("7 ..... Gerar programa por goto.\n");
  printf("8 ..... Exibir configurações.\n");
  printf("q ..... Sair.\n");
  for (int i = 0; i < 40; i++) printf("*");
  printf("\n");
  printf("O que deseja fazer? ");
  ret = getchar();
  while (getchar() != '\n')
    continue;
  printf("\n");
  return ret;
}

void get_sym(Sym *sym)
{
  printf("Quantos símbolos? ");
  scanf("%d", &sym->amount);
  getchar();

  for (int i = 0; i < sym->amount; i++) {
    printf("Qual o símbolo %d? ", i);
    scanf("%c", &sym->list[i]);
    while (getchar() != '\n')
      continue;
  }
  printf("\n");
}

void get_st_amount(int *st_amount)
{
  printf("Quantos estados? ");
  scanf("%d", st_amount);
  while (getchar() != '\n')
    continue;
  printf("\n");
}

void get_fst(FinalSt *fst)
{
  printf("Quantos estados finais? ");
  scanf("%d", &fst->amount);

  for (int i = 0; i < fst->amount; i++) {
    printf("Qual o estado final %d? ", i);
    scanf("%d", &fst->list[i]);
    while (getchar() != '\n')
      continue;
  }
  printf("\n");
}

void get_initial_st(int *ist)
{
  printf("Qual o estado inicial? ");
  scanf("%d", ist);
  while (getchar() != '\n')
    continue;
  printf("\n");
}

void get_transition_table(TrTablePointer trTable, Sym *sym, int st_amount)
{
  for (int i = 0; i < st_amount; i++) {
    for (int j = 0; j < sym->amount; j++) {
      printf("Para o estado e%d e símbolo '%c', qual o próximo estado? ", i, sym->list[j]);
      scanf("%d", &trTable[i][j]);
      while(getchar() != '\n')
        continue;
    }
  }
  printf("\n");
}

void get_program_name(char *name)
{
  printf("Qual o nome do programa? ");
  scanf("%s", name);
  while(getchar() != '\n')
    continue;
  printf("\n");
}

void debug_info(Sym *sym, int st_amount, FinalSt *finalSt, int initial_st, TrTablePointer trTable, char *name)
{
  printf("Quantidade de símbolos: %d\n", sym->amount);
  for (int i = 0; i < sym->amount; i++) {
    printf("Símbolo %d: '%c'\n", i, sym->list[i]);
  }
  printf("\n");

  printf("Quantidade de estados: %d\n\n", st_amount);

  printf("Quantidade de estados finais: %d\n", finalSt->amount);
  for (int i = 0; i < finalSt->amount; i++) {
    printf("Estado final %d: e%d\n", i, finalSt->list[i]);
  }
  printf("\n");

  printf("Estado inicial: e%d\n\n", initial_st);

  printf("Tabela de transições:\n");
  for (int i = 0; i < st_amount; i++) {
    for (int j = 0; j < sym->amount; j++) {
      int next_st = trTable[i][j];
      printf("Estado e%d e símbolo '%c', próximo estado: ", i, sym->list[j]);
      if (next_st >= 0) {
        printf("e%d\n", next_st);
      } else {
        printf("-\n");
      }
    }
  }
  printf("\n");

  printf("Nome do programa: %s\n\n", name);
}

void gen_helper(int *dirty, FILE *f)
{
  if (!*dirty) {
    *dirty = 1;
    fprintf(f, "\tif (");
  } else {
    fprintf(f, "else if (");
  }
}

/*
 * mode == 0 -> fn
 * mode == 1 -> goto
*/
void gen_state_body(TrTablePointer trTable, Sym *sym, FinalSt *fst, FILE *f, int state, int mode)
{
  int dirty = 0;
  int is_final = 0;
  int has_transitions = 0;

  fprintf(f, "\tcursor++;\n");
  for (int j = 0; j < sym->amount; j++) {
    int next_st = trTable[state][j];
    if (next_st >= 0) {
      has_transitions = 1;
      gen_helper(&dirty, f);
      fprintf(f, "input[cursor] == '%c') {\n", sym->list[j]);
      mode
      ? fprintf(f, "\t\tgoto e%d;\n", next_st)
      : fprintf(f, "\t\te%d();\n", next_st);
      fprintf(f, "\t} ");
    }
  }

  for (int j = 0; j < fst->amount && !is_final; j++) {
    if (fst->list[j] == state) {
      is_final = 1;
      gen_helper(&dirty, f);
      fprintf(f, "input[cursor] == '\\n') {\n");
      mode
      ? fprintf(f, "\t\tgoto aceita;\n")
      : fprintf(f, "\t\taceita();\n");
      fprintf(f, "\t} ");
    }
  }

  if (!is_final && !has_transitions) {
    mode
    ? fprintf(f, "\tgoto rejeita;\n")
    : fprintf(f, "\trejeita();\n");
  } else {
    fprintf(f, "else {\n");
    mode
    ? fprintf(f, "\t\tgoto rejeita;\n")
    : fprintf(f, "\t\trejeita();\n");
    fprintf(f, "\t}\n");
  }
}

void gen_fn(TrTablePointer trTable, Sym * sym, FinalSt *fst, int initial_st, int st_amount, char *program_name)
{
  FILE *f = fopen(program_name, "w");
  fprintf(f, "#include <stdio.h>\n");
  fprintf(f, "#include <stdlib.h>\n");
  fprintf(f, "#include <string.h>\n");
  fprintf(f, "\n");
  fprintf(f, "#define INPUT_LENGTH 80\n");
  fprintf(f, "\n");
  fprintf(f, "void aceita(void);\n");
  fprintf(f, "void rejeita(void);\n");
  for (int i = 0; i < st_amount; i++) {
    fprintf(f, "void e%d(void);\n", i);
  }
  fprintf(f, "\n");
  fprintf(f, "int cursor = -1;\n");
  fprintf(f, "char input[INPUT_LENGTH];\n");
  fprintf(f, "\n");
  fprintf(f, "int main(void)\n");
  fprintf(f, "{\n");
  fprintf(f, "\tprintf(\"Input: \");\n");
  fprintf(f, "\tfgets(input, INPUT_LENGTH, stdin);\n");
  fprintf(f, "\te%d();\n", initial_st);
  fprintf(f, "}\n");
  fprintf(f, "\n");
  fprintf(f, "void aceita()\n");
  fprintf(f, "{\n");
  fprintf(f, "\tprintf(\"Input aceito\\n\");\n");
  fprintf(f, "\texit(EXIT_SUCCESS);\n");
  fprintf(f, "}\n");
  fprintf(f, "\n");
  fprintf(f, "void rejeita()\n");
  fprintf(f, "{\n");
  fprintf(f, "\tprintf(\"Input rejeitado\\n\");\n");
  fprintf(f, "\texit(EXIT_FAILURE);\n");
  fprintf(f, "}\n");
  fprintf(f, "\n");

  for (int i = 0; i < st_amount; i++) {
    fprintf(f, "void e%d()\n", i);
    fprintf(f, "{\n");

    gen_state_body(trTable, sym, fst, f, i, 0);

    fprintf(f, "}\n\n");
  }

  fclose(f);
}

void gen_goto(TrTablePointer trTable, Sym * sym, FinalSt *fst, int initial_st, int st_amount, char *program_name)
{
  FILE *f = fopen(program_name, "w");
  fprintf(f, "#include <stdio.h>\n");
  fprintf(f, "#include <stdlib.h>\n");
  fprintf(f, "#include <string.h>\n");
  fprintf(f, "\n");
  fprintf(f, "#define INPUT_LENGTH 80\n");
  fprintf(f, "\n");
  fprintf(f, "int cursor = -1;\n");
  fprintf(f, "char input[INPUT_LENGTH];\n");
  fprintf(f, "\n");
  fprintf(f, "int main(void)\n");
  fprintf(f, "{\n");
  fprintf(f, "\tprintf(\"Input: \");\n");
  fprintf(f, "\tfgets(input, INPUT_LENGTH, stdin);\n");
  fprintf(f, "\tgoto e%d;\n", initial_st);
  fprintf(f, "\n");
  fprintf(f, "aceita:\n");
  fprintf(f, "\tprintf(\"Input aceito\\n\");\n");
  fprintf(f, "\texit(EXIT_SUCCESS);\n");
  fprintf(f, "\n");
  fprintf(f, "rejeita:\n");
  fprintf(f, "\tprintf(\"Input rejeitado\\n\");\n");
  fprintf(f, "\texit(EXIT_FAILURE);\n");
  fprintf(f, "\n");

  for (int i = 0; i < st_amount; i++) {
    fprintf(f, "e%d:\n", i);

    gen_state_body(trTable, sym, fst, f, i, 1);

    fprintf(f, "\n");
  }

  fprintf(f, "}\n");
  fclose(f);
}

