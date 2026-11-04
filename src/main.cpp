#include "mlms/Console.hpp"
#include <fstream>
#include <iterator>
int main(){try{DB db("mlms.db");DataAccess(db).initializeSchema("sql/schema.sql");LMS app(db);app.bootstrap();while(true){try{string name=ask("\nUsername (or exit): ");if(name=="exit")break;string pass=ask("Password: ");auto user=app.login(name,pass);runMenu(app,*user);}catch(const exception& e){cerr<<"Error: "<<e.what()<<"\n";if(!cin.good())break;}}}catch(const exception& e){cerr<<"Fatal: "<<e.what()<<"\n";return 1;}}
