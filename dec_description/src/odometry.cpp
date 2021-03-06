#include <iostream>
#include "string"
#include "ros/ros.h" 
#include "sensor_msgs/JointState.h"
#include "tf/transform_broadcaster.h" 
#include "nav_msgs/Odometry.h"
#include "dec_description/odom_data.h"
#include <ros/console.h>

dec_description::odom_data store;
double vx,vy,vth;
ros::Time current_time;
ros::Time last_time;

void odomcallback(const dec_description::odom_data::ConstPtr& odom_vl)
{
  store.vx = odom_vl->vx;
  store.vy = odom_vl->vy;
  store.vth = odom_vl->vth;
  store.header = odom_vl->header;

  vx = store.vx;
  vy = store.vy;
  vth = store.vth;
  //ROS_INFO("data is:",odom_vl->vx);
  
	current_time = store.header.stamp;
    std::cout<<current_time<<std::endl;
    last_time = store.header.stamp;

}


int main(int argc, char** argv) 
{  

    ros::init(argc, argv, "odom_node");  
    ros::NodeHandle n;
    ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom",10);
    ros::Subscriber sub = n.subscribe("/vl", 1, odomcallback);


	// initial position
	double x = 0.0; 
	double y = 0.0;
	double th = 0;

	// velocity
	// double vx = (double)store.vx;
	// double vy = (double)store.vy;
	// double vth = (double)store.vth;

    

   	// ros::Time current_time;
	// ros::Time last_time;


    tf::TransformBroadcaster broadcaster;
	ros::Rate loop_rate(2560);
    geometry_msgs::TransformStamped odom_trans;
    
        while (ros::ok()) 
        { 
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
            odom_trans.transform.rotation =tf::createQuaternionMsgFromYaw(th);     

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

            ros::spinOnce();
            loop_rate.sleep(); 

        }   


return 0; 
} 