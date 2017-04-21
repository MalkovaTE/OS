#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

static char* path_log_file = (char*)"/home/tanya/Documents/daemon.log";
static char* serial = (char*)"36UUGDC5VEG79P2Z";
static char* dir_serial1 = (char*)"/media/tanya/EC1667C216678C80/";

char* getSerial( char *str ) 
{
    ssize_t len;
    char buf[256];
    char *p;
    char buf2[256];
    int i;
    static char comText[256];
    bzero(comText, 256);
    strcpy(comText, "");

    len = readlink(str, buf, 256);
    if (len <= 0) {
        return (char*)"-";
    }
    buf[len] = '\0';
    sprintf(buf2, "%s/%s", "/sys/block/", buf);
    for (i=0; i<6; i++) {
        p = strrchr(buf2, '/');
        *p = 0;
    }
    strcat(buf2, "/serial");

    int f = open(buf2, 0);
    if (f == -1) return (char*)"-";
    len = read(f, buf, 256);
    if (len <= 0) {
        return (char*)"-";
    }
    buf[len-1] = '\0';
    strcat(comText, buf);

    return comText;
}

int isSerial(char* ser)
{
    if (strcmp(getSerial((char*)"/sys/block/sdb"), ser) == 0
        || strcmp(getSerial((char*)"/sys/block/sdc"), ser) == 0
        || strcmp(getSerial((char*)"/sys/block/sdd"), ser) == 0
        || strcmp(getSerial((char*)"/sys/block/sdf"), ser) == 0
        || strcmp(getSerial((char*)"/sys/block/sdg"), ser) == 0)
    {
        return 1;
    }
    return 0;
}


int main(int argc, char* argv[])
{
FILE *fp= NULL;
pid_t process_id = 0;
pid_t sid = 0;
// Create child process
process_id = fork();
// Indication of fork() failure
if (process_id < 0)
{
printf("fork failed!\n");
// Return failure in exit status
exit(1);
}
// PARENT PROCESS. Need to kill it.
if (process_id > 0)
{
printf("process_id of child process %d \n", process_id);
// return success in exit status
exit(0);
}
//unmask the file mode
umask(0);
//set new session
sid = setsid();
if(sid < 0)
{
// Return failure
exit(1);
}


chdir("/");
// Close stdin. stdout and stderr
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
// Open a log file in write mode.
fp = fopen (path_log_file, "w+");
while (1)
{

sleep(1);
if(isSerial(serial)) {
time_t t=time(NULL);
struct tm tm= *localtime (&t);

fprintf(fp, "Correct USB in %d-%d-%d-%d \n", tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}
else {time_t t=time(NULL);
struct tm tm= *localtime (&t);

fprintf(fp, "No USB in %d-%d-%d-%d \n", tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);}
fflush(fp);

}
fclose(fp);
return (0);
}