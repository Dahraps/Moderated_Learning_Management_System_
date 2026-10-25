#include "mlms/LMS.hpp"

void LMS::roleRequired(int uid,const string& role){
 DataQuery q(store,SqlId::Q1);
 q.bind(1,uid);q.bind(2,role);require(q.next(),"Access denied: required role is "+role);
}

bool LMS::exists(SqlId sql,int a,int b){DataQuery q(store,sql);q.bind(1,a);if(b!=-1)q.bind(2,b);return q.next();}

void LMS::audit(int sid,int uid,double oldMark,double newMark,const string& why,bool hasOld){require(!why.empty(),"Audit reason is required");DataQuery q(store,SqlId::Q2);q.bind(1,sid);q.bind(2,uid);if(hasOld)q.bind(3,oldMark);else q.bindNull(3);q.bind(4,newMark);q.bind(5,why);q.bind(6,now());q.run();}

void LMS::lecturerOwns(int uid,int assignment){require(exists(SqlId::Q39,uid,assignment),"Not lecturer for this assignment");}

void LMS::hodOwns(int uid,int assignment){require(exists(SqlId::Q40,uid,assignment),"Not HoD of this department");}

pair<string,double> LMS::deadlineAndMax(int aid,int sid){DataQuery q(store,SqlId::Q3);q.bind(1,sid);q.bind(2,aid);require(q.next(),"Assignment not found");return {q.str(0),q.number(1)};}

int LMS::lateDays(const string& submitted,const string& deadline){double sec=difftime(parseTime(submitted),parseTime(deadline));return sec<=0?0:(int)ceil(sec/86400.0);}

namespace { const StandardLatePenalty defaultPenalty; }
LMS::LMS(DB& db):store(db),ownedRepository(std::make_unique<SubmissionRepository>(db)),submissions(*ownedRepository),policy(defaultPenalty){}
LMS::LMS(DB& db,ISubmissionRepository& repository,const ILatePenaltyStrategy& penaltyStrategy)
 :store(db),submissions(repository),policy(penaltyStrategy){}

unique_ptr<User> LMS::login(const string& name,const string& pw){DataQuery q(store,SqlId::Q4);q.bind(1,name);require(q.next(),"Invalid login");require(constantEqual(passwordHash(pw,q.str(2)),q.str(1)),"Invalid login");return UserFactory::create(q.integer(0),name,q.str(3));}

void LMS::bootstrap(){DataQuery c(store,SqlId::Q5);c.next();if(c.integer(0)!=0)return;cout<<"First run: create administrator account.\n";string u=ask("Admin username: "),p=ask("Admin password (min 8 characters): ");require(p.size()>=8,"Password too short");string salt=newSalt();DataQuery q(store,SqlId::Q6);q.bind(1,u);q.bind(2,passwordHash(p,salt));q.bind(3,salt);q.run();}

void LMS::createDepartment(int uid){roleRequired(uid,"Administrator");string name=ask("Department name: ");DataQuery q(store,SqlId::Q7);q.bind(1,name);q.run();}

void LMS::createUser(int uid){roleRequired(uid,"Administrator");string name=ask("Username: "),pw=ask("Password (min 8 characters): "),role=ask("Role (Student/Lecturer/HoD/Administrator): ");require(pw.size()>=8,"Password too short");require(role=="Student"||role=="Lecturer"||role=="HoD"||role=="Administrator","Invalid role");int dept=askInt("Department ID (0 for none): ");require(dept>=0,"Invalid department ID");if(role=="HoD")require(dept>0,"HoD requires a department");string salt=newSalt();DataQuery q(store,SqlId::Q8);q.bind(1,name);q.bind(2,passwordHash(pw,salt));q.bind(3,salt);q.bind(4,role);q.bind(5,dept);q.run();}

void LMS::createCourse(int uid){roleRequired(uid,"Administrator");string code=ask("Course code: "),name=ask("Course name: ");int dept=askInt("Department ID: ");require(dept>0,"Department ID must be positive");DataQuery q(store,SqlId::Q9);q.bind(1,code);q.bind(2,name);q.bind(3,dept);q.run();}

