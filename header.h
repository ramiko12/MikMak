#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_USERS 1
#define MAX_MAKS 10 // the maximum number of maks stored on the server

typedef struct {
    long mtype;       /* message type, must be > 0 */
}token;


typedef struct {
	long mtype;
	int id;
	char message[51]; //extra 1 for '\0'
	
}write_msg;


typedef struct {
	
    int id;
	char mak_text[51];
	int vote_count;
	
}mak;



//args for controller function
typedef struct 
{
	int read_request;
	int write_request;
	
	int up_vote_request;
	int down_vote_request;
	int add_mak_request;
	
	
	int finish;
	
	
	
}controller_args;

//args for up_vote function
typedef struct 
{
	int up_vote_request;
	
	int finish;
	
	mak * mak_list;
	
}up_vote_args;



//args for the down vote function
typedef struct 
{
	int down_vote_request;
	
	int finish;
	
	mak * mak_list;
	
}down_vote_args;


//args for the add mak function
typedef struct 
{
	int add_mak_request;
	
	int finish;
	
	mak * mak_list;
	
}add_mak_args;





