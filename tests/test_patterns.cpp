#include "mlms/LMS.hpp"
#include <cassert>
#include <type_traits>
#include <cmath>
struct FakeRepository:ISubmissionRepository {
 Submission get(int id)override{return {id,1,2,"Submitted","2026-10-01T00:00:00",0,0,100};}
};
int main(){
 static_assert(std::is_abstract<User>::value);
 static_assert(std::is_abstract<ILatePenaltyStrategy>::value);
 static_assert(std::is_abstract<ISubmissionRepository>::value);
 for(auto role:{"Student","Lecturer","HoD","Administrator"}){
  std::unique_ptr<User> user=UserFactory::create(42,"example",role);
  assert(user->role()==role && !user->menu().empty());
 }
 bool rejected=false;try{UserFactory::create(1,"x","Invalid");}catch(const std::runtime_error&){rejected=true;}assert(rejected);
 StandardLatePenalty standard;NoPenaltyStrategy none;
 const ILatePenaltyStrategy& policy=standard;
 assert(std::abs(policy.penalty(2,100)-20)<1e-9);
 assert(none.penalty(4,100)==0);
 FakeRepository fake;ISubmissionRepository& repository=fake;
 assert(repository.get(7).id==7);
 DB db(":memory:");LMS service(db,repository,policy);
}
