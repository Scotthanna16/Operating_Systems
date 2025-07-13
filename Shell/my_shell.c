#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64


/* PLEASE READ:
Issues with this Program
If needed please feell free to change the size of the 
arrays BPIDS, FPIDS, PPIDS. I realize now that imple-
menting such functionality would've been more efficient 
if I used a linked list, but for the scope of this
assignment the global arrays should work.
*/

//Global variables to keep track of Background PIDS
int biterator=0;
int BPIDS[200];

//Global variables to keep track of foreground PIDS
int fiterator=0;
int FPIDS[200];

int piterator=0;
int PPIDS[200];

//Struct for commands to be executed
typedef struct{
	char **commands;
	bool backg;
	bool parallel;
} mycommand_t;

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;
  for(i =0; i < strlen(line); i++){
    char readChar = line[i];
    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

/* Execute executes every command excpet for exit and cd
*/
void *execute(void *com){
	mycommand_t *coms= (mycommand_t *)com;

	if(strcmp("cd", coms->commands[0])==0){
		//char s[100];
		//getcwd(s,100);
		int error =chdir(coms->commands[1]);
		if(error !=0){
			perror("");
		}
		return NULL;
	}
	
	// Checking if a background process has been completed
	if (coms->backg ==false){
		for(int i = 0; i<=biterator;i++){
			if(BPIDS[i]!=0){
				int cpid= waitpid(BPIDS[i], NULL, WNOHANG);
				if (cpid!=0){

					printf("Shell: Background process completed\n");
					BPIDS[i]=0;
					

				}
			}
			
		}
	}
	

	 int child = fork();
        if(child < 0){
            fprintf(stderr, "Fork failed");
        }
		//fprintf(stderr,"%s \t", coms->commands[0]);
        if(child ==0){
        	//printf("hello, I am child (pid:%d)\n", (int) getpid());
            //Check for cd
			
			if (strcmp("cd", coms->commands[0])!=0){
				
				execvp(coms->commands[0], coms->commands);
				
				printf("Shell: Incorrect Command\n");
				perror("");
				int cpid=getpid();
				kill(cpid,SIGKILL);
			}
            //Terminate process if we get here
        }
        else{
			//If parallel, dont wait
			if(coms->parallel == true){
				//wait(NULL);
				PPIDS[piterator]=child;
				piterator++;
					
			}
			//if not background wait
			else if(coms->backg == false){
				FPIDS[fiterator]=child;
				
				fiterator++;
            	waitpid(child, NULL, 0);
			}
			//if Background dont wait, change GPID
			else{
				BPIDS[biterator]=child;
				setpgid(child,BPIDS[0]);
				
				biterator++;
			}
        }
	return NULL;

}

//Function to execute exit command
int exitcom(){
	//Check if PIDS are still active
	//If they are kill them
	int i =0;
	while (i<64){
		if(BPIDS[i]!=0){
			if (kill(BPIDS[i],0)==0){
				kill(BPIDS[i],SIGTERM);
				waitpid(BPIDS[i], NULL, 0);
			}
		}
		i++;
	}
	
	return 0;
}

//Function to hand Ctrl C
void CtrlChandler(){
	int i =0;
	while (i<64){
		if(FPIDS[i]!=0){
			if (kill(FPIDS[i],0)==0){
				kill(FPIDS[i],SIGTERM);
				waitpid(FPIDS[i], NULL, 0);
			}
		}
		i++;
	}
	
}


int main(int argc, char* argv[]) {
	//Ctrl C handler
	signal(SIGINT, CtrlChandler);

	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;

	//Booleans used to check if background or exit command is being used
	bool backg=false;
	bool exitcommand=false;

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {		
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		//printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		//Split the array and then feed multiple commands into the function, if &&& each command is executed by seperate thread
       //do whatever you want with the commands, here we just print them
	    //printf("hello world (pid:%d)\n", (int) getpid());
		if (tokens[0]!=NULL){
			int count =0;
			int count2 = 0;
			for(i=0;tokens[i]!=NULL;i++){
				//printf("found token %s (remove this debug output later)\n", tokens[i]);
				// Checking how many, if any, back-to-back commands exist
				if(strcmp(tokens[i],"&&")==0){
					count++;
				}
				//checking how many, if any, parallel commands exist
				if(strcmp(tokens[i],"&&&")==0){
					count2++;
				}
			}
			//If no parallel commands exist
			if(count2==0){
				//Check if the process is a background process
				if(strcmp(tokens[i-1],"&")==0){
					backg=true;
					tokens[i-1]=NULL;
				}
				//check if it is an exit command
				if(strcmp("exit", tokens[0])==0){
					exitcom();
					exitcommand=true;
					goto CLEAN;

				}
				//check if it is a cd command
				
				// back to back commands
				if(count!=0){
					int i2=0;		
					for(int j =0; j<=count; j++){		
						char **commands = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
						int i3=0;
						while(tokens[i2]!=NULL && strcmp(tokens[i2],"&&")!=0){
							commands[i3]=tokens[i2];
							i2++;
							i3++;
						}				
						i2++;
						mycommand_t command ={commands, false, false};
						execute(&command);
						
						free(commands);				
					}			
				}
			
				// There is a background command
				if(count ==0){
					mycommand_t command ={tokens,backg, false};
						execute(&command);
				}
				backg=false;
			}
			else{
				// IF &&&, split on the commands and then feed them into seperate threads		
				int i2=0;		
					for(int j =0; j<=count2; j++){		
						char **commands = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
						int i3=0;
						while(tokens[i2]!=NULL && strcmp(tokens[i2],"&&&")!=0){
							commands[i3]=tokens[i2];
							i2++;
							i3++;
						}				
						i2++;
						mycommand_t command ={commands, false, true};
						execute(&command);
						
						free(commands);				
					}
					for(int x=0;x<=piterator;x++){
						waitpid(PPIDS[x],NULL,0);
						PPIDS[x]=0;
					}			
					piterator=0;


			}
			CLEAN:
			// Freeing the allocated memory	
			for(i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
		}
		free(tokens);
		if (exitcommand==true){
			break;
		}
	}

	return 0;
}