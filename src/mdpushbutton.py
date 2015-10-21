#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.Qt import *


class MDPushButton(QPushButton):
    def __init__(self, *args):
        QPushButton.__init__(self, *args)
        effect = QGraphicsDropShadowEffect()
        effect.setBlurRadius(5)
        effect.setOffset(0,2)
        self.setGraphicsEffect(effect)
    

class MainWindow(QMainWindow):
    def __init__(self, *args):
        QMainWindow.__init__(self, *args)
        self.frame = QFrame(self)
        self.setCentralWidget(self.frame)
        self.style_map = {}
        
        self.main_layout = QHBoxLayout()
        self.frame.setLayout(self.main_layout)
        self.widget = MDPushButton("TEST")
        self.main_layout.addStretch(10)
        self.main_layout.addWidget(self.widget)
        self.main_layout.addStretch(10)
        
class App(QApplication):
    def __init__(self, *args):
        import os.path
        QApplication.__init__(self, *args)
        self.main = MainWindow()
        
        if os.path.exists("material.qss"):
            ss = open("material.qss","r")
            self.setStyleSheet(ss.read())
        
        W = 200
        H = 200
        self.main.setGeometry(W, H, W, H)
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
