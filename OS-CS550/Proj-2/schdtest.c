#include "types.h"
#include "user.h"

#define P_LOOP_CNT 0x01000000
#define C_LOOP_CNT 0x03000000


void do_parent(void)
{
    volatile int cnt = 0;
    volatile int tmp = 0;

    while(cnt < P_LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
}


void do_child(void)
{
    volatile int cnt = 0;
    volatile int tmp = 0;

    while(cnt < C_LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }

    exit();
}

void example_test_code()
{
    int pid = 0;

    pid = fork();
    if (pid < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid == 0) // child
    {
        //sleep(100);
        do_child();
    }
    else // parent
    {
        do_parent();
        if (wait() < 0)
        {
            printf(1, "wait() failed!\n");
        }
    }
	
	printf(1, "\n");
}

void example_test_code2()
{
    int pid;
   // set_priority(getpid(), 0);
   /* for(int i=0; i <5 ; i++)
    {
    pid = fork();
    if (pid == 0) // child
    {
        sleep(20);
        do_child();
    } 
    else 
    {
        printf(1, "proc %d created priority: %d\n", pid, i);
        set_priority(pid, i);
    }
    }
    while(wait()!= -1)
        set_priority(getpid(),0);*/
    pid=fork();
    if (pid < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),1);
        do_child();
    }
    else
    {
        wait();
        set_priority(getpid(),-1);
        do_parent();
    }
}

void example_test_code3()
{
   int pid1,pid2,pid3;
   pid1 = fork();
    if (pid1 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid1 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),1);
        do_child();
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid2 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),0);
        do_child();
    }

    pid3 = fork();
    if (pid3 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid3 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),0);
        do_child();
    }   
    else
    {
        if(pid1>0 && pid2>0 && pid3>0)
        {
            for(int i=0; i<3; i++)
            {
                wait();
                set_priority(getpid(),-1);
                do_parent();
            }
        }
    } 
}


void example_test_code4()
{
    int pid1,pid2,pid3,pid4;
   pid1 = fork();
    if (pid1 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid1 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),1);
        do_child();
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid2 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),1);
        do_child();
    }

    pid3 = fork();
    if (pid3 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid3 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),0);
        do_child();
    } 
   
    pid4 = fork();
    if (pid4 < 0)
    {
        printf(1, "fork() failed!\n");
        exit();
    }
    else if (pid4 == 0) // child
    {
        //sleep(100);
        set_priority(getpid(),1);
        do_child();
    }   
  
    else
    {
        if(pid1>0 && pid2>0 && pid3>0 && pid4>0)
        {
            for(int i=0; i<4; i++)
            {
                wait();
                set_priority(getpid(),-1);
                do_parent();
            }
        }
    } 
}

int
main(int argc, char *argv[])
{
    int num;
    //enable_sched_trace(1);

    /* ---------------- start: add your test code ------------------- */
     if (argc == 0)
     {
        num = 0;
        printf(1, "\nRR policy\n");
        enable_sched_trace(1);
        example_test_code();

     }
     else
     {
        num = atoi(argv[1]);
        printf(1, "\npriority based policy\n");
        enable_sched_trace(1);
        
       
          set_sched(num);
          example_test_code2(); // Test case 2
          printf("\n ******************* \n");
          example_test_code3(); // Test case 3
          printf("\n ******************* \n");
          example_test_code4();  // Test case 4 
        
        /*else
        {
          set_sched(num);
          example_test_code();  
        }*/
     }

    //set_sched(num);

   // example_test_code();

    /* ---------------- end: add your test code ------------------- */

    enable_sched_trace(0);
    
    exit();
}
