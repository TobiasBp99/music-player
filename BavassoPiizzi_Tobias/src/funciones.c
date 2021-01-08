#include"funciones.h"
void sigchld_handler(int s){
	while(wait(NULL) > 0);
}
void rd_cfg(int cfg_fd , struct cfg * cfg){
	char buffer[MAX_BUF];
	char mat[5][50] = {"PORT=","RATE=","SIZE=","CHANNELS=","DIR="};
	
	char msg[MAX_BUF]="";
	char num[MAX_BUF]="";
	
	int port,rate,size,channels;
	char dir[MAX_BUF];
	
	int i = 0;
	int j ;
	int k ;
	read( cfg_fd , buffer , MAX_BUF );
	for( j=0 ; j<5 ;j++){
		k=0;
		while( (*(buffer+i) < '0' || *(buffer+i) > '9') && *(buffer+i) != '.' && *(buffer+i) != '\n' ){
			*(msg+k) = *(buffer+i) ;
			i++;
			k++;
		}
		if( ! strcmp(msg,*(mat+j))){
			k=0;
			while( *(buffer+i) != '\n' ){
				*(num+k) = *(buffer+i) ;
				k++;
				i++;
			}
			i++;
			*(num+k) = '\0';
			if( j == 0 ){
				port = atoi(num);
				if( !port )
					port = MYPORT;
			}
			else if( j == 1 )
				rate = atoi(num);
			else if( j == 2 )
				size = atoi(num);
			else if( j == 3 )
				channels = atoi(num);
			else if( j == 4 )
				strcpy(dir,num);
		}
		memset( num , '\0' , MAX_BUF );
		memset( msg , '\0' , MAX_BUF );
	}
	
	cfg->port = port;
	cfg->rate = rate;
	cfg->size = size;
	cfg->channels = channels;
	strcpy(cfg->dir,dir);
	cfg->buffsize = MSEG*rate*size*channels/8/1000;
	
	return;
}
void set_audio_params (int fd , struct cfg cfg){
	int 	arg;		/* argumento para ioctl */
	int	status;		/* salida de ioctl */


	arg = cfg.size ;	   /* arg = Tamaño de muestra */
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg); 

	if (status == -1) 
		perror("Error con comando SOUND_PCM_WRITE_BITS");

	if (arg != cfg.size )

		fprintf (stderr,"Tamaño de muestras no soportado. Se programó %d\n",arg);

	arg = cfg.channels ;  /* mono o stereo */
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status == -1)
		perror("Error en comando SOUND_PCM_WRITE_CHANNELS");
	if (arg != cfg.channels)
		fprintf (stderr,"Cantidad de canales no soportado. Se programó %d\n",arg);

	arg = cfg.rate ;	   /* Velocidad de Muestreo */
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if (status == -1)
		perror("Error en comando SOUND_PCM_WRITE_RATE");
	if (arg != cfg.rate)
		fprintf (stderr,"Velocidad de muestreo no soportada. Se programó %d\n",arg);

	return;
}
