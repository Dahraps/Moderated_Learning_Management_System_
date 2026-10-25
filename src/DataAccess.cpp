#include "mlms/DataAccess.hpp"
#include <stdexcept>
#include <fstream>
#include <iterator>
namespace {
const char* sqlFor(SqlId id){
 switch(id){
 case SqlId::Q1: return "SELECT 1 FROM User WHERE user_id=? AND role=? AND active=1";
 case SqlId::Q2: return "INSERT INTO MarkAudit(submission_id,changed_by,old_mark,new_mark,reason,changed_at) VALUES(?,?,?,?,?,?)";
 case SqlId::Q3: return "SELECT COALESCE(e.extended_deadline,a.deadline),a.max_mark FROM Assignment a LEFT JOIN Extension e ON e.assignment_id=a.assignment_id AND e.student_id=? WHERE a.assignment_id=?";
 case SqlId::Q4: return "SELECT user_id,password_hash,salt,role FROM User WHERE username=? AND active=1";
 case SqlId::Q5: return "SELECT COUNT(*) FROM User";
 case SqlId::Q6: return "INSERT INTO User(username,password_hash,salt,role) VALUES(?,?,?,'Administrator')";
 case SqlId::Q7: return "INSERT INTO Department(name) VALUES(?)";
 case SqlId::Q8: return "INSERT INTO User(username,password_hash,salt,role,department_id) VALUES(?,?,?,?,NULLIF(?,0))";
 case SqlId::Q9: return "INSERT INTO Course(course_code,course_name,department_id) VALUES(?,?,?)";
 case SqlId::Q10: return "INSERT INTO CourseLecturer(course_id,lecturer_id) VALUES(?,?)";
 case SqlId::Q11: return "INSERT INTO Enrolment(student_id,course_id) VALUES(?,?)";
 case SqlId::Q12: return "INSERT INTO Assignment(course_id,title,deadline,max_mark) VALUES(?,?,?,?)";
 case SqlId::Q13: return "INSERT INTO Extension(assignment_id,student_id,extended_deadline,reason) VALUES(?,?,?,?) ON CONFLICT(assignment_id,student_id) DO UPDATE SET extended_deadline=excluded.extended_deadline,reason=excluded.reason";
 case SqlId::Q14: return "SELECT a.assignment_id,c.course_code,a.title,COALESCE(x.extended_deadline,a.deadline) FROM Assignment a JOIN Course c ON c.course_id=a.course_id JOIN Enrolment e ON e.course_id=c.course_id LEFT JOIN Extension x ON x.assignment_id=a.assignment_id AND x.student_id=e.student_id WHERE e.student_id=? ORDER BY a.assignment_id";
 case SqlId::Q15: return "SELECT submission_id,status FROM Submission WHERE student_id=? AND assignment_id=?";
 case SqlId::Q16: return "UPDATE Submission SET file_path=?,submitted_at=?,status=?,raw_mark=NULL,final_mark=NULL,feedback=NULL WHERE submission_id=?";
 case SqlId::Q17: return "INSERT INTO Submission(assignment_id,student_id,file_path,submitted_at,status) VALUES(?,?,?,?,?)";
 case SqlId::Q18: return "SELECT decision FROM Moderation WHERE submission_id=? ORDER BY moderation_id DESC LIMIT 1";
 case SqlId::Q19: return "UPDATE Submission SET raw_mark=?,final_mark=?,feedback=?,status='Marked' WHERE submission_id=?";
 case SqlId::Q20: return "SELECT s.submission_id,a.title,s.status,s.final_mark,COALESCE(s.feedback,'') FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id JOIN Course c ON c.course_id=a.course_id JOIN User u ON u.department_id=c.department_id WHERE u.user_id=? AND s.status IN ('Marked','Moderated')";
 case SqlId::Q21: return "SELECT decision,mark_audit_id_at_review FROM Moderation WHERE submission_id=? ORDER BY moderation_id DESC LIMIT 1";
 case SqlId::Q22: return "SELECT COALESCE(MAX(audit_id),0) FROM MarkAudit WHERE submission_id=? AND changed_by IN (SELECT lecturer_id FROM CourseLecturer WHERE course_id=(SELECT course_id FROM Assignment WHERE assignment_id=?))";
 case SqlId::Q23: return "INSERT INTO Moderation(submission_id,reviewer_id,decision,comment,reviewed_at,mark_audit_id_at_review) VALUES(?,?,?,?,?,COALESCE((SELECT MAX(audit_id) FROM MarkAudit WHERE submission_id=?),0))";
 case SqlId::Q24: return "UPDATE Submission SET status=? WHERE submission_id=?";
 case SqlId::Q25: return "UPDATE Submission SET status='Released' WHERE submission_id=?";
 case SqlId::Q26: return "SELECT a.title,s.final_mark,s.feedback FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE s.student_id=? AND s.status='Released'";
 case SqlId::Q27: return "SELECT department_id,name FROM Department ORDER BY department_id";
 case SqlId::Q28: return "SELECT user_id,username,role,COALESCE(department_id,0),active FROM User ORDER BY user_id";
 case SqlId::Q29: return "SELECT course_id,course_code,course_name FROM Course ORDER BY course_id";
 case SqlId::Q30: return "UPDATE User SET active=0 WHERE user_id=?";
 case SqlId::Q31: return "SELECT password_hash,salt FROM User WHERE user_id=? AND active=1";
 case SqlId::Q32: return "UPDATE User SET password_hash=?,salt=? WHERE user_id=?";
 case SqlId::Q33: return "SELECT a.assignment_id,c.course_code,a.title,a.deadline,a.max_mark FROM Assignment a JOIN Course c ON c.course_id=a.course_id JOIN CourseLecturer l ON l.course_id=c.course_id WHERE l.lecturer_id=? ORDER BY a.assignment_id";
 case SqlId::Q34: return "SELECT x.submission_id,x.assignment_id,x.student_id,x.status FROM Submission x JOIN Assignment a ON a.assignment_id=x.assignment_id JOIN CourseLecturer l ON l.course_id=a.course_id WHERE l.lecturer_id=? ORDER BY x.submission_id";
 case SqlId::Q35: return "UPDATE User SET username=?,role=?,department_id=NULLIF(?,0) WHERE user_id=?";
 case SqlId::Q36: return "UPDATE Submission SET raw_mark=?,final_mark=?,status='Marked' WHERE submission_id=?";
 case SqlId::Q37: return "SELECT COUNT(*) FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE a.course_id=? AND s.status NOT IN ('Moderated','Released')";
 case SqlId::Q38: return "UPDATE Submission SET status='Released' WHERE status='Moderated' AND assignment_id IN (SELECT assignment_id FROM Assignment WHERE course_id=?)";
 case SqlId::Q39: return "SELECT 1 FROM Assignment a JOIN CourseLecturer c ON a.course_id=c.course_id WHERE c.lecturer_id=? AND a.assignment_id=?";
 case SqlId::Q40: return "SELECT 1 FROM User u JOIN Course c ON c.department_id=u.department_id JOIN Assignment a ON a.course_id=c.course_id WHERE u.user_id=? AND u.role='HoD' AND u.active=1 AND a.assignment_id=?";
 case SqlId::Q41: return "SELECT 1 FROM User WHERE user_id=? AND role='Lecturer' AND active=1";
 case SqlId::Q42: return "SELECT 1 FROM User WHERE user_id=? AND role='Student' AND active=1";
 case SqlId::Q43: return "SELECT 1 FROM CourseLecturer WHERE lecturer_id=? AND course_id=?";
 case SqlId::Q44: return "SELECT 1 FROM Enrolment e JOIN Assignment a ON a.course_id=e.course_id WHERE e.student_id=? AND a.assignment_id=?";
 case SqlId::Q45: return "SELECT 1 FROM User WHERE user_id=? AND active=1";
 case SqlId::Q46: return "SELECT 1 FROM User WHERE user_id=?";
 case SqlId::Q47: return "SELECT 1 FROM Department WHERE department_id=?";
 case SqlId::Q48: return "SELECT 1 FROM Course c JOIN User u ON u.department_id=c.department_id WHERE u.user_id=? AND c.course_id=? AND u.role='HoD'";
 default:throw std::runtime_error("Unknown query");
 }
}
}
DataQuery::DataQuery(DataAccess& access,SqlId query):stmt_(access.db_,sqlFor(query)){}
void DataAccess::begin(){db_.exec("BEGIN IMMEDIATE");}
void DataAccess::commit(){db_.exec("COMMIT");}
void DataAccess::rollback(){db_.exec("ROLLBACK");}

void DataAccess::initializeSchema(const std::string& schemaPath){
 std::ifstream file(schemaPath);
 if(!file)throw std::runtime_error("Cannot open SQL schema: "+schemaPath);
 std::string sql((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
 db_.exec(sql);
}
