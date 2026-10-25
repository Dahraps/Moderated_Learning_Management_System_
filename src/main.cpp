#include "mlms/Console.hpp"
#include <fstream>
#include <iterator>
int main(){try{DB db("mlms.db");ifstream schema("sql/schema.sql");if(!schema)throw runtime_error("Run from project folder (sql/schema.sql missing)");string sql((istreambuf_iterator<char>(schema)),istreambuf_iterator<char>());db.exec(sql);LMS app(db);app.bootstrap();while(true){try{string name=ask("\nUsername (or exit): ");if(name=="exit")break;string pass=ask("Password: ");auto user=app.login(name,pass);runMenu(app,*user);}catch(const exception& e){cerr<<"Error: "<<e.what()<<"\n";if(cin.eof())break;}}}catch(const exception& e){cerr<<"Fatal: "<<e.what()<<"\n";return 1;}}
