#include <string>
#include <cstdio>
#include <cassert>
struct String : std::string {
 using std::string::string;
 bool startsWith(const char *s)const{return rfind(s,0)==0;}
 void remove(size_t a,size_t n){erase(a,n);}
};
#define FIRMWARE_VERSION "4.6.0"
#include "../ota_version.h"
int main(){
 assert(ota_newer("v4.6.1"));assert(ota_newer("4.10.0"));assert(ota_newer("5.0.0"));
 assert(!ota_newer("v4.6.0"));assert(!ota_newer("4.5.99"));assert(!ota_newer("3.99.99"));
 assert(!ota_newer(""));assert(!ota_newer("latest"));assert(!ota_newer("v4.7.0-rc1"));assert(!ota_newer("v4.7.0broken"));
 puts("PASS: screen OTA numeric versions, equal/older releases and unsupported tags");
}
