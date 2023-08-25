/*********************************
 * Reviewer:
 * Author: Daniel Binyamin
 * File: wdlib.c
 * ******************************/

/* #define _POSIX_C_SOURCE 199506L */ /* sigaction */
#define _POSIX_C_SOURCE 200112L       /* sigaction */
#define _XOPEN_SOURCE 500             /* thresds   */
#define SEM_SHOTDOWN_NAME "/shutdown_sem"
#define SEM_IS_READY_NAME "/is_ready_sem"

#include <stdio.h>  /* printf             */
#include <stdlib.h> /* envirmant varble   */
#include <signal.h> /* signal             */
#include <string.h> /* strcmp             */

#include <pthread.h>   /* threads            */
#include <sys/types.h> /* pid                */
#include <unistd.h>    /* ppid, wait         */
#include <sys/wait.h>  /* wait               */

#include <semaphore.h> /* semaphore          */
#include <fcntl.h>     /* For O_* constants  */
#include <sys/stat.h>  /* For mode constants */

#include "uid.h"       /* UIDIsSame          */
#include "scheduler.h" /* srchduler          */
#include "wd.h"

enum status
{
    SUCESSES,
    FAILED
};
enum flag_status
{
    FLAG_OFF,
    FLAG_ON
};
enum app_name
{
    WD,
    USER_APP
};

char *apps_names[2] = {0};

void GetHeartBeats(int sig, siginfo_t *info, void *cont);
void GetShutDownBeat(int sig, siginfo_t *info, void *cont);
void *BackGroundFunc(void *scdlr);
static void Cleanup(admin_t *admin);
static int AmIUserApp();
static int SendSig(void *arg);
static int CheckSig(void *arg);
static int SchedulerInit(admin_t *admin);
int CreateProcess(admin_t *admin, char *proc_to_exc, char *user_app_name);

volatile int heart_beat = FLAG_OFF;
volatile int shutdown = FLAG_OFF;
pid_t pid;

struct adminstruct
{
    sem_t *sem_is_ready;
    sem_t *sem_is_shutdown;
    pid_t sig_target_pid;
    scheduler_t *scdlr;
    pthread_t bg_thread;
};

void printState(char *state, pid_t curr_pid, pid_t parent_pid)
{
    /* sleep(2); */
    printf("%s \ncpid= %d \nppid= %d \n\n", state, curr_pid, parent_pid);
}

admin_t *StructInit(admin_t *ad)
{
    ad->bg_thread = 0;
    /*  ad->pid = 0; */
    ad->scdlr = NULL;
    ad->sem_is_ready = 0;
    ad->sem_is_shutdown = 0;
    ad->sig_target_pid = 0;
    return ad;
}

admin_t *MMI(int argc, char *argv)
{
    struct sigaction heart_beat_sig_action = {0};
    struct sigaction shutdown_sig_action = {0};
    admin_t *admin = NULL;

    apps_names[0] = "wd.Debug.out";
    apps_names[1] = argv;

    (void)argc;

    admin = (admin_t *)malloc(sizeof(admin_t));
    admin = StructInit(admin);

    /* SIGUSR1 heandler init */
    heart_beat_sig_action.sa_sigaction = GetHeartBeats;
    heart_beat_sig_action.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &heart_beat_sig_action, NULL);

    /* SIGUSR2 heandler init */
    shutdown_sig_action.sa_sigaction = GetShutDownBeat;
    shutdown_sig_action.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &shutdown_sig_action, NULL);

    /* open semaphore(or create if not exists) give it 666 premition */
    admin->sem_is_ready = sem_open(SEM_IS_READY_NAME, O_CREAT, 0666, 0);
    if (!admin->sem_is_ready)
    {
        fprintf(stderr, "sem_is_ready failed\n");
        return NULL;
    }

    if (!(admin->scdlr = SchedulerCreate()))
    {
        fprintf(stderr, "scheduler creation failed\n");
        return NULL;
    }

    if (SchedulerInit(admin))
    {
        fprintf(stderr, "sceduler init failed\n");
        return NULL;
    }

    printState("MMI: init done", getpid(), getppid());

    /* chack if env var exist,
       if not its the first run- create WD by default*/
    if (!getenv(ENV_NAME))
    {
        printState("MMI: WD creation", getpid(), getppid());

        /* create new process to run WD */
        admin->sig_target_pid = CreateProcess(admin, apps_names[WD], apps_names[USER_APP]);

        /* create new thread to run WD scheduler */
        printState("MMI: parent pleace", getpid(), getppid());
        pthread_create(&admin->bg_thread, NULL, BackGroundFunc, (void *)admin);
    }
    /* if env var exist, just create thread to run the scheduler*/
    else
    {
        admin->sig_target_pid = getppid();

        printState("MMI: the exec activate BG", getpid(), getppid());
        pthread_create(&admin->bg_thread, NULL, BackGroundFunc, (void *)admin);
    }

    return admin;
}

