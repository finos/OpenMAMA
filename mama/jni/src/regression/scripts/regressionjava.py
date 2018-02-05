#!/usr/bin/python
import subprocess
import unittest

from common import (start_app, start_javaapp, call_wait4text, call_wait4text, check_env, kill_process, 
    call_comparelog, my_print )

class TestOpenmamajava(unittest.TestCase):
    def setUp(self):
        pass

    def test_pubsubjava(self):
        '''publish messages using basicpubjava and verify basicsubjava receives the messages'''
        my_print("\n----------Running pubsubjava test----------\n")
        basicsubjava_out = open('pubsub_basicsubjava.out','w')
        basicsubjava_err = open('pubsub_basicsubjava.err','w')
        basicpubjava_out = open('pubsub_basicpubjava.out','w')
        ret1 = start_javaapp("BasicSubJava","sub",stdout=basicsubjava_out,stderr=basicsubjava_err)
        ret2 = call_wait4text("Created inbound subscription","pubsub_basicsubjava.out")
        ret3 = start_javaapp("BasicPubJava","pub",stdout=basicpubjava_out,stderr=subprocess.STDOUT)

        ret4 = call_wait4text("EXIT", "pubsub_basicsubjava.out")
        ret5 = call_comparelog("pubsub_expectedsubjava.out","pubsub_basicsubjava.out")
        

        basicsubjava_out.close()
        basicsubjava_err.close()
        basicpubjava_out.close()
    
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

    def test_req_replyjava(self):
        '''Send request from basicinboxjava and verify it recives reply from basicpubjava'''
        my_print("\n----------Running req_replyjava test----------\n")
        basicpubjava_out = open('req_reply_basicpubjava.out','w')
        basicpubjava_err = open('req_reply_basicpubjava.err','w')
        basicinboxjava_out = open('req_reply_basicinboxjava.out','w')
        ret1 = start_javaapp("BasicPubJava","pub",args=["-v","-v","-v","-v"],stdout=basicpubjava_out,stderr=basicpubjava_err)
        ret2 = call_wait4text("Created inbound subscription","req_reply_basicpubcpp.out",args=["-t",'1'])
        ret3 = start_javaapp("BasicInboxJava","sub",stdout=basicinboxjava_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Received reply", "req_reply_basicinboxjava.out")

        basicpubjava_out.close()
        basicpubjava_err.close()
        basicinboxjava_out.close()
        kill_process(ret3.pid)
    
        self.assertTrue(ret4==0)

    def test_ft_active_standbyjava(self):
        '''start two basicftmemberjava and verify the first one goes ot active and second to standby, if both belong to same group'''
        my_print("\n----------Running basicftmemberjava test active-standby----------\n")
        ftmemberjava1_out = open('active_standby_ftmemberjava1.out','w')
        ftmemberjava2_out = open('active_standby_ftmemberjava2.out','w')

        ret1 = start_javaapp("BasicFtMemberJava","pub",args=["-g","active-standby","-b"],stdout=ftmemberjava1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: FT_STATE_ACTIVE", "active_standby_ftmemberjava1.out")
        ret3 = start_javaapp("BasicFtMemberJava","sub",args=["-g","active-standby","-b"],stdout=ftmemberjava2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: FT_STATE_STANDBY", "active_standby_ftmemberjava2.out")
        ret5 = call_wait4text("Current state is: FT_STATE_STANDBY", "active_standby_ftmemberjava1.out")

        ftmemberjava1_out.close()
        ftmemberjava2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)
  
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==1)


    def test_ft_active_activejava(self):
        '''start two basicftmemberjava and verify that both go to active, if they belong to different groups'''
        my_print("\n----------Running basicftmemberjava test active-active----------\n")
        ftmemberjava1_out = open('active_active_ftmemberjava1.out','w')
        ftmemberjava2_out = open('active_active_ftmemberjava2.out','w')

        ret1 = start_javaapp("BasicFtMemberJava","pub",args=["-g","active-active1","-b"],stdout=ftmemberjava1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: FT_STATE_ACTIVE", "active_active_ftmemberjava1.out")
        ret3 = start_javaapp("BasicFtMemberJava","sub",args=["-g","active-active2","-b"],stdout=ftmemberjava2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: FT_STATE_ACTIVE", "active_active_ftmemberjava2.out")
        ret5 = call_wait4text("Current state is: FT_STATE_STANDBY", "active_active_ftmemberjava1.out")

        ftmemberjava1_out.close()
        ftmemberjava2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==1)

    def test_ft_priorityjava(self):
        '''start two basicftmemberjava with different priority and verify that the one with lower priority goes to standby
        from active'''
        my_print("\n----------Running basicftmemberjava test priority----------\n")
        ftmemberjava1_out = open('priority_ftmemberjava1.out','w')
        ftmemberjava2_out = open('priority_ftmemberjava2.out','w')

        ret1 =start_javaapp("BasicFtMemberJava","pub",args=["-g","priority","-w","40","-b"],stdout=ftmemberjava1_out,stderr=subprocess.STDOUT)

        ret2 = call_wait4text("Current state is: FT_STATE_ACTIVE", "priority_ftmemberjava1.out")
        ret3 = start_javaapp("BasicFtMemberJava","sub",args=["-g","priority","-w","60","-b"],stdout=ftmemberjava2_out,stderr=subprocess.STDOUT)

        ret4 = call_wait4text("Current state is: FT_STATE_ACTIVE", "priority_ftmemberjava2.out")

        ret5 = call_wait4text("Current state is: FT_STATE_STANDBY", "priority_ftmemberjava1.out")
        ftmemberjava1_out.close()
        ftmemberjava2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

    def test_ft_standby_to_activejava(self):
        '''Start two basicftmemberjava with same group and stop the active one, verify the standby basicftmemberjava goes to active'''
        my_print("\n----------Running basicftmemberjava test standby-to-active----------\n")
        ftmemberjava1_out = open('standby_to_active_ftmemberjava1.out','w')
        ftmemberjava2_out = open('standby_to_active_ftmemberjava2.out','w')

        ret1 = start_javaapp("BasicFtMemberJava","pub",args=["-g","standby-to-active","-b"],stdout=ftmemberjava1_out,stderr=subprocess.STDOUT)

        ret2 = call_wait4text("Current state is: FT_STATE_ACTIVE", "standby_to_active_ftmemberjava1.out")

        ret3 = start_javaapp("BasicFtMemberJava","sub",args=["-g","standby-to-active","-b"],stdout=ftmemberjava2_out,stderr=subprocess.STDOUT)

        ret4 = call_wait4text("Current state is: FT_STATE_STANDBY", "standby_to_active_ftmemberjava2.out")

        kill_process(ret1.pid)

        ret5 = call_wait4text("Current state is: FT_STATE_ACTIVE", "standby_to_active_ftmemberjava2.out")

        ftmemberjava1_out.close()
        ftmemberjava2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)
