#pragma once
#include <sqlite3.h>
#include <string>
#include <stdexcept>
using std::string; using std::runtime_error;
class DB {
 sqlite3* db=nullptr;
public:
 explicit DB(const string& path){if(sqlite3_open(path.c_str(),&db)!=SQLITE_OK){string e=db?sqlite3_errmsg(db):"Cannot open database";if(db)sqlite3_close(db);db=nullptr;throw runtime_error(e);}exec("PRAGMA foreign_keys=ON;");}
 ~DB(){if(db)sqlite3_close(db);}
 sqlite3* handle(){return db;}
 DB(const DB&)=delete; DB& operator=(const DB&)=delete;
 void exec(const string& sql){char* err=nullptr;if(sqlite3_exec(db,sql.c_str(),nullptr,nullptr,&err)!=SQLITE_OK){string e=err?err:"SQL error";sqlite3_free(err);throw runtime_error(e);}}
 long long id()const{return sqlite3_last_insert_rowid(db);}
};
class Statement {
 sqlite3_stmt* s=nullptr;
public:
 Statement(DB& d,const string& q){if(sqlite3_prepare_v2(d.handle(),q.c_str(),-1,&s,nullptr)!=SQLITE_OK)throw runtime_error(sqlite3_errmsg(d.handle()));}
 ~Statement(){sqlite3_finalize(s);}
 Statement(const Statement&)=delete; Statement& operator=(const Statement&)=delete;
 void bindNull(int n){if(sqlite3_bind_null(s,n)!=SQLITE_OK)throw runtime_error("bind failed");}
 void bind(int n,int v){if(sqlite3_bind_int(s,n,v)!=SQLITE_OK)throw runtime_error("bind failed");}
 void bind(int n,double v){if(sqlite3_bind_double(s,n,v)!=SQLITE_OK)throw runtime_error("bind failed");}
 void bind(int n,const string& v){if(sqlite3_bind_text(s,n,v.c_str(),-1,SQLITE_TRANSIENT)!=SQLITE_OK)throw runtime_error("bind failed");}
 bool next(){int r=sqlite3_step(s);if(r==SQLITE_ROW)return true;if(r==SQLITE_DONE)return false;throw runtime_error(sqlite3_errmsg(sqlite3_db_handle(s)));}
 void run(){if(next())throw runtime_error("Unexpected query result");}
 int integer(int c)const{return sqlite3_column_int(s,c);}
 double number(int c)const{return sqlite3_column_double(s,c);}
 string str(int c)const{auto p=sqlite3_column_text(s,c);return p?reinterpret_cast<const char*>(p):"";}
 bool null(int c)const{return sqlite3_column_type(s,c)==SQLITE_NULL;}
};
