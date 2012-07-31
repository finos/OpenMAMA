/* $Id: machine.c,v 1.50.16.9 2011/08/10 14:53:24 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "wombat/port.h"
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wombat/machine.h"
#include "wombat/wtable.h"

/* Macros for extract VSIZE and RSS from /proc/pid/stats files */
#define bytetok(x)      (((x) + 512) >> 10)

#define MAXPROCSARRAY 1000     /* need to keep track of processes */

extern int useLWP;
static int numProc = 0;
static pid_t procTable[MAXPROCSARRAY];
static int flagTable[MAXPROCSARRAY];

static char* skip_ws (char* p);
static char* skip_token (char* p);
static int uniqueSet (int lhs, int rhs);
static int isInSet (int lhs, int rhs, int pid);
void searchDirs (int procChildren[],
                 int dirNames[],
                 int count,
                 cpuVals* cpv,
                 int tempPid);
pid_t getParentPid (pid_t pid,
                    int childFlag);

time_t getTimeSinceEpoch(void)
{
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return tv.tv_sec;
}

static char* skip_ws(char* p)
{
    while (isspace(*p))
        p++;
    return p;
}

static char* skip_token(char* p)
{
    while (isspace(*p))
        p++;
    while (*p && !isspace(*p))
        p++;
    return p;
}

void getProcAndChildCpu(int pid, cpuVals* cpv)
{
    int i=0;
    struct cpuVals tempCpu;
    tempCpu.userTime=0;
    tempCpu.sysTime=0;
    cpv->userTime=0;
    cpv->sysTime=0;
    for(i = 0; i < numProc; i++)
    {
        getProcessInfo(procTable[i],NULL,&tempCpu,flagTable[i]);
        cpv->userTime += tempCpu.userTime;
        cpv->sysTime += tempCpu.sysTime;
    }
}

int getNumCpu(void)
{
    return sysconf (_SC_NPROCESSORS_ONLN);
}

void initProcTable(int pid, int debuginfo)
{
    DIR *dir = NULL;
    char buffer[4096] = "";
    struct dirent *ent = NULL;
    pid_t allPpidTable[MAXPROCSARRAY];
    pid_t allProcTable[MAXPROCSARRAY];
    int allFlagTable[MAXPROCSARRAY];
    pid_t tempPid        = 0;
    int numAllProc     = 0;
    int childFlag      = 0;
    int i              = 0;
    int currentIndex   = 0;
    int childrenIndex  = 0;
    pid_t pgrp = 0;
    char *dirNamePtr        = NULL;

    /* Trying to find all child process by using ppid and pid relationship.*/
    dir = opendir("/proc/");
    while ((ent = readdir(dir)) != NULL)
    {
        dirNamePtr = ent->d_name;
        if (ent->d_name[0] == '.')
        {
            dirNamePtr += 1;
            childFlag = 1;
        }
        if (isdigit(dirNamePtr[0]))
        {
            tempPid=(pid_t)strtol(dirNamePtr,NULL,10);

            if(tempPid!=pid)
            {
                allPpidTable[numAllProc] = getParentPid(tempPid,childFlag);
                if (allPpidTable[numAllProc] == 0)
                {
                    /* process must have exited, do not store information reg
                       this pid */
                    continue;
                }
                allProcTable[numAllProc] = tempPid;
                allFlagTable[numAllProc] = childFlag;
                numAllProc++;

            }
        }
        childFlag = 0;
    }
    closedir(dir);

    procTable[0]=pid;
    flagTable[0]=0;
    numProc=1;
    while(1)
    {
        for(i=0;i<numAllProc;i++)
        {
            if(allPpidTable[i]==procTable[currentIndex])
            {
                 childrenIndex++;
                 procTable[childrenIndex]=allProcTable[i];
                 flagTable[childrenIndex]=allFlagTable[i];
                 numProc++;
            }
        }
        if(currentIndex==childrenIndex)
        {
            break;
        }
        currentIndex++;
    }


    /* Trying to find all child processes by using group pid. */
    dir = opendir("/proc/");
    while ((ent = readdir(dir)) != NULL)
    {
        /* get pgrp related processes*/
        dirNamePtr = ent->d_name;
        childFlag = 0;
        if (ent->d_name[0] == '.')
        {
            dirNamePtr += 1;
            childFlag = 1;
        }
        if (isdigit(dirNamePtr[0]))
        {
            tempPid=(int) strtol(dirNamePtr,NULL,10);
            if (tempPid==pid)
                continue;
            pgrp = getpgid (tempPid);
            if (pgrp == pid)
            {
                procTable[numProc]=tempPid;
                flagTable[numProc]=childFlag;
                numProc++;
            }
        }
    }
    closedir(dir);

    /* Trying to find all child processes via LWP on 64 bit */
    if (useLWP)
    {
        sprintf(buffer, "/proc/%d/task", pid);
        dir = opendir(buffer);
        if (dir != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                dirNamePtr = ent->d_name;
                childFlag = 0;
                if (isdigit(dirNamePtr[0]))
                {
                    tempPid=(int) strtol(dirNamePtr,NULL,10);
                    if (tempPid==pid)
                        continue;
                    procTable[numProc]=tempPid;
                    flagTable[numProc]=childFlag;
                    numProc++;
                 }
            }
            closedir(dir);
        }
    }

    /* On some linux kernels only one of above methods works. On other kernel
     *  versons both work. So we need to remove duplicated entries
     */

    numProc = uniqueSet (0, numProc);

    if(debuginfo)
    {
        printf("Debug Info for Stats - Processes Table\n");
        for(i=0;i<numProc;i++)
        {
            printf("%d-%d\n",procTable[i],flagTable[i]);
        }
    }
}

