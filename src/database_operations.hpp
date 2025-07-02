#ifndef DATABASE_OPERATIONS_H
#define DATABASE_OPERATIONS_H

#include <string>
#include "sqlite3.h"
#include <iomanip>
#include <fstream>

using namespace std;

//table creation
int createDB(const char* s);
int createInputTable(const char* s);
int createModelTable(const char* s);
int createOutputTable(const char* s);
int createSimulationTable(const char* s);

//data insertion
int insertInputData(const char* s, int inputId, const string& jsonFilePath);

int insertModelData(const char* s, int modelId, const std::string& modelDescription, 
                   int modelVersion);
int insertOutputData(const char* s, int outputId, int modelId, int inputId, 
                    const std::string& results);
int insertSimulationData(const char* s, int simulationId, int outputId, 
                        int timeDays, const std::string& simulationStatus);
int insertGeneralData(const char* s);

int printTableData(void* data, int argCount, char** argVector, char** columnName);
int checkTableData(const char* s, const string& tableName);

// Functions created after merging SQLite into the cpp model
void initializeDatabase(sqlite3* db);
std::string hashParameters(const nlohmann::json& parameters);
bool simulationExists(sqlite3* db, const::string& parameters_hash, std::string& results_json_out);

#endif // DATABASE_OPERATIONS_H