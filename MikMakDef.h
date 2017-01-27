#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#define MAX_USERS 2
#define MAX_MAKS 10 // the maximum number of maks stored on the server
#define UPVOTE 1
#define DOWNVOTE 2
#define ADDMAK 3
#define DONEWRITE 4

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






