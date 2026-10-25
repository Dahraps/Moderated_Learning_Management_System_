#pragma once
#include "mlms/Database.hpp"
#include <algorithm>
using namespace std;
struct ILatePenaltyStrategy {virtual ~ILatePenaltyStrategy()=default;virtual double penalty(int days,double maxMark)const=0;};
struct StandardLatePenalty:ILatePenaltyStrategy {double penalty(int days,double maxMark)const override{return days>3?maxMark:days*0.1*maxMark;}};
struct NoPenaltyStrategy:ILatePenaltyStrategy {double penalty(int,double)const override{return 0;}};
struct Submission {int id,assignment,student;string status,submittedAt;double raw,finalMark,maxMark;};
struct ISubmissionRepository {virtual ~ISubmissionRepository()=default;virtual Submission get(int)=0;};
class SubmissionRepository:public ISubmissionRepository {
 DB& d;
public:
 explicit SubmissionRepository(DB& x):d(x){}
 Submission get(int id)override{
  Statement q(d,"SELECT s.submission_id,s.assignment_id,s.student_id,s.status,s.submitted_at,COALESCE(s.raw_mark,0),COALESCE(s.final_mark,0),a.max_mark FROM Submission s JOIN Assignment a ON a.assignment_id=s.assignment_id WHERE s.submission_id=?");q.bind(1,id);require(q.next(),"Submission not found");return {q.integer(0),q.integer(1),q.integer(2),q.str(3),q.str(4),q.number(5),q.number(6),q.number(7)};
 }
};
