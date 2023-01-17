********************************************************************/
/* The Buggy Web Server (tm)                                         */
/*                                                                   */
/* The most vulnerable piece of software in the open-source universe */
/* Obviously, you should run this as root to maximize damage         */
/* Compile with "gcc -Wall buggy.c -lcrypt -o buggy" on RH 9         */
/*********************************************************************/
// gcc -I/usr/include  -Wall  buggy2.c -lprovenance -lcrypt  -lpthread -lz -o buggy
#include <stdio.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <pwd.h>
// #include <crypt.h>
#include <fcntl.h>

#include <time.h>


#include <errno.h>
#include "provenance.h"

extern int h_errno;
#define JMPRET 5

char logentry[256] = {0};
char* LOG_PATH = "/tmp";
char* CGI_PATH = "/tmp/cgi-bin";
char* DOC_PATH = "/home/provenance/Site";

void reaper(int sig)
{
  int status;
  while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
    {/* empty */}
   
}

int run_command(jmp_buf *e, char* f, char* u, char *p, char* o)
{
  uid_t uid;
  struct passwd *entry;
  char *hash;
  char salt[3];
  char *new_argv[3];
  int pid;
  int status;

  uid = atoi(u);

  entry = getpwuid(uid);

  if (entry == NULL)
    {
      longjmp(*e, JMPRET);
    }

  salt[0] = entry->pw_passwd[0];
  salt[1] = entry->pw_passwd[1];
  salt[2] = '\0';
  
  hash = crypt(p, salt);
  printf("hash: %s (%s)\n", hash,  entry->pw_passwd);
  fflush(stdout);

  if (strcmp(hash,  entry->pw_passwd))
    {
//      longjmp(*e, JMPRET);
    }


  printf("<h1>");
  printf(o);
  printf("::");
  printf(f);
  printf("</h1>\n");
  fflush(stdout);
  pid = fork();
  if (pid ==0)
    {
      setuid(uid);
      new_argv[0] = o;
      new_argv[1] = f;
      new_argv[2] = NULL;
      execve(o, new_argv, NULL);
    }

  waitpid(pid, &status, 0);
  printf("<tt>Status %d</tt>\n", status);
  fflush(stdout);
  return status;
}

int parse_command(jmp_buf *env, char* command)
{
  char *p;
  char *fname;
  char *uid;
  char *pass;
  char *op;

  /* Command is in the format: ?filename=<file>&user=<uid>&pass=<password>&op=<operation>*/
  p = memchr(command, '?', strlen(command));
  if (p == NULL)
    {
      fprintf(stderr, "Wrong format\n");
      return -1;
    }
  fname = p + strlen("filename=") + 1;
  p = memchr(fname, '&', strlen(fname));
  if (p == NULL)
    {
      fprintf(stderr, "Wrong format\n");
      return -1;
    }
  *p = '\0';
  uid = p + strlen("user=") + 1;
  p = memchr(uid, '&', strlen(uid));
  if (p == NULL)
    {
      fprintf(stderr, "Wrong format\n");
      return -1;
    }
  *p = '\0';
  pass = p + strlen("pass=") + 1;
  p = memchr(pass, '&', strlen(pass));
  if (p == NULL)
    {
      fprintf(stderr, "Wrong format\n");
      return -1;
    }
  *p = '\0';
  op = p + strlen("op=") + 1;

  run_command(env, fname, uid, pass, op);

  return 0;
}

void exec_command(char *command)
{
  jmp_buf env;
  char buffer[256];
  int i = 0;
  
  printf("<html>\n");
  printf("  <head><title>");
  printf(command);
  printf("</title></head>\n");
  printf("  <body>\n");
  printf("    <pre>\n");
  i = setjmp(env);
  if (i == JMPRET)
    {
      printf("An error occurred\n");
      printf("    </pre>\n");
      printf("  </body>\n"); 
      printf("</html>\n"); 
      fflush(stdout);
      return;
    }


  strcpy(buffer, command);
  parse_command(&env, buffer);

  printf("    </pre>\n");
  printf("  </body>\n"); 
  printf("</html>\n"); 
  fflush(stdout);

  return;
}

void send404(char* docpath, FILE** docfile_ptr )
{
	/* Send headers */
	printf("HTTP/1.0 404 Not Found\r\n");
	printf("Server: Buggy Web Server\r\n");
	printf("Content-Type: text/html\r\n");
	printf("Content-Length: 146\r\n");
	printf("Connection: Close\r\n");
	printf("\r\n");
	
	snprintf(docpath, 512, "%s/%s", DOC_PATH, "404Error.html");
	
	*docfile_ptr= fopen(docpath, "r");
	if (*docfile_ptr == NULL)
	{
		exit(EXIT_FAILURE);
	}
}

