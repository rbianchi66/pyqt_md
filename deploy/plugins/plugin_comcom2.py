#-*- coding: utf-8 -*-
"""
Plugin che genera un interfaccia python a partire da un .DEF comcom2
"""
import os
import tempfile
from hashlib import md5
import shutil
from common import *
from os.path import *

priority = 14

def addOptionGroup(parser):
    import optparse
    group = optparse.OptionGroup(parser, "plugin %s options" % name)
    group.add_option("--%s-template" % name, default=None,
                      help="Extra template used to generate the interface files, default None")
    group.add_option("--%s-template_dir" % name, default=None,
                      help="Templates folder")

    parser.add_option_group(group)

def _c2wrapDir():
    return os.path.join(root_dir, "src", "comcom2", "c2wrap")

def _c2wrapScript():
    return os.path.join(_c2wrapDir(), "c2wrap.py")

def _template():
    option = getattr(opts, "%s_template" % name)
    return option

def _templateDir():
    option = getattr(opts, "%s_template_dir" % name)
    if option == None:
        return _c2wrapDir()
    return option

def find():
    return glob('*.def')

def clean(files):
    pass

def _hash(fn):
    return md5(open(fn,"rb").read()).hexdigest()

def install(files):
    if not exists(_c2wrapScript()):
        trace("plugin_comcom2: c2wrap.py not found in: %s" % _c2wrapDir())
        return
    
    tmp_dir = tempfile.mkdtemp();
    for df in files:
        out_dir = dirname(df)
        os.chdir(out_dir)
        c2wrap_dest = parseConfig("build.cfg").get("c2wrap_dest", {})
        trace("Running c2wrap for %s" % df, "~")
        cmd = ["python", _c2wrapScript(), df, "--templatedir", _templateDir(), "--destdir", tmp_dir]
        if _template():
            cmd.extend(["--template", _template()])
        system3(*cmd)

        for f in os.listdir(tmp_dir):
            if f in c2wrap_dest.keys():
                dst_dir = os.path.join(out_dir, c2wrap_dest[f])
                if not os.path.exists(dst_dir):
                    os.makedirs(dst_dir)
                orig_fname = os.path.join(out_dir, c2wrap_dest[f], f)
            else:
                orig_fname = os.path.join(out_dir,f)
            new_fname = os.path.join(tmp_dir,f)
            if os.path.isfile(orig_fname) and _hash(orig_fname) == _hash(new_fname):
                print "plugin_comcom2: " + orig_fname + " left unmodified"
                os.remove(new_fname)
                continue
            else:
                shutil.move(new_fname, orig_fname)
                print "plugin_comcom2: " + orig_fname + " overwritten"
    shutil.rmtree(tmp_dir)
