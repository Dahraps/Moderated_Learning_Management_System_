#pragma once
#include "mlms/Database.hpp"
#include <string>
struct Submission {int id,assignment,student;std::string status,submittedAt;double raw,finalMark,maxMark;};
class ISubmissionRepository {
public:
 virtual ~ISubmissionRepository()=default;
 virtual Submission get(int id)=0;
};
class SubmissionRepository final:public ISubmissionRepository {
 DB& db;
public:
 explicit SubmissionRepository(DB& d):db(d){}
 Submission get(int id) override;
};