void LMS::assignLecturer(int uid){roleRequired(uid,"Administrator");int cid=askInt("Course ID: "),target=askInt("Lecturer ID: ");require(exists(SqlId::Q41,target),"User is not a lecturer");DataQuery q(store,SqlId::Q10);q.bind(1,cid);q.bind(2,target);q.run();}

void LMS::enrol(int uid){roleRequired(uid,"Administrator");int cid=askInt("Course ID: "),target=askInt("Student ID: ");require(exists(SqlId::Q42,target),"User is not a student");DataQuery q(store,SqlId::Q11);q.bind(1,target);q.bind(2,cid);q.run();}

void LMS::createAssignment(int uid){roleRequired(uid,"Lecturer");int cid=askInt("Course ID: ");require(exists(SqlId::Q43,uid,cid),"You do not teach this course");string title=ask("Title: "),date=ask("Deadline UTC (YYYY-MM-DDTHH:MM:SS): ");parseTime(date);double mx=askDouble("Maximum mark: ");require(mx>0 && mx<=1000000,"Maximum mark must be between 0 and 1000000");DataQuery q(store,SqlId::Q12);q.bind(1,cid);q.bind(2,title);q.bind(3,date);q.bind(4,mx);q.run();}

void LMS::extension(int uid){roleRequired(uid,"Lecturer");int aid=askInt("Assignment ID: "),sid=askInt("Student ID: ");lecturerOwns(uid,aid);require(exists(SqlId::Q44,sid,aid),"Student not enrolled");string date=ask("Extended deadline UTC: "),reason=ask("Reason: ");parseTime(date);auto [currentDeadline,ignoredMax]=deadlineAndMax(aid,sid);require(parseTime(date)>parseTime(currentDeadline),"Extension must be later than existing deadline");DataQuery q(store,SqlId::Q13);q.bind(1,aid);q.bind(2,sid);q.bind(3,date);q.bind(4,reason);q.run();}

