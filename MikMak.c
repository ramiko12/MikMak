#include "MikMakDef.h"


int main(void)
{
	key_t key;
	int join_queue;
	int read_request;
	int write_request;
	int finish;
	int shmid;
	int shmid_num_maks;
	char user_input[53];
	char buffer[100];
	int length;
	int *num_maks;
	int look_up;
	int i;
	mak mak_buffer[MAX_MAKS];
	token t, r_request, done_reading, buff;
	
	write_msg w_msg;
	
	r_request.mtype = 1;
	done_reading.mtype = 1;
	
	mak *shared_memory, * mak_list;
	
	
	//getting token to connect to server
	if ((key = ftok("MikMakServer.c", 'A')) == -1) {  
        perror("ftok");
        exit(1);
    }
    if ((join_queue = msgget(key, 0)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(2);
    }
	
	if (msgrcv(join_queue, &t, sizeof(token), 0, 0) == -1) {
        perror("msgrcv");
        exit(3);
    }
	
	
	
	
	
	
	
	//Connecting to write_request queue
	if ((key = ftok("MikMakServer.c", 'B')) == -1) {  
        perror("ftok");
        exit(1);
    }
    if ((write_request = msgget(key, 0666)) == -1) { /* connect to the queue */
        perror("msgget write");
        exit(2);
    }
	
	
	
	
	
	
	
	
	
	//Connecting to read_request queue
	if ((key = ftok("MikMakServer.c", 'C')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((read_request = msgget(key, 0666)) == -1) {
        perror("msgget");
        exit(2);
    }
	
	
	
	
	//conecting to finish queue
	if ((key = ftok("MikMakServer.c", 'G')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((finish = msgget(key, 0666)) == -1) {
        perror("msgget");
        exit(2);
    }

	
	
	
	
	
	
	
	
	
	
	//Connecting to shared memory
	if ((key = ftok("MikMakServer.c", 'H')) == -1) {
        perror("ftok");
        exit(1);
    }
	
	if ((shmid=shmget(key, MAX_MAKS * sizeof(mak),0666)) == -1) {
        perror("shmget");
        exit(3);
    }
	
	shared_memory =(mak *)shmat(shmid,0,0);
	mak_list = shared_memory;
	
	if ((key = ftok("MikMakServer.c", 'I')) == -1) {
        perror("ftok");
        exit(1);
    }
	
	if ((shmid_num_maks=shmget(key, sizeof(int),0666)) == -1) {
        perror("shmget");
        exit(3);
    }
	num_maks =(int *)shmat(shmid_num_maks,0,0);
	
	
	
	
	
	
	
	
	
	while(1)
	{
		
		
		printf("[S]end [U]pvote [D]ownvote [R]efresh [E]xit: \n");
		
		fgets(user_input, 53, stdin);
		
		
		
		length = strlen(user_input);
		if (user_input[length-1] == '\n') user_input[length-1] = '\0';//remove newline
	
		if(user_input[0] > 90)//if its lowercase
			user_input[0] = user_input[0] - 32;//make it a capital letter
			
			
		
		if(user_input[0] != 'R' && user_input[1] != ' ')
		{
			printf("Invalid in put please enter your choice follow by a space then either the mak# or mak you wish to send\n");
			continue;
		}
	
	
	
		switch(user_input[0])
		{
			
			case 'S' : 
			//send mak
				w_msg.mtype = ADDMAK;
				strcpy(w_msg.message, &user_input[2]);
				
				if (msgsnd(write_request, &w_msg, sizeof(write_msg), 0) == -1) 
				{
					perror("msgsnd");
					exit(6);
				}
				
				
				
				break;
			
		
			case 'R' : 
			//refresh

				
				
				if (msgsnd(read_request, &r_request, sizeof(token), 0) == -1) 
				{
					perror("msgsnd");
					exit(6);
				}
				

				
				
				if (msgrcv(read_request, &buff, sizeof(token), 2, 0) == -1) {
					perror("msgrcv");
					exit(5);
				}
				

				
				
				
				for(i= 0; i < *num_maks; i++)
				{
					mak_buffer[i] = mak_list[i];
				}
				
				if (msgsnd(finish, &done_reading, sizeof(token), 0) == -1) 
				{
					perror("msgsnd");
					exit(6);
				}
				
				
				system("clear");
				
				for(i = 0; i < *num_maks; i++)
				{
					printf("%d) %+d %s\n", i + 1, mak_buffer[i].vote_count, mak_buffer[i].mak_text);
				}
		
				break;
				
			
			case 'U' : 
			//upvote
				strcpy(buffer, &user_input[2]);
				
				if((look_up = atoi(buffer)) == 0){
					fprintf(stderr, "INVALID NUMBER \n");
					continue;
				}
				
				look_up--;
				
				if(look_up > *num_maks || look_up < 0){
					fprintf(stderr, "mak does not exist \n");
				}
				
				
				w_msg.id = mak_buffer[look_up].id;
				w_msg.mtype = UPVOTE;
				
				if (msgsnd(write_request, &w_msg, sizeof(write_msg), 0) == -1) 
				{
					perror("msgsnd");
					exit(6);
				}
				
				break;
				
				
				
			case 'D' : 
			//downvote
			
				strcpy(buffer, &user_input[2]);
				
				if((look_up = atoi(buffer)) == 0){
					fprintf(stderr, "INVALID NUMBER \n");
					continue;
				}
				
				look_up--;
				
				if(look_up > *num_maks || look_up < 0){
					fprintf(stderr, "mak does not exist \n");
				}
				
				w_msg.id = mak_buffer[look_up].id;
				w_msg.mtype = DOWNVOTE;
				
				if (msgsnd(write_request, &w_msg, sizeof(write_msg), 0) == -1) 
				{
					perror("msgsnd");
					exit(6);
				}
			
				break;
				
			case 'E' : 
			
				//client puts back token for next user
				if (msgsnd(join_queue, &t, 0, 0) == -1)
				{
					perror("msgsnd");
					exit(6);
				}
				
				exit(0);
				
				
			
			default : 
			
				printf("Invalid input please try again");
			
			
			
		
		}
		
	}
	
	
	
	
	
	
        
	
}