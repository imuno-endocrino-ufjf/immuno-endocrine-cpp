#include "database_operations.h"
#include "sqlite3.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int createDB(const char* s) {
    sqlite3* db;
    int exit = sqlite3_open(s, &db);
    sqlite3_close(db);
    return 0;
}

int createInputTable(const char* s) {
    sqlite3* DB;
    string sql = "CREATE TABLE IF NOT EXISTS INPUT("
        "INPUT_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "MODEL_PARAMETERS JSON NOT NULL CHECK(json_valid(MODEL_PARAMETERS))"
        ");";

    try {
        int exit = sqlite3_open(s, &DB);
        char* messageError;
        exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);

        if(exit != SQLITE_OK) {
            cerr << "Error create table" << endl;
            sqlite3_free(messageError);
        } else {
            cout << "Table created sucessfully" << endl;
        }
        sqlite3_close(DB);
    } catch(const exception & e) {
        cerr << e.what();
    }
    return 0;
}

int createModelTable(const char* s){
    sqlite3* DB;
    string sql = "CREATE TABLE IF NOT EXISTS MODEL("
    "MODEL_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "MODEL_DESCRIPTION TEXT NOT NULL, "
    "MODEL_VERSION INTEGER NOT NULL);";

    try{
        int exit = 0;
        exit = sqlite3_open(s, &DB);
        char* messageError;
        exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);

        if(exit != SQLITE_OK){
            cerr << "Error create table" << endl;
            sqlite3_free(messageError);
        }else{
            cout<<"MODEL table created sucessfully" << endl;
        }
        sqlite3_close(DB);
    }catch(const exception & e){
        cerr<<e.what();
    }
    return 0;
}

int createOutputTable(const char* s){
    sqlite3* DB;
    string sql = "CREATE TABLE IF NOT EXISTS OUTPUT("
    "OUTPUT_ID INTEGER PRIMARY KEY AUTOINCREMENT, "
    "MODEL_ID INTEGER NOT NULL, "
    "INPUT_ID NOT NULL, "
    "RESULTS TEXT NOT NULL, "
    "FOREIGN KEY (MODEL_ID) REFERENCES MODEL(MODEL_ID), "
    "FOREIGN KEY (INPUT_ID) REFERENCES INPUT(INPUT_ID));";

    try{
        int exit = 0;
        exit = sqlite3_open(s, &DB);
        char* messageError;
        exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);

        if(exit != SQLITE_OK){
            cerr << "Error create table" << endl;
            sqlite3_free(messageError);
        }else{
            cout<<"OUTPUT table created sucessfully" << endl;
        }
        sqlite3_close(DB);
    }catch(const exception & e){
        cerr<<e.what();
    }
    return 0;
}

int createSimulationTable(const char* s){
    sqlite3* DB;
    string sql = "CREATE TABLE IF NOT EXISTS SIMULATION("
    "SIMULATION_ID INTEGER PRIMARY KEY, "
    "OUTPUT_ID INTEGER NOT NULL, "
    "TIME_DAYS INTEGER NOT NULL, "
    "SIMULATION_STATUS TEXT NOT NULL, "
    "FOREIGN KEY (OUTPUT_ID) REFERENCES OUTPUT(OUTPUT_ID));";

    try{
        int exit = 0;
        exit = sqlite3_open(s, &DB);
        char* messageError;
        exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);

        if(exit != SQLITE_OK){
            cerr << "Error create table" << endl;
            sqlite3_free(messageError);
        }else{
            cout<<"SIMULATION table created sucessfully" << endl;
        }
        sqlite3_close(DB);
    }catch(const exception & e){
        cerr<<e.what();
    }
    return 0;
}

/*int insertInputData(const char* s, int inputId, const string& modelParam){
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);

    string sql = "INSERT INTO INPUT (INPUT_ID, MODEL_PARAMETERS) VALUES("
        + to_string(inputId) + ", '" + modelParam + "');"; 
    exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);
    if(exit != SQLITE_OK) {
        cerr << "Error inserting into INPUT: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "Record inserted into INPUT sucessfully" << endl;
    }
    sqlite3_close(DB);
    return exit;
}*/

int insertInputData(const char* s, int inputId, const string& jsonFilePath) {
    sqlite3* db;
    char* errMsg = nullptr;
    
    //reads json file
    ifstream jsonFile(jsonFilePath);

    if (!jsonFile.is_open()) {
        cerr << "Error opening JSON file: " << jsonFilePath << endl;
        return SQLITE_ERROR;
    }

    stringstream buffer;
    buffer << jsonFile.rdbuf();
    string jsonContent = buffer.str();
    jsonFile.close();

    //json validation
    int rc = sqlite3_open(s, &db);
    if (rc != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return rc;
    }

    //using prepared statement
    const char* sql = "INSERT INTO INPUT (INPUT_ID, MODEL_PARAMETERS) VALUES(?, json(?));";
    sqlite3_stmt* stmt;
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return rc;
    }

    //links parameters
    sqlite3_bind_int(stmt, 1, inputId);
    sqlite3_bind_text(stmt, 2, jsonContent.c_str(), -1, SQLITE_STATIC);

    //execution
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return rc;
    }

    cout << "Data inserted successfully (ID: " << inputId << ")" << endl;
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return SQLITE_OK;
}

