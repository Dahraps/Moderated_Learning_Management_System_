#pragma once
#include <algorithm>
#include <stdexcept>
class ILatePenaltyStrategy {
public:
 virtual ~ILatePenaltyStrategy()=default;
 virtual double penalty(int days,double maxMark)const=0;
};
class StandardLatePenalty final:public ILatePenaltyStrategy {
public:
 double penalty(int days,double maxMark)const override {
  if(days<0||maxMark<=0)throw std::invalid_argument("Invalid penalty inputs");
  return days>3?maxMark:days*0.10*maxMark;
 }
};
class NoPenaltyStrategy final:public ILatePenaltyStrategy {
public:
 double penalty(int days,double maxMark)const override {
  if(days<0||maxMark<=0)throw std::invalid_argument("Invalid penalty inputs");
  return 0;
 }
};
