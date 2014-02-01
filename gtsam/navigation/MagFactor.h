/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    MagFactor.h
 * @brief   Factors involving magnetometers
 * @author  Frank Dellaert
 * @date   January 29, 2014
 */

#include <gtsam/nonlinear/NonlinearFactor.h>
#include <gtsam/geometry/Rot2.h>
#include <gtsam/geometry/Rot3.h>
#include <gtsam/base/LieScalar.h>

namespace gtsam {

/**
 * Factor to estimate rotation given magnetometer reading
 * This version uses model measured bM = scale * bRn * direction + bias
 * and assumes scale, direction, and the bias are given.
 * Rotation is around negative Z axis, i.e. positive is yaw to right!
 */
class MagFactor: public NoiseModelFactor1<Rot2> {

  const Point3 measured_; ///< The measured magnetometer values
  const Point3 nM_; ///< Local magnetic field (mag output units)
  const Point3 bias_; ///< bias

public:

  /** Constructor */
  MagFactor(Key key, const Point3& measured, const LieScalar& scale,
      const Sphere2& direction, const Point3& bias,
      const SharedNoiseModel& model) :
      NoiseModelFactor1<Rot2>(model, key), //
      measured_(measured), nM_(scale * direction), bias_(bias) {
  }

  /// @return a deep copy of this factor
  virtual NonlinearFactor::shared_ptr clone() const {
    return boost::static_pointer_cast<NonlinearFactor>(
        NonlinearFactor::shared_ptr(new MagFactor(*this)));
  }

  static Point3 unrotate(const Rot2& R, const Point3& p,
      boost::optional<Matrix&> HR = boost::none) {
    Point3 q = Rot3::yaw(R.theta()).unrotate(p,HR);
    if (HR) *HR = HR->col(2);
    return q;
  }

  /**
   * @brief vector of errors
   */
  Vector evaluateError(const Rot2& nRb,
      boost::optional<Matrix&> H = boost::none) const {
    // measured bM = nRb� * nM + b
    Point3 hx = unrotate(nRb, nM_, H) + bias_;
    return (hx - measured_).vector();
  }
};

/**
 * Factor to estimate rotation given magnetometer reading
 * This version uses model measured bM = scale * bRn * direction + bias
 * and assumes scale, direction, and the bias are given
 */
class MagFactor1: public NoiseModelFactor1<Rot3> {

  const Point3 measured_; ///< The measured magnetometer values
  const Point3 nM_; ///< Local magnetic field (mag output units)
  const Point3 bias_; ///< bias

public:

  /** Constructor */
  MagFactor1(Key key, const Point3& measured, const LieScalar& scale,
      const Sphere2& direction, const Point3& bias,
      const SharedNoiseModel& model) :
      NoiseModelFactor1<Rot3>(model, key), //
      measured_(measured), nM_(scale * direction), bias_(bias) {
  }

  /// @return a deep copy of this factor
  virtual NonlinearFactor::shared_ptr clone() const {
    return boost::static_pointer_cast<NonlinearFactor>(
        NonlinearFactor::shared_ptr(new MagFactor1(*this)));
  }

  /**
   * @brief vector of errors
   */
  Vector evaluateError(const Rot3& nRb,
      boost::optional<Matrix&> H = boost::none) const {
    // measured bM = nRb� * nM + b
    Point3 hx = nRb.unrotate(nM_, H) + bias_;
    return (hx - measured_).vector();
  }
};

/**
 * Factor to calibrate local Earth magnetic field as well as magnetometer bias
 * This version uses model measured bM = bRn * nM + bias
 * and optimizes for both nM and the bias, where nM is in units defined by magnetometer
 */
class MagFactor2: public NoiseModelFactor2<Point3, Point3> {

  const Point3 measured_; ///< The measured magnetometer values
  const Rot3 bRn_; ///< The assumed known rotation from nav to body

public:

  /** Constructor */
  MagFactor2(Key key1, Key key2, const Point3& measured, const Rot3& nRb,
      const SharedNoiseModel& model) :
      NoiseModelFactor2<Point3, Point3>(model, key1, key2), //
      measured_(measured), bRn_(nRb.inverse()) {
  }

  /// @return a deep copy of this factor
  virtual NonlinearFactor::shared_ptr clone() const {
    return boost::static_pointer_cast<NonlinearFactor>(
        NonlinearFactor::shared_ptr(new MagFactor2(*this)));
  }

  /**
   * @brief vector of errors
   * @param nM (unknown) local earth magnetic field vector, in nav frame
   * @param bias (unknown) 3D bias
   */
  Vector evaluateError(const Point3& nM, const Point3& bias,
      boost::optional<Matrix&> H1 = boost::none, boost::optional<Matrix&> H2 =
          boost::none) const {
    // measured bM = nRb� * nM + b, where b is unknown bias
    Point3 hx = bRn_.rotate(nM, boost::none, H1) + bias;
    if (H2)
      *H2 = eye(3);
    return (hx - measured_).vector();
  }
};

/**
 * Factor to calibrate local Earth magnetic field as well as magnetometer bias
 * This version uses model measured bM = scale * bRn * direction + bias
 * and optimizes for both scale, direction, and the bias.
 */
class MagFactor3: public NoiseModelFactor3<LieScalar, Sphere2, Point3> {

  const Point3 measured_; ///< The measured magnetometer values
  const Rot3 bRn_; ///< The assumed known rotation from nav to body

public:

  /** Constructor */
  MagFactor3(Key key1, Key key2, Key key3, const Point3& measured,
      const Rot3& nRb, const SharedNoiseModel& model) :
      NoiseModelFactor3<LieScalar, Sphere2, Point3>(model, key1, key2, key3), //
      measured_(measured), bRn_(nRb.inverse()) {
  }

  /// @return a deep copy of this factor
  virtual NonlinearFactor::shared_ptr clone() const {
    return boost::static_pointer_cast<NonlinearFactor>(
        NonlinearFactor::shared_ptr(new MagFactor3(*this)));
  }

  /**
   * @brief vector of errors
   * @param nM (unknown) local earth magnetic field vector, in nav frame
   * @param bias (unknown) 3D bias
   */
  Vector evaluateError(const LieScalar& scale, const Sphere2& direction,
      const Point3& bias, boost::optional<Matrix&> H1 = boost::none,
      boost::optional<Matrix&> H2 = boost::none, boost::optional<Matrix&> H3 =
          boost::none) const {
    // measured bM = nRb� * nM + b, where b is unknown bias
    Sphere2 rotated = bRn_.rotate(direction, boost::none, H2);
    Point3 hx = scale * rotated.point3() + bias;
    if (H1)
      *H1 = rotated.point3().vector();
    if (H2) // H2 is 2*2, but we need 3*2
    {
      Matrix H;
      rotated.point3(H);
      *H2 = scale * H * (*H2);
    }
    if (H3)
      *H3 = eye(3);
    return (hx - measured_).vector();
  }
};

}

