#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>	/* needed for os x*/
#include <string.h>	/* for strlen */
#include <netdb.h>      /* for gethostbyname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>   /* defines ERESTART, EINTR */
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef ERESTART
#define ERESTART EINTR
#endif

extern int errno;

int conn(char *host, int port);	/* connect to host,port; return socket */
void disconn(int fd);	/* close a socket connection */
int debug = 0,type=1;
void serve(int port);	/* main server function */

typedef struct store
{
	char size[100];
	char time[100];
	char file[100];
	char hash[100];
}store;

void calculate();
void calculate1();

store arr[1000];

store my[1000];

store head[1000];

store foot[1000];

int server_size=0;

int h_it=0;
int f_it=0;


struct sockaddr_in servaddr;	/* server address 556 */
socklen_t slen = sizeof(servaddr);

int main(int argc, char **argv)
{
	//UDP
	printf("Enter 1 for tcp OR 0 for udp\n");
	scanf("%d",&type);


	printf("Mention your port no: \n");
	int port_server;
	scanf("%d",&port_server);
	pid_t pid;
	pid=fork();
	if(pid==0)
	{
		serve(port_server);
	}
	else
	{
		char host[20]="localhost";

		printf("Mention the port no you want to connect to : \n");
		int port;
		scanf("%d",&port);
		int fd;

		if((fd = conn(host, port)) < 0)    
			exit(1); 
		char buffer[256];
		int n;
		int i=0;
		char c=2;
		scanf("%c",&c);
		c='[';
		while(1){
			c='[';
			bzero(buffer,256);
			while(c!='\n')
			{
				scanf("%c",&c);
				if((c==' ' || c=='\t') && i==0)
					continue;
				if((c==' ' || c=='\t') && buffer[i-1]==' ')
					continue;
				else
					buffer[i]=c;
				i++;
			}
			buffer[i-1]='\0';
			i=0;
			c='[';
			if(buffer[0]=='\n' || buffer[0]=='\0')
				continue;
			printf("Command is %s\n",buffer);

			if(type==1)
				n = send(fd,buffer,256,0);
			else if(type==0)
				n=sendto(fd, buffer, 256, 0,(struct sockaddr *)&servaddr, slen);


			if(strncmp(buffer,"UPLOAD",6)==0)
			{
				int k;
				char tmp[100];
				for(k=7;buffer[k]!='\0';k++)
					tmp[k-7]=buffer[k];
				tmp[k-7]=buffer[k];
				//printf("HELLO\n");
				//printf("tmp is %s \n",tmp);
				if(strcmp(tmp,"Deny")==0)
				{
					printf("Denied\n");
				}
				else if(strcmp(tmp,"Allow")!=0)
				{
					FILE* n11=fopen("file_ka_naam","w");
					fprintf(n11,"%s",tmp);
					fclose(n11);
				}
				else if(strcmp(tmp,"Allow")==0)
				{
					char size[100];
					char name[100];
					char hash[100];
					char time[100];


					// open a file named filekanaam1 and write in
					char f_t[50];
					FILE *n22=fopen("filekanaam1","r");
					fscanf(n22,"%s",f_t);
					fclose(n22);

					FILE *fpp=fopen(f_t,"w");
					fclose(fpp);
					int fp1=open(f_t,O_WRONLY,"w");
					char recv_buffer[1000];
					bzero(recv_buffer,1000);
					while(1)
					{
						if(type==1)
						{
							if(recv(fd,recv_buffer,1000,0)>=0)
							{}
							else
								break;
						}
						else if(type==0)
						{
							if(recvfrom(fd,recv_buffer,1000,0,(struct sockaddr *)&servaddr, &slen)>=0)
							{}
							else
								break;
						}

						//printf("nn: %d s:%s\n",nn,recv_buffer);
						if(recv_buffer[810]=='$')
							write(fp1,recv_buffer,800);
						else
						{
							int first=recv_buffer[815]-48;
							int second=recv_buffer[816]-48;
							int third=recv_buffer[817]-48;

							int temp=third+(second*10)+(first*100);
							write(fp1,recv_buffer,temp);

							int x1;
							for(x1=820;recv_buffer[x1]!='$';x1++)
							{
								name[x1-820]=recv_buffer[x1];
							}
							name[x1-820]='\0';

							for(x1=850;recv_buffer[x1]!='$';x1++)
							{
								hash[x1-850]=recv_buffer[x1];
							}
							hash[x1-850]='\0';

							for(x1=900;recv_buffer[x1]!='$';x1++)
							{
								size[x1-900]=recv_buffer[x1];
							}
							size[x1-900]='\0';

							for(x1=950;recv_buffer[x1]!='$';x1++)
							{
								time[x1-950]=recv_buffer[x1];
							}
							time[x1-950]='\0';

							char new_hash[100];
							pid_t pid1=fork();
							if(pid1==0)
							{
								FILE *fpf;//reading file information from stdout and storing it in a file_inf file
								fpf=freopen("./file_inf_hash","w",stdout);
								char t1[100],t2[100];
								strcpy(t1,"md5sum ");
								strcat(t1,name);
								system(t1);
								fclose(fpf);
								_exit(0);
							}
							else
							{	
								wait(NULL);
								FILE* n1=fopen("file_inf_hash","r");
								fscanf(n1,"%s",new_hash);
								fclose(n1);
								printf("new file hash is %s\n",new_hash);
								system("rm file_inf_hash");
							}

							if(strcmp(new_hash,hash)!=0)
							{
								printf("File Corrupted in between,Download Again\n");
							}
							else
							{
								printf("File Correctly Received\n");
							}

							break;
						}
						bzero(recv_buffer,1000);
					}
					printf("File Downloaded with name: %s\n time: %s\n size: %s\n hash: %s\n",name,time,size,hash);
					close(fp1);
					system("rm filekanaam1");

				}
			}

			else if(strncmp(buffer,"DOWNLOAD",8)==0)
			{
				char size[100];
				char name[100];
				char hash[100];
				char time[100];

				int k;
				char tmp[100];
				for(k=9;buffer[k]!='\0';k++)
					tmp[k-9]=buffer[k];
				tmp[k-9]=buffer[k];

				FILE *fpp = fopen(tmp,"w");
				fclose(fpp);
				int fp1=open(tmp,O_WRONLY,"w");
				char recv_buffer[1000];
				bzero(recv_buffer,1000);
				while(1)
				{
					if(type==1)
					{
						if(recv(fd,recv_buffer,1000,0)>=0)
						{}
						else
							break;
					}
					else if(type==0)
					{
						if(recvfrom(fd,recv_buffer,1000,0,(struct sockaddr *)&servaddr, &slen)>=0)
						{}
						else
							break;
					}

					//printf("nn: %d s:%s\n",nn,recv_buffer);
					if(recv_buffer[810]=='$')
						write(fp1,recv_buffer,800);
					else
					{
						int first=recv_buffer[815]-48;
						int second=recv_buffer[816]-48;
						int third=recv_buffer[817]-48;

						int temp=third+(second*10)+(first*100);
						write(fp1,recv_buffer,temp);

						int x1;
						for(x1=820;recv_buffer[x1]!='$';x1++)
						{
							name[x1-820]=recv_buffer[x1];
						}
						name[x1-820]='\0';

						for(x1=850;recv_buffer[x1]!='$';x1++)
						{
							hash[x1-850]=recv_buffer[x1];
						}
						hash[x1-850]='\0';

						for(x1=900;recv_buffer[x1]!='$';x1++)
						{
							size[x1-900]=recv_buffer[x1];
						}
						size[x1-900]='\0';

						for(x1=950;recv_buffer[x1]!='$';x1++)
						{
							time[x1-950]=recv_buffer[x1];
						}
						time[x1-950]='\0';

						char new_hash[100];
						pid_t pid1=fork();
						if(pid1==0)
						{
							FILE *fpf;//reading file information from stdout and storing it in a file_inf file
							fpf=freopen("./file_inf_hash","w",stdout);
							char t1[100],t2[100];
							strcpy(t1,"md5sum ");
							strcat(t1,name);
							system(t1);
							fclose(fpf);
							_exit(0);
						}
						else
						{	
							wait(NULL);
							FILE* n1=fopen("file_inf_hash","r");
							fscanf(n1,"%s",new_hash);
							fclose(n1);
							printf("new file hash is %s\n",new_hash);
							system("rm file_inf_hash");
						}

						if(strcmp(new_hash,hash)!=0)
						{
							printf("File Corrupted in between,Download Again\n");
						}
						else
						{
							printf("File Correctly Received\n");
						}

						break;
					}
					bzero(recv_buffer,1000);
				}
				printf("File Downloaded with name: %s\n time: %s\n size: %s\n hash: %s\n",name,time,size,hash);
				close(fp1);

			}
			else if(strncmp(buffer,"IndexGet",8)==0)
			{
				int k;
				char tmp[100];
				for(k=9;buffer[k]!='\0';k++)
					tmp[k-9]=buffer[k];
				tmp[k-9]=buffer[k];
				if(strcmp("LongList",tmp)==0)
				{
					char recv_buffer[10];
					bzero(recv_buffer,10);
					while(1)
					{
						if(type==1)
						{
							if(recv(fd,recv_buffer,10,0)<0)
								break;
						}
						else
						{
							if(recvfrom(fd,recv_buffer,10,0,(struct sockaddr *)&servaddr, &slen)<0)
								break;
						}

						if(recv_buffer[8]=='$')
						{
							int i;
							for(i=0;i<5;i++)
								printf("%c",recv_buffer[i]);
						}
						else
						{	
							int temp=recv_buffer[8]-48;
							int i;
							for(i=0;i<temp;i++)
								printf("%c",recv_buffer[i]);
							//write(fp1,recv_buffer,temp);
							break;
						}
						bzero(recv_buffer,10);
					}
					printf("File list Downloaded Successfully\n");
				}
				else if(strncmp("RegEx",tmp,5)==0)
				{
					char recv_buffer[10];
					bzero(recv_buffer,10);
					while(1)
					{
						if(type==1)
						{
							if(recv(fd,recv_buffer,10,0)<0)
								break;
						}
						else
						{
							if(recvfrom(fd,recv_buffer,10,0,(struct sockaddr *)&servaddr, &slen)<0)
								break;
						}
						if(recv_buffer[8]=='$')
						{
							int i;
							for(i=0;i<5;i++)
								printf("%c",recv_buffer[i]);
						}
						else
						{	
							int temp=recv_buffer[8]-48;
							int i;
							for(i=0;i<temp;i++)
								printf("%c",recv_buffer[i]);
							//write(fp1,recv_buffer,temp);
							break;
						}
						bzero(recv_buffer,10);
					}
					printf("RegEx Downloaded Successfully\n");
				}
				else if(strncmp(tmp,"ShortList",9)==0)
				{
					char recv_buffer[10];
					bzero(recv_buffer,10);
					while(1)
					{
						if(type==1)
						{
							if(recv(fd,recv_buffer,10,0)<0)
								break;
						}
						else
						{
							if(recvfrom(fd,recv_buffer,10,0,(struct sockaddr *)&servaddr, &slen)<0)
								break;
						}
						if(recv_buffer[8]=='$')
						{
							int i;
							for(i=0;i<5;i++)
								printf("%c",recv_buffer[i]);
						}
						else
						{	
							int temp=recv_buffer[8]-48;
							int i;
							for(i=0;i<temp;i++)
								printf("%c",recv_buffer[i]);
							//write(fp1,recv_buffer,temp);
							break;
						}
						bzero(recv_buffer,10);
					}
					printf("File list Downloaded Successfully\n");

				}
			}
			else if(strncmp(buffer,"FileHash Verify",15)==0)
			{
				char f_tmp[100];
				int k_it;
				for(k_it=16;buffer[k_it]!='\0';k_it++)
					f_tmp[k_it-16]=buffer[k_it];
				f_tmp[k_it-16]=buffer[k_it];

				char recv_buffer[10];
				bzero(recv_buffer,10);
				while(1)
				{
					if(type==1)
					{
						if(recv(fd,recv_buffer,10,0)<0)
							break;
					}
					else
					{
						if(recvfrom(fd,recv_buffer,10,0,(struct sockaddr *)&servaddr, &slen)<0)
							break;
					}
					if(recv_buffer[8]=='$')
					{
						int i;
						for(i=0;i<5;i++)
							printf("%c",recv_buffer[i]);
					}
					else
					{	
						int temp=recv_buffer[8]-48;
						int i;
						for(i=0;i<temp;i++)
							printf("%c",recv_buffer[i]);
						//write(fp1,recv_buffer,temp);
						break;
					}
					bzero(recv_buffer,10);
				}
				printf("Hash list Downloaded Successfully\n");

			}
			else if(strcmp(buffer,"FileHash Check All")==0)
			{
				/* receive ls2 of that server */


				char recv_buffer[10];
				bzero(recv_buffer,10);
				while(1)
				{
					if(type==1)
					{
						if(recv(fd,recv_buffer,10,0)<0)
							break;
					}
					else
					{
						if(recvfrom(fd,recv_buffer,10,0,(struct sockaddr *)&servaddr, &slen)<0)
							break;
					}
					if(recv_buffer[8]=='$')
					{
						int i;
						for(i=0;i<5;i++)
							printf("%c",recv_buffer[i]);
					}
					else
					{	
						int temp=recv_buffer[8]-48;
						int i;
						for(i=0;i<temp;i++)
							printf("%c",recv_buffer[i]);
						//write(fp1,recv_buffer,temp);
						break;
					}
					bzero(recv_buffer,10);
				}
				printf("Hash list Downloaded Successfully\n");
				/* receive ls2 of that server */
			}

			else
			{
				if (n < 0) 
					error("ERROR writing to socket");
				bzero(buffer,256);
				if(type==1)
					n = recv(fd,buffer,256,0);
				else
					n=recvfrom(fd,buffer,256,0,(struct sockaddr *)&servaddr, &slen);
				if (n < 0) 
					error("ERROR reading from socket");
				printf("%s\n",buffer);
			}
		}
		//	int nbytes = write(fd, buffer, 10);
		disconn(fd);    /* disconnect */
	}
	return 0;
}


