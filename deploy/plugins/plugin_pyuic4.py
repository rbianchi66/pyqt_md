#-*- coding: utf-8 -*-

from common import *

priority = 30

def addOptionGroup(parser):
    import optparse
    group = optparse.OptionGroup(parser, "plugin %s options" % name)
    group.add_option("--%s-output" % name, metavar="DIR", default=".",
                     help="Compile *.ui files to *_ui.py to DIR, defaults to '.'")
    group.add_option("--%s-execute" % name, action="store_true", default=False,
                     help="As pyuic4 --execute, generate preview code, default=%default")
    parser.add_option_group(group)

def _output():
    return getattr(opts, "%s_output" % name)
def _execute():
    return getattr(opts, "%s_execute" % name)

def find():
    return glob("*.ui")

def install(files):
    for ui_file in files:
        os.chdir(dirname(ui_file))
        pyuic4 = "pyuic4"
        if os.name != "posix":
            pyuic4 += ".bat"
        if isfile(join(opts.source, pyuic4)):
            trace("Using custom pyuic4...", ".")
            bindir = opts.source
        else:
            trace("Using system pyuic4...", ".")
            bindir = ""
        cmd = [os.path.join(bindir, pyuic4)]

        trace("Compiling ui files...", ".")
        if _execute():
            cmd.append("-x")
        py_file = ui_file.replace(".ui", "_ui.py")
        output = join(_output(), py_file)
        system3(*cmd+["-o", output, ui_file])
    return True
