#include "mlms/LMS.hpp"
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iterator>

struct Input {
 std::istringstream source;
 std::streambuf* old;
 explicit Input(const std::string& text):source(text),old(std::cin.rdbuf(source.rdbuf())){}
 ~Input(){std::cin.rdbuf(old);std::cin.clear();}
};
template<typename F> bool denied(F f){try{f();return false;}catch(const std::runtime_error& e){return std::string(e.what()).find("Access denied")!=std::string::npos;}}
int main(){
 DB db(":memory:");std::ifstream in("sql/schema.sql");if(!in)in.open("../sql/schema.sql");assert(in.good());
 db.exec(std::string(std::istreambuf_iterator<char>(in),{}));
 auto add=[&](int id,const char* name,const char* role,int department){
  auto salt=newSalt();Statement q(db,"INSERT INTO User(user_id,username,password_hash,salt,role,department_id) VALUES(?,?,?,?,?,?)");
  q.bind(1,id);q.bind(2,std::string(name));q.bind(3,passwordHash("password123",salt));q.bind(4,salt);q.bind(5,std::string(role));
  if(department)q.bind(6,department);else q.bindNull(6);q.run();
 };
 db.exec("INSERT INTO Department(department_id,name) VALUES(1,'Science'),(2,'Arts')");
 add(1,"admin","Administrator",0);add(2,"teacher","Lecturer",1);add(3,"head","HoD",1);add(4,"student","Student",1);add(5,"otherhead","HoD",2);
 db.exec("INSERT INTO Course(course_id,course_code,course_name,department_id) VALUES(1,'CS1','Intro',1)");
 db.exec("INSERT INTO CourseLecturer(course_id,lecturer_id) VALUES(1,2)");
 db.exec("INSERT INTO Enrolment(student_id,course_id) VALUES(4,1)");
 db.exec("INSERT INTO Assignment(assignment_id,course_id,title,deadline,max_mark) VALUES(1,1,'Task','2026-11-01T12:00:00',100)");
 LMS app(db);
 assert(app.login("admin","password123")->role()=="Administrator");
 assert(app.login("teacher","password123")->role()=="Lecturer");
 assert(app.login("head","password123")->role()=="HoD");
 assert(app.login("student","password123")->role()=="Student");
 try{app.login("student","wrong");assert(false);}catch(const std::runtime_error&){}
 assert(denied([&]{app.createDepartment(4);}));
 assert(denied([&]{app.createAssignment(4);}));
 assert(denied([&]{app.moderate(2);}));
 assert(denied([&]{app.submit(1);}));
 assert(denied([&]{app.results(2);}));
 assert(denied([&]{app.pending(5);})==false); // HoD is permitted, but only sees own department
 {Input i("4\n");app.deactivateUser(1);}
 try{app.login("student","password123");assert(false);}catch(const std::runtime_error&){}
 assert(denied([&]{app.results(4);}));
 std::cout<<"Role and authentication tests passed\n";
}
