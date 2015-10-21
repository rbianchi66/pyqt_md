#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.Qt import *
import geo2d

class MainWindow(QMainWindow):
    def __init__(self, *args):
        QMainWindow.__init__(self, *args)
        self.frame = QFrame(self)
        self.setCentralWidget(self.frame)
        self.style_map = {}
        
        self.main_layout = QVBoxLayout()
        self.frame.setLayout(self.main_layout)
        self.lineedit = QLineEdit("Test")
        self.lineedit.setMaximumHeight(40)
        self.main_layout.addWidget(self.lineedit)
        self.progress = QProgressBar()
        self.progress.setMinimum(0)
        self.progress.setMaximum(100)
        self.progress.setValue(30)
        self.progress.setMaximumHeight(40)
        self.main_layout.addWidget(self.progress)
        
        self.button = QPushButton("Push")
        self.button.setMaximumHeight(40)
        self.main_layout.addWidget(self.button)
        
        self.radio1 = QRadioButton("select 1")
        self.radio2 = QRadioButton("select 2")
        self.main_layout.addWidget(self.radio1)
        self.main_layout.addWidget(self.radio2)
        
class App(QApplication):
    def __init__(self, *args):
        import os.path
        QApplication.__init__(self, *args)
        self.main = MainWindow()
        
        if os.path.exists("material.qss"):
            ss = open("material.qss","r")
            self.setStyleSheet(ss.read())
        
        W = 1000
        H = 750
        self.main.setGeometry((self.desktop().width() - W) / 2,
                              (self.desktop().height() - H) / 2, W, H)
        self.main.show()
        self.connect( self, SIGNAL("lastWindowClosed()"), self.byebye )
    def byebye( self ):
        self.exit(0)

def main(args):
    global app
    app = App(args)
    app.exec_()

if __name__ == "__main__":
    main(sys.argv)
