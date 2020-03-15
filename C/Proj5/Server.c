/*
 *
 *    Author: Pavel Podluzansky, xpodlu01
 *    IPK- Projekt 2019, varianta 1
 *    
 *
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define NotFound "404 Not Found"
#define BadRequest "400 Bad Request"
#define OK "200 OK" 
#define Notacceptable "406 Not Acceptable"

// Funkcia na ziskanie nazvu cpu
void cpuname (int size,char buffercpu[1024],FILE *f)
{
    while (fscanf(f, "%1023s",buffercpu)==1) 
    {  
        if(strcmp(buffercpu,"model") == 0)
        {   
            fscanf(f, " %1023s", buffercpu);
            if(strcmp(buffercpu,"name") == 0)
            {
               fscanf(f, " %1023s",buffercpu);
               if(strcmp(buffercpu,":") ==0)
               {
                  if(fgets(buffercpu,size, f))
                  {
                     for(unsigned int i=0;i<strlen(buffercpu);i++)
                     {
                        buffercpu[i]=buffercpu[i+1];
                     }
                     return;
                  }
               }
            }
        }
    }
}

// Funkcia na prezretie requestu
char * CheckRequest(char *chrequest)
{
   char *array[1024];
   if(chrequest[0]!='G')
   {
      return BadRequest;
   }
   if(chrequest[1]!='E')
   {
      return BadRequest;
   }
   if(chrequest[2]!='T')
   {
      return BadRequest;
   }
   if(chrequest[3]!=' ')
   {
      return BadRequest;
   }
   else
   {
   char delim[1024] = " ";
   int i=0;
	char *ptr = strtok(chrequest, delim);
      while(ptr != NULL)
      {
         array[i++]=ptr;
         ptr = strtok(NULL, delim);
         
      }
   }
   array[2] = strtok(array[2], "\r\n");

   if(strcmp(array[2],"HTTP/1.1")!=0)
   {
      return BadRequest;
   }

   chrequest=array[1];
   return chrequest;
}

// Funkcia ziska accept type 
char * GetAcceptType(char *chrequest)
{
   char *ptr=chrequest;
   char *ptr2;
   while(ptr!=NULL)
   {
      ptr2=strstr(ptr,"Accept:");
      if(ptr2!=NULL)
      {
         break;
      }
      if(ptr2==NULL)
      {
         return "text/plain";
      }
   }
   while(strcmp(ptr2,"\n")!=0)
   {
      if(strstr(ptr2,"text/plain"))
      {
         return "text/plain";
      }
      if(strstr(ptr2,"application/json"))
      {
         return "application/json";
      }

      if(strstr(ptr2,"*/*"))
      {
         return "text/plain";
      }
      else
      {
         return Notacceptable;
      }
   }
      return Notacceptable;
}

// Funkcia vypocita Cpu load
int GetCPULoad() {

    FILE* file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("Could not open stat file");
        return 0;
    }


   char buffer[1024];
   double prevuser=0,prevnice=0,prevsystem=0,previdle=0,user = 0,nice = 0,system = 0,idle = 0;
   double previowait=0,previrq=0,prevsoftirq=0,prevsteal=0,iowait = 0, irq = 0, softirq = 0, steal = 0;
   double PrevIdle=0,PrevTotal=0,PrevNonIdle=0,idled=0,Idle=0,NonIdle=0,Total=0,totald=0; 
   char* ret = fgets(buffer, sizeof(buffer) - 1, file);
   if (ret == NULL) {
      perror("Could not read stat file");
      fclose(file);
      return 0;
   }
   fclose(file);
    
   sscanf(buffer,
         "cpu  %lf %lf %lf %lf %lf %lf %lf %lf",
         &prevuser, &prevnice, &prevsystem, &previdle, &previowait, &previrq, &prevsoftirq, &prevsteal); 

   struct timespec req = {0};
   req.tv_sec = 0;
   req.tv_nsec = 500000000;
   nanosleep(&req, (struct timespec *)NULL);
   
   
    file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("Could not open stat file");
        return 0;
    }

   ret = fgets(buffer, sizeof(buffer) - 1, file);
   if (ret == NULL) {
      perror("Could not read stat file");
      fclose(file);
      return 0;
   }
   fclose(file);
    
    sscanf(buffer,
           "cpu  %lf %lf %lf %lf %lf %lf %lf %lf",
   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
   
   PrevIdle = previdle + previowait;
   Idle=idle+iowait;
   PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
   NonIdle= user+ nice + system + irq +softirq +steal;
   PrevTotal = PrevIdle  + PrevNonIdle;
   Total = Idle + NonIdle;
   totald = Total - PrevTotal;
   idled = Idle - PrevIdle;
   return ((totald-idled)/totald)*100;

}