int conn(char *host, int port)
{
	struct hostent *hp;	/* host information */
	unsigned int alen;	/* address length when we get the port number */
	struct sockaddr_in myaddr;	/* our address */

	int fd;  /* fd is the file descriptor for the connected socket */

	//	if (debug) printf("conn(host=\"%s\", port=\"%d\")\n", host, port);

	/* get a tcp/ip socket */
	/* We do this as we did it for the server */
	/* request the Internet address protocol */
	/* and a reliable 2-way byte stream */
	if(type==1){
		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("cannot create socket");
			return -1;
		}}
	else
	{
		if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket");
			return -1;
		}}
	/* bind to an arbitrary return address */
	/* because this is the client side, we don't care about the */
	/* address since no application will connect here  --- */
	/* INADDR_ANY is the IP address and 0 is the socket */
	/* htonl converts a long integer (e.g. address) to a network */
	/* representation (agreed-upon byte ordering */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		close(fd);
		return -1;
	}

	/* this part is for debugging only - get the port # that the operating */
	/* system allocated for us. */
	alen = sizeof(myaddr);
	if (getsockname(fd, (struct sockaddr *)&myaddr, &alen) < 0) {
		perror("getsockname failed");
		close(fd);
		return -1;
	}
	//	if (debug) printf("local port number = %d\n", ntohs(myaddr.sin_port));

	/* fill in the server's address and data */
	/* htons() converts a short integer to a network representation */

	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	/* look up the address of the server given its name */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", host);
		close(fd);
		return -1;
	}

	/* put the host's address into the server address structure */
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	/* connect to server */
	if(type==1){
		if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
			perror("connect failed");
			close(fd);
			return -1;
		}}
	//	if (debug) printf("connected socket = %d\n", fd);
	return fd;
}

