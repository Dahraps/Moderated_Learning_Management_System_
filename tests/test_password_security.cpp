#include "mlms/LMS.hpp"
#include <cassert>
#include <string>
#include <iostream>
int main(){
 const std::string password="correct horse battery staple";
 const auto salt1=newSalt(),salt2=newSalt();
 assert(salt1.size()==32 && salt2.size()==32 && salt1!=salt2);
 const auto hash1=passwordHash(password,salt1),hash2=passwordHash(password,salt2);
 assert(hash1.size()==64 && hash2.size()==64 && hash1!=hash2);
 assert(constantEqual(hash1,passwordHash(password,salt1)));
 assert(!constantEqual(hash1,passwordHash("incorrect",salt1)));
 assert(!constantEqual(hash1,hash2));
 try { passwordHash(password,"bad"); assert(false); } catch(const std::runtime_error&) {}
 std::cout<<"Password security tests passed\n";
}
