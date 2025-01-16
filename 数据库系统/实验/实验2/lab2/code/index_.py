import sys

import pymysql

from index import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox


class Index(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(Index, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.cerate_index)

    def cerate_index(self):
        index = self.comboBox.currentIndex()
        dic_info = {0: 'sname', 1: 'deno', 2: 'clno', 3: "dono"}
        key = dic_info[index]
        print(key)
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'create index ' + key + '_index on student (' + key + ')'
        cnt = cursor.execute(sql)
        print(cnt)
        QMessageBox.information(self, '提示', '创建索引成功')


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = Index()
    MainWindow.show()
    sys.exit(app.exec_())