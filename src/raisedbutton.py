#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.Qt import *


class RaisedButton(QPushButton):
    def __init__(self, *args):
        QPushButton.__init__(self, *args)

        self.effect = QGraphicsDropShadowEffect(self)
        self.effect.setOffset(1,3)
        self.effect.setBlurRadius(15)
        self.setGraphicsEffect(self.effect)
        self.setMinimumSize(64, 36)


class MainWindow(QMainWindow):
    def __init__(self, *args):
        QMainWindow.__init__(self, *args)
        self.frame = QFrame(self)
        p = QPalette(self.palette())
        p.setColor(QPalette.Background, QColor(250,250,250))
        self.frame.setAutoFillBackground(True)
        self.frame.setPalette(p)
        self.setCentralWidget(self.frame)
        self.style_map = {}
        
        self.main_layout = QHBoxLayout()
        self.frame.setLayout(self.main_layout)
        self.b1 = RaisedButton("RAISED BUTTON")
        self.b2 = RaisedButton("DISABLED BUTTON")
        self.b2.setEnabled(False)
        self.main_layout.addStretch(10)
        self.main_layout.addWidget(self.b1)
        self.main_layout.addWidget(self.b2)
        self.main_layout.addStretch(10)
        
class App(QApplication):
    def __init__(self, *args):
        import os.path
        QApplication.__init__(self, *args)
        self.main = MainWindow()
        
        if os.path.exists("material.qss"):
            ss = open("material.qss","r")
            self.setStyleSheet(ss.read())
        
        W = 400
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