void manage_request(u_long client_addr, u_short client_port)
{
	char buffer[512];
	int i, numc;
	FILE *log;
	char docbuffer[512];
	static char docpath[512], *logfile;
	FILE *docfile;
	char cgiexe[512];
	char *proto;
	char *url;
	char *cgibin;
	struct in_addr client_addr_in;
	fd_set read_fd;
	struct timeval timeout;
	char header[512];
	struct stat file_stat;
	static char time[31];
	
	
	i = 0;
	while((numc = read(0, &(buffer[i]), 1)) == 1)
	{
		if (buffer[i] == '\n')
		{
			if (buffer[i - 1] == '\r') buffer[i - 1] = '\0';
			else buffer[i] = '\0';
			break;
		}
		i++;
	}
	
	if (numc == 0)
	{
		perror("read");
		return;
	}
	
	client_addr_in.s_addr = client_addr;
	
	logfile = malloc(512);
	snprintf(logfile, 512, "%s/%s_%d", 
				LOG_PATH,
				inet_ntoa(client_addr_in), 
				htons(client_port));
	
	log = fopen(logfile, "a+");
	if (log == NULL)
	{
		fprintf(stderr, "Cannot open log file %s\n", logfile);
		return;
	}
	
	
	sprintf(logentry, "LOG: %s", buffer);
	fwrite(logentry, 1, strlen(logentry), log);
	fflush(log);
	
	if (strncmp(buffer, "GET ", strlen("GET ")))
	{
		fprintf(stderr, "Malformed request %s", buffer);
	}
	proto = strstr(buffer, " HTTP/1.0");
	if (proto != NULL)
	{
		*proto = '\0';
	}
	else
	{
		proto = strstr(buffer, " HTTP/1.1");
		if (proto != NULL)
		{
			*proto = '\0';
		}
	}
	url = buffer + strlen("GET ");
	
	if (!strncmp(url, "/cgi-bin/", strlen("/cgi-bin/")))
	{
		int cgifd;
		char * space_ptr = NULL;
		
		cgibin = url + strlen("/cgi-bin/");
		snprintf(cgiexe, 512, "%s/%s", CGI_PATH, cgibin);
		if ((space_ptr = strchr(cgiexe, ' ')))
		{
			*space_ptr = '\0';
		}
		
		if ((cgifd = open(cgiexe, O_RDONLY)) == -1)
		{
			send404(docpath, &docfile);
			while (1)
			{
				numc = fread(docbuffer, 1, 512, docfile);
				fwrite(docbuffer, 1, numc, stdout);
				fflush(stdout);
				
				if (numc < 512) break;
			}
			return;
		}
		close(cgifd);
		if (space_ptr != NULL) *space_ptr = ' ';
		printf("HTTP/1.0 200 OK\r\n");
		printf("Server: Buggy Web Server\r\n");
		printf("Content-Type: text/html\r\n");
		printf("Connection: Close\r\n");
		printf("\r\n");
		
		printf("<html>\n");
		printf("  <head><title>");
		printf(cgibin);
		printf("</title></head>\n");
		printf("  <body>\n");
		printf("    <pre>\n");
		fflush(stdout);
		system(cgiexe);
		printf("    </pre>\n");
		printf("  </body>\n"); 
		printf("</html>\n"); 
		fflush(stdout);
	}
	else if (!strncmp(url, "/command", strlen("/command")))
	{
		exec_command(url + strlen("/command"));
	}
	else
	{
		if (!strncmp(url + strlen(url) - 1, "/",  strlen("/")))
		{
			snprintf(docpath, 512, "%s/%sindex.html", DOC_PATH, url);
		}
		else
		{
			snprintf(docpath, 512, "%s/%s", DOC_PATH, url);
		}
	
		docfile= fopen(docpath, "r");
		
		/*if (docfile == NULL)
		{
			printf("HTTP/1.0 404 File not found\n");
			fflush(stdout);
			return;
		}*/
		if (docfile == NULL)
		{
			send404(docpath, &docfile);
		}
		else
		{
			stat(docpath, &file_stat);
			
			printf("HTTP/1.0 200 OK\r\n");
			printf("Server: Buggy Web Server\r\n");
			if (!strncmp(docpath + strlen(docpath) - strlen(".htm"), ".htm", strlen(".htm")) || !strncmp(docpath + strlen(docpath) - strlen(".html"), ".html", strlen(".html")))
			{
				snprintf(header, 512, "text/html");
			}
			else if (!strncmp(docpath + strlen(docpath) - strlen(".jpg"), ".jpg", strlen(".jpg")) ||
									!strncmp(docpath + strlen(docpath) - strlen(".jpeg"), ".jpeg", strlen(".jpeg")))
			{
				snprintf(header, 512, "image/jpeg");
			}
			else if (!strncmp(docpath + strlen(docpath) - strlen(".png"), ".png", strlen(".png")))
			{
				snprintf(header, 512, "image/png");
			}
			else
			{
				snprintf(header, 512, "text/plain");
			}
			
			printf("Content-Type: %s\r\n", header);
			printf("Content-Length: %ld\r\n", file_stat.st_size);
			strftime(time, 30, "%a, %d %b %Y %H:%M:%S GMT",  gmtime(&file_stat.st_mtime));
			printf("Last-Modified: %s\r\n", time);
			printf("Connection: Close\r\n");
			printf("\r\n");
		}
		
		while (1)
		{
			numc = fread(docbuffer, 1, 512, docfile);
			fwrite(docbuffer, 1, numc, stdout);
			fflush(stdout);
			
			if (numc < 512) break;
		}
	}
	
	/* Read the socket and then return */
	numc = 1;
	FD_ZERO(&read_fd);
	FD_SET(0, &read_fd);
	timeout.tv_sec = 0l;
	timeout.tv_usec = 100l;
	while (numc > 0 && select(2, &read_fd, NULL, NULL, &timeout) == 1)
	{
		FD_ZERO(&read_fd);
		FD_SET(0, &read_fd);
		timeout.tv_sec = 0l;
		timeout.tv_usec = 10l;
		numc = read(0, buffer, 511);
	}
	
	return;
}

