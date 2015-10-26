#!/usr/bin/env python
#-*- coding: utf-8 -*-

import sys
import math
from PyQt4.Qt import *
from raisedbutton import RaisedButton

class FloatingActionButton(RaisedButton):
    def __init__(self, radius=50, icon=None, *args):
        super(FloatingActionButton, self).__init__(*args)
        self.color = QColor(153,153,153,255*0.2)
        self.pressed_color = QColor(153,153,153,255*0.2) 
        if icon:
            self.pixmap = QPixmap(icon)
            self.disabled_pixmap = QPixmap(icon) 
            mask = self.disabled_pixmap.createMaskFromColor(QColor(0,0,0), Qt.MaskOutColor)
            p = QPainter(self.disabled_pixmap)
            p.setPen(QColor(100,100,100))
            p.drawPixmap(self.disabled_pixmap.rect(), mask, mask.rect())
            p.end()            
            
        self.setMouseTracking(True)
        self.effect = QGraphicsDropShadowEffect(self.parent())
        self.effect.setOffset(1,3)
        self.effect.setBlurRadius(15)
        self.setGraphicsEffect(self.effect)
        self.radius = radius
        self.resize(self.radius + 1, self.radius + 1)
        self.setMaximumSize(self.size())
        self.setMinimumSize(self.size())
        self.effect_size = self.radius
        self.pressed = False
        self.region = QRegion(1, 1, 
                              self.radius, self.radius, 
                                  QRegion.Ellipse)        

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.HighQualityAntialiasing)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)
        
        if self.pressed:
            painter.setBrush(self.pressed_color)
            painter.setPen(QPen(self.pressed_color))
        else:
            painter.setBrush(self.color)
            painter.setPen(QPen(self.color))
        painter.drawEllipse(1, 1, self.radius - 1, self.radius - 1)
        if self.isEnabled():
            painter.drawPixmap(QRect(14, 14, self.radius / 2, self.radius / 2), self.pixmap)
        else:
            painter.drawPixmap(QRect(14, 14, self.radius / 2, self.radius / 2), self.disabled_pixmap)

        if self.pressed:
            self.drawPressed()

        painter.end()

#     def mousePressEvent(self, e):
#         self.pressed = True
#         self.update()
# 
#     def mouseReleaseEvent(self, e):
#         self.pressed = False
#         self.update()

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
        self.b1 = FloatingActionButton(icon = "icons/ic_add_black_24dp/web/ic_add_black_24dp_1x.png")
        self.b2 = FloatingActionButton(icon = "icons/ic_arrow_back_black_24dp/web/ic_arrow_back_black_24dp_1x.png")
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