void LMS::listStudent(int sid){roleRequired(sid,"Student");DataQuery q(store,SqlId::Q14);q.bind(1,sid);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | due "<<q.str(3)<<" UTC\n";}

void LMS::submit(int sid){roleRequired(sid,"Student");int aid=askInt("Assignment ID: ");require(exists(SqlId::Q44,sid,aid),"Not enrolled");string file=ask("File path (existing local file): ");require(ifstream(file).good(),"File does not exist");auto [deadline,mx]=deadlineAndMax(aid,sid);string stamp=now();int days=lateDays(stamp,deadline);string status=days?"Late":"Submitted";
  DataQuery old(store,SqlId::Q15);old.bind(1,sid);old.bind(2,aid);bool previous=old.next();if(previous){require(stamp<=deadline,"Resubmission only allowed before effective deadline");require(old.str(1)=="Submitted"||old.str(1)=="Late","Cannot resubmit after marking");DataQuery q(store,SqlId::Q16);q.bind(1,file);q.bind(2,stamp);q.bind(3,status);q.bind(4,old.integer(0));q.run();}else{DataQuery q(store,SqlId::Q17);q.bind(1,aid);q.bind(2,sid);q.bind(3,file);q.bind(4,stamp);q.bind(5,status);q.run();}cout<<"Saved: "<<status<<" ("<<days<<" days late)\n";
 }

void LMS::mark(int uid){roleRequired(uid,"Lecturer");int id=askInt("Submission ID: ");auto s=submissions.get(id);lecturerOwns(uid,s.assignment);require(s.status=="Submitted"||s.status=="Late"||s.status=="Marked","Not available for marking");
 if(s.status=="Marked"){
  DataQuery review(store,SqlId::Q18);
  review.bind(1,id);require(review.next() && review.str(0)=="Returned","Only returned work may be re-marked");
 }
 double value=askDouble("Raw mark: ");require(value>=0&&value<=s.maxMark,"Mark out of range");string feedback=ask("Written feedback: "),reason=ask("Reason for mark entry/change: ");require(!feedback.empty()&&!reason.empty(),"Feedback and reason required");auto [deadline,mx]=deadlineAndMax(s.assignment,s.student);int days=lateDays(s.submittedAt,deadline);double finalMark=max(0.0,value-policy.penalty(days,mx));store.begin();try{DataQuery q(store,SqlId::Q19);q.bind(1,value);q.bind(2,finalMark);q.bind(3,feedback);q.bind(4,id);q.run();audit(id,uid,s.finalMark,finalMark,reason,s.status=="Marked");store.commit();}catch(...){try{store.rollback();}catch(const exception& rollbackError){cerr<<"Rollback warning: "<<rollbackError.what()<<"\n";}throw;}cout<<"Final mark after penalty: "<<finalMark<<"\n";}

void LMS::pending(int uid){roleRequired(uid,"HoD");DataQuery q(store,SqlId::Q20);q.bind(1,uid);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | final "<<q.number(3)<<" | feedback: "<<q.str(4)<<"\n";}

void LMS::moderate(int uid){roleRequired(uid,"HoD");int id=askInt("Submission ID: ");auto s=submissions.get(id);hodOwns(uid,s.assignment);require(s.status=="Marked","Only marked work can be reviewed");{
 DataQuery review(store,SqlId::Q21);
 review.bind(1,id);
 if(review.next() && review.str(0)=="Returned"){
  DataQuery lastAudit(store,SqlId::Q22);
  lastAudit.bind(1,id);lastAudit.bind(2,s.assignment);lastAudit.next();
  require(lastAudit.integer(0)>review.integer(1),"Lecturer must revise returned mark before another review");
 }
}string decision=ask("Decision (Approved/Returned): ");require(decision=="Approved"||decision=="Returned","Invalid decision");string comment=ask("Comment: ");store.begin();try{DataQuery q(store,SqlId::Q23);q.bind(1,id);q.bind(2,uid);q.bind(3,decision);q.bind(4,comment);q.bind(5,now());q.bind(6,id);q.run();DataQuery u(store,SqlId::Q24);u.bind(1,decision=="Approved"?"Moderated":"Marked");u.bind(2,id);u.run();store.commit();}catch(...){try{store.rollback();}catch(const exception& rollbackError){cerr<<"Rollback warning: "<<rollbackError.what()<<"\n";}throw;}}

void LMS::release(int uid){roleRequired(uid,"HoD");int id=askInt("Submission ID: ");auto s=submissions.get(id);hodOwns(uid,s.assignment);require(s.status=="Moderated","HoD approval required before release");DataQuery q(store,SqlId::Q25);q.bind(1,id);q.run();}

void LMS::results(int sid){roleRequired(sid,"Student");DataQuery q(store,SqlId::Q26);q.bind(1,sid);while(q.next())cout<<q.str(0)<<" | mark "<<q.number(1)<<" | feedback: "<<q.str(2)<<"\n";}

void LMS::listAdministration(int uid){
 roleRequired(uid,"Administrator");
 cout<<"\nDepartments:\n";{DataQuery q(store,SqlId::Q27);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<"\n";}
 cout<<"\nUsers:\n";{DataQuery q(store,SqlId::Q28);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | dept "<<q.integer(3)<<" | active "<<q.integer(4)<<"\n";}
 cout<<"\nCourses:\n";{DataQuery q(store,SqlId::Q29);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<"\n";}
}
void LMS::deactivateUser(int uid){
 roleRequired(uid,"Administrator");int target=askInt("User ID to deactivate: ");require(target!=uid,"Cannot deactivate your own account");
 require(exists(SqlId::Q45,target),"Active user not found");
 DataQuery q(store,SqlId::Q30);q.bind(1,target);q.run();
}
void LMS::changePassword(int uid){
 DataQuery q(store,SqlId::Q31);q.bind(1,uid);require(q.next(),"Inactive or unknown user");
 string old=ask("Current password: ");require(constantEqual(passwordHash(old,q.str(1)),q.str(0)),"Incorrect password");
 string pw=ask("New password (at least 8 characters): ");require(pw.size()>=8,"Password too short");string salt=newSalt();
 DataQuery u(store,SqlId::Q32);u.bind(1,passwordHash(pw,salt));u.bind(2,salt);u.bind(3,uid);u.run();
}
void LMS::listLecturer(int uid){
 roleRequired(uid,"Lecturer");
 DataQuery q(store,SqlId::Q33);q.bind(1,uid);
 while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | "<<q.str(3)<<" | max "<<q.number(4)<<"\n";
 DataQuery s(store,SqlId::Q34);s.bind(1,uid);
 while(s.next())cout<<"Submission "<<s.integer(0)<<" assignment "<<s.integer(1)<<" student "<<s.integer(2)<<" status "<<s.str(3)<<"\n";
}


// SRS FR-02: update an existing user without bypassing administrator authorization.
void LMS::updateUser(int uid){
 roleRequired(uid,"Administrator");
 int target=askInt("User ID to update: ");
 require(exists(SqlId::Q46,target),"User not found");
 string name=ask("New username: "),role=ask("Role (Student/Lecturer/HoD/Administrator): ");
 int dept=askInt("Department ID (0 for none): ");
 require(!name.empty(),"Username required");
 require(role=="Student"||role=="Lecturer"||role=="HoD"||role=="Administrator","Invalid role");
 require(dept>=0 && (role!="HoD" || dept>0),"Invalid department");
 if(dept>0)require(exists(SqlId::Q47,dept),"Department not found");
 DataQuery q(store,SqlId::Q35);
 q.bind(1,name);q.bind(2,role);q.bind(3,dept);q.bind(4,target);q.run();
}

// UC-03: HoD may change a mark, with mandatory justification and audit history.
// Moderation approval must be repeated after an adjustment.
void LMS::adjustMark(int uid){
 roleRequired(uid,"HoD");
 int id=askInt("Submission ID: ");auto s=submissions.get(id);hodOwns(uid,s.assignment);
 require(s.status=="Marked"||s.status=="Moderated","Cannot change an unmarked or released result");
 double newRaw=askDouble("New raw mark: ");require(newRaw>=0&&newRaw<=s.maxMark,"Mark exceeds assignment maximum");
 string reason=ask("Mandatory reason: ");require(!reason.empty(),"Reason is required");
 auto [deadline,maxMark]=deadlineAndMax(s.assignment,s.student);
 double finalMark=max(0.0,newRaw-policy.penalty(lateDays(s.submittedAt,deadline),maxMark));
 store.begin();try{
  DataQuery q(store,SqlId::Q36);
  q.bind(1,newRaw);q.bind(2,finalMark);q.bind(3,id);q.run();
  audit(id,uid,s.finalMark,finalMark,reason,true);
  store.commit();
 }catch(...){try{store.rollback();}catch(const exception& rollbackError){cerr<<"Rollback warning: "<<rollbackError.what()<<"\n";}throw;}
 cout<<"Adjusted final mark: "<<finalMark<<". Requires moderation approval before release.\n";
}

// UC-03: release every moderated result in one course, but never pending marks.
void LMS::bulkRelease(int uid){
 roleRequired(uid,"HoD");int course=askInt("Course ID to release: ");
 require(exists(SqlId::Q48,uid,course),"Not your department");
 DataQuery count(store,SqlId::Q37);
 count.bind(1,course);require(count.next(),"Cannot check submissions");require(count.integer(0)==0,"Unmoderated submissions remain; bulk release blocked");
 DataQuery q(store,SqlId::Q38);q.bind(1,course);q.run();
 cout<<"All approved marks for this course have been released.\n";
}

// Audit history is read-only. Only authorised school staff can inspect it.
void LMS::viewMarkAudit(int uid){
 int id=askInt("Submission ID for audit history: ");
 auto s=submissions.get(id);
 bool allowed=exists(SqlId::Q49,uid) || exists(SqlId::Q39,uid,s.assignment) || exists(SqlId::Q40,uid,s.assignment);
 require(allowed,"Access denied to mark audit history");
 DataQuery q(store,SqlId::Q50);q.bind(1,id);
 cout<<"Audit ID | Actor | Timestamp (UTC) | Old final mark | New final mark | Reason\n";
 while(q.next()){
  cout<<q.integer(0)<<" | "<<q.str(1)<<" (#"<<q.integer(2)<<") | "<<q.str(3)<<" | ";
  if(q.null(4))cout<<"(not previously marked)";else cout<<q.number(4);
  cout<<" | "<<q.number(5)<<" | "<<q.str(6)<<"\n";
 }
}