/* Take out duplicated entries. So the result is a unique set of pids.*/
static int uniqueSet (int lhs, int rhs)
{
    int curProc = procTable[lhs];
    int dup = 0;

    if (lhs == rhs - 1)
    {
        if (procTable [lhs] == procTable [rhs])
            return lhs;
        else
            return rhs;
    }
    rhs = uniqueSet (lhs + 1, rhs);
    dup = isInSet (lhs + 1, rhs, curProc);
    if (dup != -1)
    {
        procTable[dup] = procTable[rhs];
        flagTable[dup] = flagTable[rhs];
        rhs --;
    }
    return rhs;
}

static int isInSet (int lhs, int rhs, int pid)
{
    int i = 0;
    for (i = lhs; i <= rhs; i++)
    {
        if (pid == procTable[i])
            return i;
    }
    return -1;
}

pid_t getParentPid(pid_t pid, int childFlag)
{
    char buffer[4096], *p, *q;
    int state =0;
    pid_t ppid=0;
    char name[64];
    int len = 0;
    /* grab the proc stat info in one go */
    {
        int fd, len;
        if(childFlag==1)
            sprintf(buffer, "/proc/.%d/stat", pid);
        else
            sprintf(buffer, "/proc/%d/stat", pid);

        fd = open(buffer, 0);

        if (fd==-1)
        {
            return 0;
        }

        len = read(fd, buffer, sizeof(buffer)-1);
        close(fd);
        if (len==-1)
        {
            return 0;
        }
        buffer[len] = '\0';
    }
    /* parse out the status */
    p = buffer;
    p = strchr(p, '(');                       /* skip pid */
    if (p==NULL)
    {
        /* Process has exited since the pid was initially obtained */
        return 0;
    }
    {
        ++p; /* increment past the '(' char */
        q = strrchr(p, ')'); /*process name*/
        len = q-p;
        if (len >= sizeof(name))
            len = sizeof(name)-1;
        memcpy(name, p, len);
        name[len] = 0;
        p = q+1;
    }
    p = skip_ws(p);
    switch (*p++)
    {
      case 'R': state = 1; break;
      case 'S': state = 2; break;
      case 'D': state = 3; break;
      case 'Z': state = 4; break;
      case 'T': state = 5; break;
      case 'W': state = 6; break;
    }
    ppid = (pid_t)strtol(p,&p,10);                /* skip ppid */
    return ppid;
}

