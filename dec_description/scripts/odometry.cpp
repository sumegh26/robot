#include "string"
#include "ros/ros.h" 
#include "sensor_msgs/JointState.h"
#include "tf/transform_broadcaster.h" 
#include "nav_msgs/Odometry.h" 

 int main(int argc, char** argv) {  

    ros::init(argc, argv, "state_publisher");  
 ros::NodeHandle n;
   ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom",   10);
   
 while (ros::ok()) { 
    current_time = ros::Time::now();    
      double dt = (current_time - last_time).toSec();  
   double delta_x = (vx * cos(th) - vy * sin(th)) * dt; 
    double delta_y = (vx * sin(th) + vy * cos(th)) * dt; 
    double delta_th = vth * dt;         
 x += delta_x;   
  y += delta_y;   
  th += delta_th;

 geometry_msgs::Quaternion odom_quat;  
 odom_quat = tf::createQuaternionMsgFromRollPitchYaw(0,0,th); 

 // update transform    
odom_trans.header.stamp = current_time;  
odom_trans.transform.translation.x = x;  
odom_trans.transform.translation.y = y;  
odom_trans.transform.translation.z = 0.0;     
odom_trans.transform.rotation =     
tf::createQuaternionMsgFromYaw(th);     

 // filling the odometry 

nav_msgs::Odometry odom;     
odom.header.stamp = current_time;     
odom.header.frame_id = "odom";     
odom.child_frame_id = "base_link";     
// position  
odom.pose.pose.position.x = x;  
odom.pose.pose.position.y = y;    
odom.pose.pose.position.z = 0.0;     
odom.pose.pose.orientation = odom_quat;
     
// velocity   
odom.twist.twist.linear.x = vx;    
odom.twist.twist.linear.y = vy;     
 
odom.twist.twist.angular.z = vth;      
last_time = current_time;      
// publishing the odometry and the new tf 
broadcaster.sendTransform(odom_trans);  
 odom_pub.publish(odom);     

 loop_rate.sleep(); 

  }   

return 0; 
} 
