#pragma once
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
class User {
public:
 int id; std::string name;
 User(int i,std::string n):id(i),name(std::move(n)){}
 virtual ~User()=default;
 virtual std::string role() const=0;
};
class Student:public User {using User::User; public:std::string role()const override{return "Student";}};
class Lecturer:public User {using User::User; public:std::string role()const override{return "Lecturer";}};
class HoD:public User {using User::User; public:std::string role()const override{return "HoD";}};
class Administrator:public User {using User::User; public:std::string role()const override{return "Administrator";}};
struct UserFactory {
 static std::unique_ptr<User> create(int id,const std::string& name,const std::string& role){
  if(role=="Student")return std::make_unique<Student>(id,name);
  if(role=="Lecturer")return std::make_unique<Lecturer>(id,name);
  if(role=="HoD")return std::make_unique<HoD>(id,name);
  if(role=="Administrator")return std::make_unique<Administrator>(id,name);
  throw std::runtime_error("Unknown role");
 }
};

