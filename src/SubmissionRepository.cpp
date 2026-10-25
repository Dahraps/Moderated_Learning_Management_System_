#include "mlms/SubmissionRepository.hpp"
#include "mlms/Utility.hpp"
Submission SubmissionRepository::get(int id){
 Statement q(db,"SELECT s.submission_id,s.assignment_id,s.student_id,s.status,s.submitted_at,COALESCE(s.raw_mark,0),COALESCE(s.final_mark,0),a.max_mark FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE s.submission_id=?");
 q.bind(1,id);require(q.next(),"Submission not found");
 return {q.integer(0),q.integer(1),q.integer(2),q.str(3),q.str(4),q.number(5),q.number(6),q.number(7)};
}
