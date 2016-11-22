#include "../header/SQL.h"
#include "../header/logs.h"

void SQL::create_table()
{

	logs::getInstance();
        logs::write_logs("create_table","SQL", logs::IN_STATE);
	char *err_msg = 0;
	string sql_finger_print_table = "CREATE TABLE IF NOT EXISTS FingerPrint(userid TEXT PRIMARY KEY,";
	sql_finger_print_table.append(" username TEXT,");
	sql_finger_print_table.append(" description TEXT)");
	string sql_minutiae_table = "CREATE TABLE IF NOT EXISTS Minutiae(id INTEGER PRIMARY KEY AUTOINCREMENT,";
	sql_minutiae_table.append(" userid TEXT,");
	sql_minutiae_table.append(" attemped INT,");
	sql_minutiae_table.append(" munitiae BLOB,");
	sql_minutiae_table.append(" FOREIGN KEY(userid) REFERENCES FingerPrint(userid))");

	if(this->is_database_error) {
		logs::write_logs("create_table","SQL", logs::OUT_STATE, "if(this->is_database_error)");
		return;
	}else {
		int rc = sqlite3_exec(db, sql_finger_print_table.c_str(), 0, 0, &err_msg);
		if (rc != SQLITE_OK ) {
			std::cout << err_msg << std::endl;
	        // fprintf(stderr, "Failed to select data\n");
	        // fprintf(stderr, "SQL error: %s\n", err_msg);
			cout << err_msg << endl;
			sqlite3_free(err_msg);
			sqlite3_close(db);
		}

		rc = sqlite3_exec(db, sql_minutiae_table.c_str(), 0, 0, &err_msg);
		if (rc != SQLITE_OK ) {
	        // fprintf(stderr, "Failed to select data\n");
	        // fprintf(stderr, "SQL error: %s\n", err_msg);
			std::cout << err_msg << std::endl;
			sqlite3_free(err_msg);
			sqlite3_close(db);
		} 
	}
	logs::write_logs("create_table","SQL", logs::OUT_STATE);
}

//select count(type) from sqlite_master where type='table' and name='TABLE_NAME_TO_CHECK';

SQL::SQL()
{
	this->minutiae_database_name = "Minutiae.db";
	this->is_database_error = sqlite3_open(this->minutiae_database_name.c_str(), &db);
}

SQL::~SQL()
{
	sqlite3_close(db);
}

map<std::string, vector<Minutiae> > SQL::get_all_database()
{
	logs::getInstance();
    logs::write_logs("get_all_database","SQL", logs::IN_STATE);
	map<std::string, vector<Minutiae> > minutiae_map;
	string sql = "SELECT * FROM Minutiae";
	sqlite3_stmt *pStmt;
	is_database_error = sqlite3_prepare_v2(db, sql.c_str(), sql.length() + 1, &pStmt, NULL);

	if( is_database_error!=SQLITE_OK )
	{
		logs::write_logs("get_all_database","SQL", logs::OUT_STATE, "if( is_database_error!=SQLITE_OK )");
		sqlite3_close(db);
		return minutiae_map;
	}
	//cout << sqlite3_column_table_name(pStmt,0) << endl;
	Minutiae q;
	//int fingerprint_id = 0;
	string userid = "";
	int attemp = 0;
	string key = "";
	// For each row returned
	while (sqlite3_step(pStmt) == SQLITE_ROW)
	{

		userid = "";
		attemp = 0;
		key = "";
		for(int jj=0; jj < sqlite3_column_count(pStmt); jj++)
		{
			string column_name = sqlite3_column_name(pStmt,jj);
			if(column_name.compare("userid") == 0) {
				sqlite3_column_bytes(pStmt, jj);
				char *ptr = (char*)sqlite3_column_text(pStmt, jj);
				if(ptr) userid = ptr;
			}else if(column_name.compare("munitiae") == 0) {
				int bytes = sqlite3_column_bytes(pStmt, jj);
				q = *(Minutiae*) ( sqlite3_column_blob(pStmt, jj));
			}else if(column_name.compare("attemped") == 0) {
				attemp = sqlite3_column_int(pStmt, jj);
			}
		}
		key = userid + ":" + std::to_string(attemp);
		minutiae_map[key].push_back(q);
	}
	sqlite3_finalize(pStmt);

	sqlite3_exec(db, "END", NULL, NULL, NULL);
	logs::write_logs("get_all_database","SQL", logs::OUT_STATE);
	return minutiae_map;
}


