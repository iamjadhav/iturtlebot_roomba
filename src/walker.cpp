/**
* MIT License
*
* Copyright (c) Aditya Jadhav
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

/**
 * @file walker.cpp
 * @author Aditya Jadhav (amjadhav@umd.edu)
 * @brief ROS Walker Class Definition
 * @version 0.1
 * @date 2021-11-29
 *
 * @copyright Copyright (c) 2021
 */

#include <iostream>
#include "../include/walker.hpp"

/**
 * @brief Walk constructor to set turtlebot velocities,
 *        Publish velocities and Subscribe to scan topic
 *        and pulish velocities to the turtlebot
 */
Walk::Walk() {
  actualVelocity = n.advertise <geometry_msgs::Twist>("/cmd_vel", 1000);
  laser_points = n.subscribe<sensor_msgs::LaserScan> ("/scan", 1000, &Walk:: \
                                                      getLaser, this);
  actualVelocity.publish(message);

  linearVelocity = 0.3;
  angularVelocity = 0.5;
  obstacle_flag = false;
}

/**
 * @brief Method to determine presence of obstacles using laser scan data
 * @return returns if an obstacle is detected
 */
void Walk::getLaser(const sensor_msgs::LaserScan::ConstPtr& detect) {
  int i = 0;
  while (i < detect->ranges.size()) {
    // Detecting obstacles at -35, 0 and 35 degrees
    if (detect->ranges[0] <= 0.6 || detect->ranges[35] <= 0.6 ||
                                    detect->ranges[325] <= 0.6) {
      obstacle_flag = true;
      return;
    }
    i++;
  }
  obstacle_flag = false;
}

void Walk::go() {
  ros::Rate loop(10);
  while (ros::ok()) {
    if (obstacle_flag == true) {
      // Turn Anti-clockwise if object detected
      ROS_WARN_STREAM(" Uh Oh! Obstacle Detected! Rerouting ..");
      message.linear.x = 0.0;
      message.angular.z = angularVelocity;
      loop.sleep();
    } else {
      // Staright movement with linear velocity
      ROS_INFO_STREAM(" Moving along..");
      message.angular.z = 0.0;
      message.linear.x = linearVelocity;
    }
    actualVelocity.publish(message);
    ros::spinOnce();
    loop.sleep();
  }
}

/**
 * @brief Destroy the Walk object
 */
Walk::~Walk() {
  ROS_INFO("Walker walking no more..");
}
