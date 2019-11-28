#include "memory.cpp"

/********************************************************/
/*														*/
/*	PARTIE II : GESTION DES COMMANDES					*/
/*		SOUS PARTIE 1 : OPERATION SUR LES VALEURS		*/
/*		SOUS PARTIE 2 : DECLARATION DES COMMANDES		*/
/*		SOUS PARTIE 3 : GESTION DES INSTRUCTIONS		*/
/*		SOUS PARTIE 4 : GESTION DES TABLEAUX			*/
/*		SOUS PARTIE 5 : EXECUTION SELON LES COMMANDES	*/
/*														*/
/********************************************************/

/********************************************************/
/*		SOUS PARTIE 1 : OPERATION SUR LES VALEURS		*/
/********************************************************/


void error(errorCode cause) {
	cout << "ERREUR : " << errorMessage[cause] << endl;
	exit((int)cause + 1);//code erreur
}


valAccess castVal(valAccess value, valType cast) {
	if (value.type == cast) return value;

	else if (value.type != valType::_string_ && cast != valType::_string_) {
		valAccess result = { cast };
		switch (cast) {
		case valType::_bool_:
			result.tabPos = boolList.size();
			switch (value.type) {
			case valType::_int_:
				boolList.push_back(intList[value.tabPos]);
				break;
			case valType::_double_:
				boolList.push_back(doubleList[value.tabPos]);
				break;
			}
			break;

		case valType::_int_:
			result.tabPos = intList.size();
			switch (value.type) {
			case valType::_bool_:
				intList.push_back(boolList[value.tabPos]);
				break;
				case valType::_double_:
					intList.push_back(doubleList[value.tabPos]);
					break;
			}
			break;

		case valType::_double_:
			result.tabPos = doubleList.size();
			switch (value.type) {
			case valType::_bool_:
				doubleList.push_back(boolList[value.tabPos]);
				break;
			case valType::_int_:
				doubleList.push_back(intList[value.tabPos]);
				break;
			}
			break;
		}

		delVal(value);
		return result;
	}

	else error(errorCode::conversionType);
}


