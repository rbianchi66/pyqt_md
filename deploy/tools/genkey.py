#!/usr/bin/env python
"""
Genera la chiave di crittaggio per il build di Caligola.
"""
import sys
import os

key = os.urandom(32)

file("key.dat", "w").write("key = %r\n" % key)
file("_boot.py", "w").write("""
import sys
class KeyFile(object): pass
sys.keyfile = KeyFile()
sys.keyfile._key = %r
""" % key)
