#include"funciones.h"

int main (){
	int sockfd,new_fd,cfg_fd,song_fd;
	struct sockaddr_in my_addr; 
	struct sockaddr_in their_addr; 
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char buffer[MAX_BUF];
	struct cfg cfg;
	struct dirent * dp ;
	DIR * dir;
	char song[MAX_BUF];
	int follow = 1;
	int flag=0;
	
	memset( buffer , '\0' , MAX_BUF);
	if((cfg_fd = open("servidor.cfg" , O_RDONLY)) <0 ){
		perror("cfg");
		exit(1);
	}
	rd_cfg(cfg_fd, &cfg);
	close(cfg_fd);
	if ( ( sockfd = socket( AF_INET , SOCK_STREAM , 0 ) ) == -1){
		perror("socket");
		exit(1);
	}
	if ( setsockopt( sockfd , SOL_SOCKET , SO_REUSEADDR , &yes , sizeof(int) ) == -1) {
		perror("setsockopt");
		exit(1);
	}
	my_addr.sin_family = AF_INET; 			
	my_addr.sin_port = htons( MYPORT ); //cfg.port	
	my_addr.sin_addr.s_addr = htonl ( INADDR_ANY ); 
	memset(&(my_addr.sin_zero), '\0', 8); 			
	if ( bind( sockfd , (struct sockaddr *)&my_addr, sizeof( struct sockaddr ) ) == -1 ){
		perror("bind");
		exit(1);
	}
	if ( listen( sockfd , BACKLOG ) == -1 ){
		perror("listen");
		exit(1);
	}
	//send( sockfd , &cfg , MAX_BUF , 0);
	sa.sa_handler = sigchld_handler; 
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if ( sigaction( SIGCHLD , &sa , NULL ) == -1 )	{
		perror("sigaction");
		exit(1);
	}
	
	printf("\tEl servidor está en ejecución en el port %d\n",cfg.port);
	while( 1 ){
		sin_size = sizeof( struct sockaddr_in );
		if ( ( new_fd = accept( sockfd , (struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("accept");
			continue;
		}
		printf("Got connection from %s\n",inet_ntoa(their_addr.sin_addr));
		recv(new_fd, buffer , MAX_BUF , 0); //Recibo el alias
		printf("Got connection from %s\n",buffer);
		memset( buffer , '\0' , MAX_BUF);
		send( new_fd , &cfg , MAX_BUF , 0);
		if ( !fork( ) ){
			close(sockfd);
			do{
				dir = opendir( cfg.dir );
				while( (dp = readdir(dir)) != NULL ){
					if( strcmp(dp->d_name,".") && strcmp(dp->d_name,".." ))
					send( new_fd , dp->d_name , MAX_BUF , 0);
				}
				closedir( dir );
				send( new_fd , "end" , MAX_BUF , 0);
				recv(new_fd, buffer , MAX_BUF , 0);
				if( strcmp(buffer,"Exit") && strcmp(buffer,"salida")){
					dir = opendir( cfg.dir );
					while( (dp = readdir(dir)) != NULL ){
						if( !strcmp(buffer,dp->d_name) ){
							flag=1;
							send( new_fd , dp->d_name, MAX_BUF ,  0 );
							strcpy( song , (cfg.dir)+2 );
							strcat( song , "/");
							strcat( song , buffer);
							if ( (song_fd = open( song , O_RDONLY)) < 0 ){
								printf("%s\n",song);
								perror("songfd");
								exit(1);
							}
							while( read( song_fd, buffer , MAX_BUF )  ){
								send( new_fd , buffer, MAX_BUF ,  0 );
								memset(buffer, '\0', MAX_BUF);
							}
							send( new_fd ,"out", MAX_BUF ,  0 );
						}
					}
					if(!flag){
						send( new_fd , "NO", MAX_BUF ,  0 );
					}
					flag=0;
				}
				else
					follow = 0;
				close(song_fd);
				closedir(dir);
				recv(new_fd, buffer , MAX_BUF , 0);
				memset(buffer, '\0', MAX_BUF);
			}while(follow);
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}
