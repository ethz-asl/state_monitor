#include "state_monitor/ros_plotters.h"

OdometryPlotter::OdometryPlotter(
    const ros::NodeHandle &nh, const std::string &topic,
    const std::shared_ptr<mglGraph> &gr, const double keep_data_for_secs,
    const size_t num_subplots_wide, const size_t num_subplots_high,
    const size_t position_subplot_idx, const size_t linear_velocity_subplot_idx,
    const size_t orientation_subplot_idx,
    const size_t angular_velocity_subplot_idx)
    : RosPlotter(nh, topic, gr, keep_data_for_secs, num_subplots_wide,
                 num_subplots_high,
                 {"Position (m)", "Linear Velocity (m/s)", "Orientation (rads)",
                  "Angular Velocity (rads/s)"},
                 {position_subplot_idx, linear_velocity_subplot_idx,
                  orientation_subplot_idx, angular_velocity_subplot_idx}) {}

void OdometryPlotter::callback(const nav_msgs::OdometryConstPtr &msg) {
  const double t = msg->header.stamp.toSec();

  double roll, pitch, yaw;
  tf::Matrix3x3(tf::Quaternion(
                    msg->pose.pose.orientation.x, msg->pose.pose.orientation.y,
                    msg->pose.pose.orientation.z, msg->pose.pose.orientation.w))
      .getRPY(roll, pitch, yaw);

  sub_plots_[POSITION].addDataPoint(t, msg->pose.pose.position.x,
                                    msg->pose.pose.position.y,
                                    msg->pose.pose.position.z);
  sub_plots_[LINEAR_VELOCITY].addDataPoint(t, msg->twist.twist.linear.x,
                                           msg->twist.twist.linear.y,
                                           msg->twist.twist.linear.z);
  sub_plots_[ORIENTATION].addDataPoint(t, roll, pitch, yaw);
  sub_plots_[ANGULAR_VELOCITY].addDataPoint(t, msg->twist.twist.angular.x,
                                            msg->twist.twist.angular.y,
                                            msg->twist.twist.angular.z);
}

PointPlotter::PointPlotter(const ros::NodeHandle &nh, const std::string &topic,
                           const std::shared_ptr<mglGraph> &gr,
                           const double keep_data_for_secs,
                           const size_t num_subplots_wide,
                           const size_t num_subplots_high,
                           const std::string title, const size_t subplot_idx)
    : RosPlotter(nh, topic, gr, keep_data_for_secs, num_subplots_wide,
                 num_subplots_high, {title}, {subplot_idx}) {}

void PointPlotter::callback(const geometry_msgs::PointStampedConstPtr &msg) {
  const double t = msg->header.stamp.toSec();

  sub_plots_.front().addDataPoint(t, msg->point.x, msg->point.y, msg->point.z);
}

TransformPlotter::TransformPlotter(
    const ros::NodeHandle &nh, const std::string &topic,
    const std::shared_ptr<mglGraph> &gr, const double keep_data_for_secs,
    const size_t num_subplots_wide, const size_t num_subplots_high,
    const size_t position_subplot_idx, const size_t orientation_subplot_idx)
    : RosPlotter(nh, topic, gr, keep_data_for_secs, num_subplots_wide,
                 num_subplots_high, {"Position (m)", "Orientation (rads)"},
                 {position_subplot_idx, orientation_subplot_idx}) {}

void TransformPlotter::callback(
    const geometry_msgs::TransformStampedConstPtr &msg) {
  const double t = msg->header.stamp.toSec();

  double roll, pitch, yaw;
  tf::Matrix3x3(
      tf::Quaternion(msg->transform.rotation.x, msg->transform.rotation.y,
                     msg->transform.rotation.z, msg->transform.rotation.w))
      .getRPY(roll, pitch, yaw);

  sub_plots_[POSITION].addDataPoint(t, msg->transform.translation.x,
                                    msg->transform.translation.y,
                                    msg->transform.translation.z);
  sub_plots_[ORIENTATION].addDataPoint(t, roll, pitch, yaw);
}

PosePlotter::PosePlotter(const ros::NodeHandle &nh, const std::string &topic,
                         const std::shared_ptr<mglGraph> &gr,
                         const double keep_data_for_secs,
                         const size_t num_subplots_wide,
                         const size_t num_subplots_high,
                         const size_t position_subplot_idx,
                         const size_t orientation_subplot_idx)
    : RosPlotter(nh, topic, gr, keep_data_for_secs, num_subplots_wide,
                 num_subplots_high, {"Position (m)", "Orientation (rads)"},
                 {position_subplot_idx, orientation_subplot_idx}) {}

void PosePlotter::callback(const geometry_msgs::PoseStampedConstPtr &msg) {
  const double t = msg->header.stamp.toSec();

  double roll, pitch, yaw;
  tf::Matrix3x3(tf::Quaternion(msg->pose.orientation.x, msg->pose.orientation.y,
                               msg->pose.orientation.z,
                               msg->pose.orientation.w))
      .getRPY(roll, pitch, yaw);

  sub_plots_[POSITION].addDataPoint(t, msg->pose.position.x,
                                    msg->pose.position.y, msg->pose.position.z);
  sub_plots_[ORIENTATION].addDataPoint(t, roll, pitch, yaw);
}

ImuPlotter::ImuPlotter(const ros::NodeHandle &nh, const std::string &topic,
                       const std::shared_ptr<mglGraph> &gr,
                       const double keep_data_for_secs,
                       const size_t num_subplots_wide,
                       const size_t num_subplots_high,
                       const size_t linear_acceleration_subplot_idx,
                       const size_t orientation_subplot_idx,
                       const size_t angular_velocity_subplot_idx)
    : RosPlotter(nh, topic, gr, keep_data_for_secs, num_subplots_wide,
                 num_subplots_high,
                 {"Linear Acceleration (m^2/s)", "Orientation (rads)",
                  "Angular Velocity (rads/s)"},
                 {linear_acceleration_subplot_idx, orientation_subplot_idx,
                  angular_velocity_subplot_idx}) {}

void ImuPlotter::callback(const sensor_msgs::ImuConstPtr &msg) {
  const double t = msg->header.stamp.toSec();

  double roll, pitch, yaw;
  tf::Matrix3x3(tf::Quaternion(msg->orientation.x, msg->orientation.y,
                               msg->orientation.z, msg->orientation.w))
      .getRPY(roll, pitch, yaw);

  sub_plots_[LINEAR_ACCELERATION].addDataPoint(t, msg->linear_acceleration.x,
                                               msg->linear_acceleration.y,
                                               msg->linear_acceleration.z);
  sub_plots_[ORIENTATION].addDataPoint(t, roll, pitch, yaw);
  sub_plots_[ANGULAR_VELOCITY].addDataPoint(t, msg->angular_velocity.x,
                                            msg->angular_velocity.y,
                                            msg->angular_velocity.z);
}
