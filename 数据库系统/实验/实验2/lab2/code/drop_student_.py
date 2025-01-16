import sys

import pymysql

from drop_student import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox


class DropStu(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(DropStu, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.drop_info)

    def drop_info(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        index = self.comboBox.currentIndex()
        value = self.textEdit_2.toPlainText()
        if not value:
            QMessageBox.warning(self, '警告', '值不能为空')
            return
        dic_info = {0: 'sno', 1: 'sname', 2: 'deno', 3: 'clno', 4: "dono"}
        # print(index)
        key = dic_info[index]
        # print(key)
        sql = 'select ' + key + ' from student'
        cursor.execute(sql)
        key_all = cursor.fetchall()
        # print(key_all)
        values = []
        for i in range(len(key_all)):
                values.append(key_all[i][0])
        # print(values)
        if value not in values:
            QMessageBox.warning(self, '警告', '数据库中不存在此值')
            return
        sql = 'delete from student where ' + key + '=%s'
        cnt = cursor.execute(sql, value)
        QMessageBox.information(self, '提示', f'删除成功，删除了{cnt}条记录')
        db.commit()
        db.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = DropStu()
    MainWindow.show()
    sys.exit(app.exec_())
