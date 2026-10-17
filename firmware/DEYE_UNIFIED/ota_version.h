#pragma once
static bool ota_newer(const String &tag) {
  unsigned a,b,c,x,y,z; int n=0;
  String v=tag; if(v.startsWith("v")) v.remove(0,1);
  if(sscanf(v.c_str(),"%u.%u.%u%n",&a,&b,&c,&n)!=3 || n!=(int)v.length()) return false;
  if(sscanf(FIRMWARE_VERSION,"%u.%u.%u",&x,&y,&z)!=3) return false;
  return a!=x ? a>x : b!=y ? b>y : c>z;
}
