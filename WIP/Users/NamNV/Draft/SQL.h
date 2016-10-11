#ifndef SQL_H
#define SQL_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include "Minutiae.h"
using namespace std;

class SQL {
	public:
		SQL();
		~SQL();
		void create_table();
		map<int, vector<Minutiae> > get_all_database();
		const string minutiae_database_name = "Minutiae.db";
		std::vector<Minutiae> get_all();
		void insert_minutiae(std::vector<Minutiae> v, int fingerprint_id);
		int insert_fingerprint();
	private:
		sqlite3 *db;
		int is_database_error = SQLITE_ERROR;
		static int callback_get_all_data(void *NotUsed, int argc, char **argv, char **azColName);
		std::vector<Minutiae> minutiae;
};

#endif