#include "mlms/LMS.hpp"

void LMS::roleRequired(int uid,const string& role){
 Statement q(d,"SELECT 1 FROM User WHERE user_id=? AND role=? AND active=1");
 q.bind(1,uid);q.bind(2,role);require(q.next(),"Access denied: required role is "+role);
}

bool LMS::exists(const string& sql,int a,int b){Statement q(d,sql);q.bind(1,a);if(b!=-1)q.bind(2,b);return q.next();}

void LMS::audit(int sid,int uid,double oldMark,double newMark,const string& why,bool hasOld){Statement q(d,"INSERT INTO MarkAudit(submission_id,changed_by,old_mark,new_mark,reason,changed_at) VALUES(?,?,?,?,?,?)");q.bind(1,sid);q.bind(2,uid);if(hasOld)q.bind(3,oldMark);else q.bindNull(3);q.bind(4,newMark);q.bind(5,why);q.bind(6,now());q.run();}

void LMS::lecturerOwns(int uid,int assignment){require(exists("SELECT 1 FROM Assignment a JOIN CourseLecturer c ON a.course_id=c.course_id WHERE c.lecturer_id=? AND a.assignment_id=?",uid,assignment),"Not lecturer for this assignment");}

void LMS::hodOwns(int uid,int assignment){require(exists("SELECT 1 FROM User u JOIN Course c ON c.department_id=u.department_id JOIN Assignment a ON a.course_id=c.course_id WHERE u.user_id=? AND u.role='HoD' AND u.active=1 AND a.assignment_id=?",uid,assignment),"Not HoD of this department");}

pair<string,double> LMS::deadlineAndMax(int aid,int sid){Statement q(d,"SELECT COALESCE(e.extended_deadline,a.deadline),a.max_mark FROM Assignment a LEFT JOIN Extension e ON e.assignment_id=a.assignment_id AND e.student_id=? WHERE a.assignment_id=?");q.bind(1,sid);q.bind(2,aid);require(q.next(),"Assignment not found");return {q.str(0),q.number(1)};}

int LMS::lateDays(const string& submitted,const string& deadline){double sec=difftime(parseTime(submitted),parseTime(deadline));return sec<=0?0:(int)ceil(sec/86400.0);}

namespace { const StandardLatePenalty defaultPenalty; }
LMS::LMS(DB& db):d(db),ownedRepository(std::make_unique<SubmissionRepository>(db)),submissions(*ownedRepository),policy(defaultPenalty){}
LMS::LMS(DB& db,ISubmissionRepository& repository,const ILatePenaltyStrategy& penaltyStrategy)
 :d(db),submissions(repository),policy(penaltyStrategy){}

unique_ptr<User> LMS::login(const string& name,const string& pw){Statement q(d,"SELECT user_id,password_hash,salt,role FROM User WHERE username=? AND active=1");q.bind(1,name);require(q.next(),"Invalid login");require(constantEqual(passwordHash(pw,q.str(2)),q.str(1)),"Invalid login");return UserFactory::create(q.integer(0),name,q.str(3));}

void LMS::bootstrap(){Statement c(d,"SELECT COUNT(*) FROM User");c.next();if(c.integer(0)!=0)return;cout<<"First run: create administrator account.\n";string u=ask("Admin username: "),p=ask("Admin password (min 8 characters): ");require(p.size()>=8,"Password too short");string salt=newSalt();Statement q(d,"INSERT INTO User(username,password_hash,salt,role) VALUES(?,?,?,'Administrator')");q.bind(1,u);q.bind(2,passwordHash(p,salt));q.bind(3,salt);q.run();}

void LMS::createDepartment(int uid){roleRequired(uid,"Administrator");string name=ask("Department name: ");Statement q(d,"INSERT INTO Department(name) VALUES(?)");q.bind(1,name);q.run();}