void executeOperation(operation operation) {
	//recupere valeurs
	valAccess val2 = depiler();
	valAccess val1 = depiler();

	if (val1.type == valType::_string_ && val2.type == valType::_string_) {
		switch (operation) {
		case operation::_plus_://concatenation
			stringList[val1.tabPos] += stringList[val2.tabPos];
			break;
		}
		delVal(val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(val2);

		executionPile.push({ valType::_string_,(int)stringList.size() });
		stringList.push_back(stringList[val1.tabPos]);
		return;
	}

	else {//si pas deux reels, erreur levee par fonction cast
		val2 = castVal(val2, valType::_double_);
		val1 = castVal(val1, valType::_double_);

		double result(0);//cast au plus haut
		switch (operation) {
		case operation::_plus_:
			result = doubleList[val1.tabPos] + doubleList[val2.tabPos];
			break;
		case operation::_moins_:
			result = doubleList[val1.tabPos] - doubleList[val2.tabPos];
			break;
		case operation::_fois_:
			result = doubleList[val1.tabPos] * doubleList[val2.tabPos];
			break;
		case operation::_divisePar_:
			result = doubleList[val1.tabPos] / doubleList[val2.tabPos];
			break;
		}
		delVal(val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(val2);

		executionPile.push({ valType::_double_,(int)doubleList.size() });
		doubleList.push_back(result);
		return;
	}
}

//passer en bool / cast
void executeCrement(string varName, operation operation) {
	//recupere valeurs
	valAccess val = depiler();

	int valInt(0);
	double valDouble(0);
	string valString("");

	switch (val.type) {
	case valType::_int_:
		valInt = intList[val.tabPos];
		break;
	case valType::_double_:
		valDouble = doubleList[val.tabPos];
		break;
	case valType::_string_:
		valString = stringList[val.tabPos];
		break;
	}

	delVal(val);


	if (variables.find(varName) != variables.end() &&
		((variables[varName].type != valType::_string_ && val.type != valType::_string_) ||
		(variables[varName].type == valType::_string_ && val.type == valType::_string_))) {//var existe bien
		valAccess copy = { variables[varName].type };

		switch (variables[varName].type) {
		case valType::_int_:
			switch (operation) {
			case operation::_plus_:
				intList[variables[varName].tabPos] += (valInt ? valInt : valDouble);
				break;
			case operation::_moins_:
				intList[variables[varName].tabPos] -= (valInt ? valInt : valDouble);
				break;
			case operation::_fois_:
				intList[variables[varName].tabPos] *= (valInt ? valInt : valDouble);
				break;
			case operation::_divisePar_:
				intList[variables[varName].tabPos] /= (valInt ? valInt : valDouble);
				break;
			}

			copy.tabPos = intList.size();
			intList.push_back(intList[variables[varName].tabPos]);
			executionPile.push(copy);
			break;
		case valType::_double_:
			switch (operation) {
			case operation::_plus_:
				doubleList[variables[varName].tabPos] += (valInt ? valInt : valDouble);
				break;
			case operation::_moins_:
				doubleList[variables[varName].tabPos] -= (valInt ? valInt : valDouble);
				break;
			case operation::_fois_:
				doubleList[variables[varName].tabPos] *= (valInt ? valInt : valDouble);
				break;
			case operation::_divisePar_:
				doubleList[variables[varName].tabPos] /= (valInt ? valInt : valDouble);
				break;
			}

			copy.tabPos = doubleList.size();
			doubleList.push_back(doubleList[variables[varName].tabPos]);
			executionPile.push(copy);
			break;
		case valType::_string_:
			if (operation == operation::_plus_) stringList[variables[varName].tabPos] += valString;
			//else erreur

			copy.tabPos = stringList.size();
			stringList.push_back(stringList[variables[varName].tabPos]);
			executionPile.push(copy);
			break;
		}
	}
	else error(errorCode::unknowVariable);
}

//passer en bool / cast
void executeComparaison(comparaison operation) {
	//recupere valeurs
	valAccess val2 = depiler();
	valAccess val1 = depiler();

	int val1Int(0), val2Int(0);
	double val1Double(0), val2Double(0);
	string val1String(""), val2String("");

	switch (val1.type) {
	case valType::_int_:
		val1Int = intList[val1.tabPos];
		break;
	case valType::_double_:
		val1Double = doubleList[val1.tabPos];
		break;
	case valType::_string_:
		val1String = stringList[val1.tabPos];
		break;
	}
	switch (val2.type) {
	case valType::_int_:
		val2Int = intList[val2.tabPos];
		break;
	case valType::_double_:
		val2Double = doubleList[val2.tabPos];
		break;
	case valType::_string_:
		val2String = stringList[val2.tabPos];
		break;
	}

	delVal(val1);
	delVal(val2);
	//execute operation et enregistre nouvelle valeur
	if (val1.type != valType::_string_ && val2.type != valType::_string_) {//si non string
		bool result(0);
		switch (operation) {
		case comparaison::_equiv_:
			result = (val1Int ? val1Int : val1Double) == (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case comparaison::_diff_:
			result = (val1Int ? val1Int : val1Double) != (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case comparaison::_inferieur_:
			result = (val1Int ? val1Int : val1Double) > (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case comparaison::_superieur_:
			result = (val1Int ? val1Int : val1Double) < (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case comparaison::_inf_egal_:
			result = (val1Int ? val1Int : val1Double) >= (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case comparaison::_sup_egal_:
			result = (val1Int ? val1Int : val1Double) <= (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		}

		executionPile.push({ valType::_bool_,(int)intList.size() });
		boolList.push_back(result);
	}
	else error(errorCode::conversionType);//mieux adapté quand utilisera cast
}

/********************************************************/
/*		SOUS PARTIE 3 : GESTION DES INSTRUCTIONS		*/
/********************************************************/

valAccess addVal(valInstruct instructContent) {
	int tabPos = 0;
	switch (instructContent.type) {
	case valType::_int_:
		tabPos = intList.size();
		intList.push_back(instructContent.intVal);
		break;
	case valType::_double_:
		tabPos = doubleList.size();
		doubleList.push_back(instructContent.doubleVal);
		break;
	case valType::_string_:
		tabPos = stringList.size();
		stringList.push_back(instructContent.stringVal);
		break;
	}
	return { instructContent.type,tabPos };
}

valAccess addVar(valInstruct instructContent) {
	string name = stringList[castVal(depiler(), valType::_string_).tabPos];

	variables.insert({ name,addVal(instructContent) });
	return { instructContent.type,variables[name].tabPos };
}


void addInstruct(command command) {
	instructionList.push_back({ command, { valType::_int_,-1,-1,"" } });
};
void addInstruct(command command, int intValue) {
	instructionList.push_back({ command, { valType::_int_,intValue,-1,"" } });
};
void addInstruct(command command, double doubleValue) {
	instructionList.push_back({ command, { valType::_double_,-1,doubleValue,"" } });
};
void addInstruct(command command, string stringValue) {
	instructionList.push_back({ command, { valType::_string_,-1,-1,stringValue } });
};


/********************************************************/
/*		SOUS PARTIE 4 : GESTION DES TABLEAUX			*/
/********************************************************/

/*
remettre au gout du jour
void executeTabAction(instruction& instructContent, tabAction action) {
	string name = stringList[instructContent.second.tabPos];
	delVal(instructContent.second);//string recupere : peut supprimer du tableau

	if ((action == tabAction::_create_ && tableaux.find(name) == tableaux.end()) ||//tab est bien nouveau
		(action != tabAction::_create_ && tableaux.find(name) != tableaux.end())) {//tab existe bien
		int tabPos;
		valAccess value;
		tabAccess declaration;
		switch (action) {
		case tabAction::_empile_size_:
			switch(tableaux[name].type) {
			case valType::_int_:
				executionPile.push({ valType::_int_,(int)intList.size() });
				break;
			case valType::_double_:
				executionPile.push({ valType::_int_,(int)doubleList.size() });
				break;
			case valType::_string_:
				executionPile.push({ valType::_int_,(int)stringList.size() });
				break;
			}
			intList.push_back(tableaux[name].valuesPos.size());//name
			break;

		case tabAction::_empile_case_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				switch(tableaux[name].type) {
				case valType::_int_:
					executionPile.push({ valType::_int_,(int)intList.size() });
					intList.push_back(intArray[tabPos]);
					break;
				case valType::_double_:
					executionPile.push({ valType::_double_,(int)doubleList.size() });
					doubleList.push_back(doubleArray[tabPos]);
					break;
				case valType::_string_:
					executionPile.push({ valType::_string_,(int)stringList.size() });
					stringList.push_back(stringArray[tabPos]);
					break;
				}
			}
			break;

		case tabAction::_create_:
			value = depiler();//supprime pas : besoin de transmettre valeur associee

			if (instructContent.second.type == value.type) {//verif types
				declaration = {(unsigned)memoryLayer.size(), value.type};//ordre de declaration

				switch(value.type) {
				case valType::_int_:
					declaration.valuesPos.push_back(intArray.size());
					intArray.push_back(intList[value.tabPos]);
					break;
				case valType::_double_:
					declaration.valuesPos.push_back(doubleArray.size());
					doubleArray.push_back(doubleList[value.tabPos]);
					break;
				case valType::_string_:
					declaration.valuesPos.push_back(stringArray.size());
					stringArray.push_back(stringList[value.tabPos]);
					break;
				}

				tableaux.insert({name,declaration});
			}
			//else ? cast here

			delVal(value);//stocke en typeArray
			break;

		case tabAction::_add_:
			value = depiler();//supprime pas : besoin de transmettre valeur associee
			if (tableaux[name].type == value.type) {
				switch(value.type) {
				case valType::_int_:
					tableaux[name].valuesPos.push_back(intArray.size());
					intArray.push_back(intList[value.tabPos]);
					break;
				case valType::_double_:
					tableaux[name].valuesPos.push_back(doubleArray.size());
					doubleArray.push_back(doubleList[value.tabPos]);
					break;
				case valType::_string_:
					tableaux[name].valuesPos.push_back(doubleArray.size());
					stringArray.push_back(stringList[value.tabPos]);
					break;
				}
			}
			//else ? cast here

			delVal(value);//stocke en typeArray
			break;

		case tabAction::_update_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				valAccess value = depiler();//supprime pas : besoin de transmettre valeur associee
				if (tableaux[name].type == value.type) {
					switch(value.type) {
					case valType::_int_:
						intArray[tabPos] = intList[value.tabPos];
						break;
					case valType::_double_:
						doubleArray[tabPos] = doubleList[value.tabPos];
						break;
					case valType::_string_:
						stringArray[tabPos] = stringList[value.tabPos];
						break;
					}
				}
				//else ? cast here

				delVal(value);//stocke en typeArray
			}
			break;

		case tabAction::_remove_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				delTabVal(name,tabPos);
			}
			break;
		}
	}
	//else : pb
}
*/

/********************************************************/
/*		SOUS PARTIE 5 : EXECUTION SELON LES COMMANDES	*/
/********************************************************/
void replaceString(string& subject, const string& search, const string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}