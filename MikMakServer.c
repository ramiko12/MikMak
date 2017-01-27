/* "MikMakServer.c" needs to be present for ftok
	ignore compliler warnings
*/

#include "MikMakDef.h"



//Global variables
int shmid;
int shmid_num_maks;
int *num_maks;
int id;
int user_control;//Mailbox to limit number of users
int read_request;
int write_request;
int finish;
mak * mak_list;


void cleanup(void)
{
	if (msgctl(user_control, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(7);
	}
	if (msgctl(write_request, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(7);
	}
	if (msgctl(read_request, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(7);
	}
	if (msgctl(finish, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
		exit(7);
	}
	if ((shmctl(shmid,IPC_RMID,0)) == -1)
	{
		perror("shmctl");
		exit(8);
	}
	if ((shmctl(shmid_num_maks,IPC_RMID,0)) == -1)
	{
		perror("shmctl");
		exit(8);
	}
	
	exit(0);
}

int up_vote(int id_to_look_for)
{
	
	int i;
	mak * current = mak_list;
	
	for(i = 0; i < *num_maks; i++)
	{
		if(current[i].id == id_to_look_for)
		{
			current[i].vote_count = current[i].vote_count + 1;
			break;
		}
		
	}
	
}

int down_vote(int id_to_look_for)
{
	int i, j;
	mak * current = mak_list;
	
	for(i = 0; i < *num_maks; i++)
	{
		if(current[i].id == id_to_look_for)
		{
			current[i].vote_count = current[i].vote_count - 1;
			
			if(current[i].vote_count == -5)
			{
				for(j = i; j < *num_maks - 1; j++)
				{
					mak_list[j] = mak_list[j+1];
			
				}
				
				(*num_maks)--;
				
			}
			
			break;
			
		}
		
	}
	
	
	
}

int add_mak(char * mak_text_to_add)
{

	
	
	mak *current;
	current = mak_list;
	
	int i;
	
	
		
	if(*num_maks < MAX_MAKS)
	{
		current = mak_list;
		
		for(i = 0; i < *num_maks; i++)
			current++;
		
		id++;
		
		current->id = id;

		
		
		stpcpy(current->mak_text, mak_text_to_add);

		current->vote_count = 0;
		(*num_maks)++;
	
		
	}else{
		
		
		for(i = 0; i < MAX_MAKS-1; i++)
		{
			mak_list[i] = mak_list[i+1];
		
		}
		
		current = &mak_list[MAX_MAKS -1];
		
		id++;
		
		current->id = id;
		
		stpcpy(current->mak_text, mak_text_to_add);
	
		current->vote_count = 0;
		
		
		
	}
		
	

	
	
}




int main(void)
{
	int i;

	int count = MAX_USERS+1;
	int num_messages_finish, num_messages_write, num_messages_read;
	struct msqid_ds buf;
	
	mak *shared_memory;
	token t, t2, done_writing;
	write_msg w_msg;
	key_t key;

	
	
	t.mtype = 1;
	t2.mtype = 2;

	
	//mailbox to control number of clients
	if ((key = ftok("MikMakServer.c", 'A')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((user_control = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(2);
	}
	
	

	
	//write_request queue
	if ((key = ftok("MikMakServer.c", 'B')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((write_request = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(2);
    }
	

	
	
	//read_request queue
	if ((key = ftok("MikMakServer.c", 'C')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((read_request = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(2);
    }

	
	
	//finish message queue
	if ((key = ftok("MikMakServer.c", 'G')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((finish = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(2);
    }

	
	
	//shared memory setup
	if ((key = ftok("MikMakServer.c", 'H')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((shmid=shmget(key, MAX_MAKS * sizeof(mak),0666|IPC_CREAT)) == -1) {
        perror("shmget");
        exit(3);
    }
	shared_memory =(mak *)shmat(shmid,0,0);
	mak_list = shared_memory;
	
	
	//shared_memory setup
	if ((key = ftok("MikMakServer.c", 'I')) == -1) {
        perror("ftok");
        exit(1);
    }
	if ((shmid_num_maks=shmget(key, sizeof(int),0666|IPC_CREAT)) == -1) {
        perror("shmget");
        exit(3);
    }
	num_maks =(int *)shmat(shmid_num_maks,0,0);
	*num_maks = 0;
	
	
	
	
	//Fill with token messages
	for(i = 0; i < MAX_USERS; i++)
	{
		if (msgsnd(user_control, &t, 0, 0) == -1)
            perror("msgsnd token");	
	}
	
	
	signal(15,cleanup);//handle sigterm
	signal(2,cleanup);//handle sigint
	
	//atexit(cleanup);
	
	printf("server is running\n");
	fflush(stdout);
	
	
	
	
	//controller
	while(1)
	{
		if(count > 0)
		{
			if((msgctl(finish, IPC_STAT, &buf)) != 0){
				perror("msgctl");
				exit(4);
			}
			num_messages_finish = buf.msg_qnum;
			
			if((msgctl(write_request, IPC_STAT, &buf)) != 0){
				perror("msgctl");
				exit(4);
			}
			num_messages_write = buf.msg_qnum;
			
			if((msgctl(read_request, IPC_STAT, &buf)) != 0){
				perror("msgctl");
				exit(4);
			}
			num_messages_read = buf.msg_qnum;
			
			
			
			
			if(num_messages_finish != 0){
				
				if (msgrcv(finish, &t, sizeof(token), 0, 0) == -1) {
					perror("msgrcv");
					exit(5);
				}
				
				count++;
				
				
			}else if(num_messages_write != 0){
				
				if (msgrcv(write_request, &w_msg, sizeof(write_msg), 0, 0) == -1) {
					perror("msgrcv");
					exit(5);
				}
				
				count = count - (MAX_USERS + 1);	
				
				
				
				
			}else if(num_messages_read != 0){
				if (msgrcv(read_request, &t, sizeof(token), 1, 0) == -1) {
					perror("msgrcv");
					exit(5);
				}
				
				
				
				if (msgsnd(read_request, &t2, sizeof(token), 0) == -1) 
				{
					perror("msgsndqwe");
					exit(6);
				}
				
				count--;
				
			}		
		}
		
		
		if(count == 0){
			
			
			if(w_msg.mtype == UPVOTE){
				up_vote(w_msg.id);
				
			}
			
			
			
			if(w_msg.mtype == DOWNVOTE){
				down_vote(w_msg.id);
				
			}
			
			
			if(w_msg.mtype == ADDMAK){
				add_mak(w_msg.message);
				
			}
			
			count = MAX_USERS+1;
			
			
			
		}
		
		
		
		while(count < 0){
			
			if (msgrcv(finish, &t, sizeof(token), 0, 0) == -1) {
				perror("msgrcv");
				exit(5);
			}
			count++;
		}
		
		
		
		
	}//end while
	
	
	
	
	
	
	
	
}