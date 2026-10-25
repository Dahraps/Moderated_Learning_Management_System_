#pragma once
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
class User {
public:
 const int id;
 const std::string name;
 User(int i,std::string n):id(i),name(std::move(n)){}
 virtual ~User()=default;
 virtual std::string role() const=0;
 virtual std::string menu() const=0;
};
class Student final:public User {using User::User; public:std::string role()const override{return "Student";} std::string menu()const override;};
class Lecturer final:public User {using User::User; public:std::string role()const override{return "Lecturer";} std::string menu()const override;};
class HoD final:public User {using User::User; public:std::string role()const override{return "HoD";} std::string menu()const override;};
class Administrator final:public User {using User::User; public:std::string role()const override{return "Administrator";} std::string menu()const override;};
class UserFactory {
public:
 static std::unique_ptr<User> create(int id,const std::string& name,const std::string& role);
};