int main(int argc, char *argv[])
{
	struct stat fileinfo;
	char command[512];
	char docname[512];
	FILE *docfile;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sockfd, newsockfd, client_len;
	int port;
	char localhost[64];
	int on = 1;
	struct hostent *hp;
	int pid;



if(provenance_set_tracked(true)){
    printf("Failed Tracking, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }
  if(provenance_set_propagate(true)){
    printf("Failed propagate, error %d\n", errno);
    printf("%s\n\n",strerror(errno));
  }




	if (argv[1] != NULL)
	{
		port = atoi(argv[1]);
	}
	else
	{
		port = 7979;
	}
  
	if (stat(CGI_PATH, &fileinfo))
	{
		snprintf(command, 512, "mkdir %s\n", CGI_PATH);
		system(command);
		snprintf(command, 512, "cp `which netstat` %s\n", CGI_PATH);
		system(command);
		snprintf(command, 512, "cp `which who` %s\n", CGI_PATH);
		system(command);
		snprintf(command, 512, "cp `which uptime` %s\n", CGI_PATH);
		system(command);
	}

	if (stat(DOC_PATH, &fileinfo))
	{
		snprintf(command, 512, "mkdir %s\n", DOC_PATH);
		system(command);
		snprintf(docname, 512, "%s/index.html", DOC_PATH);
		docfile = fopen(docname, "w+");
		if (docfile == NULL)
		{
			fprintf(stderr, "Cannot create index.html file\n");
			exit(1);
		}
		fprintf(docfile, "\
		<html>\n\
			<head><title>The Buggy Web Server (tm)</title></head>\n\
			<body>\n\
				<h1>Welcome to the Buggy Web Server (tm)</h1>\n\
				<p>This is the default page for the Buggy Web Server (tm)</p>\n\
			</body>\n\
		</html>\n");
		fclose(docfile);
	}


	gethostname(localhost, 64);
	if ((hp = gethostbyname(localhost)) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}
	
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = hp->h_addrtype;
	server_addr.sin_port = htons(port);
	bcopy((char *)hp->h_addr, (char *)&server_addr.sin_addr, hp->h_length);

	sockfd = socket(hp->h_addrtype, SOCK_STREAM, 0);
	if (sockfd == -1) 
	{
		perror("socket");
		exit(1);
	}
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) 
	{
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, 5)) 
	{
		perror("listen");
		exit(1);
	}

	fprintf(stderr, "%s: using address %s and port %d\n",
	argv[0],
	inet_ntoa(server_addr.sin_addr),
	port);

	(void) signal(SIGCHLD, reaper);

	close(0);
	close(1);
	close(2);
	client_len = sizeof(client_addr);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);
		if (newsockfd < 0) 
		{
			perror("accept");
			exit(1);
		}
		
		dup(newsockfd);
		dup(newsockfd);
		dup(newsockfd);
		
		manage_request((u_long) (client_addr.sin_addr.s_addr), 
			(u_short) (client_addr.sin_port));
			
		close(0);
		close(1);
		close(2);
	
		close(newsockfd);
	}

	return 0; /* Never reached */
}
