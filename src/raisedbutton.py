#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.Qt import *


class RaisedButton(QPushButton):
    def __init__(self, *args):
        QPushButton.__init__(self, *args)

        self.pressed_color = QColor(153,153,153,255*0.2) 
        self.effect = QGraphicsDropShadowEffect(self)
        self.effect.setOffset(1,3)
        self.effect.setBlurRadius(15)
        self.setGraphicsEffect(self.effect)
        self.setMinimumSize(64, 36)
        self.setMouseTracking(True)
        self.pressed = False
        self.press_effect = 0.0
        self.press_point = None
        self.press_timer = None
        self.radius = None
        self.region = None

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
            self.press_effect += 0.1
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
