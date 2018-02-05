#!/usr/bin/python
import unittest
import optparse

from common import check_env
from regressionjava import TestOpenmamajava

import globals

if __name__ == '__main__':
    usage = "usage: python regression.py --m <middleware> --tport <pub_transport> <sub_transport>"
    parser = optparse.OptionParser()
    parser.add_option("--tport", dest="transport",nargs=2,help='Name of pub and sub transport in mama.properties')
    parser.add_option("--m",dest="middleware",default="qpid",choices=['qpid','lbm','wmw'],help='Select Middleware: qpid, lbm or wmw')
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
    if option.timeout is not None:
        globals.TIMEOUT = option.timeout

    java_suite = unittest.TestLoader().loadTestsFromTestCase(TestOpenmamajava)
    unittest.TextTestRunner().run(java_suite)
     
