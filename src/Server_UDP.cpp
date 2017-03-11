#include "ros/ros.h"
#include "std_msgs/String.h"

#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

#include <sstream>

#include <unistd.h>
#include <netinet/in.h>	//for sockaddr_in
#include <arpa/inet.h>	//for socket

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <string.h>

//#define SERV_ADDR "192.168.205.128"
//#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 1234
#define MAXNAME 1024
extern int errno;

std::stringstream ss_state_robot1, ss_state_robot2, ss_state_robot3, ss_state_robot4;
std::string s_state_robot1, s_state_robot2, s_state_robot3, s_state_robot4;

void state_robot1Callback(const nav_msgs::Odometry::ConstPtr& receiveState)
{
  ss_state_robot1.str("");
  ss_state_robot1.clear();
  ss_state_robot1 << receiveState->pose.pose.position.x << " "
                  << receiveState->pose.pose.position.y << " "
                  << receiveState->pose.pose.position.z << " ";
  s_state_robot1 = ss_state_robot1.str();
}

void state_robot2Callback(const nav_msgs::Odometry::ConstPtr& receiveState)
{
  ss_state_robot2.str("");
  ss_state_robot2.clear();
  ss_state_robot2 << receiveState->pose.pose.position.x << " "
                  << receiveState->pose.pose.position.y << " "
                  << receiveState->pose.pose.position.z << " ";
  s_state_robot2 = ss_state_robot2.str();
}

void state_robot3Callback(const nav_msgs::Odometry::ConstPtr& receiveState)
{
  ss_state_robot3.str("");
  ss_state_robot3.clear();
  ss_state_robot3 << receiveState->pose.pose.position.x << " "
                  << receiveState->pose.pose.position.y << " "
                  << receiveState->pose.pose.position.z << " ";
  s_state_robot3 = ss_state_robot3.str();
}

void state_robot4Callback(const nav_msgs::Odometry::ConstPtr& receiveState)
{
  ss_state_robot4.str("");
  ss_state_robot4.clear();
  ss_state_robot4 << receiveState->pose.pose.position.x << " "
                  << receiveState->pose.pose.position.y << " "
                  << receiveState->pose.pose.position.z;
  s_state_robot4 = ss_state_robot4.str();
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "UDP_server");
	ros::NodeHandle n;
  ros::Publisher pub_cmd_vel_robot1 = n.advertise<geometry_msgs::Twist>("/robot1/cmd_vel", 1000);
  ros::Publisher pub_cmd_vel_robot2 = n.advertise<geometry_msgs::Twist>("/robot2/cmd_vel", 1000);
  ros::Publisher pub_cmd_vel_robot3 = n.advertise<geometry_msgs::Twist>("/robot3/cmd_vel", 1000);
  ros::Publisher pub_cmd_vel_robot4 = n.advertise<geometry_msgs::Twist>("/robot4/cmd_vel", 1000);

  ros::Subscriber sub_state_robot1 = n.subscribe("/robot1/ground_truth/state", 1000, state_robot1Callback);
  ros::Subscriber sub_state_robot2 = n.subscribe("/robot2/ground_truth/state", 1000, state_robot2Callback);
  ros::Subscriber sub_state_robot3 = n.subscribe("/robot3/ground_truth/state", 1000, state_robot3Callback);
  ros::Subscriber sub_state_robot4 = n.subscribe("/robot4/ground_truth/state", 1000, state_robot4Callback);



	ros::Rate loop_rate(10);
	int socket_fd;   /* file description into transport */
	int recfd; /* file descriptor to accept        */
	int length; /* length of address structure      */
	int nbytes; /* the number of read **/
  //char buf[BUFSIZ];
  struct sockaddr_in myaddr; /* address of this service */
  struct sockaddr_in client_addr; /* address of client    */
  /*
   *      Get a socket into UDP/IP
   */
  if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) <0)
  {
    perror ("socket failed");
    exit(EXIT_FAILURE);
  }
  /*
   *    Set up our address
   */
  bzero ((char *)&myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(SERV_PORT);

  /*
   *     Bind to the address to which the service will be offered
   */
  if (bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) <0)
	{
		perror ("bind failed\n");
    exit(1);
  }

        
		
	while (ros::ok())
	{
		//socket setting
		/*
		 * Loop continuously, waiting for datagrams
         	 * and response a message
           */
    length = sizeof(client_addr);
    printf("Server is ready to receive !!\n");
    char buf[BUFSIZ];	//receive from client
    //printf("Can strike Cntrl-c to stop Server >>\n");
		if ((nbytes = recvfrom(socket_fd, &buf, MAXNAME, 0, (struct sockaddr*)&client_addr, (socklen_t *)&length)) <0) 
    {
      perror ("could not read datagram!!");
      continue;
    }


		printf("Received data form %s : %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
		printf("%s\n", buf);

    /* return to client */
    std::string pos_feedback = s_state_robot1 + s_state_robot2 + s_state_robot3 + s_state_robot4;
    char* send_buf = pos_feedback.c_str();

    if (sendto(socket_fd, &send_buf, MAXNAME, 0, (struct sockaddr*)&client_addr, length) < 0)
    {
      perror("Could not send datagram!!\n");
      continue;
    }
    //printf("Can Strike Crtl-c to stop Server >>\n");

		/*
		std_msgs::String msg;
		std::stringstream ss;
		ss << "hello world " << count;
		msg.data = ss.str();

		ROS_INFO("%s", msg.data.c_str());
		 */
	
		geometry_msgs::Twist cmd_vel_robot1;
		geometry_msgs::Twist cmd_vel_robot2;
		geometry_msgs::Twist cmd_vel_robot3;
		geometry_msgs::Twist cmd_vel_robot4;
		//split message into correspoinding parameter
    /*coordinate rotation
		 *omni	UAV
		 * x	 y
		 * y	 z
		 * z 	 x
		 */
		char *token = NULL, *rest_message = NULL;
		const char *delim = " ";

    cmd_vel_robot1.linear.x = atof( strtok_r( buf, delim, &rest_message ) );
    cmd_vel_robot1.linear.y = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot1.linear.z = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot1.angular.x = 0.0;
    cmd_vel_robot1.angular.y = 0.0;
    cmd_vel_robot1.angular.z = 0.0;

    cmd_vel_robot2.linear.x = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot2.linear.y = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot2.linear.z = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot2.angular.x = 0.0;
    cmd_vel_robot2.angular.y = 0.0;
    cmd_vel_robot2.angular.z = 0.0;

    cmd_vel_robot3.linear.x = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot3.linear.y = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot3.linear.z = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot3.angular.x = 0.0;
    cmd_vel_robot3.angular.y = 0.0;
    cmd_vel_robot3.angular.z = 0.0;

    cmd_vel_robot4.linear.x = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot4.linear.y = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot4.linear.z = atof( strtok_r( NULL, delim, &rest_message ) );
    cmd_vel_robot4.angular.x = 0.0;
    cmd_vel_robot4.angular.y = 0.0;
    cmd_vel_robot4.angular.z = 0.0;

    //publish command to robot respectively
    pub_cmd_vel_robot1.publish( cmd_vel_robot1 );
    pub_cmd_vel_robot2.publish( cmd_vel_robot2 );
    pub_cmd_vel_robot3.publish( cmd_vel_robot3 );
    pub_cmd_vel_robot4.publish( cmd_vel_robot4 );
	
		ros::spinOnce();
		loop_rate.sleep();
	    
	}
	
	return 0;
}

