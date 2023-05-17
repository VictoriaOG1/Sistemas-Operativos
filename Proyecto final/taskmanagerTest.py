import sys
import os
from PyQt5.QtWidgets import QApplication, QMainWindow, QAction, QVBoxLayout, QWidget, QLabel, QListWidget, QListWidgetItem
from PyQt5.QtGui import QIcon
from pathlib import Path

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setFixedSize(800, 600)

        self.initUI()

    def initUI(self):
        # Create a container widget
        container = QWidget()
        self.setCentralWidget(container)

        # Create a layout for the container widget
        container_layout = QVBoxLayout(container)

        # Create a side menu widget
        side_menu_widget = QListWidget()
        side_menu_widget.setFixedWidth(170)

        # Set styles for the side menu
        side_menu_widget.setStyleSheet(
            "QListWidget {"
            "   background-color: #333333;"
            "   border: none;"
            "}"
            "QListWidget::Item {"
            "   padding: 8px 12px;"
            "   color: #ffffff;"
            "   margin-bottom: 30px"
            "}"
            "QListWidget::Item:hover {"
            "   background-color: #1B998B;"
            "}"
            "QListWidget::Item:selected {"
            "   background-color: #2B4570;"
            "}"
        )
        # Create actions for the side menu
        cpu_action = QAction(QIcon(str(os.getcwd())+ '/Proyecto final/icon_procesador.png'), 'CPU Usage', self)
        memory_action = QAction(QIcon(str(os.getcwd())+ '/Proyecto final/icon_memoria.png'), 'Memory Usage', self)
        storage_action = QAction(QIcon(str(os.getcwd())+ '/Proyecto final/icon_disk.png'),'Storage Usage', self)
        network_action = QAction(QIcon(str(os.getcwd())+ '/Proyecto final/icon_red.png'),'Network Usage', self)
        processes_action = QAction(QIcon(str(os.getcwd())+ '/Proyecto final/icon_proceso.png'),'Processes Usage', self)


        # Add actions to the side menu
        self.add_action_to_menu(side_menu_widget, cpu_action)
        self.add_action_to_menu(side_menu_widget, memory_action)
        self.add_action_to_menu(side_menu_widget, storage_action)
        self.add_action_to_menu(side_menu_widget, network_action)
        self.add_action_to_menu(side_menu_widget, processes_action)

        # Create a main content widget
        main_content_widget = QWidget()
        main_content_layout = QVBoxLayout(main_content_widget)


        # Add the side menu widget and main content widget to the container layout
        container_layout.addWidget(side_menu_widget)
        container_layout.addWidget(main_content_widget)


        # Set the main window properties
        self.setWindowTitle('System Monitor')
        self.setGeometry(300, 300, 400, 300)

        # Connect the actions to their respective slots
        #new_action.triggered.connect(lambda: content_label.setText('New Action'))
        #open_action.triggered.connect(lambda: content_label.setText('Open Action'))
        #save_action.triggered.connect(lambda: content_label.setText('Save Action'))
        #exit_action.triggered.connect(self.close)

        # Show the main window
        self.show()

    def add_action_to_menu(self, menu_widget, action):
        item = QListWidgetItem(action.text())
        item.setIcon(action.icon())
        item.setData(32, action)  # Set action as item's data
        menu_widget.addItem(item)

if __name__ == '__main__':
    app = QApplication(sys.argv)

    # Set global style for the application (Dark Mode)
    app.setStyleSheet(
        "QWidget {"
        "   background-color: #222222;"
        "}"
    )

    window = MainWindow()
    sys.exit(app.exec_())