void SQL::insert_minutiae(std::vector<Minutiae> v, std::string userid, int attemped)
{
	for(int i = 0; i < v.size(); i++) {
		string sql = "INSERT INTO Minutiae(userid, munitiae,attemped) VALUES(@userid,@munitiae,@attemped)";
	//SELECT Id, Name FROM Cars WHERE Id = @id
		sqlite3_stmt *pStmt;
		const char  **pzTail;
		is_database_error = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, 0);
		if (is_database_error == SQLITE_OK) {
			int userid_idx = sqlite3_bind_parameter_index(pStmt, "@userid");
			int attemped_idx = sqlite3_bind_parameter_index(pStmt, "@attemped");
			int munitiae_idx = sqlite3_bind_parameter_index(pStmt, "@munitiae");
			sqlite3_bind_blob(pStmt, munitiae_idx, &v[i], sizeof(Minutiae), SQLITE_TRANSIENT);
			sqlite3_bind_text(pStmt, userid_idx, userid.c_str(), -1, 0);
			sqlite3_bind_int(pStmt, attemped_idx, attemped);
			sqlite3_step(pStmt);
			sqlite3_finalize(pStmt);
			sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
		}
	}
}


std::string SQL::find_username(std::string userid)
{
	string sql = "SELECT username FROM FingerPrint WHERE userid = @userid";

	sqlite3_stmt *pStmt;
	is_database_error = sqlite3_prepare_v2(db, sql.c_str(), sql.length() + 1, &pStmt, NULL);

	if( is_database_error!=SQLITE_OK )
	{
		sqlite3_close(db);
		return "";
	}
	int userid_idx = sqlite3_bind_parameter_index(pStmt, "@userid");
	sqlite3_bind_text(pStmt, userid_idx, userid.c_str(), -1, 0);
	string username = "";
	while (sqlite3_step(pStmt) == SQLITE_ROW)
	{
		username = "";
		for(int jj=0; jj < sqlite3_column_count(pStmt); jj++)
		{
			string column_name = sqlite3_column_name(pStmt,jj);
			if(column_name.compare("username") == 0) {
				username = std::string(reinterpret_cast<const char*>(
				      sqlite3_column_text(pStmt, jj)
				  ));
			}
		}
	}
	sqlite3_finalize(pStmt);

	sqlite3_exec(db, "END", NULL, NULL, NULL);
	return username;
}


int SQL::insert_fingerprint(std::string userid, std::string username)
{
	char *err_msg = 0;
	string sql = "INSERT INTO FingerPrint(userid, username, description) VALUES(@userid, @username, 'Test')";
	//SELECT Id, Name FROM Cars WHERE Id = @id
	// is_database_error = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

	// if (is_database_error != SQLITE_OK ) {
	// 	sqlite3_free(err_msg);        
	// 	sqlite3_close(db);
	// 	return -1;
	// }

	sqlite3_stmt *pStmt;
	const char  **pzTail;
	is_database_error = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, 0);
	if (is_database_error == SQLITE_OK) {
		int userid_idx = sqlite3_bind_parameter_index(pStmt, "@userid");
		int username_idx = sqlite3_bind_parameter_index(pStmt, "@username");
		sqlite3_bind_text(pStmt, userid_idx, userid.c_str(), -1, 0);
		sqlite3_bind_text(pStmt, username_idx, username.c_str(), -1, 0);
		sqlite3_step(pStmt);
		sqlite3_finalize(pStmt);
		sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
	}

	return sqlite3_last_insert_rowid(db);
}
