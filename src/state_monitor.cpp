#include "state_monitor/state_monitor.h"

StateMonitor::StateMonitor(const ros::NodeHandle& nh,
                           const ros::NodeHandle& nh_private) {
  nh_private_.param("plot_time_length_secs", plot_time_length_secs_, 10.0);

  draw_timer_ =
      nh_.createTimer(ros::Duration(0.1), &StateMonitor::drawCallback, this);

  node_search_timer_ = nh_.createTimer(ros::Duration(1.0),
                                       &StateMonitor::nodeSearchCallback, this);
}

void StateMonitor::createNodePlotterFromTopicInfo(
    const ros::master::TopicInfo& topic_info) {
  size_t match = topic_info.name.find("swf/local_odometry");
  if (match != std::string::npos) {
    const std::string topic_base = topic_info.name.substr(0, match);
    node_plotter_map_.emplace(std::make_pair(
        topic_base,
        std::make_shared<SWFPlotter>(topic_base, nh_, x11_window_.getMGLGraph(),
                                     plot_time_length_secs_)));
    return;
  }
}

void StateMonitor::nodeSearchCallback(const ros::TimerEvent& event) {
  // try to be fancy and find topic names automatically
  ros::master::V_TopicInfo topic_info_vector;
  ros::master::getTopics(topic_info_vector);

  for (const ros::master::TopicInfo& topic_info : topic_info_vector) {
    createNodePlotterFromTopicInfo(topic_info);
  }
}

// cycle state estimator in focus
void StateMonitor::cycleFocus(const int key) {
  static int prev_key = 0;
  if (key == prev_key) {
    prev_key = key;
    return;
  }
  prev_key = key;

  if (key == 111) {
    auto it = node_plotter_map_.find(node_in_focus_);
    ++it;
    if (it == node_plotter_map_.end()) {
      it = node_plotter_map_.begin();
    }
    node_in_focus_ = it->first;
  } else if (key == 116) {
    auto it = node_plotter_map_.find(node_in_focus_);
    if (it == node_plotter_map_.begin()) {
      it = node_plotter_map_.end();
    }
    --it;
    node_in_focus_ = it->first;
  }
}

void StateMonitor::printSidebar() {
  std::shared_ptr<mglGraph> gr = x11_window_.getMGLGraph();
  gr->SubPlot(4, 1, 0, "");
  gr->SetRanges(0, 1, 0, 1);
  mreal text_location = 1.05;
  gr->Puts(mglPoint(0, text_location), "\\b{State Monitor}", "w:L", 5);
  text_location -= 0.05;
  gr->Puts(mglPoint(0, text_location), "Monitored Nodes:", "w:L", 5);

  for (auto it = node_plotter_map_.begin(); it != node_plotter_map_.end();
       ++it) {
    std::string format;
    if (node_plotter_map_.find(node_in_focus_) == it) {
      format = "y:L";
    } else {
      format = "w:L";
    }

    text_location -= 0.05;
    gr->Puts(mglPoint(0, text_location), ("  \\b{" + it->first + "}").c_str(),
             format.c_str(), 5);
  }
}

void StateMonitor::drawCallback(const ros::TimerEvent& event) {
  x11_window_.resizeAndClear();  // note this command destroys any work done by
                                 // calls to plot()

  if (node_in_focus_.empty()) {
    if (node_plotter_map_.empty()) {
      return;
    } else {
      node_in_focus_ = node_plotter_map_.begin()->first;
    }
  }

  const int key = x11_window_.getKeypress();

  cycleFocus(key);

  node_plotter_map_[node_in_focus_]->plot();

  printSidebar();

  x11_window_.render();
}