// Funkcia zisti co klient pozaduje
char * WhichRequest(char *number,int size,char *whichr,char buffer[1024])
{
   if(strcmp(whichr,"/hostname")==0)
   {
      gethostname(buffer, 1023);
            buffer[strlen(buffer)]='\0';
      return buffer;
   }
   else if(strcmp(whichr,"/cpu-name")==0)
   {
         FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
         cpuname(size,buffer,cpuinfo);
         fclose(cpuinfo);
               buffer[strlen(buffer)]='\0';
         return buffer;
   }
   else if(strcmp(whichr,"/load")==0)
   {
      sprintf(buffer,"%d%%",GetCPULoad());
      buffer[strlen(buffer)]='\0';
      return buffer;     
   }
   else if(number!=0)
   {
      sprintf(buffer,"%d%%",GetCPULoad());
      buffer[strlen(buffer)]='\0';
      return buffer;    
   }
   else
   {
      strcpy(buffer,NotFound);  
      buffer[strlen(buffer)]='\0';
      return buffer;
   }
}

// Hlavne spracovanie errorov
char * ErrorHandle(char * number,char * whichr)
{
     
   if(strcmp(whichr,"/hostname")==0)
   {
      return OK;
   }
   
   else if(strcmp(whichr,"/cpu-name")==0)
   {
         return OK;
   }
   
   else if(strcmp(whichr,"/load")==0)
   { 
      return OK;   
   }

   else if(strncmp(whichr,"/load?refresh=",14)==0)
   {
      int j=0;
      for(unsigned int i=14;i<strlen(whichr);i++)
      {
         if(!isdigit(whichr[i]))
         {
            return BadRequest;
         }
         else
         {
         number[j]=whichr[i];
         j++;
         }
         
      }
      number[j]='\0';
      return OK;
   }
   else
   {
      return BadRequest;
   }
}

/*
 *
 * Vsetky mozne response headery
 *
 *
 */
void responsejsonerror(char *response,char * erbuff, char * content_length)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nContent-Type: application/json\r\n\r\n{ \"Error\":\"%s\" }\r\n\r\n",erbuff,content_length,erbuff);
}

void responsejson(char *response,char * erbuff, char * content_length,char * returnedrequest)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nContent-Type: application/json\r\n\r\n{ \"Requested data\":\"%s\" }\r\n\r\n",erbuff,content_length,returnedrequest);
}

void responsejsoncpuname(char *response,char * erbuff, char * content_length,char * returnedrequest)
{
   returnedrequest[strlen(returnedrequest)-1]='\0';
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nContent-Type: application/json\r\n\r\n{ \"Requested data\":\"%s\" }\r\n\r\n",erbuff,content_length,returnedrequest);
}

void responsejsonload(char * number,char *response,char * erbuff, char * content_length,char * returnedrequest)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nRefresh: %s\r\nContent-Type: application/json\r\n\r\n{ \"load\":\"%s\" }\r\n\r\n",erbuff,content_length,number,returnedrequest);
}

void responsehead(char *response,char * erbuff, char * content_length,char * returnedrequest)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nContent-Type: text/plain\r\n\r\n%s\r\n\r\n",erbuff,content_length,returnedrequest);
}

void responseheadload(char * number,char *response,char * erbuff, char * content_length,char * returnedrequest)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nRefresh: %s\r\nContent-Type: text/plain\r\n\r\n%s\r\n\r\n",erbuff,content_length,number,returnedrequest);
}

void responseheaderror(char *response,char * erbuff, char * content_length)
{
   sprintf(response,"HTTP/1.1 %s\r\nContent-Length: %s\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=10, max=20\r\nContent-Type: text/plain\r\n\r\n%s\r\n\r\n",erbuff,content_length,erbuff);
}