void LMS::createUser(int uid){roleRequired(uid,"Administrator");string name=ask("Username: "),pw=ask("Password (min 8 characters): "),role=ask("Role (Student/Lecturer/HoD/Administrator): ");require(pw.size()>=8,"Password too short");require(role=="Student"||role=="Lecturer"||role=="HoD"||role=="Administrator","Invalid role");int dept=askInt("Department ID (0 for none): ");require(dept>=0,"Invalid department ID");if(role=="HoD")require(dept>0,"HoD requires a department");string salt=newSalt();Statement q(d,"INSERT INTO User(username,password_hash,salt,role,department_id) VALUES(?,?,?,?,NULLIF(?,0))");q.bind(1,name);q.bind(2,passwordHash(pw,salt));q.bind(3,salt);q.bind(4,role);q.bind(5,dept);q.run();}

void LMS::createCourse(int uid){roleRequired(uid,"Administrator");string code=ask("Course code: "),name=ask("Course name: ");int dept=askInt("Department ID: ");Statement q(d,"INSERT INTO Course(course_code,course_name,department_id) VALUES(?,?,?)");q.bind(1,code);q.bind(2,name);q.bind(3,dept);q.run();}

void LMS::assignLecturer(int uid){roleRequired(uid,"Administrator");int cid=askInt("Course ID: "),target=askInt("Lecturer ID: ");require(exists("SELECT 1 FROM User WHERE user_id=? AND role='Lecturer' AND active=1",target),"User is not a lecturer");Statement q(d,"INSERT INTO CourseLecturer(course_id,lecturer_id) VALUES(?,?)");q.bind(1,cid);q.bind(2,target);q.run();}

void LMS::enrol(int uid){roleRequired(uid,"Administrator");int cid=askInt("Course ID: "),target=askInt("Student ID: ");require(exists("SELECT 1 FROM User WHERE user_id=? AND role='Student' AND active=1",target),"User is not a student");Statement q(d,"INSERT INTO Enrolment(student_id,course_id) VALUES(?,?)");q.bind(1,target);q.bind(2,cid);q.run();}

void LMS::createAssignment(int uid){roleRequired(uid,"Lecturer");int cid=askInt("Course ID: ");require(exists("SELECT 1 FROM CourseLecturer WHERE lecturer_id=? AND course_id=?",uid,cid),"You do not teach this course");string title=ask("Title: "),date=ask("Deadline UTC (YYYY-MM-DDTHH:MM:SS): ");parseTime(date);double mx=askDouble("Maximum mark: ");require(mx>0,"Maximum must be positive");Statement q(d,"INSERT INTO Assignment(course_id,title,deadline,max_mark) VALUES(?,?,?,?)");q.bind(1,cid);q.bind(2,title);q.bind(3,date);q.bind(4,mx);q.run();}

void LMS::extension(int uid){roleRequired(uid,"Lecturer");int aid=askInt("Assignment ID: "),sid=askInt("Student ID: ");lecturerOwns(uid,aid);require(exists("SELECT 1 FROM Enrolment e JOIN Assignment a ON a.course_id=e.course_id WHERE e.student_id=? AND a.assignment_id=?",sid,aid),"Student not enrolled");string date=ask("Extended deadline UTC: "),reason=ask("Reason: ");parseTime(date);auto [currentDeadline,ignoredMax]=deadlineAndMax(aid,sid);require(parseTime(date)>parseTime(currentDeadline),"Extension must be later than existing deadline");Statement q(d,"INSERT INTO Extension(assignment_id,student_id,extended_deadline,reason) VALUES(?,?,?,?) ON CONFLICT(assignment_id,student_id) DO UPDATE SET extended_deadline=excluded.extended_deadline,reason=excluded.reason");q.bind(1,aid);q.bind(2,sid);q.bind(3,date);q.bind(4,reason);q.run();}

