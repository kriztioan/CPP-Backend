#ifndef _ATOM_H_
#define _ATOM_H_

#include <string>
#include <string_view>

class Atom {

public:
  Atom(double x, double y, double z, int type);

  std::string_view getCPKColor() const;

  void setX(double x);

  const double &getX() const;

  void setY(double y);

  const double &getY() const;

  void setZ(double z);

  double const &getZ() const;

  void setSize(double);

  void setType(int type);

  const double &getMass() const;

  const double &getSize() const;

  const int &getType() const;

private:
  static constexpr const double AtomicMasses[] = {
      0.0,        1.007940,   4.002602,   6.941000,   9.012182,   10.811000,
      12.011000,  14.006740,  15.999400,  18.998404,  20.179701,  22.989767,
      24.305000,  26.981539,  28.085501,  30.973763,  32.066002,  35.452702,
      39.948002,  39.098301,  40.077999,  44.955910,  47.880001,  50.941502,
      51.996101,  54.938049,  55.847000,  58.933201,  58.693401,  63.546001,
      65.389999,  69.723000,  72.610001,  74.921593,  78.959999,  79.903999,
      83.800003,  85.467796,  87.620003,  88.905853,  91.223999,  92.906380,
      95.940002,  98.000000,  101.070000, 102.905502, 106.419998, 107.868202,
      112.411003, 114.820000, 118.709999, 121.757004, 127.599998, 126.904472,
      131.289993, 132.905426, 137.326996, 138.905502, 140.115005, 140.907654,
      144.240005, 145.000000, 150.360001, 151.964996, 157.250000, 158.925339,
      162.500000, 164.930313, 167.259995, 168.934204, 173.039993, 174.966995,
      178.490005, 180.947906, 183.850006, 186.207001, 190.199997, 192.220001,
      195.080002, 196.966537, 200.589996, 204.383301, 207.199997, 208.980377,
      209.000000, 210.000000, 222.000000, 223.000000, 226.024994, 227.028000,
      232.038101, 231.035904, 238.028900, 237.048004, 244.000000, 243.000000,
      247.000000, 247.000000, 251.000000, 252.000000, 257.000000, 258.000000,
      259.000000, 262.000000, 261.000000, 262.000000, 263.000000, 262.000000,
      265.000000, 266.000000};

  static constexpr const char *AtomicCPKColors[] = {
      "",       "ededed", "d9ffff", "cc80ff", "c2ff00", "ffb5b5", "909090",
      "3050f8", "ff0d0d", "90e050", "b3e3f5", "ab5cf2", "8aff00", "bfa6a6",
      "f0c8a0", "ff8000", "ffff30", "1ff01f", "80d1e3", "8f40d4", "3dff00",
      "e6e6e6", "bfc2c7", "a6a6ab", "8a99c7", "9c7ac7", "e06633", "f090a0",
      "50d050", "c88033", "7d80b0", "c28f8f", "668f8f", "bd80e3", "ffa100",
      "a62929", "5cb8d1", "702eb0", "00ff00", "94ffff", "94e0e0", "73c2c9",
      "54b5b5", "3b9e9e", "248f8f", "0a7d8c", "006985", "c0c0c0", "ffd98f",
      "a67573", "668080", "9e63b5", "d47a00", "940094", "429eb0", "57178f",
      "00c900", "70d4ff", "ffffc7", "d9ffc7", "c7ffc7", "a3ffc7", "8fffc7",
      "61ffc7", "45ffc7", "30ffc7", "1fffc7", "00ff9c", "00e675", "00d452",
      "00bf38", "00ab24", "4dc2ff", "4da6ff", "2194d6", "267dab", "266696",
      "175487", "d0d0e0", "ffd123", "b8b8d0", "a6544d", "575961", "9e4fb5",
      "ab5c00", "754f45", "428296", "420066", "007d00", "70abfa", "00baff",
      "00a1ff", "008fff", "0080ff", "006bff", "545cf2", "785ce3", "8a4fe3",
      "a136d4", "b31fd4", "b31fba", "b30da6", "bd0d87", "c70066", "cc0059",
      "d1004f", "d90045", "e00038", "e6002e", "eb0026"};

  static constexpr const double AtomicSizes[] = {
      0.0,   100.0, 140.0, 182.0, 112.0, 192.0, 170.0, 95.0,  152.0, 42.0,
      154.0, 227.0, 145.0, 118.0, 210.0, 98.0,  180.0, 79.0,  188.0, 243.0,
      231.0, 184.0, 176.0, 171.0, 166.0, 161.0, 0.0,   0.0,   163.0, 140.0,
      139.0, 187.0, 125.0, 185.0, 103.0, 185.0, 202.0, 265.0, 219.0, 0.0,
      206.0, 0.0,   190.0, 183.0, 178.0, 0.0,   169.0, 172.0, 158.0, 193.0,
      217.0, 133.0, 123.0, 198.0, 216.0, 343.0, 268.0, 0.0,   0.0,   247.0,
      206.0, 205.0, 238.0, 231.0, 233.0, 0.0,   228.0, 0.0,   0.0,   0.0,
      222.0, 217.0, 0.0,   200.0, 193.0, 0.0,   0.0,   0.0,   177.0, 166.0,
      155.0, 156.0, 202.0, 207.0, 135.0, 0.0,   220.0, 0.0,   283.0, 0.0,
      0.0,   0.0,   186.0, 0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0};

  std::string _cpkcolor;

  double _x;

  double _y;

  double _z;

  double _mass;

  double _size;

  int _type;
};

inline std::string_view Atom::getCPKColor() const { return (_cpkcolor); }

inline const double &Atom::getMass() const { return (_mass); }

inline const double &Atom::getSize() const { return (_size); }

inline void Atom::setX(double x) { _x = x; }

inline const double &Atom::getX() const { return (_x); }

inline void Atom::setY(double y) { _y = y; }

inline const double &Atom::getY() const { return (_y); }

inline void Atom::setZ(double z) { _z = z; }

inline const double &Atom::getZ() const { return (_z); }

inline const int &Atom::getType() const { return (_type); }

#endif /* _PLOT_H_ */