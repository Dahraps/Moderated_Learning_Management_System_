#include "mlms/Utility.hpp"
#include "mlms/Database.hpp"
#include <cassert>
#include <sstream>
#include <limits>
#include <functional>
static bool fails(const std::function<void()>& f){try{f();return false;}catch(const std::exception&){return true;}}
int main(){
 assert(fails([]{parseTime("2026-02-30T12:00:00");}));
 assert(fails([]{parseTime("not-a-date");}));
 assert(fails([]{std::istringstream in("   \n");auto* old=std::cin.rdbuf(in.rdbuf());try{ask("Test: ");}catch(...){std::cin.rdbuf(old);throw;}std::cin.rdbuf(old);}));
 assert(fails([]{std::istringstream in("xyz\n");auto* old=std::cin.rdbuf(in.rdbuf());try{askInt("Number: ");}catch(...){std::cin.rdbuf(old);throw;}std::cin.rdbuf(old);}));
 DB db(":memory:");db.exec("CREATE TABLE t(v INTEGER PRIMARY KEY)");
 Statement s(db,"INSERT INTO t(v) VALUES(?)");s.bind(1,1);s.run();
 assert(fails([&]{Statement q(db,"INSERT INTO t(v) VALUES(?)");q.bind(1,1);q.run();}));
 assert(fails([&]{Statement q(db,"SELECT ?");q.bind(1,std::numeric_limits<double>::infinity());}));
 assert(fails([&]{Statement q(db,"SELECT FROM");}));
}
