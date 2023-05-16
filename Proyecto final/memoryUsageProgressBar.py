import psutil
import sys
import time
from PyQt5.QtCore import Qt, QThread, QTimer, pyqtSignal
from PyQt5.QtGui import QPainter, QColor, QFont, QPen, QBrush
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout
import pyqtgraph as pg


class MemoryMonitorThread(QThread):
    update_signal = pyqtSignal(float)

    def run(self):
        while True:
            memory_usage = psutil.virtual_memory().percent
            self.update_signal.emit(memory_usage)
            time.sleep(1)


class CircularProgressBar(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(200, 200)
        self.value = 0

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        width = self.width()
        height = self.height()
        size = min(width, height)
        radius = size / 2 - 10

        # Draw the outer circle
        outer_pen = QPen(QColor(200, 200, 200))
        outer_pen.setWidth(20)
        painter.setPen(outer_pen)
        painter.drawEllipse(width / 2 - radius, height / 2 - radius, 2 * radius, 2 * radius)

        # Draw the progress arc
        progress_pen = QPen(QColor(55, 192, 245))
        progress_pen.setWidth(20)
        progress_brush = QBrush(QColor(55, 192, 245))
        painter.setPen(progress_pen)
        painter.setBrush(progress_brush)
        painter.drawArc(width / 2 - radius, height / 2 - radius, 2 * radius, 2 * radius, 90 * 16, -self.value * 16 * 3.6)

        # Draw the text
        font = QFont()
        font.setPointSize(20)
        painter.setFont(font)
        painter.setPen(Qt.black)
        painter.drawText(0, 0, width, height, Qt.AlignCenter, f'{self.value}%')

    def setValue(self, value):
        self.value = value
        self.update()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Memory Monitor")
        self.resize(200, 200)

        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)

        self.layout = QVBoxLayout(self.central_widget)

        self.progress_bar = CircularProgressBar(self.central_widget)
        self.layout.addWidget(self.progress_bar)

        self.thread = MemoryMonitorThread()
        self.thread.update_signal.connect(self.update_memory_progress)
        self.thread.start()

        self.timer = QTimer()
        self.timer.timeout.connect(self.update_gui)
        self.timer.start(100)

    def update_memory_progress(self, memory_usage):
        self.progress_bar.setValue(memory_usage)

    def update_gui(self):
        QApplication.processEvents()

    def closeEvent(self, event):
        self.thread.quit()
        self.thread.wait()
        super().closeEvent(event)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())