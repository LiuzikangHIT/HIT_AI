import sys

import pymysql

from show import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication

from table_ import Table
from query_ import Query


class Show(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(Show, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.info_stu)
        self.pushButton_2.clicked.connect(self.info_tea)
        self.pushButton_3.clicked.connect(self.query)
        self.pushButton_4.clicked.connect(self.info_cou)
        self.pushButton_5.clicked.connect(self.info_gra)

    def info_stu(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select sno, sname, dename, clno, dono from student natural join department'
        cnt = cursor.execute(sql)
        stu = cursor.fetchall()
        head = ['学号', '姓名', '所在系名', '班号', '寝室号']
        self.ui_stu = Table(head, stu)
        self.ui_stu.show()

    def info_tea(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select tno, tname, dename from teacher natural join department'
        cnt = cursor.execute(sql)
        tea = cursor.fetchall()
        # print(tea)
        head = ['教师工号', '姓名', '所在系名']
        self.ui_stu = Table(head, tea)
        self.ui_stu.show()

    def info_cou(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select cno, cname, tname, dename from course natural join teacher natural join department'
        cursor.execute(sql)
        cou = cursor.fetchall()
        # print(tea)
        head = ['课程编号', '课程名称', '授课教师', '所在系']
        self.ui_cou = Table(head, cou)
        self.ui_cou.show()

    def info_gra(self):
        db = pymysql.connect(host='localhost', user='root', passwd='123456', database='student')
        cursor = db.cursor()
        sql = 'select sno, sname, cno, cname, gno from grade natural join student natural join course'
        cursor.execute(sql)
        gra = cursor.fetchall()
        head = ['学号', '姓名', '课程编号', '课程名称', '成绩']
        self.ui_gra = Table(head, gra)
        self.ui_gra.show()

    def query(self):
        self.ui_query = Query()
        self.ui_query.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = Show()
    MainWindow.show()
    sys.exit(app.exec_())
