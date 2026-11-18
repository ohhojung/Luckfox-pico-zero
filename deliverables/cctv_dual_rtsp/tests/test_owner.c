#define main cctv_program_main
#include "../src/main.c"
#undef main
#include <assert.h>
#include <sys/prctl.h>
#include <sys/wait.h>
volatile sig_atomic_t stopping;
void deadline(int sig) { (void)sig; }
int run_once(int argc, char **argv, unsigned attempt) { (void)argc; (void)argv; (void)attempt; return 0; }
int main(void) {
    char *bad_args[]={"cctv","-F","184",NULL};
    assert(cctv_program_main(3,bad_args)==EXIT_FAILURE);
    assert(rkipc_active()==0);
    int ready[2]; assert(pipe(ready)==0);
    pid_t child=fork(); assert(child>=0);
    if (!child) {
        close(ready[0]);
        if (prctl(PR_SET_NAME,"rkipc",0,0,0)!=0) _exit(2);
        if (write(ready[1],"x",1)!=1) _exit(3);
        for (;;) pause();
    }
    close(ready[1]); char byte; assert(read(ready[0],&byte,1)==1); close(ready[0]);
    int detected=rkipc_active();
    kill(child,SIGTERM); waitpid(child,NULL,0);
    assert(detected==1);

    assert(cctv_program_main(3,bad_args)==EXIT_FAILURE);
    assert(rkipc_active()==0);
    puts("PASS rkipc owner guard: detected active process, cleared after exit");
    return 0;
}