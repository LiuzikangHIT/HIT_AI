import sys

import pymysql

from query import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox

from table_ import Table


class Query(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(Query, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.search_1)
        self.pushButton_2.clicked.connect(self.search_2)
        self.pushButton_3.clicked.connect(self.search_3)

    def search_1(self):
        name = self.textEdit.toPlainText()
        print(name)
        if not name:
            QMessageBox.warning(self, '警告', '请输入姓名')
        else:
            db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
            cursor = db.cursor()
            sql = f'select avg(gno), count(*) from student natural join grade where sname=%s'
            cursor.execute(sql, name)
            result = cursor.fetchall()
            if result[0][0] is None:
                QMessageBox.warning(self, '警告', '查无此人')
                return
            print(result)
            str_avg = name + f'选了{result[0][1]}门课，所有课程平均分为{round(result[0][0], 2)}'
            print(str_avg)
            QMessageBox.information(self, '查询成功', str_avg)

    def search_2(self):
        dorm = self.textEdit_2.toPlainText()
        print(dorm)
        if not dorm:
            QMessageBox.warning(self, '警告', '请输入宿舍名')
        else:
            db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
            cursor = db.cursor()
            sql = ('select sno, sname, dename, clno, dono from student natural join '
                   'department where dono in (select dono from dorm where noname=%s)')
            cnt = cursor.execute(sql, dorm)
            result = cursor.fetchall()
            head = ['学号', '姓名', '所在系名', '班号', '寝室号']
            if not result:
                QMessageBox.warning(self, '警告', '查无此舍')
                return
            print(result)
            self.table_1 = Table(head, result)
            self.table_1.show()

    def search_3(self):
        score = self.textEdit_3.toPlainText()
        print(score)
        if not score:
            QMessageBox.warning(self, '警告', '请输入阈值')
        elif int(score) > 100 or int(score) < 0:
            QMessageBox.warning(self, '警告', '请输入正确的阈值')
        else:
            db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
            cursor = db.cursor()
            sql = 'select cname, avg(gno) from course natural join grade group by cno having avg(gno)>%s'
            cnt = cursor.execute(sql, score)
            result = cursor.fetchall()
            head = ['课程名', '平均分']
            self.table_2 = Table(head, result)
            self.table_2.show()
            print(result)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = Query()
    MainWindow.show()
    sys.exit(app.exec_())
