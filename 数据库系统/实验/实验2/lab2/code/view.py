from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 481)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(200, 60, 611, 61))
        font = QtGui.QFont()
        font.setFamily("宋体")
        font.setPointSize(20)
        self.label.setFont(font)
        self.label.setObjectName("label")

        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(280, 180, 141, 41))
        font = QtGui.QFont()
        font.setFamily("宋体")
        font.setPointSize(16)
        self.label_2.setFont(font)
        self.label_2.setObjectName("label_2")

        self.comboBox = QtWidgets.QComboBox(self.centralwidget)
        self.comboBox.setGeometry(QtCore.QRect(400, 175, 151, 51))
        font = QtGui.QFont()
        font.setFamily("宋体")
        font.setPointSize(14)
        self.comboBox.setFont(font)
        self.comboBox.setObjectName("comboBox")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")

        self.pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.pushButton.setGeometry(QtCore.QRect(310, 290, 141, 51))
        font = QtGui.QFont()
        font.setFamily("宋体")
        font.setPointSize(16)
        self.pushButton.setFont(font)
        self.pushButton.setObjectName("pushButton")

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 30))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "教务管理系统"))
        self.label.setText(_translate("MainWindow", "为某班级的所有学生信息创建视图"))
        self.label_2.setText(_translate("MainWindow", "班级编号"))
        self.comboBox.setItemText(0, _translate("MainWindow", "2203101"))
        self.comboBox.setItemText(1, _translate("MainWindow", "2203102"))
        self.comboBox.setItemText(2, _translate("MainWindow", "2203103"))
        self.comboBox.setItemText(3, _translate("MainWindow", "2203601"))
        self.comboBox.setItemText(4, _translate("MainWindow", "2203602"))
        self.pushButton.setText(_translate("MainWindow", "创建"))