void searchDirs(int procChildren[], int dirNames[],
                      int count,cpuVals *cpv,int tempPid)
{
    int x=0,ppid=0;
    struct cpuVals tempCpu;
    for(x = 0; x < count; x++)
    {
        ppid = getProcessInfo(dirNames[x],NULL,&tempCpu,procChildren[x]);
        if(ppid==tempPid)
        {
            cpv->userTime += tempCpu.userTime;
            cpv->sysTime += tempCpu.sysTime;
            searchDirs(procChildren,dirNames,count,cpv,dirNames[x]);
        }
        else
        {
            tempCpu.userTime=0;
            tempCpu.sysTime=0;
        }
    }
}

static long pagetok(long x)
{
    /*
     * This replaces:
     *
     * #include <asm/page.h>
     *
     * #define pagetok(x)    ((x) << (PAGE_SHIFT - 10))
     *
     * which is deprecated according to RedHat
     */
    static int initDone = 0;
    static int pageShift = 0;

    if (!initDone) {
    int pgSize = sysconf(_SC_PAGESIZE);

    /* We have to calc the power-of-2 that the page size represents */
    for (pageShift = 0; !(pgSize & 0x01); pageShift++, pgSize >>= 1)
        ;

    /* Convert to 1K units */
    pageShift -= 10;
    initDone = 1;
    }
    return (x << pageShift);
}

int getProcessInfo(int pid ,memVals *memV,cpuVals *cpuV,int childFlag)
{
    char buffer[4096], *p;
    int state =0;
    int ppid=0;
    long totalMem = getTotalSystemMem();
    double cpuClockTicksPerSecond = (double)sysconf(_SC_CLK_TCK);
    /* grab the proc stat info in one go */
    {
        int fd, len;
        if(childFlag==1)
            sprintf(buffer, "/proc/.%d/stat", pid);
        else
            sprintf(buffer, "/proc/%d/stat", pid);

        fd = open(buffer, 0);
        if (fd == -1)
        {
            if (cpuV)
            {
                cpuV->userTime = 0;
                cpuV->sysTime = 0;
            }
            if(memV)
            {
                memV->vsize = 0;
                memV->rss = 0;
                memV->memPercent = 0;
            }
            return 0;
        }
        len = read(fd, buffer, sizeof(buffer)-1);
        close(fd);
        buffer[len] = '\0';
    }
    /* parse out the status */
    p = buffer;
    p = strchr(p, '(')+1;                       /* skip pid */
    {
        char name[64];
        char *q = strrchr(p, ')'); /*process name*/
        int len = q-p;
        if (len >= sizeof(name))
            len = sizeof(name)-1;
        memcpy(name, p, len);
        name[len] = 0;
        p = q+1;
    }
    p = skip_ws(p);
    switch (*p++)
    {
      case 'R': state = 1; break;
      case 'S': state = 2; break;
      case 'D': state = 3; break;
      case 'Z': state = 4; break;
      case 'T': state = 5; break;
      case 'W': state = 6; break;
    }

    ppid = (int)strtol(p,&p,10);                /* skip ppid */
    p = skip_token(p);                          /* skip pgrp */
    p = skip_token(p);                          /* skip session */
    p = skip_token(p);                          /* skip tty */
    p = skip_token(p);                          /* skip tty pgrp */
    p = skip_token(p);                          /* skip flags */
    p = skip_token(p);                          /* skip min flt */
    p = skip_token(p);                          /* skip cmin flt */
    p = skip_token(p);                          /* skip maj flt */
    p = skip_token(p);                          /* skip cmaj flt */
    if(cpuV)
    {
        cpuV->userTime = ((double)strtoul(p, &p, 10))/cpuClockTicksPerSecond; /* utime */
        cpuV->sysTime = ((double)strtoul(p, &p, 10))/cpuClockTicksPerSecond;  /* stime */
    }
    else
    {
        p = skip_token(p);                          /* skip utime */
        p = skip_token(p);                          /* skip stime */
    }
    p = skip_token(p);                          /* skip cutime */
    p = skip_token(p);                          /* skip cstime */

    p = skip_token(p);                          /* skip priority */
    p = skip_token(p);                          /* skip nice */

    p = skip_token(p);                          /* skip timeout */
    p = skip_token(p);                          /* skip it_real_val */
    p = skip_token(p);                          /* skip start_time */

    if(memV)
    {
        memV->vsize = bytetok(strtoul(p, &p, 10));  /* vsize */
        memV->rss = pagetok(strtoul(p, &p, 10));    /* rss */
        memV->memPercent =(double)(memV->rss*100.0)/(double)totalMem;
    }

    return ppid;
}

