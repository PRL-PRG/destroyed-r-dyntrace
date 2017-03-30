//
// Created by ksiek on 29.03.17.
//

#include "sql_generator.h"

#include <initializer_list>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>

using namespace std;
using namespace sql_generator;

inline sql_stmt_t make_multirow_insert_statement(string table, vector<string> & values);

sql_stmt_t make_insert_function_statement(sql_val_t id, sql_val_t location, sql_val_t definition) {
    stringstream statement;

    statement << "insert into functions values ("
              << id << ","
              << location << ","
              << definition
              << ");\n";

    return statement.str();
}

sql_stmt_t make_insert_function_call_statement(sql_val_t id, sql_val_t ptr, sql_val_t name, sql_val_t type, sql_val_t location,  sql_val_t function_id) {
    stringstream statement;

    statement << "insert into calls values ("
              << id << ","
              << ptr << ","
              << name << ","
              << location << ","
              << type << ","
              << function_id
              << ");\n";

    return statement.str();
}

sql_stmt_t make_insert_promise_statement(sql_val_t id) {
    stringstream statement;

    statement << "insert into promises values ("
              << id
              << ");\n";

    return statement.str();
}

sql_stmt_t make_insert_promise_evaluation_statement(sql_val_t clock, sql_val_t event_type, sql_val_t promise_id, sql_val_t call_id) {
    stringstream statement;

    "insert into promise_evaluations values ("
            << clock << ","
            << event_type << ","
            << promise_id << ","
            << call_id
            << ");\n";

    return statement.str();
}

sql_stmt_t insert_promise_associations_statement(vector<sql_val_cell_t> & associations) {
    return make_multirow_insert_statement("promise_associations", associations);
}

sql_stmt_t insert_arguments_statement(int number_of_rows, vector<sql_val_cell_t> & arguments) {
    return make_multirow_insert_statement("promise_associations", arguments);
}

sql_stmt_t make_multirow_insert_statement(string table, vector<string> & values) {
    int num_rows = values.size();

    // Weird cases.
    assert(num_rows > 0);

    // Special case for 1 value.
    if (num_rows == 1)
        return "insert into " + table + " values (" + values[0] + ");\n";

    // General case;
    stringstream statement;
    auto iter = values.begin();

    statement << "insert into "
              << table
              << " select "
              << *iter;

    for (iter++; iter != values.end(); iter++)
        statement << "union all select "
                  << *iter;

    statement << ";\n";

    return statement.str();
}

// XXX I'm keeping this around just in case.
//sql_stmt_t multirow_insert_template(string table, int num_columns, int num_rows) {
//    // Weird cases.
//    assert(num_rows > 0 && num_columns > 0);
//
//    // Make a string represeting one row of placeholders/values.
//    string value_cell;
//    value_cell.reserve(1 + (num_columns - 1) * 3);
//    value_cell += "%s";
//    for (int i = 1; i < num_columns; i++)
//        value_cell += ", %s";
//
//    // Special case for 1 value.
//    if (num_rows == 1)
//         return "insert into " + table + " values (" + value_cell + ");\n";
//
//    // General case;
//    stringstream statement;
//    statement << "insert into "
//              << table
//              << " select "
//              << value_cell;
//
//    for (int i = num_rows - 1; i--; num_rows >= 0)
//        statement << "union all select "
//                  << value_cell;
//
//    statement << ";\n";
//
//    return statement.str();
//}

sql_stmt_t make_begin_transaction_statement() {
    return "begin transaction;\n";
}

sql_stmt_t make_abort_transaction_statement() {
    return "abort;\n";
}

sql_stmt_t make_commit_transaction_statement() {
    return "commit;\n";
}

sql_stmt_t make_create_tables_and_views_statement() {
    ifstream schema_file(RDT_SQL_SCHEMA);
    string schema_string;
    schema_file.seekg(0, ios::end);
    schema_string.reserve(schema_file.tellg());
    schema_file.seekg(0, ios::beg);
    schema_string.assign((istreambuf_iterator<char>(schema_file)), istreambuf_iterator<char>());
    return schema_string.c_str();
}

sql_val_cell_t join(std::initializer_list<sql_val_t> values) {
    stringstream cell;
    bool first = true;
    for (sql_val_t value : values) {
        if (first)
            first = false;
        else
            cell << ",";
        cell << value;
    }
    return cell.str();
}

sql_val_t from_int(int i) {
    return to_string(i);
}
sql_val_t from_hex(int h) {
    stringstream output;
    output << hex << h;
    return output.str();
}
sql_val_t from_nullable_string(std::string s) {
    return s == nullptr ? "null" : s;
}
sql_val_t from_nullable_cstring(const char * s) {
    return s == NULL ? "null" : string(s);
}