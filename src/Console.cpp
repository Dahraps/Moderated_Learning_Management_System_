#include "mlms/Console.hpp"
void runMenu(LMS& app, User& u){while(true){cout<<"\n"<<u.role()<<" "<<u.name<<" (ID "<<u.id<<")\n";
  cout<<u.menu()<<"\n";
  try{int n=askInt("Choice: ");if(n==0)return;
   if(u.role()=="Administrator"){switch(n){case 1:app.createDepartment(u.id);break;case 2:app.createUser(u.id);break;case 3:app.createCourse(u.id);break;case 4:app.assignLecturer(u.id);break;case 5:app.enrol(u.id);break;case 6:app.listAdministration(u.id);break;case 7:app.deactivateUser(u.id);break;case 8:app.changePassword(u.id);break;case 9:app.updateUser(u.id);break;default:cout<<"Unknown choice\n";}}
   else if(u.role()=="Lecturer"){switch(n){case 1:app.createAssignment(u.id);break;case 2:app.extension(u.id);break;case 3:app.mark(u.id);break;case 4:app.listLecturer(u.id);break;case 5:app.changePassword(u.id);break;default:cout<<"Unknown choice\n";}}
   else if(u.role()=="HoD"){switch(n){case 1:app.pending(u.id);break;case 2:app.moderate(u.id);break;case 3:app.release(u.id);break;case 4:app.changePassword(u.id);break;case 5:app.adjustMark(u.id);break;case 6:app.bulkRelease(u.id);break;default:cout<<"Unknown choice\n";}}
   else {switch(n){case 1:app.listStudent(u.id);break;case 2:app.submit(u.id);break;case 3:app.results(u.id);break;case 4:app.changePassword(u.id);break;default:cout<<"Unknown choice\n";}}
  }catch(const exception& e){cerr<<"Error: "<<e.what()<<"\n";}
 }}
