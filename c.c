#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main()
{		
	int portnum;//portnumber
	char option[2];//array for options, option[0] is for the main menu and the second is for the vote menu
	printf("Enter socket number\n");
	scanf("%d",&portnum);
	int sd;
//sockets
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(portnum);//I have used the portnum 4999
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");//ip adress for internal sockets 
	sd=socket(AF_INET,SOCK_STREAM,0);
	connect(sd,(struct sockaddr *) &server_addr,sizeof(struct sockaddr_in));  
	printf("Choose from the following options.\na.Enter vote \nb.See the results\n\n\n");
	scanf("%s",&option[0]);
	
	if(option[0]=='a')//if user chooses to vote
	{
		printf("a.Go Nawaz Go \nb.Go Imran Go\n");
		scanf("%s",&option[1]);
		send(sd,option,2,0);//send option to server
	}
	else if(option[0]=='b')//if user chooses to see the results
	{
		send(sd,option,2,0);//send option to server
		char nv[5];
		char iv[5];
		recv(sd,nv,5,0);//recieve results from server
		recv(sd,iv,5,0);
		printf("Results:\nGo Nawaz Go:%s\n",nv);
		printf("Go Imran Go:%s\n",iv);
	}
	return 0;
	
}

