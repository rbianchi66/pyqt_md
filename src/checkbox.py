#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.Qt import *


class CheckBox(QWidget):
    def __init__(self, *args):
        QWidget.__init__(self, *args)
        self.focused_color = QColor(153,153,153,255*0.2) 
        self.pressed_color = QColor(153,153,153,255*0.4) 
        self.unchecked_icon = QPixmap("icons/ic_check_box_outline_blank_black_24dp/web/ic_check_box_outline_blank_black_24dp_1x.png")
        self.checked_icon = QPixmap("icons/ic_check_box_black_24dp/web/ic_check_box_black_24dp_1x.png")
        self.checked = False
        self.pressed = False
        self.focused = False
        self.setMinimumWidth(24*3)
        self.setMaximumHeight(24*3)
        self.setMouseTracking(True)
        self.press_effect = 0.0
        self.press_point = None
        self.press_timer = None
        r = max(self.width(), self.height())/2 
        self.region = QRegion(0, 0, r, r, QRegion.Ellipse)        

    def enterEvent(self, e):
        self.focused = True
        self.update()

    def leaveEvent(self, e):
        self.focused = False
        self.update()

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
        self.checked = not self.checked
        self.press_effect = 0.0
        if self.press_timer is not None:
            self.press_timer.stop()
            self.press_timer = None
        self.update()
        
    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.HighQualityAntialiasing)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        painter.setPen(Qt.black)

        if self.focused:
            r = max(self.width(), self.height())/2 
            painter.setBrush(self.focused_color)
            painter.setPen(self.focused_color)
            painter.drawEllipse(0, 0, r, r)
        if self.checked:
            painter.drawPixmap(QRect(6, 6, 24, 24), self.checked_icon)
        else:
            painter.drawPixmap(QRect(6, 6, 24, 24), self.unchecked_icon)
        painter.end()
        if self.pressed:
            self.drawPressed()

    def drawPressed(self):
        painter = QPainter(self)
        painter.setClipRegion(self.region)
        painter.setRenderHint(QPainter.HighQualityAntialiasing)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        painter.setBrush(self.pressed_color)
        painter.setPen(QPen(self.pressed_color))
        r = max(self.width(), self.height())
        print "r:", r*self.press_effect 
        painter.drawEllipse(self.press_point, r*self.press_effect, r*self.press_effect) 
        painter.end()

    def updatePressEffect(self):
        if self.press_effect < 1.0:
            self.press_effect += 0.1
            self.repaint()
        

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
        self.c1 = CheckBox(self)
        self.main_layout.addStretch(10)
        self.main_layout.addWidget(self.c1)
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
