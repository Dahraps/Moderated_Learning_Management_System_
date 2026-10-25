#pragma once
#include "mlms/Database.hpp"
#include <string>
// All SQL statements are defined exclusively in the data-access implementation.
enum class SqlId {Q1,Q2,Q3,Q4,Q5,Q6,Q7,Q8,Q9,Q10,Q11,Q12,Q13,Q14,Q15,Q16,Q17,Q18,Q19,Q20,Q21,Q22,Q23,Q24,Q25,Q26,Q27,Q28,Q29,Q30,Q31,Q32,Q33,Q34,Q35,Q36,Q37,Q38,Q39,Q40,Q41,Q42,Q43,Q44,Q45,Q46,Q47,Q48,Q49,Q50 };
class DataAccess {
 DB& db_;
public:
 explicit DataAccess(DB& db):db_(db){}
 void initializeSchema(const std::string& schemaPath);
 void begin();
 void commit();
 void rollback();
 friend class DataQuery;
};
class DataQuery {
 Statement stmt_;
public:
 DataQuery(DataAccess& access,SqlId query);
 void bind(int index,int value){stmt_.bind(index,value);}
 void bind(int index,double value){stmt_.bind(index,value);}
 void bind(int index,const std::string& value){stmt_.bind(index,value);}
 void bindNull(int index){stmt_.bindNull(index);}
 bool next(){return stmt_.next();}
 void run(){stmt_.run();}
 int integer(int column)const{return stmt_.integer(column);}
 double number(int column)const{return stmt_.number(column);}
 std::string str(int column)const{return stmt_.str(column);}
 bool null(int column)const{return stmt_.null(column);}
};
