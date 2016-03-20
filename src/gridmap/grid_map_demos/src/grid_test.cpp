#include <ros/ros.h>
#include <grid_map_ros/grid_map_ros.hpp>
#include <grid_map_msgs/GridMap.h>
#include <vector>
#include <string>
#include <cmath>
#include <limits>

using namespace grid_map;

int main(int argc, char** argv)
{
  // Initialize node and publisher.
  ros::init(argc, argv, "grid_test");
  ros::NodeHandle nh("~");
  ros::Publisher publisher = nh.advertise<grid_map_msgs::GridMap>("grid_map", 1, true);

  // Create grid map.
  GridMap map({"density"});
  map.setFrameId("map");
  map.setGeometry(Length(21.0, 21.0), 0.333);
  ROS_INFO("Created map with size %f x %f m (%i x %i cells).",
  map.getLength().x(), map.getLength().y(),
  map.getSize()(0), map.getSize()(1));

  //loops map
  ros::Rate rate(30.0);

  bool getlow;


  while (nh.ok()) {
    ros::Time time = ros::Time::now();

    // testing updates

    if ( abs(time.toSec()) % 2 == 0){
      getlow = true;
    } else {
      getlow = false;
    }

    // testing layer

    if (getlow){
      map["density"].setConstant(.9);
    } else {


      // Add elevation and surface normal (iterating through grid map and adding data).
      for (GridMapIterator it(map); !it.isPastEnd(); ++it) {
        Position position;
        map.getPosition(*it, position);
        map.at("density", *it) = -0.04 + 0.2 * std::sin(3.0 * time.toSec() + 5.0 * position.y()) * position.x();
      }
    }

    // Publish grid map.
    map.setTimestamp(time.toNSec());
    grid_map_msgs::GridMap message;
    GridMapRosConverter::toMessage(map, message);
    publisher.publish(message);
    ROS_INFO_THROTTLE(1.0, "Grid map (timestamp %f) published.", message.info.header.stamp.toSec());

    rate.sleep();

  }

  return 0;
}
