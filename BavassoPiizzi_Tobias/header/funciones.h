#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_BUF 256
#define BACKLOG 1000
#define MYPORT 	8888
#define MSEG 10
struct cfg{
	int port;
	int rate;
	int size;
	int channels;
	int buffsize;
	char dir[MAX_BUF];
}	;



void sigchld_handler( int );
void rd_cfg( int , struct cfg * );
void set_audio_params ( int , struct cfg );
