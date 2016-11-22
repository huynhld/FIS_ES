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
		map<std::string, vector<Minutiae> > get_all_database();
		string minutiae_database_name;
		std::vector<Minutiae> get_all();
		void insert_minutiae(std::vector<Minutiae> v, std::string userid, int attemped);
		int insert_fingerprint(std::string userid, std::string username);
		std::string find_username(std::string userid);
	private:
		sqlite3 *db;
		int is_database_error = SQLITE_ERROR;
		static int callback_get_all_data(void *NotUsed, int argc, char **argv, char **azColName);
		std::vector<Minutiae> minutiae;
};

#endif