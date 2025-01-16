import sys

import pymysql

from modify_grade import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox


class ModifyGrd(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(ModifyGrd, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.add_grade)
        self.pushButton_2.clicked.connect(self.update_grade)
        self.pushButton_3.clicked.connect(self.drop_grade)

    def add_grade(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()

        cno = self.textEdit_2.toPlainText()
        sql = 'select cno from course'
        cnt = cursor.execute(sql)
        cnos = cursor.fetchall()
        for c in cnos:
            if c[0] == cno:
                break
        else:
            QMessageBox.warning(self, '警告', '课程号不存在')
            return

        sno = self.textEdit_3.toPlainText()
        sql = 'select sno from student'
        cnt = cursor.execute(sql)
        snos = cursor.fetchall()
        for s in snos:
            if s[0] == sno:
                break
        else:
            QMessageBox.warning(self, '警告', '学号不存在')
            return

        grade = self.textEdit_4.toPlainText()
        pri_key = (cno, sno)
        print(pri_key)
        sql = 'select cno, sno from grade'
        cnt = cursor.execute(sql)
        pri_keys = cursor.fetchall()
        if pri_key in pri_keys:
            QMessageBox.warning(self, '警告', '该学生的该课程成绩已存在')
            return
        if not cno or not sno or not grade:
            QMessageBox.warning(self, '警告', '成绩不能为空')
            return
        value = [cno, sno, grade]
        sql = 'insert into grade values(%s, %s, %s)'
        cnt = cursor.execute(sql, value)
        print(cnt)
        QMessageBox.information(self, '提示', '插入成功')
        db.commit()
        db.close()

    def update_grade(self):
        cno = self.textEdit_2.toPlainText()
        sno = self.textEdit_3.toPlainText()
        grade = self.textEdit_4.toPlainText()
        pri_key = (cno, sno)
        print(pri_key)
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select cno, sno from grade'
        cnt = cursor.execute(sql)
        pri_keys = cursor.fetchall()
        if not cno or not sno:
            QMessageBox.warning(self, '警告', '值不能为空')
            return
        if pri_key not in pri_keys:
            QMessageBox.warning(self, '警告', '该学生的该课程成绩不存在')
            return
        value = [grade, cno, sno]
        sql = 'update grade set gno=%s where cno=%s and sno=%s'
        cnt = cursor.execute(sql, value)
        QMessageBox.information(self, '提示', '成功修改一条记录')
        db.commit()
        db.close()

    def drop_grade(self):
        cno = self.textEdit_2.toPlainText()
        sno = self.textEdit_3.toPlainText()
        grade = self.textEdit_4.toPlainText()
        pri_key = (cno, sno)
        print(pri_key)
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select cno, sno from grade'
        cnt = cursor.execute(sql)
        pri_keys = cursor.fetchall()
        if not cno or not sno:
            QMessageBox.warning(self, '警告', '值不能为空')
            return
        if pri_key not in pri_keys:
            QMessageBox.warning(self, '警告', '该学生的该课程成绩不存在')
            return
        value = [cno, sno]
        sql = 'delete from grade where cno=%s and sno=%s'
        cnt = cursor.execute(sql, value)
        print(cnt)
        QMessageBox.information(self, '提示', '成功删除一条记录')
        db.commit()
        db.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = ModifyGrd()
    MainWindow.show()
    sys.exit(app.exec_())
