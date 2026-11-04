#include "mlms/LMS.hpp"
#include <cassert>
#include <iostream>
int main(){
 StandardLatePenalty policy;
 assert(policy.penalty(0,100)==0);
 assert(policy.penalty(1,100)==10);
 assert(std::abs(policy.penalty(3,100)-30)<1e-8);
 assert(policy.penalty(4,100)==100);
 NoPenaltyStrategy no;assert(no.penalty(5,100)==0);
 assert(UserFactory::create(1,"student","Student")->role()=="Student");
 assert(UserFactory::create(2,"hod","HoD")->role()=="HoD");
 auto salt=newSalt();auto h=passwordHash("longpassword123",salt);
 assert(h==passwordHash("longpassword123",salt));
 assert(h!=passwordHash("wrongpassword",salt));
 assert(parseTime("2026-11-15T23:59:00")>parseTime("2026-11-14T23:59:00"));
 try{parseTime("2026-02-30T10:00:00");assert(false);}catch(const std::runtime_error&){}
 std::cout<<"All basic unit tests passed\n";
}
