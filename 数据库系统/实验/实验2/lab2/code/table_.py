import sys

from table import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QTableWidgetItem


class Table(QMainWindow, Ui_MainWindow):
    def __init__(self, head, datas, parent=None):
        super(Table, self).__init__(parent)
        self.setupUi(self)
        self.tableWidget.setRowCount(len(datas))
        self.tableWidget.setColumnCount(len(head))
        self.tableWidget.setHorizontalHeaderLabels(head)
        for i, data in enumerate(datas):
            for j, item in enumerate(data):
                self.tableWidget.setItem(i, j, QTableWidgetItem(str(item)))


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = Table(['学号', '姓名', '所在系名', '班号', '寝室号'], [(1, 2, 3, 4, 5)])
    MainWindow.show()
    sys.exit(app.exec_())
