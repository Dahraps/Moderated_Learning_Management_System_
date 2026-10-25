#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
using namespace std;
static void require(bool ok,const string& message);
static string now(){time_t t=time(nullptr);tm v=*gmtime(&t);ostringstream o;o<<put_time(&v,"%Y-%m-%dT%H:%M:%S");return o.str();}
static time_t parseTime(const string& x){tm t={};istringstream in(x);in>>get_time(&t,"%Y-%m-%dT%H:%M:%S");if(in.fail()||x.size()!=19)throw runtime_error("Date must be YYYY-MM-DDTHH:MM:SS (UTC)");t.tm_isdst=0;
#ifdef _WIN32
 time_t result=_mkgmtime(&t);
#else
 time_t result=timegm(&t);
#endif
 if(result==(time_t)-1)throw runtime_error("Invalid timestamp");
 tm check=*gmtime(&result);char buffer[20];strftime(buffer,sizeof(buffer),"%Y-%m-%dT%H:%M:%S",&check);require(string(buffer)==x,"Invalid calendar date");return result;
}
static string toHex(const unsigned char* b,size_t n){ostringstream o;for(size_t i=0;i<n;i++)o<<hex<<setfill('0')<<setw(2)<<(unsigned)b[i];return o.str();}
static vector<unsigned char> unhex(const string& s){if(s.size()%2)throw runtime_error("Invalid stored hash");vector<unsigned char> a;for(size_t i=0;i<s.size();i+=2){unsigned x;istringstream in(s.substr(i,2));in>>std::hex>>x;if(in.fail() || !in.eof())throw runtime_error("Invalid stored hash");a.push_back((unsigned char)x);}return a;}
static string passwordHash(const string& pw,const string& salt){auto b=unhex(salt);require(b.size()==16,"Invalid salt length");unsigned char out[32];if(!PKCS5_PBKDF2_HMAC(pw.c_str(),(int)pw.size(),b.data(),(int)b.size(),210000,EVP_sha256(),32,out))throw runtime_error("Password hashing failed");return toHex(out,32);}
static string newSalt(){unsigned char b[16];if(RAND_bytes(b,16)!=1)throw runtime_error("Random generation failed");return toHex(b,16);}
static bool constantEqual(const string& a,const string& b){if(a.size()!=b.size())return false;return CRYPTO_memcmp(a.data(),b.data(),a.size())==0;}
static void require(bool ok,const string& message){if(!ok)throw runtime_error(message);}
static int askInt(const string& label){cout<<label;string s;if(!getline(cin,s))throw runtime_error("Input closed");size_t n=0;int v;try{v=stoi(s,&n);}catch(...){throw runtime_error("Enter a valid integer");}require(n==s.size(),"Enter a valid integer");return v;}
static double askDouble(const string& label){cout<<label;string s;if(!getline(cin,s))throw runtime_error("Input closed");size_t n=0;double v;try{v=stod(s,&n);}catch(...){throw runtime_error("Enter a valid number");}require(n==s.size()&&isfinite(v),"Enter a valid number");return v;}
static string ask(const string& label){cout<<label;string s;if(!getline(cin,s))throw runtime_error("Input closed");require(!s.empty() && s.find_first_not_of(" \t\r\n")!=string::npos,"Value cannot be empty or whitespace");require(s.size()<=4096,"Input is too long");return s;}
