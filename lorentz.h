#ifndef LORENTZ_H
#define LORENTZ_H

#include <cmath>
#include <ostream>

namespace lorentz
{

/// Lorentz 4-vector
class vec
{
  double components[4];

public:
  /// Constructor
  explicit vec() { for (int i = 0; i < 4; ++i) components[i] = 0; }

  /// Returns the t-component
  double t() const { return components[0]; }
  /// Returns the x-component
  double x() const { return components[1]; }
  /// Returns the y-component
  double y() const { return components[2]; }
  /// Returns the z-component
  double z() const { return components[3]; }
  /// Returns the square norm of the 4-vector, v^2
  double norm2() const { return (*this) * (*this); }
  /// Returns the norm of the 4-vector, sqrt(v^2)
  double norm() const { return std::sqrt(norm2()); }

  /// Returns a reference to the ith component (v[0] = t())
  double &operator[](int i) { return components[i]; }
  /// Returns a const reference to the ith component (v[0] = t())
  const double &operator[](int i) const { return components[i]; }

  inline vec operator-() const;
  inline vec operator-(const vec &other) const;
  inline vec operator-=(const vec &other);

  inline vec operator+() const;
  inline vec operator+(const vec &other) const;
  inline vec &operator+=(const vec &other);

  inline vec operator*(double d) const;
  inline vec &operator*=(double d);

  /// Lorentz scalar product
  inline double operator*(const vec &other) const;

  /// Constructs a 4-vector with the given mass and spatial components
  inline static vec mxyz(double m, double x, double y, double z);
  /// Constructs a 4-vector with the given temporal and spatial components
  inline static vec txyz(double t, double x, double y, double z);
  /// Constructs a 4-vector with the given temporal and spatial components
  inline static vec xyzt(double x, double y, double z, double t);
  /// Constructs a 4-vector with the given temporal and spatial components
  /// (given in spherical coordinates)
  inline static vec m_r_phi_theta(double m, double r, double phi, double theta);
};

vec vec::operator-() const
{
  vec v = *this;
  v *= -1;
  return v;
}

vec vec::operator-(const vec &other) const
{
  return (*this) + (-other);
}

vec vec::operator-=(const vec &other)
{
  return (*this) += (-other);
}

vec vec::operator+() const
{
  return *this;
}

vec vec::operator+(const vec &other) const
{
  vec v = *this;
  v += other;
  return v;
}

vec &vec::operator+=(const vec &other)
{
  for (int i = 0; i < 4; ++i) {
    components[i] += other[i];
  }
  return *this;
}

vec vec::operator*(double d) const
{
  vec v = *this;
  v *= d;
  return v;
}

vec &vec::operator*=(double d)
{
  for (int i = 0; i < 4; ++i) {
    components[i] *= d;
  }
  return *this;
}

double vec::operator*(const vec &other) const
{
  return t() * other.t() - x() * other.x() - y() * other.y() - z() * other.z();
}

vec vec::mxyz(double m, double x, double y, double z)
{
  double t = std::sqrt(m * m + x * x + y * y + z * z);
  return txyz(t, x, y, z);
}

vec vec::txyz(double t, double x, double y, double z)
{
  vec v;
  v[0] = t;
  v[1] = x;
  v[2] = y;
  v[3] = z;
  return v;
}

vec vec::xyzt(double x, double y, double z, double t)
{
  return txyz(t, x, y, z);
}

vec vec::m_r_phi_theta(double m, double r, double phi, double theta)
{
  return mxyz(m,
              r * std::cos(theta) * std::cos(phi),
              r * std::cos(theta) * std::sin(phi),
              r * std::sin(theta));
}


inline double eta(const vec &v)
{
  lorentz::vec fake = lorentz::vec::mxyz(0, v.x(), v.y(), v.z());
  return std::atanh(fake.z() / fake.t());
}

inline double rapidity(const vec &v)
{
  return 0.5 * std::log((v.t() + v.z()) / (v.t() - v.z()));
}

inline double spatial_norm(const vec &v)
{
  return std::sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
}

inline double pt2(const vec &v)
{
  return v.x() * v.x() + v.y() * v.y();
}

inline double pt(const vec &v)
{
  return std::sqrt(pt2(v));
}

inline double phi(const vec &v)
{
  return std::atan2(v.y(), v.x());
}

inline double delta_r(const vec &v1, const vec &v2)
{
  double deta = eta(v1) - eta(v2);
  double dphi = phi(v1) - phi(v2);
  return std::sqrt(deta * deta + dphi * dphi);
}

} // namespace lorentz

inline std::ostream &operator<<(std::ostream &out, const lorentz::vec &vec)
{
  return out << "(" << vec.t() << " " << vec.x()
             << " " << vec.y() << " " << vec.z() << ")";
}

#endif // LORENTZ_H
