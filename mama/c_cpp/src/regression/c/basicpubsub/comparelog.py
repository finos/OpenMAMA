'''
	version 0.2
	Python version 2.3 and 2.4
'''
import optparse
import os
import sys

DEFAULT_CACHE_SIZE=10
DEFAULT_COMPARE_MODE='exact'

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
    
class MamaTickCache(object):
    def __init__(self, max_cache_size):
        self.max_cache_size = max_cache_size
        self.tickcache = []
        self.expected_index = 0
    
    def __iter__(self):
        return iter(self.tickcache)
    
    def __len__(self):
        return len(self.tickcache)
    
    def push(self, tick):
        self.tickcache.append(tick)
        if len(self) > self.max_cache_size +1:
            sys.exit(1)
    def member_search(self,tick):
        for index, cached_tick in enumerate(self):
            if cached_tick==tick:
                self.tickcache.pop(index)
                return cached_tick
        return None

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
               
           if line=='mamasubscriberc: Recieved msg.':
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
    
    if not VERBOSE:
        
        return
    if source_tick is None:
       print result,"  ",target_tick.line_num
    elif target_tick is None:
        print result, source_tick.line_num
    else:
        print result,source_tick.line_num,target_tick.line_num
    
def compare(source_ticks, target_ticks, 
            mode, cache_size):
    
    result=None
    if mode==None:
        mode=DEFAULT_COMPARE_MODE
    
    target_tick_gen = iter(target_ticks)
    source_tick_gen = iter(source_ticks)
    
    if mode=='cached':
        
        if cache_size is None:
            cache_size=DEFAULT_CACHE_SIZE
            
        targetCache = MamaTickCache(cache_size)                               
        for targettickcount, targetTick in enumerate(target_tick_gen):    
            targetCache.push(targetTick)
            if targettickcount >= cache_size//2:
                break
        stopTargetGeneration=False
        for sourceTick in source_tick_gen:    
            
            if stopTargetGeneration==False:
                try:
                    targetTick = target_tick_gen.next()
                    targetCache.push(targetTick)
                    if len(targetCache) >= cache_size:
                        targetCache.tickcache.pop(0)
                except StopIteration:
                    stopTargetGeneration=True
            targetMember = targetCache.member_search(sourceTick)
            if targetMember:
                print_result('Pass',sourceTick,targetMember)
                if result is None:
                    result=0
            else:
                print_result('Fail',sourceTick)
                if result is None or result==0:
                    result=1
    
    elif mode=='exact':
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
            
                
    
    elif mode=='exhaustive':
        for sourceTick in source_tick_gen:
            target_tick_gen = iter(target_ticks)
            for targetTick in target_tick_gen:
                if(sourceTick==targetTick):
                    print_result('Pass',sourceTick,targetTick)
                    if result is None:
                        result=0
                    break
            else:           
                print_result('Fail',sourceTick,targetTick)
                if result is None or result==0:
                    result=1
    return result

def run():
        
    parser = optparse.OptionParser()
    parser.add_option("--diff", "--diff-mode", dest='diff_mode',help='Which diff algorthim to use,"exact","cached" or "exhaustive"')
    parser.add_option("--cache", "--cache-size", dest="cache_size",help='Cache size if diff-mode is cached')
    parser.add_option("--i", "--ignore_fields", dest="ignore_fields",help='fields to ignore while comparing')
    parser.add_option("--v", action="store_true",help='Prints comparison results')
    
    (option,argument) = parser.parse_args()
    if len(argument)!=2:
        print "Invalid arguments"
        print "Need log1 and log2 as positional arguments"
        sys.exit(1)
    elif not os.path.isfile(argument[0]):
        print "Could not find",argument[0]
        sys.exit(1)
    elif not os.path.isfile(argument[1]):
        print "Could not find", argument[1]        
        sys.exit(1)
    source_file = argument[0]
    target_file = argument[1]
    
    diff_mode = option.diff_mode
    cache_size = option.cache_size
    ignore_fields = option.ignore_fields
    global VERBOSE 
    VERBOSE = option.v

    source_ticks = MamaTickGen(source_file,ignore_fields)
    target_ticks = MamaTickGen(target_file,ignore_fields)
    
    result = compare(source_ticks, target_ticks,mode=diff_mode, cache_size=cache_size)
    if result==0 or result is None:
        sys.exit(0)
    else:
        sys.exit(1)
if __name__ == '__main__':
    run()
    
