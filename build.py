#!/usr/bin/env python
import sys,os
__ddir = os.path.join(os.path.dirname(__file__), "deploy")
sys.path[0] = __ddir 
execfile(os.path.join(__ddir, "build.py"))

