#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <string>


std::string get_stime(void)
{ 
    char timestr[200] ={0};
    struct tm * pTempTm;
    struct timeval time;
        
    gettimeofday(&time,NULL);
    pTempTm = localtime(&time.tv_sec);
    if( NULL != pTempTm )
    {
        snprintf(timestr,199,"%04d-%02d-%02d %02d:%02d:%02d.%03ld",
            pTempTm->tm_year+1900,
            pTempTm->tm_mon+1, 
            pTempTm->tm_mday,
            pTempTm->tm_hour, 
            pTempTm->tm_min, 
            pTempTm->tm_sec,
            time.tv_usec/1000);
    }
    std::string retval= std::string(timestr);
    return retval;
}