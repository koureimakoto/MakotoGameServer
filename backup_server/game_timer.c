#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

int main() {
        // setitimerにより通知されるシグナルをマスク
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGALRM);
        pthread_sigmask(SIG_BLOCK, &sigset, NULL);

        // タイマ設定　５秒毎に通知
        timer_t         timerid;
        const struct timespec interval = { 5, 0 };  // Primeiro são segundo e o segundo sao nanosegundos
        const struct timespec inter    = { 100, 0 };
        const struct itimerspec itimer = {interval, inter};
        int ret = timer_create(CLOCK_REALTIME, NULL, &timerid);
        if( ret == -1) {
                perror("timer_create");
                return -1;
        }
        ret = timer_settime(timerid, TIMER_ABSTIME, &itimer, NULL);
        if ( ret != 0 ) {
                perror("timer_settime");
                return -1;
        }

        int i;
        for ( i=0; i<10; i++ ) {
                int signo;
                sigwait(&sigset, &signo);

                struct timeval tv;
                gettimeofday(&tv, NULL);
                char    timebuf[26];
                ctime_r(&tv.tv_sec, timebuf);
                *strchr(timebuf, '\n') = '\0';
                printf("[%s] [%06d] signal[%d] recieved.\n",
                        timebuf, tv.tv_usec, signo);

                // たまに時間かかる処理がある想定
                /*if ( i % 4 == 3 ) {
                        unsigned int waittime = 0;
                        printf("wait %d seconds.\n", waittime);
                        sleep(waittime);
                }*/
        }

        ret = timer_delete(timerid);
        if ( ret != 0 ) {
                perror("timer_delete");
                return -1;
        }

        return 0;
}
