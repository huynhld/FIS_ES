#include "../header/SQL.h"

void SQL::create_table()
{
	char *err_msg = 0;
	string sql_finger_print_table = "CREATE TABLE IF NOT EXISTS FingerPrint(id INTEGER PRIMARY KEY AUTOINCREMENT,";
	sql_finger_print_table.append(" description TEXT)");
	string sql_minutiae_table = "CREATE TABLE IF NOT EXISTS Minutiae(id INTEGER PRIMARY KEY AUTOINCREMENT,";
	sql_minutiae_table.append(" fingerprint_id INT,");
	sql_minutiae_table.append(" munitiae BLOB,");
	sql_minutiae_table.append(" FOREIGN KEY(fingerprint_id) REFERENCES FingerPrint(id))");

	if(this->is_database_error) {
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

map<int, vector<Minutiae> > SQL::get_all_database()
{
	map<int, vector<Minutiae> > minutiae_map;
	string sql = "SELECT * FROM Minutiae";
	sqlite3_stmt *pStmt;
	is_database_error = sqlite3_prepare_v2(db, sql.c_str(), sql.length() + 1, &pStmt, NULL);

	if( is_database_error!=SQLITE_OK )
	{
		sqlite3_close(db);
		return minutiae_map;
	}
	//cout << sqlite3_column_table_name(pStmt,0) << endl;
	Minutiae q;
	int fingerprint_id = 0;
	// For each row returned
	while (sqlite3_step(pStmt) == SQLITE_ROW)
	{
		for(int jj=0; jj < sqlite3_column_count(pStmt); jj++)
		{
			string column_name = sqlite3_column_name(pStmt,jj);
			if(column_name.compare("fingerprint_id") == 0) {
				fingerprint_id = sqlite3_column_int(pStmt, jj);
			}else if(column_name.compare("munitiae") == 0) {
				q = *(Minutiae*) ( sqlite3_column_blob(pStmt, jj) );
			}
		}
		q.setFingerPrintId(fingerprint_id);
		minutiae_map[fingerprint_id].push_back(q);
	}
	sqlite3_finalize(pStmt);

	sqlite3_exec(db, "END", NULL, NULL, NULL);
	return minutiae_map;
}


void SQL::insert_minutiae(std::vector<Minutiae> v, int fingerprint_id)
{
	for(int i = 0; i < v.size(); i++) {
		string sql = "INSERT INTO Minutiae(fingerprint_id, munitiae) VALUES(@id,@munitiae)";
	//SELECT Id, Name FROM Cars WHERE Id = @id
		sqlite3_stmt *pStmt;
		const char  **pzTail;
		is_database_error = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, 0);
		if (is_database_error == SQLITE_OK) {
			int fingerprint_id_idx = sqlite3_bind_parameter_index(pStmt, "@id");
			int munitiae_idx = sqlite3_bind_parameter_index(pStmt, "@munitiae");
			sqlite3_bind_blob(pStmt, munitiae_idx, &v[i], sizeof(Minutiae), SQLITE_TRANSIENT);
			sqlite3_bind_int(pStmt, fingerprint_id_idx, fingerprint_id);
			sqlite3_step(pStmt);
			sqlite3_finalize(pStmt);
			sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
		}
	}
}



int SQL::insert_fingerprint()
{
	char *err_msg = 0;
	string sql = "INSERT INTO FingerPrint(description) VALUES('Test')";
	//SELECT Id, Name FROM Cars WHERE Id = @id
	is_database_error = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

	if (is_database_error != SQLITE_OK ) {
		sqlite3_free(err_msg);        
		sqlite3_close(db);
		return -1;
	}
	return sqlite3_last_insert_rowid(db);
}