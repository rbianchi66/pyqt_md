#!/usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import QWidget, QPainter, QPen, QColor, QBrush, QPushButton
from PyQt4.QtGui import QFrame, QMainWindow, QPalette
from PyQt4.QtGui import QHBoxLayout, QVBoxLayout, QApplication, QGraphicsDropShadowEffect
from PyQt4.QtCore import SIGNAL, Qt
from mdobject import MDObject

class MDButton(QPushButton):
    def __init__(self, *args):
        QPushButton.__init__(self, *args)
        self.elevation = 8

        self.pressed_color = QColor(153,153,153,255*0.2) 
        self.effect = QGraphicsDropShadowEffect(self)
        self.effect.setOffset(1, self.elevation)
        self.effect.setBlurRadius(self.elevation)
        self.setGraphicsEffect(self.effect)
        self.setFixedSize(128, 36)
        self.setMouseTracking(True)
        self.pressed = False
        self.press_effect = 0.0
        self.press_point = None
        self.press_timer = None
        self.radius = None
        self.region = None

    def setElevation(self, e):
        self.elevation = e
        r = self.elevation/2
        if r == 0:
            r = 1
        self.effect.setOffset(1, r)
        self.effect.setBlurRadius(2*r)

    def paintEvent(self, event):
        QPushButton.paintEvent(self, event)
        if self.pressed:
            self.drawPressed()

    def drawPressed(self):
        painter = QPainter()
        painter.begin(self)
        if self.region is not None:
            painter.setClipRegion(self.region)
        painter.setRenderHint(QPainter.HighQualityAntialiasing)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        painter.setBrush(self.pressed_color)
        painter.setPen(QPen(self.pressed_color))
        r = self.radius
        if r is None:
            r = max(self.width(), self.height())
        painter.drawEllipse(self.press_point, r*self.press_effect, r*self.press_effect) 
        painter.end()

    def updatePressEffect(self):
        if self.press_effect < 1.0:
            self.press_effect += 0.2
            self.repaint()

    def mousePressEvent(self, e):
        self.pressed = True
        self.press_point = e.pos()
        self.press_timer = QTimer(self)
        self.press_timer.setInterval(20)
        self.connect(self.press_timer, SIGNAL("timeout()"), self.updatePressEffect)
        self.press_timer.start()
        self.update()

    def mouseReleaseEvent(self, e):
        self.pressed = False
        self.press_effect = 0.0
        if self.press_timer is not None:
            self.press_timer.stop()
            self.press_timer = None
        self.update()
    
    
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
        self.b1 = MDButton("BUTTON1")
        self.b2 = MDButton("BUTTON2")
        self.b3 = MDButton("BUTTON3")
        self.b1.setElevation(1)
        self.b2.setElevation(8)
        self.b3.setElevation(16)
        self.main_layout.addStretch(10)
        self.main_layout.addWidget(self.b1)
        self.main_layout.addWidget(self.b2)
        self.main_layout.addWidget(self.b3)
        self.main_layout.addStretch(10)
        
class App(QApplication):
    def __init__(self, *args):
        QApplication.__init__(self, *args)
        self.main = MainWindow()
        
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

import sys
if __name__ == "__main__":
    main(sys.argv)
    