#-*- coding: utf-8 -*-
import os
import os.path
import glob
import imp
import sys

plugin_path = os.path.dirname(os.path.abspath(__file__))
plugin_filter = os.path.join(plugin_path, "plugin_*.py")

all = []
for plugin in glob.glob(plugin_filter):
    name = os.path.basename(plugin)[:-3]
    full_name = "plugins." + name
    imp.load_source(full_name, plugin)
    all.append(sys.modules.get(full_name))
    locals()[name] = all[-1]
    all[-1].name = name.replace("plugin_", "")

def matchName(name):
    return [p for p in all if name in p.__name__]

def get(name):
    matches = matchName(name)
    if len(matches) > 1:
        raise ValueError("ambiguous plugin name: %r matches %r" %
                         (name, [p.name for p in matches]))
    elif not matches:
        raise ValueError("no plugin matching %r in %r" %
                         (name, [p.name for p in all]))
    return matches[0]

__all__ = []
