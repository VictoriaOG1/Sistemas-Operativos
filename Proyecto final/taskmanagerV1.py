import sys
import psutil
from PyQt5.QtWidgets import QApplication, QMainWindow, QGridLayout, QGroupBox, QVBoxLayout, QLabel, QStyle
from PyQt5.QtCore import Qt, QTimer, QThread, pyqtSignal
import pyqtgraph as pg


class CPUMonitorThread(QThread):
    cpu_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            cpu_percent = psutil.cpu_percent()
            self.cpu_data_updated.emit([cpu_percent])
            self.msleep(1000)


class MemoryMonitorThread(QThread):
    mem_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            mem_percent = psutil.virtual_memory().percent
            self.mem_data_updated.emit([mem_percent])
            self.msleep(1000)


class CPUUsageGraph(pg.PlotWidget):
    def __init__(self):
        super().__init__(background='w')
        self.setTitle('CPU Usage')
        self.setYRange(0, 100)
        self.showGrid(True, True)
        self.cpu_curve = self.plot()
        self.cpu_data = [0]

    def update_data(self, data):
        self.cpu_data.append(data[0])
        self.cpu_curve.setData(self.cpu_data)


class MemoryUsageGraph(pg.PlotWidget):
    def __init__(self):
        super().__init__(background='w')
        self.setTitle('Memory Usage')
        self.setYRange(0, 100)
        self.showGrid(True, True)
        self.mem_curve = self.plot()
        self.mem_data = [0]

    def update_data(self, data):
        self.mem_data.append(data[0])
        self.mem_curve.setData(self.mem_data)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set window title and size
        self.setWindowTitle('System Monitor')
        self.setFixedSize(800, 600)

        # Create CPU usage widget
        self.cpu_usage_graph = CPUUsageGraph()
        self.cpu_usage_label = QLabel(self)
        self.cpu_usage_label.setAlignment(Qt.AlignCenter)
        self.cpu_usage_label.setText('0%')
        self.cpu_usage_label.setStyleSheet('color: #fff; background-color: #4CAF50; border-radius: 10px; font-weight: bold; font-size: 16pt; padding: 10px')

        # Create memory usage widget
        self.mem_usage_graph = MemoryUsageGraph()
        self.mem_usage_label = QLabel(self)
        self.mem_usage_label.setAlignment(Qt.AlignCenter)
        self.mem_usage_label.setText('0%')
        self.mem_usage_label.setStyleSheet('color: #fff; background-color: #2196F3; border-radius: 10px; font-weight: bold; font-size: 16pt; padding: 10px')

        # Add widgets to main window
        central_widget = QGroupBox(self)
        layout = QVBoxLayout(central_widget)
        layout.addWidget(self.cpu_usage_graph)
        layout.addWidget(self.cpu_usage_label)
        layout.addWidget(self.mem_usage_graph)
        layout.addWidget(self.mem_usage_label)
        self.setCentralWidget(central_widget)

        # Start threads to update CPU and memory usage data
        self.cpu_monitor_thread = CPUMonitorThread()
        self.cpu_monitor_thread.cpu_data_updated.connect(self.cpu_usage_graph.update_data)
        self.cpu_monitor_thread.cpu_data_updated.connect(lambda x: self.cpu_usage_label.setText(f'{x[-1]:.0f}%'))
        self.cpu_monitor_thread.start()

        self.mem_monitor_thread = MemoryMonitorThread()
        self.mem_monitor_thread.mem_data_updated.connect(self.mem_usage_graph.update_data)
        self.mem_monitor_thread.mem_data_updated.connect(lambda x: self.mem_usage_label.setText(f'{x[-1]:.0f}%'))

        # Set stylesheet
        self.setStyleSheet('''
           QMainWindow {
              background-color: #f2f2f2;
           }
        ''')

    def update_widgets(self):
        # Update CPU usage
        cpu_percent = psutil.cpu_percent()
        self.cpu_data = self.cpu_data[1:] + [cpu_percent]
        self.cpu_curve.setData(self.cpu_data)
        self.cpu_usage_label.setText(f'{cpu_percent}%')

        # Update memory usage
        mem = psutil.virtual_memory()
        mem_percent = mem.percent
        self.mem_data = self.mem_data[1:] + [mem_percent]
        self.mem_curve.setData(self.mem_data)
        self.mem_usage_label.setText(f'{mem_percent}%')                                              
                                                         
if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())