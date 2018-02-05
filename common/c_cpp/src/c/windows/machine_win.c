/* 
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "wombat/machine.h"
#include "wombat/wtable.h"

#define DMKEY "HARDWARE\\DESCRIPTION" /*where to look*/
void GetProcessNameById(DWORD Id, const char *buffer[]);

time_t getTimeSinceEpoch(void)
{
    time_t rawTime;
    time(&rawTime);
    return rawTime;
}

int getSystemMemory (systemMemVals* mem)
{
    return 0;
}

long getTotalSystemMem(void)
{
    return 0;
}

int getProcessInfo(int pid ,memVals *memV,cpuVals *cpuV,int childFlag)
{
    PDH_FMT_COUNTERVALUE pValue={0};
    HANDLE     gProcessHandle;
    FILETIME lpCreationTime;
    FILETIME lpExitTime;
    FILETIME lpKernelTime;
    FILETIME lpUserTime;

    LONGLONG tUser64;
    LONGLONG tKernel64;
    int error=0;
    
    static const char* buffer[64];
    static DWORD processId = 0;
        
    if (processId ==0)
    {
        processId = GetCurrentProcessId();
        GetProcessNameById(processId,buffer);
    }
    
    /*PROCESS_MEMORY_COUNTERS pmc;*/
    if(cpuV)
    {
        if((gProcessHandle = 
                    OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                        FALSE, processId))==NULL)
        {
            printf("OpenProcess Error code : %ld \n\n\n",GetLastError());
        }
        if((GetProcessTimes(gProcessHandle,&lpCreationTime,&lpExitTime,&lpKernelTime,&lpUserTime))==0)
        {
            printf("GetProcessTimes Error code : %ld \n\n\n",GetLastError());
        }
        else
        {
            tUser64 = *(LONGLONG *)&lpUserTime;
            tKernel64 = *(LONGLONG *)&lpKernelTime;
            cpuV->userTime =(tUser64 * 0.0000001);
            cpuV->sysTime =(tKernel64 * 0.0000001);
            CloseHandle(gProcessHandle);
        }
    }
    if(memV)
    {
        TCHAR      memCounterPath[64];
        HCOUNTER   memCounter;
        PDH_STATUS  pdhStatus;          /* return status of PDH functions */
        LPCTSTR     szDataSource = NULL;/* NULL for dynamic registry entries */
        HQUERY      phQuery;            /* query Handle */
        PDH_FMT_COUNTERVALUE pValue={0};

        snprintf(memCounterPath,64,"\\Process(%s)\\Private Bytes",buffer);

        /* create query */
        pdhStatus = PdhOpenQuery(NULL,0,&phQuery);
        if (pdhStatus != ERROR_SUCCESS) 
        {
            /* Print the error value. */
            _tprintf (TEXT("PdhOpenQuery failed with %ld\n"), pdhStatus);
            error=1;
        }
        /* add counter for MEM */
        pdhStatus = PdhAddCounter(phQuery,memCounterPath,0,&memCounter);
        if (pdhStatus != ERROR_SUCCESS) 
        {
            /* Print the error value. */
            _tprintf (TEXT("PdhAddCounter Failed with 0x%x\n"), pdhStatus);
            error=1;
        }
        /* collect Data */
        pdhStatus = PdhCollectQueryData(phQuery);
        if (pdhStatus != ERROR_SUCCESS) 
        {
            /* Print the error value. */
            _tprintf (TEXT("PdhCollectQueryData Failed with 0x%x\n"), pdhStatus); 
            error=1;
        }
        pdhStatus = PdhGetFormattedCounterValue(memCounter,
                PDH_FMT_DOUBLE,
                NULL,
                &pValue);
        if (pdhStatus == ERROR_SUCCESS) 
        {

            memV->rss = (long)(pValue.doubleValue/1.0e3);
        }       
        else 
        {
            /* Print the error value. */
            _tprintf (TEXT("PdhGetFormattedCounterValue Failed with 0x%x\n"), pdhStatus);
            error=1;
        }
        PdhCloseQuery(phQuery);
    }
    return error;
}

void searchDirs(int procChildren[], int dirNames[],
                      int count,cpuVals *cpv,int tempPid)
{
    /*function not needed on windows*/
}

void getProcAndChildCpu(int pid, cpuVals* cpv)
{
    (void)getProcessInfo(pid ,NULL,cpv,0);
}

int getNumCpu(void)
{
    int value;

    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    value = siSysInfo.dwNumberOfProcessors;
    
    return value;
}

void initProcTable(int pid, int debuginfo)
{
    /*function not needed on windows*/
}

void getSystemTime(double* upTime,double* idleTime)
{
    *upTime=0.0;
    *idleTime=0.0;
}

void getProcessorTime(long* usageTime,long* idleTime,int processor)
{
    *usageTime=0;
    *idleTime=0;
}

void GetProcessNameById(DWORD Id, const char* buffer[]) 
{
    PROCESSENTRY32 pEntry = { 0 };
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    pEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapShot, &pEntry)) 
    {
        do
        {
            if (pEntry.th32ProcessID == Id) 
            {
                *buffer = (const char*)
                    malloc(strlen((const char*)pEntry.szExeFile) - 4);

                pEntry.szExeFile[strlen((const char*)pEntry.szExeFile) -4]='\0';
                strcpy((char*)buffer,(const char*)pEntry.szExeFile);
                break;
            }
        }while (Process32Next(hSnapShot, &pEntry));
    }
    CloseHandle(hSnapShot);
}

