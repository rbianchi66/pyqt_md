#############################################################################
##
## Copyright (c) 2011 Riverbank Computing Limited <info@riverbankcomputing.com>
## 
## This file is part of PyQt.
## 
## This copy of PyQt is licensed for use under the terms of the PyQt
## Commercial License Agreement version 3.6.  See the file LICENSE for more
## details.
## 
## PyQt is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
##
#############################################################################


# If pluginType is MODULE, the plugin loader will call moduleInformation.  The
# variable MODULE is inserted into the local namespace by the plugin loader.
pluginType = MODULE


# moduleInformation() must return a tuple (module, widget_list).  If "module"
# is "A" and any widget from this module is used, the code generator will write
# "import A".  If "module" is "A[.B].C", the code generator will write
# "from A[.B] import C".  Each entry in "widget_list" must be unique.
def moduleInformation():
    return "PyQt4.QtDeclarative", ("QDeclarativeView", )
