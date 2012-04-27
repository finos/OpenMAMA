#!/usr/bin/python
import subprocess
import unittest

from common import (start_app, start_javaapp, call_wait4text, call_wait4text, check_env, kill_process, 
    call_comparelog, my_print )

class TestOpenmamacpp(unittest.TestCase):
    def setUp(self):
        pass

    def test_pubsubcpp(self):
        '''publish messages using basicpubcpp and verify basicsubcpp receives the messages'''
        my_print("\n----------Running pubsubcpp test----------\n")
        basicsubcpp_out = open('pubsub_basicsubcpp.out','w')
        basicsubcpp_err = open('pubsub_basicsubcpp.err','w')
        basicpubcpp_out = open('pubsub_basicpubcpp.out','w')
        ret1 = start_app("basicsubcpp","sub",args=["-v","-v","-v","-v"],stdout=basicsubcpp_out,stderr=basicsubcpp_err)

        ret2 = call_wait4text("Created inbound subscription","pubsub_basicsubcpp.out")
        ret3 = start_app("basicpubcpp","pub",stdout=basicpubcpp_out,stderr=subprocess.STDOUT)

        ret4 = call_wait4text("EXIT", "pubsub_basicsubcpp.out")
        ret5 = call_comparelog("pubsub_expectedsubcpp.out","pubsub_basicsubcpp.out")

        basicsubcpp_out.close()
        basicsubcpp_err.close()
        basicpubcpp_out.close()

        if ret1.poll() is None:
            kill_process(ret1.pid)
        if ret3.poll() is None:
            kill_process(ret3.pid)

 
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

    def test_req_replycpp(self):
        '''Send request from basicinboxcpp and verify it recives reply from basicpubcpp'''
        my_print("\n----------Running req_replycpp test----------\n")
        basicpubcpp_out = open('req_reply_basicpubcpp.out','w')
        basicpubcpp_err = open('req_reply_basicpubcpp.err','w')
        basicinboxcpp_out = open('req_reply_basicinboxcpp.out','w')
        ret1 = start_app("basicpubcpp","pub",args=["-v","-v","-v","-v"],stdout=basicpubcpp_out,stderr=basicpubcpp_err) 
        ret2 = call_wait4text("Created inbound subscription","req_reply_basicpubcpp.out",args=["-t",'1'])
        ret3 = start_app("basicinboxcpp","sub",stdout=basicinboxcpp_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Received reply", "req_reply_basicinboxcpp.out")

        basicpubcpp_out.close()
        basicpubcpp_err.close()
        basicinboxcpp_out.close()
        kill_process(ret3.pid)

        self.assertTrue(ret4==0)

    def test_ft_active_standbycpp(self):
        '''start two basicftmembercpp and verify the first one goes to active and second to standby, if both belong to same group'''
        my_print("\n----------Running basicftmembercpp test active-standby----------\n")
        ftmembercpp1_out = open('active_standby_ftmembercpp1.out','w')
        ftmembercpp2_out = open('active_standby_ftmembercpp2.out','w')

        ret1 = start_app("basicftmembercpp","pub",args=["-g","active-standby","-b"],stdout=ftmembercpp1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: active", "active_standby_ftmembercpp1.out")
        ret3 = start_app("basicftmembercpp","sub",args=["-g","active-standby","-b"],stdout=ftmembercpp2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: standby", "active_standby_ftmembercpp2.out")
        ret5 = call_wait4text("Current state is: standby", "active_standby_ftmembercpp1.out")

        ftmembercpp1_out.close()
        ftmembercpp2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)
        
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==1) 

    def test_ft_active_activecpp(self):
        '''start two basicftmembercpp and verify that both go to active, if they belong to different groups'''
        my_print("\n----------Running basicftmemberpp test active-active----------\n")
        ftmembercpp1_out = open('active_active_ftmembercpp1.out','w')
        ftmembercpp2_out = open('active_active_ftmembercpp2.out','w')


        ret1 = start_app("basicftmembercpp","pub",args=["-g","active-active1","-b"],stdout=ftmembercpp1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: active", "active_active_ftmembercpp1.out")
        ret3 = start_app("basicftmembercpp","sub",args=["-g","active-active2","-b"],stdout=ftmembercpp2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: active", "active_active_ftmembercpp2.out")
        ret5 = call_wait4text("Current state is: standby", "active_active_ftmembercpp1.out")

        ftmembercpp1_out.close()
        ftmembercpp2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0) 
        self.assertTrue(ret5==1)

    def test_ft_prioritycpp(self):
        '''start two basicftmembercpp with different priority and verify that the one with lower priority goes to standby
        from active'''
        my_print("\n----------Running basicftmemberpp test priority----------\n")
        ftmembercpp1_out = open('priority_ftmembercpp1.out','w')
        ftmembercpp2_out = open('priority_ftmembercpp2.out','w')

        ret1 =start_app("basicftmembercpp","pub",args=["-g","priority","-w","40","-b"],stdout=ftmembercpp1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: active", "priority_ftmembercpp1.out")
        ret3 = start_app("basicftmembercpp","sub",args=["-g","priority","-w","60","-b"],stdout=ftmembercpp2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: active", "priority_ftmembercpp2.out")
        ret5 = call_wait4text("Current state is: standby", "priority_ftmembercpp1.out")
        ftmembercpp1_out.close()
        ftmembercpp2_out.close()
        kill_process(ret1.pid)
        kill_process(ret3.pid)
        
        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

    def test_ft_standby_to_activecpp(self):
        '''Start two basicftmembercpp with same group and stop the active one, verify the standby basicftmemberpp goes to active'''
        my_print("\n----------Running basicftmemberpp test standby-to-active----------\n")
        ftmembercpp1_out = open('standby_to_active_ftmembercpp1.out','w')
        ftmembercpp2_out = open('standby_to_active_ftmembercpp2.out','w')

        ret1 = start_app("basicftmembercpp","pub",args=["-g","standby-to-active","-b"],stdout=ftmembercpp1_out,stderr=subprocess.STDOUT)
        ret2 = call_wait4text("Current state is: active", "standby_to_active_ftmembercpp1.out")
        ret3 = start_app("basicftmembercpp","sub",args=["-g","standby-to-active","-b"],stdout=ftmembercpp2_out,stderr=subprocess.STDOUT)
        ret4 = call_wait4text("Current state is: standby", "standby_to_active_ftmembercpp2.out")

        kill_process(ret1.pid)

        ret5 = call_wait4text("Current state is: active", "standby_to_active_ftmembercpp2.out")

        ftmembercpp1_out.close()
        ftmembercpp2_out.close()

        kill_process(ret1.pid)
        kill_process(ret3.pid)

        self.assertTrue(ret2==0)
        self.assertTrue(ret4==0)
        self.assertTrue(ret5==0)

