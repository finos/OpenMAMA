'''
	version 0.2
	Python version 2.3 and 2.4
'''
import optparse
import os
import sys
from common import my_print

VERBOSE=True

class MamaTick(object):
    def __init__(self,line_num,tick_num):
        self.line_num=line_num;
        self.msg_dict = {};
    
    def __eq__(self,other_tick):
        if not isinstance(other_tick,MamaTick):
            return False
        
        if self.msg_dict==other_tick.msg_dict:
            return True
        else:
            return False
class MamaTickGen(object):
    def __init__(self,file_name,ignore_fields):
        self.file_name = file_name
        self.ignore_fields = ignore_fields
    def __iter__(self):
        return self.tick_gen()
    def tick_gen(self):
        tick_obj = None
        tick_num = -1
        
        file_obj = open(self.file_name,'r')
        for line_num, line in enumerate(file_obj):
           line=line.strip()
           if not line:
               continue
           if "Received msg" in line:
               if tick_obj:
                   yield tick_obj;
               tick_num = tick_num+1;
               tick_obj = MamaTick(line_num+1,tick_num)
               continue
           elif tick_obj is None:
               continue
           else:
               line_data = line.strip().split()
               tick_key = line_data[0].strip()
               if self.ignore_fields is not None and tick_key in self.ignore_fields:
                   continue
               else:
                   tick_data= line_data[1:len(line_data)]
                   tick_obj.msg_dict[tick_key]= tick_data
        
        if tick_obj:
            yield tick_obj
        file_obj.close()   
                
def print_result(result,source_tick,target_tick=None):
    
   # if not VERBOSE:
    #    return
    if source_tick is None:
        my_print(result +":"+ "Could not find matching record for target at line:"+str(target_tick.line_num))
    elif target_tick is None:
        my_print(result+":"+"Could not find matching record for source at line:"+str(source_tick.line_num))
    else:
        my_print(result+":"+"Source line number="+str(source_tick.line_num)+",Target line number="+str(target_tick.line_num))
    
def compare(source_ticks, target_ticks):
    
    result=None
    
    target_tick_gen = iter(target_ticks)
    source_tick_gen = iter(source_ticks)
    for sourceTick in source_tick_gen:
        try:    
            targetTick = target_tick_gen.next() 
        except:
            targetTick=None
        if(sourceTick==targetTick):
            print_result('Pass', sourceTick, targetTick)
            if result is None:
                result=0
        else:
            print_result('Fail', sourceTick, targetTick)
            if result is None or result==0:
                result=1
            
    return result

def run():
    global VERBOSE    
    parser = optparse.OptionParser()
    parser.add_option("--i", "--ignore_fields", dest="ignore_fields",help='fields to ignore while comparing')
    parser.add_option("-q","--quiet",action="store_false",dest="verbose",help="Quiet mode") 
    (option,argument) = parser.parse_args()
    
    if len(argument)!=2:
        my_print("Invalid arguments")
        my_print("Need log1 and log2 as positional arguments")
        sys.exit(1)

    source_file = argument[0]
    target_file = argument[1]
    if option.verbose is not None:
        VERBOSE = option.verbose
    my_print("Comparing files "+ str(source_file) + " with " + str(target_file),VERBOSE)
 
    if not os.path.isfile(source_file):
        my_print("Could not find " + str(source_file))
        sys.exit(1)
    elif not os.path.isfile(target_file):
        my_print("Could not find " + str(target_file))
        sys.exit(1)

    source_file = argument[0]
    target_file = argument[1]
    
    ignore_fields = option.ignore_fields

    source_ticks = MamaTickGen(source_file,ignore_fields)
    target_ticks = MamaTickGen(target_file,ignore_fields)
    
    result = compare(source_ticks, target_ticks)
    if result==0 :
        sys.exit(0)
    else:
        sys.exit(1)
if __name__ == '__main__':
    run()
    
