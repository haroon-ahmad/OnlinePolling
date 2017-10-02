#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
struct sockaddr_in my_addr,client_addr;
int sd;
int new_sd;
int gon=0;//votes for go nawaz go
int goi=0;//votes for go imran go
sem_t w,r;// semaphores for read and write
int rc=0;//counter used in synchronization

//struct for parameters
typedef struct parameter
{
	int	nawaz;//votes of nawaz
	int 	imran;//votes of imran
	char	op[2];//option number
}a;
void* thread(void *parameters)//thread for writing
{
	
	a *data;
	data=(a*)parameters;
		if(data->op[0]=='a')//if user wants to vote 
		{	
		  	sem_wait(&w);//check if any other thread is writing
			if(data->op[1]=='a')
			{//inrement go nawaz go
				gon++;
			}
			else if(data->op[1]=='b')
			{
				goi++;//increment go imran go
			}
			sem_post(&w);//free a thread
		}	
		
	pthread_exit(0);//exit thread
}
void* print(void *parameters)//thread for printing results
{
	char nv[5];//array for nawaz votes, 5 is the max digits
	char iv[5];//array for imran votes
	char temp[5];//a temporary array
	int num;
	int num1;
	int l;
	int j;
//readers synchronization
	sem_wait(&r);//wait for readers
	++rc;//increment the counter
	if(rc==1)// if it is the first reader
	{
		sem_wait(&w);// stop the writing
	}
	sem_post(&r);//allow other readers
	num=gon;//reading
	//conversion from integer to char array	
	for(l=0;num>9;l++)
	{
		num1=num%10;	
		j = num1 + (int)'0'; 
		temp[l] = (char)j;
		num=num/10;
	}
	j = num + (int)'0'; 
	temp[l] = (char)j;
	temp[l+1]='\0';
	int t,q;
	int size=strlen(temp);
	for(t=size-1,q=0;t>0;t--,q++)
	{
		 nv[q]=temp[t];
	}
	nv[q]=temp[t];
	nv[q+1]='\0';
	send(new_sd,nv,5,0);//sending to client
	num=goi;//reading go imran go votes
//conversion of integer to char array
	for(l=0;num>9;l++)
	{
		num1=num%10;	
		j = num1 + (int)'0'; 
		temp[l] = (char)j;
		num=num/10;
			
	}
	j = num + (int)'0'; 
	temp[l] = (char)j;
	temp[l+1]='\0';
	size=strlen(temp);
	for(t=size-1,q=0;t>0;t--,q++)
	{
		iv[q]=temp[t];
	}
	iv[q]=temp[t];
	iv[q+1]='\0';
	sem_wait(&r);//wait for readers
	--rc;//decrementing number of readers
	if(rc==0)//if last reader leaving
	{
		sem_post(&w);//allow writing
	}
	sem_post(&r);//allow reading
	send(new_sd,iv,5,0);//send to client

}
int main()
{
//initalizaion of semaphores
	sem_init(&r,0,1);
	sem_init(&w,0,1);
	int portnum;
	printf("Enter socket number");
	scanf("%d",&portnum);
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(portnum);
	my_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	sd=socket(AF_INET,SOCK_STREAM,0);
	bind(sd,(struct sockaddr *) &my_addr,sizeof(struct sockaddr_in));
	listen(sd,5);
	int i=0;
	char option[2];
	pthread_t threads[20];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	a arr[20];
	int th;
	int j;
	while(1)
	{
		int size=sizeof(struct sockaddr);
		new_sd=accept(sd,(struct sockaddr *) &client_addr, &size);
		recv(new_sd,option,2,0);//recieve options
		if(option[0]=='b')// if user wants to see the results
		{
			arr[i].op[0]=option[0];
			arr[i].op[1]=option[1];
		
			arr[i].nawaz=gon;
			arr[i].imran=goi;
			th=pthread_create(&threads[i],&attr,&print, (void*) &arr[i]);
			if(th)	
			{
				printf("ERROR; pthread_create() return code id %d\n",th);

			}
			
		}
		else	//reader wants to vote
		{
			arr[i].op[0]=option[0];
			arr[i].op[1]=option[1];
		
			arr[i].nawaz=gon;
			arr[i].imran=goi;
			th=pthread_create(&threads[i],&attr,&thread, (void*) &arr[i]);
			if(th)	
			{
				printf("ERROR; pthread_create() return code id %d\n",th);

			}
		
		}		
		i++;
	}
	return 0;
}
