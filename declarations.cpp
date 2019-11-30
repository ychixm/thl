//memory
#include <string>
#include <deque>
#include <stack>
#include <map>

//processing (gestion affichage)
#include <iostream>
#include <cstdio> //getchar
#include <iomanip>
#include <cmath>

using namespace std;


//I. ENUMERATIONS
enum class valType {//fixe types
	_bool_,
	_int_,
	_double_,
	_string_
};

enum class operation {//fixe operations
	_plus_,
	_moins_,
	_fois_,
	_divisePar_
};

enum class comparaison {//fixe comparaison
	_and_,
	_or_,
	_equiv_,
	_diff_,
	_inferieur_,
	_superieur_,
	_inf_egal_,
	_sup_egal_
};


enum class tabAction {//fixe operations
	_empile_size_,
	_empile_case_,
	_create_,
	_add_,
	_update_,
	_remove_
};

enum class command {
	//MEMOIRE
	_ENTER_BLOCK_,
	_EXIT_BLOCK_,

	//EMPILEMENT
	_EMPILE_VALUE_,
	_EMPILE_VARIABLE_,
	_EMPILE_TABLE_SIZE_,
	_EMPILE_TABLE_ELEMENT_,

	//OPERATIONS (var to var)
	_INCREMENT_,
	_DECREMENT_,
	_PLUS_CREMENT_,
	_MOINS_CREMENT_,
	_FOIS_CREMENT_,
	_DIVISE_CREMENT_,
	_PLUS_,
	_MOINS_,
	_FOIS_,
	_DIVISE_PAR_,

	//COMPARAISON
	_AND_,
	_OR_,
	_EQUIV_,
	_DIFF_,
	_SUPERIEUR_,
	_INFERIEUR_,
	_SUP_EGAL_,
	_INF_EGAL_,

	//SAUTS (conditions, boucles, fonctions)
	_GOTO_,
	_GOTO_TEST_,
	_GOTO_TEST_INV_,

	//VARIABLES
	_CREATE_VARIABLE_,
	_UPDATE_VARIABLE_,

	//TABLEAUX
	_CREATE_TABLE_,
	_ADD_TABLE_ELEMENT_,
	_UPDATE_TABLE_ELEMENT_,
	_REMOVE_TABLE_ELEMENT_,

	//ENTREE SORTIE
	_PRINT_,
	_STOP_
};

enum class errorCode {
	conversionType,
	unknowVariable,
	alreadyUseVariable,
	emptyExecutionStack
};


//II. TYPES PERSONNALISES
typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int tabPos = -1;//valeur par defaut : flag d'invalidation
} valAccess;

typedef struct {//stockage tableau a part : doit preserver valeur ajoutee dans couche memoire (voir part 5) differente de celle supportant la declaration
	unsigned int memoryLayer = 0;
	valType type = valType::_int_;
	deque<int> valuesPos;
} tabAccess;

typedef struct {
	unsigned int boolListSize = 0;
	unsigned int intListSize = 0;
	unsigned int doubleListSize = 0;
	unsigned int stringListSize = 0;
} memoryState;

typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int intVal = -1;
	double doubleVal = -1;
	string stringVal = "";
} valInstruct;

typedef pair<command, valInstruct> instruction;

typedef void (*functionPointer)(valInstruct& instructContent);//necessaire pour map de commandes - fonctions lambda


//III. VARIABLES GLOBALES
deque<bool>     boolList;
deque<int>		intList;
deque<double>	doubleList;
deque<string>	stringList;

stack<valAccess> executionPile;

map<string, valAccess> variables;

deque<int>		intArray;
deque<double>	doubleArray;
deque<string>	stringArray;
map<string, tabAccess> tableaux;

stack<memoryState> memoryLayer;

deque<instruction> instructionList;

unsigned int indexInstruction = 0;   // compteur instruction 

map<errorCode,string> errorMessage = {
	{errorCode::conversionType,"types incompatibles - échec de conversion"},
	{errorCode::unknowVariable,"nom de variable inconnu"},
	{errorCode::alreadyUseVariable,"nom de variable déjà en utilisation"},
	{errorCode::emptyExecutionStack,"pile vide"}
};


//IV. PROTOTYPES
 // Memory
void printVal(string beginMessage, valAccess val, string endMessage);
valAccess depiler();
void delVal(valAccess val);
void delVar(string name);
void delTabVal(string tabName, int tabCase);
void delTab(string tabName);
void enterMemoryLayer();
void exitMemoryLayer();

// Utils
void error(errorCode cause);
void executeOperation(operation operation);
void executeComparaison(comparaison comparaison);
void executeCrement(string varName, operation operation);
valAccess addVal(valInstruct instructContent);
valAccess addVar(valInstruct instructContent);
void addInstruct(command command);
void addInstruct(command command, int intValue);
void addInstruct(command command, double doubleValue);
void addInstruct(command command, string stringValue);
void executeTabAction(instruction& instructContent, tabAction action);
void replaceString(string& subject, const string& search, const string& replace);

// Processing
void displayGeneratedProgram();
void executeGeneratedProgram();
