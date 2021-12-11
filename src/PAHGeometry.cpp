#include "PAHGeometry.h"

PAHGeometry::PAHGeometry() : _mass(0.0), _dimensions(3, {2.0, 0.0, 0.0}) {
  _atoms.reserve(16);
}

PAHGeometry::PAHGeometry(const std::vector<Atom> &atoms)
    : _atoms(atoms.cbegin(), atoms.cend()), _mass(0.0),
      _dimensions(3, {2.0, 0.0, 0.0}) {

  for (const auto &atom : _atoms) {

    _mass += atom.getMass();
  }
}

void PAHGeometry::diagonalize() {

  double x_com = 0, y_com = 0, z_com = 0, m_tot = 0;

  for (const auto &atom : _atoms) {

    if (atom.getType() == 12 || atom.getType() == 26)
      continue;

    x_com += atom.getMass() * atom.getX();
    y_com += atom.getMass() * atom.getY();
    z_com += atom.getMass() * atom.getZ();
    m_tot += atom.getMass();
  }

  x_com /= m_tot;
  y_com /= m_tot;
  z_com /= m_tot;

  for (auto &atom : _atoms) {

    atom.setX(atom.getX() - x_com);
    atom.setY(atom.getY() - y_com);
    atom.setZ(atom.getZ() - z_com);
  }

  double I[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

  for (const auto &atom : _atoms) {

    if (atom.getType() == 12 || atom.getType() == 26)
      continue;

    I[0][0] += atom.getMass() *
               (atom.getY() * atom.getY() + atom.getZ() * atom.getZ());
    I[1][1] += atom.getMass() *
               (atom.getX() * atom.getX() + atom.getZ() * atom.getZ());
    I[2][2] += atom.getMass() *
               (atom.getX() * atom.getX() + atom.getY() * atom.getY());

    // I[0][1] -= atom.getMass() * atom.getX() * atom.getY();
    // I[0][2] -= atom.getMass() * atom.getX() * atom.getZ();
    // I[1][2] -= atom.getMass() * atom.getY() * atoml.getZ();
  }
  /*
  if (abs(I[0][1] / I[0][0]) < 1e-3 && abs(I[0][1] / I[1][1]) < 1e-3) {

          I[0][1] = 0.0;
  }

  if (abs(I[0][2] / I[0][0]) < 1e-3 && abs(I[0][2] / I[2][2]) < 1e-3) {

          I[0][2] = 0.0;
  }

  if (abs(I[1][2] / I[1][1]) < 1e-3 && abs(I[1][2] / I[2][2]) < 1e-3){

          I[1][2] = 0.0;
  }
  */
  double v[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
         b[3] = {I[0][0], I[1][1], I[2][2]}, *d = b, f[3] = {0.0, 0.0, 0.0};

  int nrot = 0;

  double sm, tresh, g, h, t, theta, c, s, tau;

  for (size_t run = 0; run < 50; run++) {

    sm = fabs(I[0][1]) + fabs(I[0][2]) + fabs(I[1][2]);

    if (sm == 0.0)
      break;

    if (run < 4)
      tresh = 0.2 * sm / 9.0;
    else
      tresh = 0.0;

    for (size_t ip = 0; ip < 2; ip++) {
      for (size_t iq = ip + 1; iq < 3; iq++) {
        g = 100.0 * fabs(I[ip][iq]);
        if (run > 4 && fabs(d[ip]) + g == fabs(d[ip]) &&
            fabs(d[iq]) + g == fabs(d[iq]))
          I[ip][iq] = 0.0;
        else if (fabs(I[ip][iq]) > tresh) {
          h = d[iq] - d[ip];
          if (fabs(h) + g == fabs(h))
            t = I[ip][iq] / h;
          else {
            theta = 0.5 * h / I[ip][iq];
            t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
            if (theta < 0.0)
              t = -t;
          }

          c = 1.0 / sqrt(1.0 + t * t);
          s = t * c;
          tau = s / (1.0 + c);
          h = t * I[ip][iq];
          f[ip] -= h;
          f[iq] += h;
          d[ip] -= h;
          d[iq] += h;
          I[ip][iq] = 0.0;

          for (size_t j = 0; j <= ip - 1; j++) {
            g = I[j][ip];
            h = I[j][iq];
            I[j][ip] = g - s * (h + g * tau);
            I[j][iq] = h + s * (g - h * tau);
          }

          for (size_t j = ip + 1; j <= iq - 1; j++) {
            g = I[ip][j];
            h = I[j][iq];
            I[ip][j] = g - s * (h + g * tau);
            I[j][iq] = h + s * (g - h * tau);
          }

          for (size_t j = iq + 1; j < 3; j++) {
            g = I[ip][j];
            h = I[iq][j];
            I[ip][j] = g - s * (h + g * tau);
            I[iq][j] = h + s * (g - h * tau);
          }

          for (size_t j = 0; j < 3; j++) {
            g = v[j][ip];
            h = v[j][iq];
            v[j][ip] = g - s * (h + g * tau);
            v[j][iq] = h + s * (g - h * tau);
          }
          ++nrot;
        }
      }
    }

    for (size_t ip = 0; ip < 3; ip++) {
      b[ip] += f[ip];
      d[ip] = b[ip];
      f[ip] = 0.0;
    }
  }

  size_t k, j;

  double p;

  for (size_t i = 0; i < 3; i++) {
    p = d[k = i];
    for (j = i + 1; j < 3; j++) {
      if (d[j] < p)
        p = d[k = j];
    }

    if (k != i) {
      d[k] = d[i];
      d[i] = p;
      for (j = 0; j < 3; j++) {
        p = v[i][j];
        v[i][j] = v[k][j];
        v[k][j] = p;
      }
    }
  }

  double x, y, z;

  for (auto &atom : _atoms) {

    x = atom.getX() * v[0][0] + atom.getY() * v[0][1] + atom.getZ() * v[0][2];
    y = atom.getX() * v[1][0] + atom.getY() * v[1][1] + atom.getZ() * v[1][2];
    z = atom.getX() * v[2][0] + atom.getY() * v[2][1] + atom.getZ() * v[2][2];

    atom.setX(x);
    atom.setY(y);
    atom.setZ(z);
  }
}

std::vector<std::vector<int>> &PAHGeometry::getBonds() {

  if (_bonds.size() > 0) {

    return _bonds;
  }

  _bonds.reserve(64);

  std::vector<int> ls;

  ls.reserve(32);

  for (const auto &atom1 : _atoms) {

    int i = 0;

    for (const auto &atom2 : _atoms) {

      double d = sqrt(pow(atom1.getX() - atom2.getX(), 2) +
                      pow(atom1.getY() - atom2.getY(), 2));

      if (d > 0 && d < 1.6) {

        ls.push_back(i);
      }
      ++i;
    }

    _bonds.push_back(ls);

    ls.clear();
  }

  for (auto &bond : _bonds) {

    for (const auto &b : bond) {

      _bonds[b].erase(_bonds[b].begin());
    }
  }

  return _bonds;
}