int getSystemMemory (systemMemVals* mem)
{
    FILE* fp;
    char buffer[4096];
    size_t bytes_read;
    char* match;
    long totalMem=-1,freeMem=-1,totalSwap=-1,freeSwap=-1;

    fp = fopen ("/proc/meminfo","r");
    if (fp==NULL)
        return -1;
    bytes_read = fread(buffer, 1, sizeof (buffer), fp);
    fclose(fp);

    if (bytes_read==0 || bytes_read == sizeof (buffer))
        return -1;
    buffer[bytes_read] = '\0';

    match = strstr (buffer, "MemTotal:");
    if (match == NULL)
        return -1;
    sscanf (match, "MemTotal:%ld", &totalMem);
    match = strstr (buffer, "MemFree:");
    if (match == NULL)
        return -1;
    sscanf (match, "MemFree:%ld", &freeMem);
    match = strstr (buffer, "SwapTotal:");
    if (match == NULL)
        return -1;
    sscanf (match, "SwapTotal:%ld", &totalSwap);
    match = strstr (buffer, "SwapFree:");
    if (match == NULL)
        return -1;
    sscanf (match, "SwapFree:%ld", &freeSwap);

    mem->totalMem=totalMem;
    mem->freeMem=freeMem;
    mem->totalSwap=totalSwap;
    mem->freeSwap=freeSwap;
    return 0;
}

long getTotalSystemMem(void)
{
    /* get system wide memory usage */
    FILE* fp;
    char buffer[4096];
    size_t bytes_read;
    char* match;
    long totalMem=-1;

    fp = fopen ("/proc/meminfo","r");
    if (fp==NULL)
        return totalMem;
    bytes_read = fread(buffer, 1, sizeof (buffer), fp);
    fclose(fp);

    if (bytes_read==0 || bytes_read == sizeof (buffer))
        return totalMem;
    buffer[bytes_read] = '\0';

    match = strstr (buffer, "MemTotal:");
    if (match == NULL)
        return totalMem;

    sscanf (match, "MemTotal:%ld", &totalMem);
    return totalMem;
}

void getSystemTime(double* upTime,double* idleTime)
{
    FILE* fp;
    *upTime = 0;
    *idleTime = 0;
    fp = fopen("/proc/uptime", "r");
    if (fp==NULL)
        return;
    fscanf (fp, "%lf %lf\n", upTime, idleTime);
    fclose (fp);
}

void getProcessorTime(long* usageTime,long* idleTime,int processor)
{
    FILE* fp;
    char buffer[4096];
    char cpu[10];
    size_t bytes_read;
    char* match;
    long user_read, system_read, nice_read, idle_read;
    *usageTime = 0;
    *idleTime = 0;

    fp = fopen ("/proc/stat","r");
    if (fp==NULL)
        return;
    bytes_read = fread(buffer, 1, sizeof (buffer), fp);
    fclose(fp);
    if (bytes_read==0 || bytes_read == sizeof (buffer))
        return;
    buffer[bytes_read] = '\0';
    sprintf(cpu,"cpu%d ",processor);
    match = strstr (buffer, cpu);
    if (match == NULL)
        return;
    sscanf(match, "%*s %ld %ld %ld %ld", &user_read,&system_read,&nice_read,&idle_read);
    *usageTime = user_read + system_read + nice_read;
    *idleTime = idle_read;
}