/* runs the scherduler in the backgraound thresd */
void *BackGroundFunc(void *admin)
{
    scheduler_t *bg_scdlr = ((admin_t *)admin)->scdlr;
    int scdulr_return_value = 0;

    /* in the child proc */
    /*  if (((admin_t *)admin)->pid == 0) */
    if(!getenv(ENV_NAME))
    {
        /* otherwise its the parent - then wait */
        printState("BGF: parent is waitting", getpid(), getppid());
        sem_wait(((admin_t *)admin)->sem_is_ready);
        printState("BGF: parent continuse", getpid(), getppid());
       
    }
    else
    {
        /* post the wattig parant */
        printState("BGF: child is posting", getpid(), getppid());
        sem_post(((admin_t *)admin)->sem_is_ready);
        printState("BGF: child next move", getpid(), getppid());
    }
    while(1)
    {

        printf("alive? i am %d\n", getpid());
        scdulr_return_value = SchedulerRun(bg_scdlr);
        if (FAILED == scdulr_return_value)
        {
            return NULL;
        }

        if (shutdown)
        {
            printState("BGF: shutdown has activated", getpid(), getppid());
            sem_post(((admin_t *)admin)->sem_is_shutdown);
            printState("BGF: shutdown posted", getpid(), getppid());
            break;
        }
        printState("BGF: resuscitating", getpid(), getppid());
        
        /*  if the prog got this state,
            one of the processes died,
            do wait for the child process */
        wait(NULL);

        /* check witch of the Apps got here */
        if (AmIUserApp())
        {
            /* USER APP resuscitating the WD*/
            printf("1*****************i am %s , resuscitating -> %s \n", apps_names[USER_APP], apps_names[WD]);
            if (FAILED == CreateProcess(admin, apps_names[WD], apps_names[USER_APP]))
            {
                fprintf(stderr, "resuscitating %s FAILED\n", apps_names[WD]);
                return NULL;
            }
        }
        else
        {
            /* WD resuscitating the USER APP*/
            printf("2*****************i am %s , resuscitating -> %s \n", apps_names[WD], apps_names[USER_APP]);
            if (FAILED == CreateProcess(admin, apps_names[USER_APP], apps_names[USER_APP]))
            {
                fprintf(stderr, "resuscitating %s FAILED\n", apps_names[USER_APP]);
                return NULL;
            }
        }

        printState("CheckSig: the new parent is waitting", getpid(), getppid());
        /* wait for the new process to register the sig heandler */
        sem_wait(((admin_t *)admin)->sem_is_ready);
        printState("CheckSig:the new parent continuse", getpid(), getppid());
    }
    printState("BGF: breaked from the main loop (and call cleanup)", getpid(), getppid());

    Cleanup((admin_t *)admin);
    return NULL;
}

/* create new process - fork and exec at the child proc*/
int CreateProcess(admin_t *admin, char *proc_to_exc, char *user_app_name)
{
    printState("CreateProcess: activated", getpid(), getppid());

    /*  admin->pid = fork(); */
    pid = fork();
    if (pid < 0) /* fork failed */
    {
        printf("FORK FAILED\n");
        return FAILED;
    }
    if (pid == 0) /* we  are in the child process */
    {
        printf("curr = %s , other = %s", proc_to_exc, user_app_name);
        execlp(proc_to_exc, proc_to_exc, user_app_name, NULL);
    }
    else /* we  are in the parent process */
    {
        admin->sig_target_pid = pid;
    }

    return pid;
}

