import sys
import psutil
from PyQt5.QtWidgets import QApplication, QMainWindow, QGridLayout, QGroupBox, QVBoxLayout, QLabel, QStyle
from PyQt5.QtCore import Qt, QTimer, QThread, pyqtSignal
import pyqtgraph as pg


class CPUMonitorThread(QThread):
    cpu_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            cpu_usage = psutil.cpu_percent() 
            self.cpu_data_updated.emit([cpu_usage])
            self.msleep(1000)
    

class MemoryMonitorThread(QThread):
    mem_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            mem_percent = psutil.virtual_memory().percent
            self.mem_data_updated.emit([mem_percent])
            self.msleep(1000)

class StorageMonitorThread(QThread):
    storage_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            disk_usage = psutil.disk_usage('/')
            disk_io = psutil.disk_io_counters()
            self.storage_data_updated.emit([disk_usage.percent, disk_io.write_bytes/10000000, disk_io.read_bytes /10000000])
            self.msleep(1000)


class CPUUsageGraph(pg.PlotWidget):
    def __init__(self):
        super().__init__(background=(50, 50, 50))
        self.setTitle('CPU Usage')
        self.setYRange(0, 100)
        self.showGrid(False, False)
        self.cpu_curve = self.plot(pen='w')
        self.cpu_data = [0]

    def update_data(self, data):
        self.cpu_data.append(data[0])
        self.cpu_curve.setData(self.cpu_data)


class MemoryUsageGraph(pg.PlotWidget):
    def __init__(self):
        super().__init__(background=(50, 50, 50))
        self.setTitle('Memory Usage')
        self.setYRange(0, 100)
        self.showGrid(False, False)
        self.mem_curve = self.plot(pen='w')
        self.mem_data = [0]

    def update_data(self, data):
        self.mem_data.append(data[0])
        self.mem_curve.setData(self.mem_data)

class StorageUsageGraph(pg.PlotWidget):
    def __init__(self):
        super().__init__(background=(50, 50, 50))
        self.setTitle('Storage Usage')
        self.setYRange(0, 100)
        self.showGrid(False, False)
        self.write_curve = self.plot(pen='r', name='Write')
        self.read_curve = self.plot(pen='b', name='Read')
        self.write_data = [0]
        self.read_data = [0]

    def update_data(self, data):
        self.write_data.append(data[1])
        self.read_data.append(data[2])
        self.write_curve.setData(self.write_data)
        self.read_curve.setData(self.read_data)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Set window title and size
        self.setWindowTitle('System Monitor')
        self.setFixedSize(800, 800)

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

        #Create storage usage widget 
        self.storage_usage_graph = StorageUsageGraph()
        self.storage_usage_label = QLabel(self)
        self.storage_usage_label.setAlignment(Qt.AlignCenter)
        self.storage_usage_label.setText('Write: 0B  Read 0B')
        self.storage_usage_label.setStyleSheet('color: #fff; background-color: #FFC107; border-radius: 10px; font-weight: bold; font-size: 16pt; padding: 10px')
        
        # Add widgets to main window
        central_widget = QGroupBox(self)
        layout = QVBoxLayout(central_widget)
        layout.addWidget(self.cpu_usage_graph)
        layout.addWidget(self.cpu_usage_label)
        layout.addWidget(self.mem_usage_graph)
        layout.addWidget(self.mem_usage_label)
        layout.addWidget(self.storage_usage_graph)
        layout.addWidget(self.storage_usage_label)
        self.setCentralWidget(central_widget)

        # Start threads to update CPU, memory usage data and storage data
        self.cpu_monitor_thread = CPUMonitorThread()
        self.cpu_monitor_thread.cpu_data_updated.connect(self.cpu_usage_graph.update_data)
        self.cpu_monitor_thread.cpu_data_updated.connect(lambda x: self.cpu_usage_label.setText(f'{x[-1]:.2f}%'))
        self.cpu_monitor_thread.start()

        self.mem_monitor_thread = MemoryMonitorThread()
        self.mem_monitor_thread.mem_data_updated.connect(self.mem_usage_graph.update_data)
        self.mem_monitor_thread.mem_data_updated.connect(lambda x: self.mem_usage_label.setText(f'{x[-1]:.2f}%'))
        self.mem_monitor_thread.start()

        self.storage_monitor_thread = StorageMonitorThread()
        self.storage_monitor_thread.storage_data_updated.connect(self.storage_usage_graph.update_data)
        self.storage_monitor_thread.storage_data_updated.connect(lambda x: self.storage_usage_label.setText(f'{x[-1]:.2f}%'))
        self.storage_monitor_thread.start()

        # Update widgets every second
        self.timer = QTimer(self)
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.update_widgets)
        self.timer.start()

        # Set stylesheet
        self.setStyleSheet('''
           QMainWindow {
              background-color: #222;
           }
        ''')

    def update_widgets(self):
        # Update CPU usage
        cpu_percent = psutil.cpu_percent()
        self.cpu_usage_graph.update_data([cpu_percent])
        self.cpu_usage_label.setText(f'{cpu_percent:.2f}%')

        # Update memory usage
        mem_percent = psutil.virtual_memory().percent
        self.mem_usage_graph.update_data([mem_percent])
        self.mem_usage_label.setText(f'{mem_percent:.2f}%')

        # Update disk I/O counters
        disk_io_counters = psutil.disk_io_counters()
        write_bytes = (disk_io_counters.write_bytes)/10000000
        read_bytes = disk_io_counters.read_bytes/10000000
        self.storage_usage_graph.update_data([write_bytes, read_bytes])
        self.mem_usage_label.setText(f'{mem_percent:.2f}%')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())