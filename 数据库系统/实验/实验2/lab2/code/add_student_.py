import sys

import pymysql

from add_student import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox


class AddStu(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(AddStu, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.add_info)

    def add_info(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()

        sno = self.textEdit.toPlainText()
        sql = 'select sno from student'
        cursor.execute(sql)
        snos = cursor.fetchall()
        for s in snos:
            if s[0] == sno:
                QMessageBox.warning(self, '警告', '学号已存在')
                return

        sname = self.textEdit_2.toPlainText()
        deno = self.textEdit_3.toPlainText()
        sql = 'select deno from department'
        cursor.execute(sql)
        denos = cursor.fetchall()
        for d in denos:
            if d[0] == deno:
                break
        else:
            QMessageBox.warning(self, '警告', '系号不存在')
            return

        clno = self.textEdit_4.toPlainText()
        sql = 'select clno from class'
        cursor.execute(sql)
        clnos = cursor.fetchall()
        for c in clnos:
            if c[0] == clno:
                break
        else:
            QMessageBox.warning(self, '警告', '班号不存在')
            return

        dono = self.textEdit_5.toPlainText()
        sql = 'select dono from dorm'
        cursor.execute(sql)
        donos = cursor.fetchall()
        for d in donos:
            if d[0] == dono:
                break
        else:
            QMessageBox.warning(self, '警告', '寝室号不存在')
            return

        args = [sno, sname, deno, clno, dono]
        if not sno or not sname or not deno or not clno or not dono:
            QMessageBox.warning(self, '警告', '值不能为空')
            return
        print(args)
        sql = 'insert into student values(%s, %s, %s, %s, %s)'
        cnt = cursor.execute(sql, args)
        print(cnt)
        QMessageBox.information(self, '提示', '插入成功')
        db.commit()
        db.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = AddStu()
    MainWindow.show()
    sys.exit(app.exec_())