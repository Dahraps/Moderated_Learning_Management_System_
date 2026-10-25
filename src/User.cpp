#include "mlms/User.hpp"
std::string Student::menu()const{return "1 My assignments  2 Submit/resubmit  3 Released results  4 Change password  0 Logout";}
std::string Lecturer::menu()const{return "1 Create assignment  2 Grant extension  3 Mark submission  4 View assignments/submissions  5 Change password  0 Logout";}
std::string HoD::menu()const{return "1 Pending marks  2 Moderate  3 Release  4 Change password  5 Adjust mark  6 Bulk release  0 Logout";}
std::string Administrator::menu()const{return "1 Department  2 User  3 Course  4 Assign lecturer  5 Enrol student  6 List records  7 Deactivate user  8 Change password  9 Update user  0 Logout";}
std::unique_ptr<User> UserFactory::create(int id,const std::string& name,const std::string& role){
 if(role=="Student")return std::make_unique<Student>(id,name);
 if(role=="Lecturer")return std::make_unique<Lecturer>(id,name);
 if(role=="HoD")return std::make_unique<HoD>(id,name);
 if(role=="Administrator")return std::make_unique<Administrator>(id,name);
 throw std::runtime_error("Unknown user role");
}
