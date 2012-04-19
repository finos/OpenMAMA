#!/usr/bin/python
import subprocess
import unittest

from common import (start_app, start_javaapp, call_wait4text, check_env, kill_process, 
    call_comparelog, my_print )

class TestOpenmamac(unittest.TestCase):
    def setUp(self):
        pass
    def test_pubsubc(self):
        '''publish messages using basicpubc and verify basicsubc receives the messages'''
        my_print("\n-------------------------Running pubsubc test-------------------------\n")
        basicsubc_out = open('pubsub_basicsubc.out','w')
        basicsubc_err = open('pubsub_basicsubc.err','w')
        basicpubc_out = open('pubsub_basicpubc.out','w')
        ret1 = start_app("basicsubc","sub",args=["-v","-v","-v","-v"],stdout=basicsubc_out,stderr=basicsubc_err)
           
        ret2 = call_wait4text("Created inbound subscription","pubsub_basicsubc.out")
        ret3 = start_app("basicpubc","pub",stdout=basicpubc_out,stderr=subprocess.STDOUT)
       
        ret4 = call_wait4text("EXIT", "pubsub_basicsubc.out")
        ret5 = call_comparelog("pubsub_expectedsubc.out","pubsub_basicsubc.out")

        basicsubc_out.close()
        basicsubc_err.close() 
        basicpubc_out.close()
        if ret1.poll() is None:
            kill_process(ret1.pid)
        if ret3.poll() is None:
            kill_process(ret3.pid)

        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)
 
    def test_req_replyc(self):
        '''Send request from basicinbox and verify it recives reply from basicpub'''
        my_print("\n----------------------Running request_replyc test---------------------\n")
        basicpubc_out = open('req_reply_basicpubc.out','w')
        basicpubc_err = open('req_reply_basicpubc.err','w')
        basicinboxc_out = open('req_reply_basicinboxc.out','w')
        
        ret1 = start_app("basicpubc","pub",args=["-v","-v","-v","-v"],stdout=basicpubc_out,stderr=basicpubc_err)
        ret2 = call_wait4text("Created inbound subscription", "req_reply_basicpubc.out")
        ret3 = start_app("basicinboxc","sub",stdout=basicinboxc_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Received reply", "req_reply_basicinboxc.out")
        
        basicpubc_out.close()
        basicpubc_err.close()
        basicinboxc_out.close()
        kill_process(ret3.pid)
        
        self.assertTrue(ret4==0)

    def test_ft_active_standbyc(self):
        '''start two basicftmemberc and verify the first one goes ot active and second to standby, if both belong to same group'''
        my_print("\n--------------Running basicftmemberc test active-standby--------------\n")
        ftmemberc1_out = open('active_standby_ftmemberc1.out','w')
        ftmemberc2_out = open('active_standby_ftmemberc2.out','w')

        
        ret1 = start_app("basicftmemberc","pub",args=["-g","active-standby","-b"],stdout=ftmemberc1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is active", "active_standby_ftmemberc1.out")
        ret3 = start_app("basicftmemberc","sub",args=["-g","active-standby","-b"],stdout=ftmemberc2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is standby", "active_standby_ftmemberc2.out")
        ret5 = call_wait4text("Current state is standby", "active_standby_ftmemberc1.out")

        ftmemberc1_out.close()
        ftmemberc2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)
   
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==1)

 
    def test_ft_active_activec(self):
        '''start two basicftmemberc and verify that both go to active, if they belong to different groups'''
        my_print("\n---------------Running basicftmemberc test active-active--------------\n")
        ftmemberc1_out = open('active_active_ftmemberc1.out','w')
        ftmemberc2_out = open('active_active_ftmemberc2.out','w')

        
        ret1 = start_app("basicftmemberc","pub",args=["-g","active-active1","-b"],stdout=ftmemberc1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is active", "active_active_ftmemberc1.out")
        ret3 = start_app("basicftmemberc","sub",args=["-g","active-active2","-b"],stdout=ftmemberc2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is active", "active_active_ftmemberc2.out")
        ret5 = call_wait4text("Current state is standby", "active_active_ftmemberc1.out")
        
        ftmemberc1_out.close()
        ftmemberc2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)
 
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==1)

    def test_ft_priorityc(self):
        '''start two basicftmemberc with different priority and verify that the one with lower priority goes to standby
        from active'''
        my_print("\n-----------------Running basicftmemberc test priority-----------------\n")
        ftmemberc1_out = open('priority_ftmemberc1.out','w')
        ftmemberc2_out = open('priority_ftmemberc2.out','w')

        ret1 =start_app("basicftmemberc","pub",args=["-g","priority","-w","40","-b"],stdout=ftmemberc1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is active", "priority_ftmemberc1.out")
        ret3 = start_app("basicftmemberc","sub",args=["-g","priority","-w","60","-b"],stdout=ftmemberc2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is active", "priority_ftmemberc2.out")
        ret5 = call_wait4text("Current state is standby", "priority_ftmemberc1.out")
        
        ftmemberc1_out.close()
        ftmemberc2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

    def test_ft_standby_to_activec(self):
        '''Start two basicftmemberc with same group and stop the active one, verify the standby basicftmember goes to active'''
        my_print("\n-------------Running basicftmemberc test standby-to-active------------\n")
        ftmemberc1_out = open('standby_to_active_ftmemberc1.out','w')
        ftmemberc2_out = open('standby_to_active_ftmemberc2.out','w')

        ret1 = start_app("basicftmemberc","pub",args=["-g","standby-to-active","-b"],stdout=ftmemberc1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is active", "standby_to_active_ftmemberc1.out")
        ret3 = start_app("basicftmemberc","sub",args=["-g","standby-to-active","-b"],stdout=ftmemberc2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is standby", "standby_to_active_ftmemberc2.out")
        kill_process(ret1.pid)
        
        ret5 = call_wait4text("Current state is active", "standby_to_active_ftmemberc2.out")

        ftmemberc1_out.close()
        ftmemberc2_out.close()
        
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)
