/*fichier bison : parse*/
%{
  #include "storage.cpp"
  #include "processing.cpp"

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char const *s) { fprintf (stderr, "%s\n", s); return 1; }//preciser erreurs? https://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
%}

%union{
  double valeur;
  char nom[50];
  idInstruct adresse;
}

%token <valeur> NUMBER
%token <nom> IDENTIFIER
%type <valeur> expression
%token <adresse> IF
%token THEN
%token ELSE
%token END_IF
%token REPEAT
%token JUMP 
%token JUMP_IF_ZERO
%token PRINT
%token SIN
%token TAN
%token SQRT

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */


%%
bloc :
    bloc instruction '\n' 
    | bloc instruction
    |    /* Epsilon */
    ;

instruction : 
    expression { addInstruct(command::_PRINT_);   /* imprimer le résultat de l'expression */  }

    | IF expression '\n'  { 
                            $1.jumpToInstruct = indexInstruction;/*enregistre position*/
                            addInstruct(command::_JUMP_IF_ZERO_);
                          }
      THEN '\n' bloc      { 
                            $1.jumpToInstructIfFalse = indexInstruction;
                            addInstruct(command::_JUMP_);
                            instructionList[$1.jumpToInstruct].second = indexInstruction;/*enregistre position*/
                          }
      ELSE '\n' bloc      { instructionList[$1.jumpToInstructIfFalse].second = indexInstruction;/*enregistre position*/ }
      END_IF              {   }

    | REPEAT '(' expression ')' expression { /* TO DO */ }

    | IDENTIFIER '=' expression { 
                                  variables[$1] = $3; 
                                  addInstruct(command::_SET_IDENTIFIER_, $3); 
                                }
    |   /* Ligne vide*/
    ;

expression: 
    expression '+' expression     { addInstruct(command::_PLUS_);}
    | expression '-' expression     { addInstruct(command::_MOINS_);}
    | expression '*' expression     { addInstruct(command::_FOIS_);}
    | expression '/' expression     { addInstruct(command::_DIVISE_PAR_);}
    | '(' expression ')'            { }
    | NUMBER                        { addInstruct(command::_NUMBER_, $1);}
    | IDENTIFIER                    { addInstruct(command::_GET_IDENTIFIER_, variables[$1]);}
  /*
    | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
    | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
    | SQRT '(' expr ')' { $$ = sqrt($3); cout << "sqrt(" << $3 << ") = " << $$ << endl;}
  */
    ;
%%


int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
  yyparse();

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}