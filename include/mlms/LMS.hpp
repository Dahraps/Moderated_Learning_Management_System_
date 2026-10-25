#pragma once
#include "mlms/User.hpp"
#include "mlms/Database.hpp"
#include "mlms/Utility.hpp"
#include "mlms/Domain.hpp"
#include <fstream>
#include <memory>
#include <algorithm>
#include <cmath>
using namespace std;
class LMS {
 DB& d;std::unique_ptr<ISubmissionRepository> ownedRepository;ISubmissionRepository& submissions;const ILatePenaltyStrategy& policy;
 void roleRequired(int uid,const string& role);
 bool exists(const string& sql,int a,int b=-1);
 void audit(int sid,int uid,double oldMark,double newMark,const string& why,bool hasOld);
 void lecturerOwns(int uid,int assignment);
 void hodOwns(int uid,int assignment);
 pair<string,double> deadlineAndMax(int aid,int sid);
 int lateDays(const string& submitted,const string& deadline);
public:
 explicit LMS(DB& db);
 LMS(DB& db,ISubmissionRepository& repository,const ILatePenaltyStrategy& penalty);
 unique_ptr<User> login(const string& name,const string& pw);
 void bootstrap();
 void createDepartment(int uid);
 void createUser(int uid);
 void createCourse(int uid);
 void assignLecturer(int uid);
 void enrol(int uid);
 void listAdministration(int uid);
 void deactivateUser(int uid);
 void updateUser(int uid);
 void adjustMark(int uid);
 void bulkRelease(int uid);
 void changePassword(int uid);
 void listLecturer(int uid);
 void createAssignment(int uid);
 void extension(int uid);
 void listStudent(int sid);
 void submit(int sid);
 void mark(int uid);
 void pending(int uid);
 void moderate(int uid);
 void release(int uid);
 void results(int sid);

};
