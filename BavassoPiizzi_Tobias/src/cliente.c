#include"funciones.h"

int main(int argc, char *argv[]){
	int sockfd;
	char buffer[MAX_BUF];
	char sensitive[MAX_BUF];
	int i;
	int dspfd;	/* sound device file descriptor */
	
	int follow = 1;
	
		
	struct hostent *he;
	struct sockaddr_in their_addr; 
	
	struct cfg cfg;
	
	if(argc > 2)
		perror("Arguments");
	else
		if( *(argv+1) == NULL)
			he = gethostbyname( "localhost" );
		else
			he = gethostbyname( *(argv+1) );
	printf("-->Username:\t");
	scanf("%s",buffer);
	their_addr.sin_family = AF_INET; 	
	their_addr.sin_port = htons( MYPORT );//cfg.port
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);	
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1 ){
		perror("socket");
		exit(1);
	}
	memset(&(their_addr.sin_zero), 0 , 8); 							
	if ( connect( sockfd , (struct sockaddr *)&their_addr,	sizeof(struct sockaddr)) == -1 ){
		perror("connect");
		exit(1);
	}
	send( sockfd, buffer , MAX_BUF , 0);
	recv( sockfd , &cfg , MAX_BUF , 0);
	
	if ((dspfd = open("/dev/dsp", O_RDWR))<0){ 
		fprintf(stderr,"Error en función open, Código de error: %s\n",strerror (dspfd)); 
		exit(1);
	}
	set_audio_params ( dspfd , cfg );
	memset( buffer , '\0' , MAX_BUF);
	
	do{
		printf("\n\t---Menú de temas---\n");
		recv( sockfd , buffer , MAX_BUF , 0);
		while( strcmp(buffer,"end")){
			printf("%s\n",buffer);
			recv( sockfd , buffer , MAX_BUF , 0);
		}
		memset( buffer , '\0' , MAX_BUF);
		printf("\t---------------------\n");
		printf("-->Canción a reproducir:");
		scanf("%s",buffer);
		for( i=0 ; *(buffer+i)!='\0' ; i++)
			*(sensitive+i) = tolower(*(buffer+i));
		*(sensitive+i) = '\0';
		if( strcmp(buffer,"Exit") && strcmp(sensitive,"salida")){
			send( sockfd, buffer , MAX_BUF , 0);
			recv( sockfd, sensitive , MAX_BUF , 0);
			if(!strcmp(buffer,sensitive)){
				printf("-->Reproduciendo: %s\n",buffer);
				memset( buffer , '\0' , MAX_BUF);
				do{
					memset(buffer, '\0', MAX_BUF);
					recv(sockfd, buffer , MAX_BUF, 0);
					write(dspfd, buffer, MAX_BUF);
				}while(strcmp(buffer,"out"));
			}
			else
				printf("Canción inexistente\n\n");
		}
		else{
			if(!strcmp(buffer,"Exit")){
				send( sockfd, "Exit" , MAX_BUF , 0);
				follow = 0;
			}
			else{
				if(!strcmp(sensitive,"salida")){
					send( sockfd, "salida" , MAX_BUF , 0);
					printf("-->Se desconectó del servidor\n");
					follow = 2 ;
				}
			}
		}
		send( sockfd, "sinc" , MAX_BUF , 0);
	}while(follow == 1);
	close(sockfd);
	close(dspfd);
	return 0;
}
