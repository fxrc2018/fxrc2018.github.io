## Shell Lab

```cpp
void eval(char *cmdline) 
{
    char *argv[MAXARGS];
    char buf[MAXLINE];

    int bg;
    pid_t pid;
    strcpy(buf,cmdline);

    bg = parseline(buf,argv);
    if(argv[0] == NULL){
        return;
    }

    if( !builtin_cmd(argv) ){
        /* parent process block SIGCHLD signal before call fork*/
        sigset_t mask,pre_mask;
        sigemptyset(&mask);
        sigaddset(&mask,SIGCHLD);
        sigprocmask(SIG_BLOCK,&mask,&pre_mask);

        pid = fork();
        if(pid < 0){
            unix_error("fork error");
        }
        if(pid == 0){
            /* child process unblock SIGCHLD signal before call execve */
            sigprocmask(SIG_SETMASK,&pre_mask,NULL);
            setpgid(0,0);
            if( execve(argv[0],argv,environ) < 0){
                printf("%s: Command not found.\n",argv[0]);
                exit(0);
            }
        }
        addjob(jobs,pid,bg?BG:FG,cmdline);
        /* parent process unblock SIGCHLD signal after call addjob */
        sigprocmask(SIG_SETMASK,&pre_mask,NULL);
        if( !bg ){
            /* wait for child process */
            waitfg(pid);
        }else{
            struct job_t *job;
            job = getjobpid(jobs,pid);
            printf("[%d] (%d) %s",job->jid,job->pid,job->cmdline);
        }
    }

    return;
}
```

```cpp
int builtin_cmd(char **argv) 
{
    if( !strcmp(argv[0],"quit") ){
        exit(0);
    }
    if( !strcmp(argv[0],"jobs") ){
        listjobs(jobs);
        return 1;
    }
    if(!strcmp(argv[0],"kill") ){
        if(argv[1] != NULL){

        }
        return 1;
    }
    if( !strcmp(argv[0],"bg") || !strcmp(argv[0],"fg") ){
        do_bgfg(argv);
        return 1;
    }

    return 0;     /* not a builtin command */
}
```

```cpp
void do_bgfg(char **argv) 
{
    if(argv[1] == NULL){
        printf("%s command requires PID or %%jobid argument\n",argv[0]);
        return;
    }
    int bg,isjob,jid;
    pid_t pid;
    if( !strcmp(argv[0],"bg") ){
        bg = 1;
    }else{
        bg = 0;
    }
    struct job_t *job = NULL;
    if( *(argv[1]) == '%' ){
        isjob = 1;
        /* check whether all the char is number*/
        jid = atoi(argv[1] + 1);
        /* if atoi function fail, the return is 0 */
        if(jid == 0){
            printf("%s: argument must be a PID or %%jobid\n",argv[0]);
            return;
        }
        job = getjobjid(jobs,jid);
    }else{
        isjob = 0;
        pid = atoi(argv[1]);
        if(pid == 0){
            printf("%s: argument must be a PID or %%jobid\n",argv[0]);
            return;
        }
        job = getjobpid(jobs,pid);
    }
    
    if(job == NULL){
        if(isjob){
            printf("%%%d: No such job\n",jid);
        }else{
            printf("(%d): No such process\n",pid);
        }
        return;
    }
    if(bg){
        printf("[%d] (%d) %s",job->jid,job->pid,job->cmdline);
        job->state = BG;
        kill(-(job->pid),SIGCONT);
    }else{
        job->state = FG;
        /* the difference between bg and fg are the shell waiting for the bg.
         * them both running in another process */
        kill(-(job->pid),SIGCONT);
        waitfg(job->pid);
    }

    return;
}
```

```cpp
void waitfg(pid_t pid)
{
    /* reap child always in signal handler,here just wait for global varien change */
    while(pid == fgpid(jobs) ){
        sleep(0);
    }
    return;
}
```

```cpp
void sigchld_handler(int sig) 
{
    pid_t pid;
    int status;
    while( (pid = waitpid(-1,&status,WNOHANG | WUNTRACED) )>0 ){
        int sig;
        if( WSTOPSIG(status) ){
            sig = WSTOPSIG(status);
            printf("Job [%d] (%d) stopped by signal %d\n",pid2jid(pid),pid,SIGTSTP);
            getjobpid(jobs,pid)->state = ST;
        }else if(WIFSIGNALED(status)){
            sig = WTERMSIG(status);
            printf("Job [%d] (%d) teminated by signal %d\n",pid2jid(pid),pid,sig);
            deletejob(jobs,pid);
        }else if(WIFEXITED(status)){
            deletejob(jobs,pid);
        }
    }
    return;
}
```

```cpp
void sigtstp_handler(int sig) 
{
    //printf("catch SIGTSTP");
    /* this pid is not belong to shell */
    pid_t pid = fgpid(jobs);
    if(pid > 0){
        int ret = kill(-pid,SIGTSTP);
        if(ret == -1){
            unix_error("kill error");
        }
        
        //deletejob(jobs,pid);
    }
    return;
}
```

```cpp
void sigint_handler(int sig) 
{
    /* maintain errno unchanged */
    int temp = errno;
    /* this pid is not belong to shell */
    pid_t pid = fgpid(jobs);
    if(pid > 0){
        int ret = kill(-pid,SIGINT);
        if(ret == -1){
            unix_error("kill error");
        }
    }
    errno = temp;
    return;
}
```
