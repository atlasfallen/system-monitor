#include "format.h"

#include <string>

using std::string;

// format helper funtion to padd zeros
string padzero(long d) {
  string s = std::to_string(d);
  while (s.length() < 2) s = "0" + s;
  return s;
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds [[maybe_unused]]) {
  long hh, mm, ss;
  double dseconds, dhh, dmm;
  string shh, smm, sss;

  dseconds = (double)seconds;
  dhh = dseconds / 3600;
  hh = (long)dhh;
  dhh -= (double)hh;
  dmm = 60.0 * dhh;
  mm = (long)dmm;
  dmm -= (double)mm;
  ss = dmm * 60;

  return padzero(hh) + ":" + padzero(mm) + ":" + padzero(ss);
}