#include "mlms/LMS.hpp"
#include <cassert>
#include <sstream>
#include <fstream>
#include <iterator>
#include <cstdio>
struct Input {std::istringstream in;std::streambuf* old;explicit Input(const std::string& s):in(s),old(std::cin.rdbuf(in.rdbuf())){}~Input(){std::cin.rdbuf(old);std::cin.clear();}};
static std::string stamp(time_t t){tm v=*gmtime(&t);char b[20];strftime(b,sizeof(b),"%Y-%m-%dT%H:%M:%S",&v);return b;}
template<class F>bool denied(F f){try{f();return false;}catch(const std::runtime_error&){return true;}}
int main(){
 StandardLatePenalty p;assert(std::abs(p.penalty(0,100)-0)<1e-8&&std::abs(p.penalty(1,100)-10)<1e-8&&std::abs(p.penalty(2,100)-20)<1e-8&&std::abs(p.penalty(3,100)-30)<1e-8&&std::abs(p.penalty(4,100)-100)<1e-8);
 DB db(":memory:");std::ifstream f("sql/schema.sql");assert(f.good());db.exec(std::string(std::istreambuf_iterator<char>(f),{}));
 db.exec("INSERT INTO Department VALUES(1,'Science'),(2,'Arts')");
 db.exec("INSERT INTO User(user_id,username,password_hash,salt,role,department_id) VALUES(1,'admin','x','y','Administrator',NULL),(2,'lecturer','x','y','Lecturer',1),(3,'hod','x','y','HoD',1),(4,'student','x','y','Student',1),(5,'otherhod','x','y','HoD',2)");
 db.exec("INSERT INTO Course VALUES(1,'CS101','Computer Science',1)");db.exec("INSERT INTO CourseLecturer VALUES(1,2)");db.exec("INSERT INTO Enrolment VALUES(4,1)");
 auto t=time(nullptr);auto past=stamp(t-2*86400),future=stamp(t+3*86400);
 {Statement q(db,"INSERT INTO Assignment VALUES(1,1,'Assignment',?,100)");q.bind(1,past);q.run();}
 std::ofstream("rules_example.txt")<<"Sample work";LMS app(db);
 {Input i("1\nrules_example.txt\n");app.submit(4);}
 {Statement q(db,"SELECT status FROM Submission WHERE student_id=4");assert(q.next()&&q.str(0)=="Late");}
 assert(denied([&]{Input i("1\nrules_example.txt\n");app.submit(4);}));
 {Input i("1\n4\n"+future+"\nApproved extension\n");app.extension(2);}
 {Input i("1\nrules_example.txt\n");app.submit(4);}
 {Statement q(db,"SELECT status FROM Submission WHERE student_id=4");assert(q.next()&&q.str(0)=="Submitted");}
 assert(denied([&]{Input i("1\n4\n"+future+"\nNo\n");app.extension(4);}));
 {Input i("1\n80\nInitial feedback\nInitial grading\n");app.mark(2);}
 {Input i("1\nReturned\nNeeds correction\n");app.moderate(3);}
 assert(denied([&]{Input i("1\nApproved\nNot corrected\n");app.moderate(3);}));
 {Input i("1\n75\nCorrected feedback\nRevision after review\n");app.mark(2);}
 {Input i("1\nApproved\nVerified\n");app.moderate(3);}
 assert(denied([&]{Input i("1\n");app.release(5);}));
 {Input i("1\n");app.release(3);}
 {Statement q(db,"SELECT status,final_mark FROM Submission WHERE submission_id=1");assert(q.next()&&q.str(0)=="Released"&&q.number(1)==75);}
 {Statement q(db,"SELECT COUNT(*) FROM MarkAudit WHERE submission_id=1");assert(q.next()&&q.integer(0)==2);}
 assert(denied([&]{Input i("1\n85\nChanged\nReason\n");app.mark(2);}));
 assert(denied([&]{Input i("1\nrules_example.txt\n");app.submit(4);}));
 std::remove("rules_example.txt");
}
