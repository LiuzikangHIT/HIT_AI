import sys

from view_idx import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication
from view_ import View
from index_ import Index


class ViewIdx(QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(ViewIdx, self).__init__(parent)
        self.setupUi(self)
        self.pushButton.clicked.connect(self.view)
        self.pushButton_2.clicked.connect(self.index)

    def view(self):
        self.ui_view = View()
        self.ui_view.show()

    def index(self):
        self.ui_index = Index()
        self.ui_index.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = ViewIdx()
    MainWindow.show()
    sys.exit(app.exec_())