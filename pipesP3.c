#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define READ 0
#define WRITE 1
#define N 4


int min(int a, int b){
	if(a<b)
		return a;
	return b;
}

int ids[N];

int main(int argc, char *argv[]){

	int input_array[200];

	for(int i=0; i<200; i++){
		input_array[i] = i%50;
	}
	int fd[2];
	if(pipe(fd) == -1){
		fprintf (stderr, "Pipe failed.\n");
   		return -1;
	}
	

	int nrOfProcs = N;
	int searched_value;
	printf("Please enter searched_value:");
	scanf("%d", &searched_value);
	int input_size = 200;
	int batch_size = input_size/nrOfProcs;
	int batch_remainer = input_size%nrOfProcs;
	int closing_signal = -15;

	for(int i=0; i<nrOfProcs; i++){
		int id = fork();
		ids[i] = id;
		if(id == 0){//child
			close(fd[READ]);
			int bufindex=0,j;
			int buffer[200];
			for(j=i*batch_size; j<min((i+1)*batch_size, input_size); j++){
				if(searched_value==input_array[j]){
					buffer[bufindex] = j;
					bufindex++;
					// write(fd[WRITE], &j, sizeof(int));
				}
			}
			if(i==nrOfProcs-1){
				while(j<=input_size){
					if(searched_value==input_array[j]){
						buffer[bufindex] = j;
						bufindex++;
						// write(fd[WRITE], &j, sizeof(int));
					}
					j++;
				}
			}

			// buffer[bufindex] = closing_signal;
			// bufindex++;

			write(fd[WRITE], buffer, bufindex*sizeof(int));//write signal that we searched the whole interval
			exit(0);
		}
		
	}

	
	int position;
	int signals = N;

	close(fd[WRITE]);
	int octets, index = 0;
	int result[500];
	while(octets=read(fd[READ], &result[index], sizeof(result))){
		index += octets/sizeof(int);

	}

	for(int i=0; i<index; i++)
		printf("%d ", result[i]);

/*
	while(signals){
		read(fd[READ], &position, sizeof(int));
		if(position!=closing_signal){
			printf("%d\n", position);
		}
		else{
			signals--;
		}
	}*/

	for(int i = 0; i< N; i++){
		kill(ids[i],SIGKILL);
	}


	return  0;
}
