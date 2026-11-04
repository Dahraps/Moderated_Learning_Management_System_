#include "mlms/LMS.hpp"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iterator>
struct Input {std::istringstream in;std::streambuf* previous;explicit Input(const std::string& s):in(s),previous(std::cin.rdbuf(in.rdbuf())){}~Input(){std::cin.rdbuf(previous);std::cin.clear();}};
int main(){
 DB db(":memory:");std::ifstream f("sql/schema.sql");assert(f.good());db.exec(std::string(std::istreambuf_iterator<char>(f),{}));
 db.exec("INSERT INTO Department VALUES(1,'Science'),(2,'Arts');");
 db.exec("INSERT INTO User(user_id,username,password_hash,salt,role,department_id) VALUES(1,'admin','x','y','Administrator',NULL),(2,'lecturer','x','y','Lecturer',1),(3,'hod','x','y','HoD',1),(4,'student','x','y','Student',1),(5,'otherhod','x','y','HoD',2);");
 db.exec("INSERT INTO Course VALUES(1,'CS101','CS',1);");
 db.exec("INSERT INTO CourseLecturer VALUES(1,2);");
 db.exec("INSERT INTO Enrolment VALUES(4,1);");
 db.exec("INSERT INTO Assignment VALUES(1,1,'Exam','2026-11-01T00:00:00',100);");
 db.exec("INSERT INTO Submission(submission_id,assignment_id,student_id,file_path,submitted_at,status,raw_mark,final_mark,feedback) VALUES(1,1,4,'example.txt','2026-10-30T00:00:00','Marked',60,60,'Good');");
 LMS app(db);
 {Input input("4\nnewstudent\nStudent\n1\n");app.updateUser(1);}
 {Statement q(db,"SELECT username FROM User WHERE user_id=4");assert(q.next()&&q.str(0)=="newstudent");}
 {Input input("1\n75\nCorrection following review\n");app.adjustMark(3);}
 {Statement q(db,"SELECT raw_mark,final_mark,status FROM Submission WHERE submission_id=1");assert(q.next()&&q.number(0)==75&&q.number(1)==75&&q.str(2)=="Marked");}
 {Statement q(db,"SELECT old_mark,new_mark,reason,changed_by FROM MarkAudit WHERE submission_id=1");assert(q.next()&&q.number(0)==60&&q.number(1)==75&&q.str(2)=="Correction following review"&&q.integer(3)==3);}
 bool blocked=false;try{Input input("1\n");app.bulkRelease(3);}catch(const std::runtime_error&){blocked=true;}assert(blocked);
 db.exec("UPDATE Submission SET status='Moderated' WHERE submission_id=1");
 {Input input("1\n");app.bulkRelease(3);}
 {Statement q(db,"SELECT status FROM Submission WHERE submission_id=1");assert(q.next()&&q.str(0)=="Released");}
 bool otherBlocked=false;try{Input input("1\n");app.bulkRelease(5);}catch(const std::runtime_error&){otherBlocked=true;}assert(otherBlocked);
}
