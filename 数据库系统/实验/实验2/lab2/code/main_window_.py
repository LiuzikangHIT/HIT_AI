import sys

import pymysql

from main_window import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox

from show_ import Show
from change_ import Change
from view_idx_ import ViewIdx


class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setupUi(self)
        self.setup_db_connection()
        self.pushButton_5.clicked.connect(self.query_all)
        self.pushButton_4.clicked.connect(self.change_all)
        self.pushButton_3.clicked.connect(self.view)

    def setup_db_connection(self):
        try:
            pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
            # QMessageBox.information(self, '提示', '数据库连接成功')
        except Exception as e:
            QMessageBox.critical(self, '错误', f'数据库连接失败: {str(e)}')

    def query_all(self):
        self.ui_show = Show()
        self.ui_show.show()

    def change_all(self):
        self.ui_change = Change()
        self.ui_change.show()

    def view(self):
        self.ui_view = ViewIdx()
        self.ui_view.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    Window = MainWindow()
    Window.show()
    sys.exit(app.exec_())