int SchedulerInit(admin_t *admin)
{

    if (UIDIsSame(g_bad_uid, SchedulerAddTask(((admin_t *)admin)->scdlr,
                                              SendSig,
                                              (void *)admin,
                                              1)))
    {
        return FAILED;
    }

    if (UIDIsSame(g_bad_uid, SchedulerAddTask(((admin_t *)admin)->scdlr,
                                              CheckSig,
                                              (void *)admin,
                                              5)))
    {
        return FAILED;
    }
    return SUCESSES;
}

/* sends signal to the targeted process */
int SendSig(void *admin)
{
    (void)((admin_t *)admin)->scdlr;
    kill(((admin_t *)admin)->sig_target_pid, SIGUSR1);
    printf("%d ***************************************> send to %d \n", getpid(), ((admin_t *)admin)->sig_target_pid);

    return SUCESSES;
}

/* checks if the curr process got signal */
int CheckSig(void *admin)
{
    if (heart_beat == FLAG_ON && shutdown == FLAG_OFF)
    {
        heart_beat = FLAG_OFF;
        printf("%d bippp \n", getpid());

        return SUCESSES;
    }
    /*  if no signal (X times) then the other process died,
        or the USER APP call DNR(),
        stop the scherduler*/
    SchedulerStop(((admin_t *)admin)->scdlr);
    printState("CheckSig: scheduler stops", getpid(), getppid());

    return SUCESSES;
}

/* check it the curr process is the USER APP, using the env var */
int AmIUserApp()
{
    return (!getenv(ENV_NAME) ||
         (getpid() != atoi(getenv(ENV_NAME))));
}

/* signal heandler func for SIGUSE1*/
void GetHeartBeats(int sig, siginfo_t *info, void *cont)
{
    (void)sig;
    (void)info;
    (void)cont;
    heart_beat = FLAG_ON;
}

/* signal heandler func for SIGUSE2*/
void GetShutDownBeat(int sig, siginfo_t *info, void *cont)
{

    (void)sig;
    (void)info;
    (void)cont;
    shutdown = FLAG_ON;
}

void DNR(admin_t *admin)
{
    struct timespec abs_timeout = {0};
    int i = 0, sem_res = 1;
    printState("DNR: started", getpid(), getppid());

    admin->sem_is_shutdown = sem_open(SEM_SHOTDOWN_NAME, O_CREAT, 0666, 0);
    if (!admin->sem_is_shutdown)
    {
        fprintf(stderr, "sem_is_shutdown failed\n");
        return;
    }

    if (AmIUserApp())
    {

        for (i = 0; (i < 4) && (sem_res != 0); ++i)
        {
            shutdown = FLAG_ON;
            abs_timeout.tv_sec = time(NULL) + 5;
            printState("DNR: shutdown beat has been sent", getpid(), getppid());

            kill(admin->sig_target_pid, SIGUSR2);

            printState("DNR: is time wating", getpid(), getppid());
            while(0 != sem_res )
            {
                sem_res = sem_timedwait(admin->sem_is_shutdown, &abs_timeout);
            }
            printState("DNR: posted", getpid(), getppid());
        }
    }
    printState("DNR: starts its own cleanup", getpid(), getppid());
    pthread_join(admin->bg_thread, NULL);
}

void Cleanup(admin_t *admin)
{

    SchedulerDestroy(admin->scdlr);
    printState("Clean: scduler destroy", getpid(), getppid());
    
    sem_close(admin->sem_is_ready);
    sem_unlink(SEM_IS_READY_NAME);
    printState("Clean: sem is_ready destroy", getpid(), getppid());

    sem_close(admin->sem_is_shutdown);
    sem_unlink(SEM_SHOTDOWN_NAME);
    printState("Clean: sem is_shutdown destroy", getpid(), getppid());


    StructInit(admin);
    free(admin);
    printState("Clean: admin struct destroy", getpid(), getppid());

}