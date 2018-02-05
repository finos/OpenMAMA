
import optparse
import time
import  sys
import os
from common import my_print

TIMEOUT = 5

def run():
    '''
    wait4text waits for appearance of 'searchtext' in the file. It waits TIMEOUT seconds for file creation and
    specified optional time in seconds or number of lines for appearance of text
    '''
    parser = optparse.OptionParser()
    parser.add_option("-t", "--timeout", dest="timeout",default=TIMEOUT,type=int,help="Time to wait for the text to appear, in seconds")
    parser.add_option("-l","--max-lines",dest="maxlines",default=1000,type=int,help="Prints comparison results")
    parser.add_option("-q","--quiet",action="store_false",dest="verbose",help="Quiet mode")
    (option,argument) = parser.parse_args()
    if len(argument)!=2:
        my_print("Error: Invalid number of arguments")
        my_print("Usage: wait4text 'SearchText' filename [options] ")
        my_print("use wait4text -h for help")
        sys.exit(1)

    file_name = argument[1]
    searchtext = argument[0]
    timeout = option.timeout
    maxlines = option.maxlines
    if option.verbose is not None:
        verbose = option.verbose
    else:
        verbose = True
    wait_time = 0
    while wait_time < TIMEOUT:
        if os.path.isfile(file_name):
            break
        else:
            my_print("waiting for "+ str(file_name) + " file to appear",verbose)
            time.sleep(0.05)
            wait_time = wait_time+1
    else:
        my_print("Error: No file found with name:%s" % (file_name))
        sys.exit(1)
   
    
    line_count = 0
    
    file_obj = open(file_name)
    start_time = time.time()
    my_print("Searching %s for text:%s, timeout=%d, maxlines=%d" % (file_name, searchtext, timeout, maxlines),verbose)
        
    while True:
        line = file_obj.readline()
        if line == '':
            time.sleep(0.05)
        else:
            line_count+=1
            if searchtext in line:
                my_print("Found text: line_number=%s, line=%s" %(line_count, line.rstrip()),verbose)
                sys.exit(0)
        
        duration = time.time() - start_time
            
        if maxlines is not None:
            if line_count >= maxlines:
                my_print("Search exceeded max number of lines %d without finding match" % (maxlines),verbose)
                sys.exit(1)
            
            
        if duration >= float(timeout):
            my_print("Search exceeded max time (%d seconds) without finding match, lines read=%d" % (timeout, line_count),verbose)
            sys.exit(1) 
    

if __name__ == '__main__':
    run()