void LMS::listStudent(int sid){roleRequired(sid,"Student");Statement q(d,"SELECT a.assignment_id,c.course_code,a.title,COALESCE(x.extended_deadline,a.deadline) FROM Assignment a JOIN Course c ON c.course_id=a.course_id JOIN Enrolment e ON e.course_id=c.course_id LEFT JOIN Extension x ON x.assignment_id=a.assignment_id AND x.student_id=e.student_id WHERE e.student_id=? ORDER BY a.assignment_id");q.bind(1,sid);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | due "<<q.str(3)<<" UTC\n";}

void LMS::submit(int sid){roleRequired(sid,"Student");int aid=askInt("Assignment ID: ");require(exists("SELECT 1 FROM Enrolment e JOIN Assignment a ON a.course_id=e.course_id WHERE e.student_id=? AND a.assignment_id=?",sid,aid),"Not enrolled");string file=ask("File path (existing local file): ");require(ifstream(file).good(),"File does not exist");auto [deadline,mx]=deadlineAndMax(aid,sid);string stamp=now();int days=lateDays(stamp,deadline);string status=days?"Late":"Submitted";
  Statement old(d,"SELECT submission_id,status FROM Submission WHERE student_id=? AND assignment_id=?");old.bind(1,sid);old.bind(2,aid);bool previous=old.next();if(previous){require(stamp<=deadline,"Resubmission only allowed before effective deadline");require(old.str(1)=="Submitted"||old.str(1)=="Late","Cannot resubmit after marking");Statement q(d,"UPDATE Submission SET file_path=?,submitted_at=?,status=?,raw_mark=NULL,final_mark=NULL,feedback=NULL WHERE submission_id=?");q.bind(1,file);q.bind(2,stamp);q.bind(3,status);q.bind(4,old.integer(0));q.run();}else{Statement q(d,"INSERT INTO Submission(assignment_id,student_id,file_path,submitted_at,status) VALUES(?,?,?,?,?)");q.bind(1,aid);q.bind(2,sid);q.bind(3,file);q.bind(4,stamp);q.bind(5,status);q.run();}cout<<"Saved: "<<status<<" ("<<days<<" days late)\n";
 }

void LMS::mark(int uid){roleRequired(uid,"Lecturer");int id=askInt("Submission ID: ");auto s=submissions.get(id);lecturerOwns(uid,s.assignment);require(s.status=="Submitted"||s.status=="Late"||s.status=="Marked","Not available for marking");
 if(s.status=="Marked"){
  Statement review(d,"SELECT decision FROM Moderation WHERE submission_id=? ORDER BY moderation_id DESC LIMIT 1");
  review.bind(1,id);require(review.next() && review.str(0)=="Returned","Only returned work may be re-marked");
 }
 double value=askDouble("Raw mark: ");require(value>=0&&value<=s.maxMark,"Mark out of range");string feedback=ask("Written feedback: "),reason=ask("Reason for mark entry/change: ");require(!feedback.empty()&&!reason.empty(),"Feedback and reason required");auto [deadline,mx]=deadlineAndMax(s.assignment,s.student);int days=lateDays(s.submittedAt,deadline);double finalMark=max(0.0,value-policy.penalty(days,mx));d.exec("BEGIN IMMEDIATE");try{Statement q(d,"UPDATE Submission SET raw_mark=?,final_mark=?,feedback=?,status='Marked' WHERE submission_id=?");q.bind(1,value);q.bind(2,finalMark);q.bind(3,feedback);q.bind(4,id);q.run();audit(id,uid,s.finalMark,finalMark,reason,s.status=="Marked");d.exec("COMMIT");}catch(...){d.exec("ROLLBACK");throw;}cout<<"Final mark after penalty: "<<finalMark<<"\n";}

void LMS::pending(int uid){roleRequired(uid,"HoD");Statement q(d,"SELECT s.submission_id,a.title,s.status,s.final_mark,COALESCE(s.feedback,'') FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id JOIN Course c ON c.course_id=a.course_id JOIN User u ON u.department_id=c.department_id WHERE u.user_id=? AND s.status IN ('Marked','Moderated')");q.bind(1,uid);while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | final "<<q.number(3)<<" | feedback: "<<q.str(4)<<"\n";}

void LMS::moderate(int uid){roleRequired(uid,"HoD");int id=askInt("Submission ID: ");auto s=submissions.get(id);hodOwns(uid,s.assignment);require(s.status=="Marked","Only marked work can be reviewed");{
 Statement review(d,"SELECT decision,mark_audit_id_at_review FROM Moderation WHERE submission_id=? ORDER BY moderation_id DESC LIMIT 1");
 review.bind(1,id);
 if(review.next() && review.str(0)=="Returned"){
  Statement lastAudit(d,"SELECT COALESCE(MAX(audit_id),0) FROM MarkAudit WHERE submission_id=? AND changed_by IN (SELECT lecturer_id FROM CourseLecturer WHERE course_id=(SELECT course_id FROM Assignment WHERE assignment_id=?))");
  lastAudit.bind(1,id);lastAudit.bind(2,s.assignment);lastAudit.next();
  require(lastAudit.integer(0)>review.integer(1),"Lecturer must revise returned mark before another review");
 }
}string decision=ask("Decision (Approved/Returned): ");require(decision=="Approved"||decision=="Returned","Invalid decision");string comment=ask("Comment: ");d.exec("BEGIN IMMEDIATE");try{Statement q(d,"INSERT INTO Moderation(submission_id,reviewer_id,decision,comment,reviewed_at,mark_audit_id_at_review) VALUES(?,?,?,?,?,COALESCE((SELECT MAX(audit_id) FROM MarkAudit WHERE submission_id=?),0))");q.bind(1,id);q.bind(2,uid);q.bind(3,decision);q.bind(4,comment);q.bind(5,now());q.bind(6,id);q.run();Statement u(d,"UPDATE Submission SET status=? WHERE submission_id=?");u.bind(1,decision=="Approved"?"Moderated":"Marked");u.bind(2,id);u.run();d.exec("COMMIT");}catch(...){d.exec("ROLLBACK");throw;}}

void LMS::release(int uid){roleRequired(uid,"HoD");int id=askInt("Submission ID: ");auto s=submissions.get(id);hodOwns(uid,s.assignment);require(s.status=="Moderated","HoD approval required before release");Statement q(d,"UPDATE Submission SET status='Released' WHERE submission_id=?");q.bind(1,id);q.run();}

void LMS::results(int sid){roleRequired(sid,"Student");Statement q(d,"SELECT a.title,s.final_mark,s.feedback FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE s.student_id=? AND s.status='Released'");q.bind(1,sid);while(q.next())cout<<q.str(0)<<" | mark "<<q.number(1)<<" | feedback: "<<q.str(2)<<"\n";}

void LMS::listAdministration(int uid){
 roleRequired(uid,"Administrator");
 cout<<"\nDepartments:\n";{Statement q(d,"SELECT department_id,name FROM Department ORDER BY department_id");while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<"\n";}
 cout<<"\nUsers:\n";{Statement q(d,"SELECT user_id,username,role,COALESCE(department_id,0),active FROM User ORDER BY user_id");while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | dept "<<q.integer(3)<<" | active "<<q.integer(4)<<"\n";}
 cout<<"\nCourses:\n";{Statement q(d,"SELECT course_id,course_code,course_name FROM Course ORDER BY course_id");while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<"\n";}
}
void LMS::deactivateUser(int uid){
 roleRequired(uid,"Administrator");int target=askInt("User ID to deactivate: ");require(target!=uid,"Cannot deactivate your own account");
 require(exists("SELECT 1 FROM User WHERE user_id=? AND active=1",target),"Active user not found");
 Statement q(d,"UPDATE User SET active=0 WHERE user_id=?");q.bind(1,target);q.run();
}
void LMS::changePassword(int uid){
 Statement q(d,"SELECT password_hash,salt FROM User WHERE user_id=? AND active=1");q.bind(1,uid);require(q.next(),"Inactive or unknown user");
 string old=ask("Current password: ");require(constantEqual(passwordHash(old,q.str(1)),q.str(0)),"Incorrect password");
 string pw=ask("New password (at least 8 characters): ");require(pw.size()>=8,"Password too short");string salt=newSalt();
 Statement u(d,"UPDATE User SET password_hash=?,salt=? WHERE user_id=?");u.bind(1,passwordHash(pw,salt));u.bind(2,salt);u.bind(3,uid);u.run();
}
void LMS::listLecturer(int uid){
 roleRequired(uid,"Lecturer");
 Statement q(d,"SELECT a.assignment_id,c.course_code,a.title,a.deadline,a.max_mark FROM Assignment a JOIN Course c ON c.course_id=a.course_id JOIN CourseLecturer l ON l.course_id=c.course_id WHERE l.lecturer_id=? ORDER BY a.assignment_id");q.bind(1,uid);
 while(q.next())cout<<q.integer(0)<<" | "<<q.str(1)<<" | "<<q.str(2)<<" | "<<q.str(3)<<" | max "<<q.number(4)<<"\n";
 Statement s(d,"SELECT x.submission_id,x.assignment_id,x.student_id,x.status FROM Submission x JOIN Assignment a ON a.assignment_id=x.assignment_id JOIN CourseLecturer l ON l.course_id=a.course_id WHERE l.lecturer_id=? ORDER BY x.submission_id");s.bind(1,uid);
 while(s.next())cout<<"Submission "<<s.integer(0)<<" assignment "<<s.integer(1)<<" student "<<s.integer(2)<<" status "<<s.str(3)<<"\n";
}


// SRS FR-02: update an existing user without bypassing administrator authorization.
void LMS::updateUser(int uid){
 roleRequired(uid,"Administrator");
 int target=askInt("User ID to update: ");
 require(exists("SELECT 1 FROM User WHERE user_id=?",target),"User not found");
 string name=ask("New username: "),role=ask("Role (Student/Lecturer/HoD/Administrator): ");
 int dept=askInt("Department ID (0 for none): ");
 require(!name.empty(),"Username required");
 require(role=="Student"||role=="Lecturer"||role=="HoD"||role=="Administrator","Invalid role");
 require(dept>=0 && (role!="HoD" || dept>0),"Invalid department");
 if(dept>0)require(exists("SELECT 1 FROM Department WHERE department_id=?",dept),"Department not found");
 Statement q(d,"UPDATE User SET username=?,role=?,department_id=NULLIF(?,0) WHERE user_id=?");
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
 d.exec("BEGIN IMMEDIATE");try{
  Statement q(d,"UPDATE Submission SET raw_mark=?,final_mark=?,status='Marked' WHERE submission_id=?");
  q.bind(1,newRaw);q.bind(2,finalMark);q.bind(3,id);q.run();
  audit(id,uid,s.finalMark,finalMark,reason,true);
  d.exec("COMMIT");
 }catch(...){d.exec("ROLLBACK");throw;}
 cout<<"Adjusted final mark: "<<finalMark<<". Requires moderation approval before release.\n";
}

// UC-03: release every moderated result in one course, but never pending marks.
void LMS::bulkRelease(int uid){
 roleRequired(uid,"HoD");int course=askInt("Course ID to release: ");
 require(exists("SELECT 1 FROM Course c JOIN User u ON u.department_id=c.department_id WHERE u.user_id=? AND c.course_id=? AND u.role='HoD'",uid,course),"Not your department");
 Statement count(d,"SELECT COUNT(*) FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE a.course_id=? AND s.status NOT IN ('Moderated','Released')");
 count.bind(1,course);require(count.next(),"Cannot check submissions");require(count.integer(0)==0,"Unmoderated submissions remain; bulk release blocked");
 Statement q(d,"UPDATE Submission SET status='Released' WHERE status='Moderated' AND assignment_id IN (SELECT assignment_id FROM Assignment WHERE course_id=?)");q.bind(1,course);q.run();
 cout<<"All approved marks for this course have been released.\n";
}
