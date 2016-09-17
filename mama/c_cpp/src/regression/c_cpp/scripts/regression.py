#!/usr/bin/python
import unittest
import optparse

from common import check_env
from regressionc import TestOpenmamac
from regressioncpp import TestOpenmamacpp

import globals

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option("--tport", dest="transport",nargs=2,help='Name of pub and sub transport in mama.properties')
    parser.add_option("--m",dest="middleware",default="qpid",choices=['qpid','lbm','wmw'],help='Select Middleware: qpid, lbm or wmw')
    parser.add_option("--test",dest="test",default="all",choices=['c','cpp','all'],help='Select test: c, cpp or all')
    parser.add_option("--timeout",dest="timeout",type="int",help="Timeout for wait4text")
    parser.add_option("--q",action="store_false",dest="verbose",help="Suppress verbose output")
    (option,argument) = parser.parse_args()
    check_env()
    globals.MIDDLEWARE = option.middleware
    if option.verbose is not None:
        globals.VERBOSE = option.verbose

    if option.transport is not None:
        globals.TRANSPORTPUB = option.transport[0]
        globals.TRANSPORTSUB = option.transport[1]
    test = option.test
    if option.timeout is not None:
        globals.TIMEOUT = option.timeout

    c_suite = unittest.TestLoader().loadTestsFromTestCase(TestOpenmamac)
    cpp_suite = unittest.TestLoader().loadTestsFromTestCase(TestOpenmamacpp)
    all_suite = unittest.TestSuite([c_suite,cpp_suite])

    if test=='c':
        unittest.TextTestRunner().run(c_suite)
    elif test=='cpp':
        unittest.TextTestRunner().run(cpp_suite)
    elif test=='all':
        unittest.TextTestRunner().run(all_suite)
    
