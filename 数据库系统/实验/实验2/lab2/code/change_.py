import sys

from change import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication
from add_student_ import AddStu
from drop_student_ import DropStu
from modify_grade_ import ModifyGrd


class Change(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(Change, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.add_stu)
        self.pushButton_2.clicked.connect(self.drop_stu)
        self.pushButton_3.clicked.connect(self.modify_grd)

    def add_stu(self):
        self.ui_addstu = AddStu()
        self.ui_addstu.show()

    def drop_stu(self):
        self.ui_dropstu = DropStu()
        self.ui_dropstu.show()

    def modify_grd(self):
        self.ui_modifygrd = ModifyGrd()
        self.ui_modifygrd.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = Change()
    MainWindow.show()
    sys.exit(app.exec_())