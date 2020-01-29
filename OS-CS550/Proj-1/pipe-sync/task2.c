#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define print_usage(cmd) printf("Usage: %s 1|2|3\n", cmd)

int pipeEnds1[2];  // Declare two ends of first pipe 
int pipeEnds2[2];  // Declare two ends of second pipe 
int pipeEnds3[2];  // Declare two ends of third pipe 
int pipeEnds4[2];  // Declare two ends of fourth pipe 
int pipeEnds5[2];  // Declare two ends of fifth pipe 
int pipeEnds6[2];  // Declare two ends of sixth pipe

pid_t fk1,fk2,fk3;
 

int main(int argc, char * argv[]) 
{
	int option = 0;

	setbuf(stdout, NULL);

    	if (argc < 2)
    	{
        	print_usage(argv[0]);
        	return 1;
    	}

    	option = atoi(argv[1]);
    	if (option != 1 && option != 2 && option != 3)
    	{
    		print_usage(argv[0]);
        	return 1;        
    	}
	else
	{
		// Code Implementation
		if(pipe(pipeEnds1) == -1)
		{
			fprintf(stderr, "Pipe-1 not working");
			return 1;
		}
		if(pipe(pipeEnds2) == -1)
		{
			fprintf(stderr, "Pipe-2 not working");
			return 1;	
		}
		if(pipe(pipeEnds3) == -1)
		{
			fprintf(stderr, "Pipe-3 not working");
			return 1;	
		}
		if(pipe(pipeEnds4) == -1)
		{
			fprintf(stderr, "Pipe-4 not working");
			return 1;	
		}
		if(pipe(pipeEnds5) == -1)
		{
			fprintf(stderr, "Pipe-5 not working");
			return 1;	
		}
		if(pipe(pipeEnds6) == -1)
		{
			fprintf(stderr, "Pipe-6 not working");
			return 1;	
		}

		// Create Child Processes
		fk1 = fork();
		if(fk1<0)
		{
			fprintf(stderr, "Fork 1 not working");
			return 1;
		}	
		else
		{
			if(fk1 == 0)
			{
        			close(pipeEnds1[1]); // Close writing end of first pipe

        			// Read a string using first pipe 
        			char received_str[100]; 
        			read(pipeEnds1[0], received_str, 100); 
        			printf("Child-1 (PID= %d) is running.\n", getpid());
	
        			// Close both reading ends 
       				close(pipeEnds1[0]); 
        			close(pipeEnds2[0]); 

        			// Write concatenated string and close writing end 
        			write(pipeEnds2[1], "Child1", 7); 
        			close(pipeEnds2[1]); 

        			exit(0); 
    			}
		}

		fk2 = fork();
		if(fk2<0)
		{
			fprintf(stderr, "Fork 2 not working");
			return 1;
		}
		else
		{
			// child process 
    			if(fk2 == 0)
    			{ 
        			close(pipeEnds3[1]); // Close writing end of third pipe
        			// Read a string using third pipe 
        			char received_str[100]; 
        			read(pipeEnds3[0], received_str, 100); 
        			printf("Child-2 (PID= %d) is running.\n", getpid());

        			// Close both reading ends 
        			close(pipeEnds3[0]); 
        			close(pipeEnds4[0]); 

        			// Write concatenated string and close writing end 
        			write(pipeEnds4[1], "Child2", 7); 
        			close(pipeEnds4[1]); 

        			exit(0); 
    			} 
		}

		fk3 = fork();
		if(fk3<0)
		{
			fprintf(stderr, "Fork 3 not working");
			return 1;
		}
		else
		{
			if(fk3 == 0)
    			{ 
        			close(pipeEnds5[1]); // Close writing end of fifth pipe
        			// Read a string using fifth pipe 
        			char received_str[100]; 
        			read(pipeEnds5[0], received_str, 100); 
        			printf("Child-3 (PID= %d) is running.\n", getpid());

      
        			// Close both reading ends 
        			close(pipeEnds5[0]); 
        			close(pipeEnds6[0]); 

        			// Write concatenated string and close writing end 
        			write(pipeEnds6[1], "Child3", 7); 
        			close(pipeEnds6[1]); 

        			exit(0); 
    			} 
		}

		// Parent Process
		if(fk1>0 && fk2>0 && fk3>0)
		{
			int child1, child2, child3;
            		char received_str[100];
            		close(pipeEnds1[0]); // Close reading end of first pipe 
            		close(pipeEnds3[0]); // Close reading end of third pipe 
            		close(pipeEnds5[0]); // Close reading end of fifth pipe 

            		//Check the input option
            		switch(option){
                	case 1:
                    		//Parent writes to the first child to be executed
                    		write(pipeEnds5[1], "parent3", 8);
                    		close(pipeEnds5[1]);

                    		//Process ID of child is returned upon exit
                    		child3= waitpid(fk3, &fk3, 0);
                    		close(pipeEnds6[1]);

                    		//Child reads the message
                    		read(pipeEnds6[0], received_str, 100);
                    
                    		//Same procedure
                    		write(pipeEnds3[1], "parent2", 8);
                    		close(pipeEnds3[1]);

                    		child2 = waitpid(fk2, &fk2, 0);
                    		close(pipeEnds4[1]);  
                    		read(pipeEnds4[0], received_str, 100); 

                    		write(pipeEnds1[1], "parent1", 8);
                    		close(pipeEnds1[1]);

                    		child1 = waitpid(fk1, &fk1, 0);
                    		close(pipeEnds2[1]); 
                    		read(pipeEnds2[0], received_str, 100);
                	break;

                	case 2:
                    		write(pipeEnds3[1], "parent2", 8);
                    		close(pipeEnds3[1]);

                    		child2 = waitpid(fk2, &fk2, 0);
                    		close(pipeEnds4[1]);  
                    		read(pipeEnds4[0], received_str, 100); 

                    		write(pipeEnds1[1], "parent1", 8);
                    		close(pipeEnds1[1]);

                    		child1 = waitpid(fk1, &fk1, 0);
                    		close(pipeEnds2[1]); 
                    		read(pipeEnds2[0], received_str, 100);

                    		write(pipeEnds5[1], "parent3", 8);
                    		close(pipeEnds5[1]);

                    		child3= waitpid(fk3, &fk3, 0);
                    		close(pipeEnds6[1]); 
                    		read(pipeEnds6[0], received_str, 100);
                	break;

                	case 3:
                    		write(pipeEnds5[1], "parent3", 8);
                    		close(pipeEnds5[1]);

                    		child3= waitpid(fk3, &fk3, 0);
                    		close(pipeEnds6[1]); 
                    		read(pipeEnds6[0], received_str, 100);

                    		write(pipeEnds1[1], "parent1", 8);
                    		close(pipeEnds1[1]); 

                    		child1 = waitpid(fk1, &fk1, 0);
                    		close(pipeEnds2[1]); 
                    		read(pipeEnds2[0], received_str, 100);

                    		write(pipeEnds3[1], "parent2", 8);
                    		close(pipeEnds3[1]);

                    		child2 = waitpid(fk2, &fk2, 0);
                    		close(pipeEnds4[1]);  
                    		read(pipeEnds4[0], received_str, 100); 
                	break;
            		}

            	//Print final messages
            	printf("In Parent (PID= %d): successfully reaped child (PID= %d)\n", getpid(), child1);
            	printf("In Parent (PID= %d): successfully reaped child (PID= %d)\n", getpid(), child2);
            	printf("In Parent (PID= %d): successfully reaped child (PID= %d)\n", getpid(), child3);
            
        	}  

		
	}

    	return 0;
}
