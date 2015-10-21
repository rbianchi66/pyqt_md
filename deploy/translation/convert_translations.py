#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
import os
import os.path
import glob
import re
import difflib

pjoin = os.path.join

def list_recursive_files(basepath, glob_filter):
    for filename in glob.glob(pjoin(basepath, glob_filter)):
        yield filename
    for root, dirs, files in os.walk(basepath):
        for dir in dirs:
            for filename in glob.glob(pjoin(pjoin(root, dir), glob_filter)):
                yield filename

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Provide a path"
    if not os.path.isdir(sys.argv[1]):
        print "Provide a valid path"
    for filename in list_recursive_files(sys.argv[1], "*.py"):
        sys.stderr.write("Processing file %s... " % filename)
        orig = open(filename).read()
        res = re.sub(r"""([\*\(\ \.]tr\()("[^"]*")[\ ]*,[\ ]*""", r"\1", orig)
        for tfunc in "translate warningMessage warnfunct".split():
            res = re.sub(r"""([\*\(\ \.]%(tfunc)s\((?:self,|)[^,\)]+),[^,\)]+(,[^,\)]+,[^\)]+\))""" % locals(), r"\1\2", res)
        res = re.sub(r"""([\*\(\.]tr\()([\w]*[\ ]*,[\ ]*)([^\)]*)""", r"\1\3", res)

        # BILINGUIST: Deve controllare anche pattern del tipo .tr('
        res = re.sub(r"""([\*\(\ \.]tr\()('[^']*')[\ ]*,[\ ]*""", r"\1", res)

        if orig != res:
            sys.stderr.write("MATCH FOUND")
            print "Index:", filename
            print "=" * 80
            print ''.join(list(difflib.unified_diff(orig.splitlines(1),
                                                    res.splitlines(1),
                                                    filename + " (original)",
                                                    filename + " (modified)"))),
        sys.stderr.write("\n")
