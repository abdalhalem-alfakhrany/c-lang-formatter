#define STB_C_LEXER_IMPLEMENTATION
#include "stb_c_lexer.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"


static void print_token(stb_lexer *lexer)
{
    switch (lexer->token) {
    case CLEX_id        : printf("_%s", lexer->string); break;
    case CLEX_eq        : printf("=="); break;
    case CLEX_noteq     : printf("!="); break;
    case CLEX_lesseq    : printf("<="); break;
    case CLEX_greatereq : printf(">="); break;
    case CLEX_andand    : printf("&&"); break;
    case CLEX_oror      : printf("||"); break;
    case CLEX_shl       : printf("<<"); break;
    case CLEX_shr       : printf(">>"); break;
    case CLEX_plusplus  : printf("++"); break;
    case CLEX_minusminus: printf("--"); break;
    case CLEX_arrow     : printf("->"); break;
    case CLEX_andeq     : printf("&="); break;
    case CLEX_oreq      : printf("|="); break;
    case CLEX_xoreq     : printf("^="); break;
    case CLEX_pluseq    : printf("+="); break;
    case CLEX_minuseq   : printf("-="); break;
    case CLEX_muleq     : printf("*="); break;
    case CLEX_diveq     : printf("/="); break;
    case CLEX_modeq     : printf("%%="); break;
    case CLEX_shleq     : printf("<<="); break;
    case CLEX_shreq     : printf(">>="); break;
    case CLEX_eqarrow   : printf("=>"); break;
    case CLEX_dqstring  : printf("\"%s\"", lexer->string); break;
    case CLEX_sqstring  : printf("'\"%s\"'", lexer->string); break;
    case CLEX_charlit   : printf("'%s'", lexer->string); break;
#if defined(STB__clex_int_as_double) && !defined(STB__CLEX_use_stdlib)
    case CLEX_intlit    : printf("#%g", lexer->real_number); break;
#else
    case CLEX_intlit    : printf("#%ld", lexer->int_number); break;
#endif
    case CLEX_floatlit  : printf("%g", lexer->real_number); break;
    default:
        if (lexer->token >= 0 && lexer->token < 256)
            printf("%c", (int) lexer->token);
        else {
            printf("<<<UNKNOWN TOKEN %ld >>>\n", lexer->token);
        }
        break;
    }
}

#define PARAN_OPEN 1
#define PARAN_CLOSE 2

int is_type(char *str) { return strstr("void int char float", str) != NULL; }
int is_binary(char c) { return (c == '+' || c == '-' || c == '*' || c == '/');  }

int main(void) {
    char *code =
        "void sum(int y,int x) {\n"
        "int z=(x+y)+(10);\n"
        "}\n";
    size_t formated_code_size = 0;
    char *formated_code = malloc(sizeof(char) * strlen(code) + 10);
    
    int buff_size = 0;
    char buff[255];
                
    stb_lexer lex;
    stb_c_lexer_init(&lex, code, code + strlen(code), (char *) malloc(0x10000), 0x10000);
    stb_c_lexer_get_token(&lex);
    
    int tabs_level = 0;
    
    while (1) {
        buff_size = 0;

        if (lex.token == CLEX_eof) break;
        
        if (lex.token == CLEX_id) {
            if(is_type(lex.string)) {
                buff_size += sprintf(&buff[buff_size], "%s ", lex.string);
                
                stb_c_lexer_get_token(&lex);
                buff_size += sprintf(&buff[buff_size], "%s ", lex.string);

                stb_c_lexer_get_token(&lex);
                if (lex.token == '=') {
                    // var assigenement
                    buff_size += sprintf(&buff[buff_size], "= ");

                    size_t paran_stack_size = 0;
                    int paran_stack[255];
                    
                    // get lhs expresion
                    while (1) {

                        if (lex.token == '(') {
                            paran_stack[paran_stack_size++] = PARAN_OPEN;
                            buff_size += sprintf(&buff[buff_size],"(", lex.token);
                        }

                        if (lex.token == ')') {
                            if (paran_stack[paran_stack_size] = PARAN_OPEN)
                                buff_size += sprintf(&buff[buff_size],")", lex.token), paran_stack_size--;
                        }

                        // binary operator
                        if (is_binary(lex.token))
                            buff_size += sprintf(&buff[buff_size]," %c ", lex.token);

                        // variable
                        if (lex.token == CLEX_id)
                            buff_size += sprintf(&buff[buff_size],"%s", lex.string);

                        // int litriral
                        if (lex.token == CLEX_intlit)
                            buff_size += sprintf(&buff[buff_size],"%d", lex.int_number);

                        // simicolon
                        if (lex.token == ';') {
                            buff_size+= sprintf(&buff[buff_size],";", lex.int_number);
                            break;
                        }
                        stb_c_lexer_get_token(&lex);                    
                    }
                    // end lhs expresion
                    
                    formated_code_size += sprintf(&formated_code[formated_code_size], "%.*s%s", tabs_level, "\t", buff);
                } else if (lex.token == '(') {
                    // function dec
                    buff_size += sprintf(&buff[buff_size], "(");

                    // get parametes
                    stb_c_lexer_get_token(&lex);
                    while (1) {
                        if (is_type(lex.string)) {
                            // tpye
                            if (is_type(lex.string))
                                buff_size += sprintf(&buff[buff_size], "%s ", lex.string);

                            stb_c_lexer_get_token(&lex);
                            // parameter name
                            buff_size += sprintf(&buff[buff_size], "%s", lex.string);
                            stb_c_lexer_get_token(&lex);
                        }
                        
                        if (lex.token == ',') {
                            buff_size += sprintf(&buff[buff_size], ", ");
                            stb_c_lexer_get_token(&lex);
                        } else if (lex.token == ')') {
                            buff_size += sprintf(&buff[buff_size], ") ");
                            break;
                        }
                    }
                    // end geting parameters
                    
                    formated_code_size += sprintf(&formated_code[formated_code_size], buff);
                }
            } else {
                printf("maybe var %s\n", lex.string);
            }
        } else {
            switch (lex.token) {
            case '{':
                formated_code_size += sprintf(&formated_code[formated_code_size], "{\n"), tabs_level++;
                break;
            case '}':
                formated_code_size += sprintf(&formated_code[formated_code_size], "\n}\n"), tabs_level--;
                break;
            defult:
                break;
            }
        }
        stb_c_lexer_get_token(&lex);
    }
    printf("original code\n\n-----\n%s-----\n\n", code);
    printf("formated code\n\n-----\n%s-----\n", formated_code);
    free(formated_code);
    
    return 0;
}
