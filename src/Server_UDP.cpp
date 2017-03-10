#include "ros/ros.h"
#include "std_msgs/String.h"

#include <geometry_msgs/Twist.h>

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


int main(int argc, char **argv)
{
	ros::init(argc, argv, "UDP_server");
	ros::NodeHandle n;
	ros::Publisher chatter_pub_robot1 = n.advertise<geometry_msgs::Twist>("/robot1/cmd_vel", 1000);
	ros::Publisher chatter_pub_robot2 = n.advertise<geometry_msgs::Twist>("/robot2/cmd_vel", 1000);
	ros::Publisher chatter_pub_robot3 = n.advertise<geometry_msgs::Twist>("/robot3/cmd_vel", 1000);
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
        	/*
        	if (sendto(socket_fd, &buf, nbytes, 0, (struct sockaddr*)&client_addr, length) < 0) 
		{
			perror("Could not send datagram!!\n");
        	        continue;
        	}
			printf("Can Strike Crtl-c to stop Server >>\n");
        	 */
			
		/*
		std_msgs::String msg;
		std::stringstream ss;
		ss << "hello world " << count;
		msg.data = ss.str();

		ROS_INFO("%s", msg.data.c_str());
		 */
	
		geometry_msgs::Twist msg;
		//split message into correspoinding parameter
		/*coordinate rotation
		 *omni	UAV
		 * x	 y
		 * y	 z
		 * z 	 x
		 */
		char *token = NULL, *rest_message = NULL;
		const char *delim = " ";
		msg.linear.y = atof( strtok_r( buf, delim, &rest_message ) );
		msg.linear.z = atof( strtok_r( NULL, delim, &rest_message ) );
		msg.linear.x = atof( strtok_r( NULL, delim, &rest_message ) );
		msg.angular.x = 0.0;
		msg.angular.y = 0.0;
		msg.angular.z = 0.0;
			
		chatter_pub_robot1.publish( msg );
		chatter_pub_robot2.publish( msg );
		chatter_pub_robot3.publish( msg );
	
		ros::spinOnce();
		loop_rate.sleep();
	    
	}
	
	return 0;
}

