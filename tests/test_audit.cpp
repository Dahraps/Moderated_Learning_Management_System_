#include "mlms/LMS.hpp"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iterator>
#include <cstdio>
struct Input {std::istringstream in;std::streambuf* prior;explicit Input(std::string s):in(s),prior(std::cin.rdbuf(in.rdbuf())){}~Input(){std::cin.rdbuf(prior);std::cin.clear();}};
template<class F> bool denied(F fn){try{fn();return false;}catch(const std::runtime_error&){return true;}}
int main(){
 const std::string path="audit_test.sqlite";std::remove(path.c_str());
 {
 DB db(path);std::ifstream schema("sql/schema.sql");assert(schema.good());db.exec(std::string(std::istreambuf_iterator<char>(schema),{}));
 db.exec("INSERT INTO Department VALUES(1,'Science'),(2,'Arts');");
 db.exec("INSERT INTO User(user_id,username,password_hash,salt,role,department_id) VALUES(1,'admin','x','y','Administrator',NULL),(2,'lecturer','x','y','Lecturer',1),(3,'hod','x','y','HoD',1),(4,'student','x','y','Student',1),(5,'otherhod','x','y','HoD',2);");
 db.exec("INSERT INTO Course VALUES(1,'CS101','CS',1);");db.exec("INSERT INTO CourseLecturer VALUES(1,2);");db.exec("INSERT INTO Enrolment VALUES(4,1);");
 db.exec("INSERT INTO Assignment VALUES(1,1,'Exam','2099-11-01T00:00:00',100);");
 db.exec("INSERT INTO Submission(submission_id,assignment_id,student_id,file_path,submitted_at,status) VALUES(1,1,4,'exam.txt','2099-10-30T00:00:00','Submitted');");
 LMS app(db);
 {Input i("1\n80\nGood work\nInitial marking\n");app.mark(2);}
 {Statement q(db,"SELECT old_mark,new_mark,changed_by,reason,changed_at FROM MarkAudit WHERE submission_id=1");assert(q.next() && q.null(0) && q.number(1)==80 && q.integer(2)==2 && q.str(3)=="Initial marking" && !q.str(4).empty());}
 {Input i("1\n75\nFix calculation\n");app.adjustMark(3);}
 {Statement q(db,"SELECT old_mark,new_mark,changed_by,reason FROM MarkAudit WHERE submission_id=1 ORDER BY audit_id DESC LIMIT 1");assert(q.next() && q.number(0)==80 && q.number(1)==75 && q.integer(2)==3 && q.str(3)=="Fix calculation");}
 assert(denied([&]{Input i("1\n70\n\n");app.adjustMark(3);}));
 assert(denied([&]{Input i("1\n");app.viewMarkAudit(4);}));
 assert(denied([&]{Input i("1\n");app.viewMarkAudit(5);}));
 {Input i("1\n");app.viewMarkAudit(1);}
 {Input i("1\n");app.viewMarkAudit(2);}
 }
 {DB db(path);Statement q(db,"SELECT COUNT(*),MIN(changed_at),MAX(changed_at) FROM MarkAudit");assert(q.next() && q.integer(0)==2 && !q.str(1).empty() && !q.str(2).empty());}
 std::remove(path.c_str());
}
