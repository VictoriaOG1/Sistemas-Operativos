import sys
import psutil
from PyQt5.QtWidgets import QApplication, QMainWindow, QGridLayout, QGroupBox, QVBoxLayout, QLabel, QStyle, QHBoxLayout, QTableWidgetItem, QWidget
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

class ProcessMonitorThread(QThread):
    process_data_updated = pyqtSignal(list)

    def run(self):
        while True:
            process_list = []
            for process in psutil.process_iter():
                try:
                    name = process.name()
                    pid = process.pid
                    status = process.status()
                    memory_info = process.memory_info()
                    memory_usage = memory_info.rss / (1024 * 1024)  # Convert to MB
                    process_list.append((name, pid, status, memory_usage))
                except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                    pass
            self.process_data_updated.emit(process_list)
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
        self.write_data.append(data[0])
        self.read_data.append(data[1])
        self.write_curve.setData(self.write_data)
        self.read_curve.setData(self.read_data)

class ProcessUsageTable(pg.TableWidget):
    def __init__(self):
        super().__init__(sortable=True, editable=False)
        self.verticalHeader().setVisible(False)
        self.setColumnWidth(0, 200)
        self.setColumnWidth(1, 100)
        self.setColumnWidth(2, 100)
        self.setColumnWidth(3, 100)
        self.setHorizontalHeaderLabels(['Name', 'PID', 'Status', 'Memory Usage (MB)'])
        self.setData([])

    def setData(self, data):
        self.clearContents()
        self.setRowCount(len(data))
        for i, (name, pid, status, memory_usage) in enumerate(data):
            self.setItem(i, 0, QTableWidgetItem(name))
            self.setItem(i, 1, QTableWidgetItem(str(pid)))
            self.setItem(i, 2, QTableWidgetItem(status))
            self.setItem(i, 3, QTableWidgetItem(f'{memory_usage:.2f}'))
        self.resizeColumnsToContents()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('Task Manager')
        self.setGeometry(100, 100, 800, 600)
        self.setup_ui()

    def setup_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)

        # CPU Usage
        cpu_group_box = QGroupBox('CPU Usage')
        cpu_layout = QVBoxLayout()
        cpu_group_box.setLayout(cpu_layout)
        self.cpu_usage_graph = CPUUsageGraph()
        cpu_layout.addWidget(self.cpu_usage_graph)
        main_layout.addWidget(cpu_group_box)

        # Memory Usage
        memory_group_box = QGroupBox('Memory Usage')
        memory_layout = QVBoxLayout()
        memory_group_box.setLayout(memory_layout)
        self.memory_usage_graph = MemoryUsageGraph()
        memory_layout.addWidget(self.memory_usage_graph)
        main_layout.addWidget(memory_group_box)

        # Storage Usage
        storage_group_box = QGroupBox('Storage Usage')
        storage_layout = QVBoxLayout()
        storage_group_box.setLayout(storage_layout)
        self.storage_usage_graph = StorageUsageGraph()
        storage_layout.addWidget(self.storage_usage_graph)
        main_layout.addWidget(storage_group_box)

        # Process Usage
        process_group_box = QGroupBox('Process Usage')
        process_layout = QVBoxLayout()
        process_group_box.setLayout(process_layout)
        self.process_usage_table = ProcessUsageTable()
        process_layout.addWidget(self.process_usage_table)
        main_layout.addWidget(process_group_box)

        self.show()

        # Start monitoring threads
        self.cpu_monitor_thread = CPUMonitorThread()
        self.cpu_monitor_thread.cpu_data_updated.connect(self.cpu_usage_graph.update_data)
        self.cpu_monitor_thread.start()

        self.memory_monitor_thread = MemoryMonitorThread()
        self.memory_monitor_thread.mem_data_updated.connect(self.memory_usage_graph.update_data)
        self.memory_monitor_thread.start()

        self.storage_monitor_thread = StorageMonitorThread()
        self.storage_monitor_thread.storage_data_updated.connect(self.storage_usage_graph.update_data)
        self.storage_monitor_thread.start()

        self.process_monitor_thread = ProcessMonitorThread()
        self.process_monitor_thread.process_data_updated.connect(self.process_usage_table.setData)
        self.process_monitor_thread.start()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())