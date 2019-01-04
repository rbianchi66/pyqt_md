#!/usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import QWidget, QPainter

class MDObject(QWidget):
    def __init__(self, parent = None):
        QWidget.__init__(self, parent)
        self.elevation = 0
        self.enabled = True

    def paintEvent(self, event):
        print("paint")
        dc = QPainter(self)
        self.drawShadow(dc)
        self.drawSurface(dc)
        dc.end()

    def drawSurface(self, dc):
        pass
            
    def drawShadow(self, dc):
        pass