int insertModelData(const char* s, int modelId, const string& modelDescription, int modelVersion){
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);

    string sql = "INSERT INTO MODEL (MODEL_ID, MODEL_DESCRIPTION, MODEL_VERSION) VALUES("
        + to_string(modelId) + ", '" + modelDescription + "', " + to_string(modelVersion) + ");"; 
    exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);
    if(exit != SQLITE_OK){
        cerr<<"Error inserting into MODEL table: "<<messageError<<endl;
        sqlite3_free(messageError);
    }else{
        cout<<"Record inserted into MODEL table sucessfully"<<endl;
    }
    sqlite3_close(DB);
    return exit;
}

int insertOutputData(const char* s, int outputId, int modelId, int inputId, const string& results){
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);

    string sql = "INSERT INTO OUTPUT (OUTPUT_ID, MODEL_ID, INPUT_ID, RESULTS) VALUES("
        + to_string(outputId) + ", " + to_string(modelId) + ", " + to_string(inputId) + ", '" + results + "');"; 
    exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);
    if(exit != SQLITE_OK){
        cerr<<"Error inserting into OUTPUT table: "<<messageError<<endl;
        sqlite3_free(messageError);
    }else{
        cout<<"Record inserted into OUTPUT table sucessfully"<<endl;
    }
    sqlite3_close(DB);
    return exit;
}

int insertSimulationData(const char* s, int simulationId, int outputId, int timeDays, const string& simulationStatus){
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);

    string sql = "INSERT INTO SIMULATION (SIMULATION_ID, OUTPUT_ID, TIME_DAYS, SIMULATION_STATUS) VALUES("
        + to_string(simulationId) + ", " + to_string(outputId) + ", " + to_string(timeDays) + ", '" + simulationStatus + "');"; 
    exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);
    if(exit != SQLITE_OK){
        cerr<<"Error inserting into SIMULATION table: "<<messageError<<endl;
        sqlite3_free(messageError);
    }else{
        cout<<"Record inserted into SIMULATION table sucessfully"<<endl;
    }
    sqlite3_close(DB);
    return exit;
}

int insertGeneralData(const char* s) {
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);

    string sql("INSERT INTO INPUT (INPUT_ID, CORTISOL_EXP_PARAMETERS, SIMULATION_GENDER, INITIAL_CONDITIONS) VALUES(1, 'cortisolExp', 'F', 'initialConditions');"
    "INSERT INTO MODEL (MODEL_ID, MODEL_DESCRIPTION, MODEL_VERSION) VALUES(1, 'modelDescription', 10);"
    "INSERT INTO OUTPUT (OUTPUT_ID, MODEL_ID, INPUT_ID, RESULTS) VALUES(1, 1, 1, 'results');"
    "INSERT INTO SIMULATION (SIMULATION_ID, OUTPUT_ID, TIME_DAYS, SIMULATION_STATUS) VALUES(1, 1, 365, 'simulation run');");

    exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &messageError);
    if(exit != SQLITE_OK) {
        cerr << "error insert" << endl;
        sqlite3_free(messageError);
    } else {
        cout << "Records created sucessfully" << endl;
    }
    return 0;
}

struct PrintContext {
    bool headerPrinted = false;
    string tableName;
};

int printTableData(void* data, int argCount, char** argVector, char** columnName) {
    PrintContext* context = static_cast<PrintContext*>(data);
    
    //prints header only once
    if (!context->headerPrinted) {
        //prints table title
        cout << "\nContents of table '" << context->tableName << "':" << endl;
        
        //prints column names
        for(int i = 0; i < argCount; i++) {
            cout << left << setw(17) << columnName[i] << " | ";
        }
        cout << endl;
        context->headerPrinted = true;
    }
    
    //prints data from each row
    for(int i = 0; i < argCount; i++) {
        cout << left << setw(17) << (argVector[i] ? argVector[i] : "NULL") << " | ";
    }
    cout << endl;
    
    return 0;
}

int checkTableData(const char* s, const string& tableName) {
    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(s, &DB);
    
    if (exit != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(DB) << endl;
        sqlite3_close(DB);
        return exit;
    }

    //creates printContext
    PrintContext context;
    context.tableName = tableName;
    context.headerPrinted = false;

    string sql = "SELECT * FROM " + tableName + ";";
    
    exit = sqlite3_exec(DB, sql.c_str(), printTableData, &context, &messageError);
    
    if(exit != SQLITE_OK) {
        cerr << "Error while reading from table " << tableName << ": " << messageError << endl;
        sqlite3_free(messageError);
    }
    
    sqlite3_close(DB);
    return exit;
}