int main (int argc, char **argv) {
   char bufferresponse[1024];
   memset(bufferresponse,0,sizeof(bufferresponse));
   char number[1024];
   int rc;
	int welcome_socket;
	struct sockaddr_in6 sa;
	struct sockaddr_in6 sa_client;
   int port_number;
   char content_length[1024];
   memset(&content_length,0,sizeof(content_length));
   if (argc != 2) 
   {
      fprintf(stderr,"usage: %s <port>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   int length = strlen(argv[1]);
   for (int i=0;i<length; i++)
   {
      if (!isdigit(argv[1][i]))
      {
         fprintf (stderr,"Entered input is not a number\n");
         exit(1);
      }
   }
   port_number = atoi(argv[1]);   
   if(port_number<0 || port_number>65535)
   {
      fprintf(stderr,"Out of range\n");
      exit(EXIT_FAILURE);
   }

   socklen_t sa_client_len=sizeof(sa_client);
	if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}
	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_addr = in6addr_any;
	sa.sin6_port = htons(port_number);	

   if (setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
   {
      close(welcome_socket);
      perror("ERROR: setsockopt");
      return 1;
   }

	if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0)
	{
		perror("ERROR: bind");
		exit(EXIT_FAILURE);		
	}

	if ((listen(welcome_socket,50)) < 0)
	{
		perror("ERROR: listen");
		exit(EXIT_FAILURE);				
	}
	while(1)
	{
		int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);	

      if (comm_socket > 0)
		{
         struct timeval tv;
         tv.tv_sec = 10;  // 10 Sekundovy Timeout 
         if (setsockopt(comm_socket, SOL_SOCKET, SO_KEEPALIVE, &(int){ 1 }, sizeof(int)) < 0)
         {
            close(comm_socket);
            perror("ERROR: setsockopt");
            continue;
         }
         if (setsockopt(comm_socket, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval)))
         {
            close(comm_socket);
            perror("ERROR: setsockopt");
            continue;
         }
         char accepttype[1024];
         char buff[1024];
         int res = 0;
         int cl=0;
         int sizeofbuff=0;
         signal(SIGCHLD,SIG_IGN);
         int pid=fork();   
         if(pid!=0)
         {
            close(comm_socket);
            continue;
         }
         
         if(pid==0)
         {
            close(welcome_socket);
            while(1)
            {
               res = recv(comm_socket, buff, 1024,0);
               if(res<0)
               {
                  close(comm_socket);
                  exit(1);
                  break;
               }
               else if(res==0)
               {

                  close(comm_socket);
                  exit(1);
                  break;
               }
               else
               {
                  memset(accepttype,0,sizeof(accepttype));
                  strcpy(accepttype,buff);
                  char * accepted=GetAcceptType(accepttype);
                  memset(&number,0,sizeof(number));
                  char * returnedrequest;
                  memset(&returnedrequest,0,sizeof(returnedrequest));
                  returnedrequest=CheckRequest(buff);
                  sizeofbuff=sizeof(buff);
                  char * erbuff=buff;
                  erbuff=ErrorHandle(number,returnedrequest);
                  returnedrequest=WhichRequest(number,sizeofbuff,returnedrequest,buff);                                            
                  
                  // pokial sa jedna o text plain
                  if(strcmp(accepted,"text/plain")==0)
                  {
                     if(strcmp(erbuff,OK)==0) // pokial prislo 200 OK
                     {
                        if(strlen(number)!=0) // ak si vyziadal klient refresh load
                        {
                           sprintf(returnedrequest,"%s",returnedrequest);
                           cl=strlen(returnedrequest);
                           sprintf(content_length,"%d",cl);     
                           responseheadload(number,bufferresponse,erbuff,content_length,returnedrequest);
                           send(comm_socket,bufferresponse,strlen(bufferresponse),0);
                        }
                        else // vsetko ostatne co klient ziada ako text/plain
                        {
                           cl=strlen(returnedrequest);
                           sprintf(content_length,"%d",cl);  
                           responsehead(bufferresponse,erbuff,content_length,returnedrequest);
                           send(comm_socket,bufferresponse,strlen(bufferresponse),0);
                        }

                     }
                     else
                     {                  
                        cl=strlen(erbuff);
                        sprintf(content_length,"%d",cl);
                        responseheaderror(bufferresponse,erbuff,content_length);
                        send(comm_socket,bufferresponse,strlen(bufferresponse),0);             

                     }
                        
                  }
                  // ak sa jedna o json
                  if(strcmp(accepted,"application/json")==0)
                  {       
                     if(strcmp(erbuff,OK)==0)
                     {
                        if(strlen(number)!=0)
                        {
                           cl=strlen(returnedrequest)+15; // 14 znakov z dovodu ze sa navysi o 14 znakov kvoli body
                           sprintf(content_length,"%d",cl);     
                           responsejsonload(number,bufferresponse,erbuff,content_length,returnedrequest);
                           send(comm_socket,bufferresponse,strlen(bufferresponse),0);
                        }
                        else
                        {
                              if(strstr(returnedrequest,"/cpu-name")!=NULL)
                              {
                                 cl=strlen(returnedrequest)+25; // 24 znakov z dovodu ze sa navysi o 24 znakov kvoli body
                                 sprintf(content_length,"%d",cl);     
                                 responsejsoncpuname(bufferresponse,erbuff,content_length,returnedrequest);
                                 send(comm_socket,bufferresponse,strlen(bufferresponse),0);                                                                 
                              }
                              else
                              {
                                 cl=strlen(returnedrequest)+25; // 24 znakov z dovodu ze sa navysi o 24 znakov kvoli body
                                 sprintf(content_length,"%d",cl);     
                                 responsejson(bufferresponse,erbuff,content_length,returnedrequest);
                                 send(comm_socket,bufferresponse,strlen(bufferresponse),0);
                              }
                        }
                     }
                     else
                     {                  
                        cl=strlen(erbuff)+16; // 15 znakov z dovodu ze sa navysi o 15 znakov kvoli body
                        sprintf(content_length,"%d",cl);
                        responsejsonerror(bufferresponse,erbuff,content_length);
                        send(comm_socket,bufferresponse,strlen(bufferresponse),0);             

                     }

                  }
                  // vypis erroru
                  else{
                     cl=strlen(accepted);
                     sprintf(content_length,"%d",cl);
                     responseheaderror(bufferresponse,accepted,content_length);
                     send(comm_socket,bufferresponse,strlen(bufferresponse),0);
                  }
      
               }
            }
         }
      }	
   }
   return 0;
}