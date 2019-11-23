/*fichier bison : parse*/
%{
  #include "storage.cpp"
  #include "processing.cpp"

  //sauts conditionnels
  typedef struct {
    int refInstruct;
    int refInstructTest;
  } instructAdress;

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char const *s) { fprintf (stderr, "%s\n", s); return 1; }//preciser erreurs? https://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
%}

%union{//variables
  int intValeur;
  double doubleValeur;
  char* stringValeur;
  char nom[50];
  instructAdress adresse;
}

%token <intValeur>    INT_VALUE
%token <doubleValeur> DOUBLE_VALUE
%token <stringValeur> STRING_VALUE

%token INT
%token DOUBLE
%token STRING
%token TAB
%token <nom> VARIABLE_NAME

%token SIZE
%token DELETE

//conserver?
%token SIN
%token TAN
%token SQRT
//fin conserver ?

%token DISPLAY

%token <adresse> IF
%token ELSE
%token END_IF

%token <adresse> WHILE
%token END_WHILE

%token <adresse> REPEAT
%token END_REPEAT

%token FOREACH
%token END_FOREACH

%token END_PRGM

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%

program : bloc END_PRGM { addInstruct(command::_EXIT_BLOCK_); };

bloc :
    bloc instruction '\n'
    |    /* Epsilon */
    ;

instruction : 
    INT    VARIABLE_NAME '=' expression {
                                            addInstruct(command::_EMPILE_VALUE_,$2);//nom var
                                            addInstruct(command::_CREATE_VARIABLE_,(int)1);//type var
                                          }
    | DOUBLE VARIABLE_NAME '=' expression { 
                                            addInstruct(command::_EMPILE_VALUE_,$2);//nom var
                                            addInstruct(command::_CREATE_VARIABLE_,(double)1);//type var
                                          }
    | STRING VARIABLE_NAME '=' expression { 
                                            addInstruct(command::_EMPILE_VALUE_,$2);//nom var
                                            addInstruct(command::_CREATE_VARIABLE_,"");//type var
                                          }
    
    | VARIABLE_NAME '=' expression  { addInstruct(command::_UPDATE_VARIABLE_,$1);/*nom var*/ }
    
    | TAB INT     VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_EMPILE_VALUE_,$3);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,(int)1);//type var
                                                  }
    | TAB DOUBLE  VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_EMPILE_VALUE_,$3);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,(double)1);//type var
                                                  }
    | TAB STRING  VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_EMPILE_VALUE_,$3);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,"");//type var
                                                  }
    
    | VARIABLE_NAME'['']' '=' expression          { addInstruct(command::_ADD_TABLE_ELEMENT_,$1);/*nom tab*/ }
    | VARIABLE_NAME'['INT_VALUE']' '=' expression  { 
                                                    addInstruct(command::_EMPILE_VALUE_,$3);//index tab
                                                    addInstruct(command::_UPDATE_TABLE_ELEMENT_,$1);//nom tab
                                                  }
    | DELETE VARIABLE_NAME'['INT_VALUE']'         { 
                                                    addInstruct(command::_EMPILE_VALUE_,$4);//index tab
                                                    addInstruct(command::_REMOVE_TABLE_ELEMENT_,$2);//nom tab
                                                  }

    | DISPLAY expression  { addInstruct(command::_PRINT_); }

    | IF                  { addInstruct(command::_ENTER_BLOCK_); }
      expression '\n'     {//ajouter comparaison empilant 0 ou 1
                                //apres interpretation de expression :
                            $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans then, sinon sauter à <adresse fin then / debut else>)
                          }
      bloc                {//THEN 
                                //apres interpretation de bloc :
                            $1.refInstruct = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse fin else / debut end_if>)

                            instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse fin then / debut else>
                          }
      bloc_else           { instructionList[$1.refInstruct].second.intVal = instructionList.size(); }//<adresse fin else / debut end_if>
      END_IF              { addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/ }

    | WHILE               { 
                            addInstruct(command::_ENTER_BLOCK_);
                            $1.refInstruct = instructionList.size();//<adresse test>
                          }
      expression '\n'     { //ajouter comparaison empilant 0 ou 1
                                //apres interpretation de expression :
                            $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                          }
      bloc                {
                                //apres interpretation de bloc :
                            addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                            instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                            instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                          }
      END_WHILE           { addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/ }

    
    | REPEAT '(' expression ')' bloc { /* TO DO */ }

    |   /* Ligne vide*/
    ;

bloc_else : ELSE '\n' bloc | /* Epsilon */ ;

expression :
    '(' expression ')'   { } //reduit expression

    | expression '+' expression     { addInstruct(command::_PLUS_);}
    | expression '-' expression     { addInstruct(command::_MOINS_);}
    | expression '*' expression     { addInstruct(command::_FOIS_);}
    | expression '/' expression     { addInstruct(command::_DIVISE_PAR_);}
    
    | INT_VALUE       { addInstruct(command::_EMPILE_VALUE_,$1); }
    | DOUBLE_VALUE    { addInstruct(command::_EMPILE_VALUE_,$1); }
    | STRING_VALUE    { addInstruct(command::_EMPILE_VALUE_,$1); }
    
    | VARIABLE_NAME   { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    
    | SIZE VARIABLE_NAME    { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }
    | VARIABLE_NAME'['INT_VALUE']'    { 
                                        addInstruct(command::_EMPILE_VALUE_,$3);//index tab
                                        addInstruct(command::_EMPILE_TABLE_ELEMENT_,$1);//nom tab
                                      }

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
