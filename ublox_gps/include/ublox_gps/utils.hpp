#ifndef UBLOX_GPS_UTILS_HPP
#define UBLOX_GPS_UTILS_HPP

#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <time.h>

#include <ros/ros.h>

#include <ublox_gps/mkgmtime.h>

namespace ublox_node {

/**
 * @brief Convert date/time to UTC time in seconds.
 */
template<typename NavPVT>
long toUtcSeconds(const NavPVT& msg) {
  // Create TM struct for mkgmtime
  struct tm time = {0};
  time.tm_year = msg.year - 1900;
  time.tm_mon = msg.month - 1;
  time.tm_mday = msg.day;
  time.tm_hour = msg.hour;
  time.tm_min = msg.min;
  time.tm_sec = msg.sec;
  // C++ STL doesn't have a mkgmtime (though other libraries do)
  // STL mktime converts date/time to seconds in local time
  // A modified version of code external library is used for mkgmtime
  return mkgmtime(&time);
}

/**
 * @brief Check that the parameter is above the minimum.
 * @param val the value to check
 * @param min the minimum for this value
 * @param name the name of the parameter
 * @throws std::runtime_error if it is below the minimum
 */
template <typename V, typename T>
void checkMin(V val, T min, const std::string & name) {
  if (val < min) {
    std::stringstream oss;
    oss << "Invalid settings: " << name << " must be > " << min;
    throw std::runtime_error(oss.str());
  }
}

/**
 * @brief Check that the parameter is in the range.
 * @param val the value to check
 * @param min the minimum for this value
 * @param max the maximum for this value
 * @param name the name of the parameter
 * @throws std::runtime_error if it is out of bounds
 */
template <typename V, typename T>
void checkRange(V val, T min, T max, const std::string & name) {
  if (val < min || val > max) {
    std::stringstream oss;
    oss << "Invalid settings: " << name << " must be in range [" << min <<
        ", " << max << "].";
    throw std::runtime_error(oss.str());
  }
}

/**
 * @brief Check that the elements of the vector are in the range.
 * @param val the vector to check
 * @param min the minimum for this value
 * @param max the maximum for this value
 * @param name the name of the parameter
 * @throws std::runtime_error value it is out of bounds
 */
template <typename V, typename T>
void checkRange(std::vector<V> val, T min, T max, const std::string & name) {
  for (size_t i = 0; i < val.size(); i++)  {
    std::stringstream oss;
    oss << name << "[" << i << "]";
    checkRange(val[i], min, max, oss.str());
  }
}

/**
 * @brief Get a unsigned integer value from the parameter server.
 * @param key the key to be used in the parameter server's dictionary
 * @param u storage for the retrieved value.
 * @throws std::runtime_error if the parameter is out of bounds
 * @return true if found, false if not found.
 */
template <typename U>
bool getRosUint(ros::NodeHandle* node, const std::string& key, U &u) {
  int param;
  if (!node->getParam(key, param)) {
    return false;
  }
  // Check the bounds
  U min = std::numeric_limits<U>::lowest();
  U max = std::numeric_limits<U>::max();
  checkRange(param, min, max, key);
  // set the output
  u = (U) param;
  return true;
}

/**
 * @brief Get a unsigned integer value from the parameter server.
 * @param key the key to be used in the parameter server's dictionary
 * @param u storage for the retrieved value.
 * @param val value to use if the server doesn't contain this parameter.
 * @throws std::runtime_error if the parameter is out of bounds
 * @return true if found, false if not found.
 */
template <typename U, typename V>
void getRosUint(ros::NodeHandle* node, const std::string& key, U &u, V default_val) {
  if (!getRosUint(node, key, u)) {
    u = default_val;
  }
}

/**
 * @brief Get a unsigned integer vector from the parameter server.
 * @throws std::runtime_error if the parameter is out of bounds.
 * @return true if found, false if not found.
 */
template <typename U>
bool getRosUint(ros::NodeHandle* node, const std::string& key, std::vector<U> &u) {
  std::vector<int> param;
  if (!node->getParam(key, param)) {
    return false;
  }

  // Check the bounds
  U min = std::numeric_limits<U>::lowest();
  U max = std::numeric_limits<U>::max();
  checkRange(param, min, max, key);

  // set the output
  u.insert(u.begin(), param.begin(), param.end());
  return true;
}

/**
 * @brief Get a integer (size 8 or 16) value from the parameter server.
 * @param key the key to be used in the parameter server's dictionary
 * @param u storage for the retrieved value.
 * @throws std::runtime_error if the parameter is out of bounds
 * @return true if found, false if not found.
 */
template <typename I>
bool getRosInt(ros::NodeHandle* node, const std::string& key, I &u) {
  int param;
  if (!node->getParam(key, param)) {
    return false;
  }
  // Check the bounds
  I min = std::numeric_limits<I>::lowest();
  I max = std::numeric_limits<I>::max();
  checkRange(param, min, max, key);
  // set the output
  u = (I) param;
  return true;
}

/**
 * @brief Get an integer value (size 8 or 16) from the parameter server.
 * @param key the key to be used in the parameter server's dictionary
 * @param u storage for the retrieved value.
 * @param val value to use if the server doesn't contain this parameter.
 * @throws std::runtime_error if the parameter is out of bounds
 * @return true if found, false if not found.
 */
template <typename U, typename V>
void getRosInt(ros::NodeHandle* node, const std::string& key, U &u, V default_val) {
  if (!getRosInt(node, key, u)) {
    u = default_val;
  }
}

/**
 * @brief Get a int (size 8 or 16) vector from the parameter server.
 * @throws std::runtime_error if the parameter is out of bounds.
 * @return true if found, false if not found.
 */
template <typename I>
bool getRosInt(ros::NodeHandle* node, const std::string& key, std::vector<I> &i) {
  std::vector<int> param;
  if (!node->getParam(key, param)) {
    return false;
  }

  // Check the bounds
  I min = std::numeric_limits<I>::lowest();
  I max = std::numeric_limits<I>::max();
  checkRange(param, min, max, key);

  // set the output
  i.insert(i.begin(), param.begin(), param.end());
  return true;
}

static inline bool declareRosBoolean(ros::NodeHandle* node, const std::string &name, bool default_value)
{
  bool ret;

  if (!node->hasParam(name)) {
    node->setParam(name, default_value);
  }
  // implicit else: If the ROS node already has the parameter, just leave it

  if (!node->getParam(name, ret)) {
    throw std::runtime_error("Required parameter '" + name + "' has the wrong type (expected bool)");
  }
}

static inline bool getRosBoolean(ros::NodeHandle* node, const std::string &name)
{
  bool ret;
  if (!node->getParam(name, ret)) {
    // Note that if this is used after declareRosBoolean, this should never happen.
    throw std::runtime_error("Required parameter '" + name + "' has the wrong type (expected bool)");
  }

  return ret;
}

}  // namespace ublox_node

#endif  // UBLOX_GPS_UTILS_HPP