/* disconnect from the service */
/* lame: we can just as easily do a shutdown() or close() ourselves */

	void
disconn(int fd)
{
	//	if (debug) printf("disconn(%d)\n", fd);
	//	shutdown(fd, 2);    /* 2 means future sends & receives are disallowed */
}

void serve(int port)
{
	int svc;        /* listening socket providing service */
	int rqst;       /* socket accepting the request */
	socklen_t alen;       /* length of address structure */
	struct sockaddr_in my_addr;    /* address of this service */
	struct sockaddr_in client_addr;  /* client's address  copied to global 649 */
	int sockoptval = 1;
	char hostname[128]; /* host name, for debugging */

	gethostname(hostname, 128);

	/* get a tcp/ip socket */
	/*   AF_INET is the Internet address (protocol) family  */
	/*   with SOCK_STREAM we ask for a sequenced, reliable, two-way */
	/*   conenction based on byte streams.  With IP, this means that */
	/*   TCP will be used */

	if(type==1){
		if ((svc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("cannot create socket");
			exit(1);
		}
	}
	else if(type==0){
		if ((svc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket");
			exit(1);
		}
	}

	/* we use setsockopt to set SO_REUSEADDR. This allows us */
	/* to reuse the port immediately as soon as the service exits. */
	/* Some operating systems will not allow immediate reuse */
	/* on the chance that some packets may still be en route */
	/* to the port. */

	setsockopt(svc, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* set up our address */
	/* htons converts a short integer into the network representation */
	/* htonl converts a long integer into the network representation */
	/* INADDR_ANY is the special IP address 0.0.0.0 which binds the */
	/* transport endpoint to all IP addresses on the machine. */

	memset((char*)&my_addr, 0, sizeof(my_addr));  /* 0 out the structure */
	my_addr.sin_family = AF_INET;   /* address family */
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		perror("bind failed");
		exit(1);
	}

	if(type==1){
		if (listen(svc, 5) < 0) {
			perror("listen failed");
			exit(1);
		}
	}

	//	printf("server started on %s, listening on port %d\n", hostname, port);

	alen = sizeof(client_addr);   


	char buffer[256];
	int n,nn;
	for (;;) {
		if(type==1)
			while ((rqst = accept(svc,(struct sockaddr *)&client_addr, &alen)) < 0) {
			}

		while(1)
		{
			bzero(buffer,256);
			if(type==1)
				n = recv(rqst,buffer,256,0);
			else
			{
				if((n = recvfrom(svc,buffer,256,0,(struct sockaddr *)&client_addr, &alen))==-1)
				{
					printf("Connection failed");
					exit(0);
				}
			}
			if(strncmp(buffer,"UPLOAD",6)==0)
			{	
				int k;
				char tmp[100];
				for(k=7;buffer[k]!='\0';k++)
					tmp[k-7]=buffer[k];
				tmp[k-7]=buffer[k];

				//printf("UPLOAD SE RECVD %s\n",tmp);

				if(strcmp(tmp,"Deny")==0)
					printf("Denied\n");
				else if(strcmp(tmp,"Allow")!=0)
				{
					printf("UPLOAD Allow/Deny\n");
					FILE* fpp=fopen("filekanaam1","w");
					fprintf(fpp,"%s",tmp);
					fclose(fpp);
				}
				else if(strcmp(tmp,"Allow")==0)
				{
					// read from file ka naam n send
					char f_t[50];
					FILE* fpp=fopen("file_ka_naam","r");
					fscanf(fpp,"%s",f_t);
					fclose(fpp);



					char name[100];
					char time[100];
					char size[100];
					char hash[100];


					char send_buffer[1000];
					int k=0,x,nn=0;

					int fp=open(f_t,O_RDONLY,"r");

					while((x=read(fp,send_buffer,800))>0)	
					{
						if(x==800)
						{
							send_buffer[810]='$';
							send_buffer[815]=8+'0';
							send_buffer[816]=0+'0';
							send_buffer[817]=0+'0';

						}
						else
						{
							int first=x/100,second=(x/10)%10,third=x%10;
							send_buffer[810]='#';
							send_buffer[815]=first+'0';
							send_buffer[816]=second+'0';
							send_buffer[817]=third+'0';

							int it;
							for(it=0;f_t[it]!='\0';it++)
							{
								send_buffer[820+it]=f_t[it];
								name[it]=f_t[it];//send_buffer[820+it_12];
							}

							send_buffer[it+820]='$';
							name[it]='\0';
							printf("name is %s\n",name);

							h_it=0;
							calculate();

							int j_it;
							for(j_it=0;j_it<h_it;j_it++)
							{
								if(strcmp(head[j_it].file,f_t)==0)
								{
									char hash_t[100];
									strcpy(hash_t,head[j_it].hash);
									printf("hash is %s\n",hash_t);

									for(it=0;it<32;it++)
									{
										send_buffer[850+it]=hash_t[it];
										hash[it]=hash_t[it];
									}
									send_buffer[850+it]='$';
									hash[it]='\0';

									strcpy(hash_t,head[j_it].size);
									printf("hash is %s\n",hash_t);

									for(it=0;hash_t[it]!='\0';it++)
									{
										send_buffer[900+it]=hash_t[it];
										size[it]=hash_t[it];
									}
									send_buffer[900+it]='$';
									size[it]='\0';

									strcpy(hash_t,head[j_it].time);
									printf("hash is %s\n",hash_t);

									for(it=0;hash_t[it]!='\0';it++)
									{
										send_buffer[950+it]=hash_t[it];
										time[it]=hash_t[it];
									}
									send_buffer[950+it]='$';
									time[it]='\0';


									//break;
								}
							}

						}	

						//printf("x:%d s:%s\n",x,send_buffer);
						if(type==1)
							nn=send(rqst,send_buffer,1000,0);
						else
							nn=sendto(svc,send_buffer,1000, 0,(struct sockaddr *)&client_addr, alen);
						//printf("Sent from client : %d\n",nn);
						//bzero(send_buffer,10);
					}
					bzero(send_buffer,1000);
					bzero(buffer,256);

					close(fp);


					printf("Phase 4\n");
					system("rm file_ka_naam");
				}

			}
			else if(strncmp(buffer,"DOWNLOAD",8)==0)
			{
				char name[100];
				char time[100];
				char size[100];
				char hash[100];

				printf("Download-Detected\n");
				//printf("%s&&\n",buffer);
				char send_buffer[1000];
				int k=0,x,nn=0;
				char tmp[100];
				for(k=9;buffer[k]!='\0';k++)
					tmp[k-9]=buffer[k];
				tmp[k-9]=buffer[k];
				//printf("%s**\n",tmp);
				int fp=open(tmp,O_RDONLY,"r");
				//bzero(send_buffer,10);
				while((x=read(fp,send_buffer,800))>0)	
				{
					if(x==800)
					{
						send_buffer[810]='$';
						send_buffer[815]=8+'0';
						send_buffer[816]=0+'0';
						send_buffer[817]=0+'0';

					}
					else
					{
						int first=x/100,second=(x/10)%10,third=x%10;
						send_buffer[810]='#';
						send_buffer[815]=first+'0';
						send_buffer[816]=second+'0';
						send_buffer[817]=third+'0';

						int it;
						for(it=0;tmp[it]!='\0';it++)
						{
							send_buffer[820+it]=tmp[it];
							name[it]=tmp[it];//send_buffer[820+it_12];
						}

						send_buffer[it+820]='$';
						name[it]='\0';
						printf("name is %s\n",name);

						f_it=0;
						calculate1();

						int j_it;
						for(j_it=0;j_it<f_it;j_it++)
						{
							if(strcmp(foot[j_it].file,tmp)==0)
							{
								char hash_t[100];
								strcpy(hash_t,foot[j_it].hash);
								printf("hash is %s\n",hash_t);

								for(it=0;it<32;it++)
								{
									send_buffer[850+it]=hash_t[it];
									hash[it]=hash_t[it];
								}
								send_buffer[850+it]='$';
								hash[it]='\0';

								strcpy(hash_t,foot[j_it].size);
								printf("hash is %s\n",hash_t);

								for(it=0;hash_t[it]!='\0';it++)
								{
									send_buffer[900+it]=hash_t[it];
									size[it]=hash_t[it];
								}
								send_buffer[900+it]='$';
								size[it]='\0';

								strcpy(hash_t,foot[j_it].time);
								printf("hash is %s\n",hash_t);

								for(it=0;hash_t[it]!='\0';it++)
								{
									send_buffer[950+it]=hash_t[it];
									time[it]=hash_t[it];
								}
								send_buffer[950+it]='$';
								time[it]='\0';


								//break;
							}
						}

					}	

					//printf("x:%d s:%s\n",x,send_buffer);
					if(type==1)
						nn=send(rqst,send_buffer,1000,0);
					else
						nn=sendto(svc,send_buffer,1000, 0,(struct sockaddr *)&client_addr, alen);
					//printf("Sent from client : %d\n",nn);
					//bzero(send_buffer,10);
				}
				bzero(send_buffer,1000);
				bzero(buffer,256);
				printf("File Sent Successfully\n");
				close(fp);

			}
			else if(strncmp(buffer,"IndexGet",8)==0)
			{
				char temp[100];
				char send_buffer[10];
				int k=0,x,nn=0;
				char tmp[100];
				for(k=9;buffer[k]!='\0';k++)
					tmp[k-9]=buffer[k];
				tmp[k-9]=buffer[k];

				if(strcmp("LongList",tmp)==0)
				{
					pid_t pid=fork();
					if(pid==0)
					{
						FILE *fpf;//reading file information from stdout and storing it in a file_inf file
						fpf=freopen("./file_inf","w",stdout);
						system("ls --full-time | tr -s ' ' | cut -d ' ' -f5-7,9");
						fclose(fpf);
						_exit(0);
					}
					else
					{
						wait(NULL);
						//now we will send this file to the client requesting this file
						int fp=open("file_inf",O_RDONLY,"r");
						while((x=read(fp,send_buffer,5))>0)	
						{
							if(x==5)
								send_buffer[8]='$';
							else
								send_buffer[8]=x+'0';
							if(type==1)
								nn=send(rqst,send_buffer,10,0);
							else
								nn=sendto(svc,send_buffer,10, 0,(struct sockaddr *)&client_addr, alen);
						}
						bzero(send_buffer,10);
						bzero(buffer,256);
						printf("File inf Sent Successfully\n");
						close(fp);
						system("rm file_inf");
					}

				}
				else if(strncmp("RegEx",tmp,5)==0)
				{
					char reg[100];//stores *mp3
					int i;
					for(i=6;tmp[i]!='\0';i++)
						reg[i-6]=tmp[i];
					reg[i-6]=tmp[i];
					char cmd[100]="ls ";
					strcat(cmd,reg);
					pid_t pid=fork();
					if(pid==0)
					{
						FILE *fpf;//reading file information from stdout and storing it in a file_inf file
						fpf=freopen("./file_inf","w",stdout);
						system(cmd);
						fclose(fpf);
						_exit(0);
					}
					else
					{
						wait(NULL);
						//now we will send this file to the client requesting this file
						int fp=open("file_inf",O_RDONLY,"r");
						while((x=read(fp,send_buffer,5))>0)	
						{
							if(x==5)
								send_buffer[8]='$';
							else
								send_buffer[8]=x+'0';
							if(type==1)
								nn=send(rqst,send_buffer,10,0);
							else
								nn=sendto(svc,send_buffer,10, 0,(struct sockaddr *)&client_addr, alen);
						}
						bzero(send_buffer,10);
						bzero(buffer,256);
						printf("RegEx Sent Successfully\n");
						close(fp);
						system("rm file_inf");
					}

				}
				else if(strncmp(tmp,"ShortList",9)==0)
				{
					int nn;
					char from[100];
					char to[100];
					int l_it=0;
					int sp_flg=0;
					for(k=10;tmp[k]!='\0';k++)
					{
						if(tmp[k]==' ')
						{
							sp_flg=1;
							from[k-10]='\0';
						}
						if(sp_flg==0)
							from[k-10]=tmp[k];
						else
							to[l_it++]=tmp[k];
					}
					to[l_it]='\0';					
					//printf("%s %s",from,to);
					char f1[100];
					strcpy(f1,from);
					int l1=strlen(f1);
					int it1,it2=0;
					for(it1=0;it1<l1;it1++)
					{
						if(f1[it1]!='-' && f1[it1]!=':' && f1[it1]!=' ')
							from[it2++]=f1[it1];
					}
					from[it2]='\0';
					//printf("%s %s",from,to);
					strcpy(f1,to);
					l1=strlen(f1);
					it2=0;
					for(it1=0;it1<l1;it1++)
					{
						if(f1[it1]!='-' && f1[it1]!=':' && f1[it1]!=' ')
							to[it2++]=f1[it1];
					}
					to[it2]='\0';
					//printf("from is: %s , to is: %s\n",from,to);
					pid_t pid=fork();
					if(pid==0)
					{
						system("ls --full-time -lh | tr -s ' ' | cut -d ' ' -f5,6,7,9 > ls1");
						_exit(0);
					}
					else
					{
						wait(NULL);
						FILE* fp1=fopen("ls1","r");
						int i=0,j=0;
						char c='[';
						char temp[100];
						int flag=0;
						int space=0;
						int size_flag=0;
						int time_flag=0;
						int file_flag=0;
						while(fscanf(fp1,"%c",&c)!=EOF)
						{
							//printf("%c",c);
							if(flag==0)
							{	
								flag=1;
								continue;
							}
							if(c==' ')
							{
								space++;
								if(space==1)
								{
									size_flag=1;
									temp[j]='\0';
									j=0;
									strcpy(arr[i].size,temp);
									continue;
								}
								if(space==2)
									continue;
								if(space==3)
								{
									time_flag=1;
									temp[j]='\0';
									j=0;
									strcpy(arr[i].time,temp);
									temp[j]='\0';
									continue;
								}
							}
							if(c=='\n')
							{
								temp[j]='\0';
								strcpy(arr[i].file,temp);
								space=0;
								i++;
								j=0;
								temp[j]='\0';
								size_flag=0;
								time_flag=0;
								file_flag=0;
								continue;
							}
							if(c==':' || c=='-')
								continue;
							if(size_flag!=1)
							{
								temp[j]=c;
								j++;
								continue;
							}
							else if(time_flag!=1)
							{
								temp[j]=c;
								j++;
								continue;
							}
							else if(file_flag!=1)
							{
								temp[j]=c;
								j++;
								continue;
							}
						}
						fclose(fp1);
						int k;
						//printf("\n");
						FILE* fp2=fopen("ls2","w");

						for(k=0;k<i;k++)
						{
							int x=strlen(arr[k].time);
							int j;
							for(j=0;j<x;j++)
							{
								if((arr[k].time)[j]=='.')
									(arr[k].time)[j]='\0';
							}
							if((strcmp(from,arr[k].time)<=0) && (strcmp(arr[k].time,to)<=0))
							{
								//fprintf(fp2,"%s\t%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c\t%s\n",arr[k].size,((arr[k].time)[0]),((arr[k].time)[1]),((arr[k].time)[2]),((arr[k].time)[3]),((arr[k].time)[4]),((arr[k].time)[5]),((arr[k].time)[6]),((arr[k].time)[7]),((arr[k].time)[8]),((arr[k].time)[9]),((arr[k].time)[10]),((arr[k].time)[11]),((arr[k].time)[12]),((arr[k].time)[13]),((arr[k].time)[14]),arr[k].file);
								fprintf(fp2,"%s\t%s\t%s\n",arr[k].size,arr[k].time,arr[k].file);
							}
						}
						fclose(fp2);

						int fp3=open("ls2",O_RDONLY,"r");
						//bzero(send_buffer,10);
						while((x=read(fp3,send_buffer,5))>0)	
						{
							if(x==5)
								send_buffer[8]='$';
							else
								send_buffer[8]=x+'0';

							//printf("x:%d s:%s\n",x+'0',send_buffer);
							if(type==1)
								nn=send(rqst,send_buffer,10,0);
							else
								nn=sendto(svc,send_buffer,10, 0,(struct sockaddr *)&client_addr, alen);
							//printf("Sent from client : %d\n",nn);
							//bzero(send_buffer,10);
						}
						bzero(send_buffer,10);
						bzero(buffer,256);
						printf("File inf Sent Successfully\n");
						close(fp3);
						system("rm ls1 ls2");
					}

				}
			}
			else if(strncmp(buffer,"FileHash Verify",15)==0)
			{
				char f_name[100];
				int k;
				for(k=16;buffer[k]!='\0';k++)
					f_name[k-16]=buffer[k];
				f_name[k-16]=buffer[k];

				//h_it=0;
				//calculate1();
				pid_t pid=fork();
				if(pid==0)
				{
					system("ls --full-time -lh | tr -s ' ' | cut -d ' ' -f5,6,7,9 > ls1");
					_exit(0);
				}
				else
				{
					wait(NULL);
					FILE* fp1=fopen("ls1","r");
					int i=0,j=0;
					char c='[';
					char temp[100];
					int flag=0;
					int space=0;
					int size_flag=0;
					int time_flag=0;
					int file_flag=0;
					while(fscanf(fp1,"%c",&c)!=EOF)
					{
						//printf("%c",c);
						if(flag==0)
						{	
							flag=1;
							continue;
						}
						if(c==' ')
						{
							space++;
							if(space==1)
							{
								size_flag=1;
								temp[j]='\0';
								j=0;
								strcpy(arr[i].size,temp);
								continue;
							}
							if(space==2)
								continue;
							if(space==3)
							{
								time_flag=1;
								temp[j]='\0';
								j=0;
								strcpy(arr[i].time,temp);
								temp[j]='\0';
								continue;
							}
						}
						if(c=='\n')
						{
							temp[j]='\0';
							strcpy(arr[i].file,temp);
							space=0;
							i++;
							j=0;
							temp[j]='\0';
							size_flag=0;
							time_flag=0;
							file_flag=0;
							continue;
						}
						if(c==':' || c=='-')
							continue;
						if(size_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
						else if(time_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
						else if(file_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
					}
					fclose(fp1);
					int k;
					//printf("\n");

					FILE* fp2=fopen("ls2","w");

					for(k=0;k<i;k++)
					{
						int x=strlen(arr[k].time);
						int j;
						for(j=0;j<x;j++)
						{
							if((arr[k].time)[j]=='.')
								(arr[k].time)[j]='\0';
						}

						pid_t pid2=fork();
						if(pid2==0)
						{
							FILE *fpf;//reading file information from stdout and storing it in a file_inf file
							fpf=freopen("./file_inf_hash_all","w",stdout);
							char t1[100],t2[100];
							strcpy(t1,"md5sum ");
							strcat(t1,arr[k].file);
							system(t1);
							fclose(fpf);
							_exit(0);
						}
						else
						{	
							wait(NULL);
							FILE* n1=fopen("file_inf_hash_all","r");
							fscanf(n1,"%s",arr[k].hash);
							fclose(n1);
							//printf("%s\t%s\t%s\t%s\n",arr[k].size,arr[k].time,arr[k].file,arr[k].hash);
						}			


						if(strcmp(arr[k].file,f_name)==0)
							fprintf(fp2,"%s\t%s\t%s\t%s\n",arr[k].size,arr[k].time,arr[k].file,arr[k].hash);
					}
					fclose(fp2);

					/* read ls2 and send */

					char send_buffer[10];
					int fp=open("ls2",O_RDONLY,"r");
					//bzero(send_buffer,10);
					int x;
					while((x=read(fp,send_buffer,5))>0)	
					{
						if(x==5)
							send_buffer[8]='$';
						else
							send_buffer[8]=x+'0';

						//printf("x:%d s:%s\n",x+'0',send_buffer);
						if(type==1)
							nn=send(rqst,send_buffer,10,0);
						else
							nn=sendto(svc,send_buffer,10, 0,(struct sockaddr *)&client_addr, alen);
						//printf("Sent from client : %d\n",nn);
						//bzero(send_buffer,10);
					}
					bzero(send_buffer,10);
					bzero(buffer,256);
					printf("Hashing Sent SuccessFully\n");
					close(fp);


					/* read ls2 and send */


				}
				system("rm ls1 ls2 file_inf_hash_all");

			}
			else if(strcmp(buffer,"FileHash Check All")==0)
			{
				/* new */

				pid_t pid=fork();
				if(pid==0)
				{
					system("ls --full-time -lh | tr -s ' ' | cut -d ' ' -f5,6,7,9 > ls1");
					_exit(0);
				}
				else
				{
					wait(NULL);
					FILE* fp1=fopen("ls1","r");
					int i=0,j=0;
					char c='[';
					char temp[100];
					int flag=0;
					int space=0;
					int size_flag=0;
					int time_flag=0;
					int file_flag=0;
					while(fscanf(fp1,"%c",&c)!=EOF)
					{
						//printf("%c",c);
						if(flag==0)
						{	
							flag=1;
							continue;
						}
						if(c==' ')
						{
							space++;
							if(space==1)
							{
								size_flag=1;
								temp[j]='\0';
								j=0;
								strcpy(arr[i].size,temp);
								continue;
							}
							if(space==2)
								continue;
							if(space==3)
							{
								time_flag=1;
								temp[j]='\0';
								j=0;
								strcpy(arr[i].time,temp);
								temp[j]='\0';
								continue;
							}
						}
						if(c=='\n')
						{
							temp[j]='\0';
							strcpy(arr[i].file,temp);
							space=0;
							i++;
							j=0;
							temp[j]='\0';
							size_flag=0;
							time_flag=0;
							file_flag=0;
							continue;
						}
						if(c==':' || c=='-')
							continue;
						if(size_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
						else if(time_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
						else if(file_flag!=1)
						{
							temp[j]=c;
							j++;
							continue;
						}
					}
					fclose(fp1);
					int k;
					//printf("\n");

					FILE* fp2=fopen("ls2","w");

					for(k=0;k<i;k++)
					{
						int x=strlen(arr[k].time);
						int j;
						for(j=0;j<x;j++)
						{
							if((arr[k].time)[j]=='.')
								(arr[k].time)[j]='\0';
						}

						pid_t pid2=fork();
						if(pid2==0)
						{
							FILE *fpf;//reading file information from stdout and storing it in a file_inf file
							fpf=freopen("./file_inf_hash_all","w",stdout);
							char t1[100],t2[100];
							strcpy(t1,"md5sum ");
							strcat(t1,arr[k].file);
							system(t1);
							fclose(fpf);
							_exit(0);
						}
						else
						{	
							wait(NULL);
							FILE* n1=fopen("file_inf_hash_all","r");
							fscanf(n1,"%s",arr[k].hash);
							fclose(n1);
							//printf("%s\t%s\t%s\t%s\n",arr[k].size,arr[k].time,arr[k].file,arr[k].hash);
						}			



						//if(arr[k].time <= from && arr[k].time >= to)
						fprintf(fp2,"%s\t%s\t%s\t%s\n",arr[k].size,arr[k].time,arr[k].file,arr[k].hash);
					}
					fclose(fp2);

					/* read ls2 and send */

					char send_buffer[10];
					int fp=open("ls2",O_RDONLY,"r");
					//bzero(send_buffer,10);
					int x;
					while((x=read(fp,send_buffer,5))>0)	
					{
						if(x==5)
							send_buffer[8]='$';
						else
							send_buffer[8]=x+'0';

						//printf("x:%d s:%s\n",x+'0',send_buffer);
						if(type==1)
							nn=send(rqst,send_buffer,10,0);
						else
							nn=sendto(svc,send_buffer,10, 0,(struct sockaddr *)&client_addr, alen);
						//printf("Sent from client : %d\n",nn);
						//bzero(send_buffer,10);
					}
					bzero(send_buffer,10);
					bzero(buffer,256);
					printf("Hashing Sent SuccessFully\n");
					close(fp);



					/* read ls2 and send */


				}
				system("rm ls1 ls2 file_inf_hash_all");
			}
			else
			{
				if (n < 0) error("ERROR reading from socket\n");
				printf("Here is the message: %s\n",buffer);
				bzero(buffer,256);
				if(type==1)
					n = send(rqst,"I got your message",18,0);
				else
					n=sendto(svc,"I got your message",18,0,(struct sockaddr *)&client_addr, alen);
				if (n < 0) error("ERROR writing to socket");
			}
		}

	}
}
void calculate()
{
	pid_t pid=fork();
	if(pid==0)
	{
		system("ls --full-time -lh | tr -s ' ' | cut -d ' ' -f5,6,7,9 > ls1");
		_exit(0);
	}
	else
	{
		wait(NULL);
		FILE* fp1=fopen("ls1","r");
		int i=0,j=0;
		char c='[';
		char temp[100];
		int flag=0;
		int space=0;
		int size_flag=0;
		int time_flag=0;
		int file_flag=0;
		while(fscanf(fp1,"%c",&c)!=EOF)
		{
			//printf("%c",c);
			if(flag==0)
			{	
				flag=1;
				continue;
			}
			if(c==' ')
			{
				space++;
				if(space==1)
				{
					size_flag=1;
					temp[j]='\0';
					j=0;
					strcpy(head[i].size,temp);
					continue;
				}
				if(space==2)
					continue;
				if(space==3)
				{
					time_flag=1;
					temp[j]='\0';
					j=0;
					strcpy(head[i].time,temp);
					temp[j]='\0';
					continue;
				}
			}
			if(c=='\n')
			{
				temp[j]='\0';
				strcpy(head[i].file,temp);
				space=0;
				i++;
				j=0;
				temp[j]='\0';
				size_flag=0;
				time_flag=0;
				file_flag=0;
				continue;
			}
			if(c==':' || c=='-')
				continue;
			if(size_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
			else if(time_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
			else if(file_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
		}
		fclose(fp1);
		int k;
		//printf("\n");
		//FILE* fp2=fopen("ls2","w");
		h_it=i;
		for(k=0;k<i;k++)
		{
			int x=strlen(head[k].time);
			int j;
			for(j=0;j<x;j++)
			{
				if((head[k].time)[j]=='.')
					(head[k].time)[j]='\0';
			}
			//if(arr[k].time <= from && arr[k].time >= to)

			pid_t pid1=fork();
			if(pid1==0)
			{
				FILE *fpf;//reading file information from stdout and storing it in a file_inf file
				fpf=freopen("./file_inf_hash","w",stdout);
				char t1[100],t2[100];
				strcpy(t1,"md5sum ");
				strcat(t1,head[k].file);
				system(t1);
				fclose(fpf);
				_exit(0);
			}
			else
			{	
				wait(NULL);
				FILE* n1=fopen("file_inf_hash","r");
				fscanf(n1,"%s",head[k].hash);
				//printf("hasing is %s\n",head[k].hash);
				fclose(n1);
			}
			/* older */
		}

	}
	printf("length is %d\n",h_it);
	system("rm ls1 file_inf_hash");

}
void calculate1()
{
	pid_t pid=fork();
	if(pid==0)
	{
		system("ls --full-time -lh | tr -s ' ' | cut -d ' ' -f5,6,7,9 > ls1");
		_exit(0);
	}
	else
	{
		wait(NULL);
		FILE* fp1=fopen("ls1","r");
		int i=0,j=0;
		char c='[';
		char temp[100];
		int flag=0;
		int space=0;
		int size_flag=0;
		int time_flag=0;
		int file_flag=0;
		while(fscanf(fp1,"%c",&c)!=EOF)
		{
			//printf("%c",c);
			if(flag==0)
			{	
				flag=1;
				continue;
			}
			if(c==' ')
			{
				space++;
				if(space==1)
				{
					size_flag=1;
					temp[j]='\0';
					j=0;
					strcpy(foot[i].size,temp);
					continue;
				}
				if(space==2)
					continue;
				if(space==3)
				{
					time_flag=1;
					temp[j]='\0';
					j=0;
					strcpy(foot[i].time,temp);
					temp[j]='\0';
					continue;
				}
			}
			if(c=='\n')
			{
				temp[j]='\0';
				strcpy(foot[i].file,temp);
				space=0;
				i++;
				j=0;
				temp[j]='\0';
				size_flag=0;
				time_flag=0;
				file_flag=0;
				continue;
			}
			if(c==':' || c=='-')
				continue;
			if(size_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
			else if(time_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
			else if(file_flag!=1)
			{
				temp[j]=c;
				j++;
				continue;
			}
		}
		fclose(fp1);
		int k;
		//printf("\n");
		//FILE* fp2=fopen("ls2","w");
		f_it=i;
		for(k=0;k<i;k++)
		{
			int x=strlen(foot[k].time);
			int j;
			for(j=0;j<x;j++)
			{
				if((foot[k].time)[j]=='.')
					(foot[k].time)[j]='\0';
			}
			//if(arr[k].time <= from && arr[k].time >= to)

			pid_t pid1=fork();
			if(pid1==0)
			{
				FILE *fpf;//reading file information from stdout and storing it in a file_inf file
				fpf=freopen("./file_inf_hash","w",stdout);
				char t1[100],t2[100];
				strcpy(t1,"md5sum ");
				strcat(t1,foot[k].file);
				system(t1);
				fclose(fpf);
				_exit(0);
			}
			else
			{	
				wait(NULL);
				FILE* n1=fopen("file_inf_hash","r");
				fscanf(n1,"%s",foot[k].hash);
				//printf("hasing is %s\n",head[k].hash);
				fclose(n1);
			}
			/* older */
		}

	}
	printf("length is %d\n",f_it);
	system("rm ls1 file_inf_hash");

}
