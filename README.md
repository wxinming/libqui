# libqui
qt平台的界面库，内部封装了控件、主题、文件操作类和软件自动更新模块，可供使用者方便快速开发。
效果图展示：
<img width="1598" height="1257" alt="libqui" src="https://github.com/user-attachments/assets/33572bb5-2156-4d03-a13f-13ff29a41167" />

```
一、环境要求
Qt 版本：需 ≥ 5.14.2（代码中明确校验：#error the qt version must be greater than or equal to 5.14.2）。
编码：默认使用 UTF-8（#pragma execution_character_set("utf-8")）。
编译：若使用动态库，需定义宏QUI_BUILD_DLL（编译库时）或QUI_DLL_IMPORT（使用库时，自动处理）。
二、基础配置
引入头文件在项目中包含核心头文件：
cpp
运行
#include "libqui/libqui/include/libqui.h"
链接库文件根据编译方式（静态 / 动态），在 Qt 项目文件（.pro）中配置库路径和链接选项，例如：
qmake
INCLUDEPATH += path/to/libqui/include
LIBS += -Lpath/to/libqui/lib -lqui  # 动态库或静态库链接
三、核心功能使用示例
1. 窗口创建与布局
libqui::Widget是基础窗口类，支持预定义布局（通过宏配置按钮、标题栏等），可创建模态 / 非模态窗口。
示例：创建主窗口使用QUI_MAIN_WINDOW_LAYOUT（默认包含 logo、菜单、最小化 / 最大化 / 关闭按钮等）：
cpp
运行
#include <QWidget>

// 自定义主窗口内容
class MyMainWidget : public QWidget {
public:
    MyMainWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("我的应用");
        // 添加自定义控件...
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 防止程序重复启动（使用唯一标识）
    QUI_APP_IS_RUNNING("MyAppUniqueKey");

    // 创建非模态主窗口（使用预定义布局）
    qui::Widget::doModeless<MyMainWidget, QUI_MAIN_WINDOW_LAYOUT>();

    return app.exec();
}
布局宏说明：预定义布局宏可直接使用，也可组合自定义（如QUI_SHOW_CLOSE_BTN | QUI_HIDE_WINDOW_TITLE）：
QUI_MAIN_WINDOW_LAYOUT：主窗口默认布局（含完整按钮组）。
QUI_DIALOG_WINDOW_LAYOUT：对话框布局（最小化 / 最大化 / 关闭按钮）。
QUI_FRAMELESS_WINDOW_LAYOUT：无边框窗口布局。
2. Ini 配置文件操作
通过qui::Ini类及相关宏（QUI_INI、QUI_INI_GROUP_LOCKER等）实现配置读写，支持多线程安全、加密及 JSON 类型。
示例：读写 Ini 配置
cpp
运行
// 初始化Ini（全局调用一次）
QUI_INI_INIT("config.ini");  // 指定配置文件路径

// 写入配置（支持基本类型、QStringList、QJsonObject等）
{
    QUI_INI_GROUP_LOCKER("User");  // 自动管理分组生命周期
    QUI_INI.setValue("Name", "张三");
    QUI_INI.setValue("Age", 25);
    QUI_INI.setValue("Hobbies", QStringList() << "读书" << "运动");
}

// 读取配置
QString name = QUI_INI.value("User", "Name").toString();
int age = QUI_INI.value("User", "Age").toInt();
QStringList hobbies = QUI_INI.value("User", "Hobbies").toStringList();
3. 控件使用
库中封装了多种扩展控件（如LoginDialog、CaptchaLabel、CalendarLineEdit等），直接实例化即可使用。
示例：使用登录对话框
cpp
运行
// 显示登录对话框（含验证码）
qui::LoginDialog loginDlg;
loginDlg.setWindowTitle("用户登录");
if (loginDlg.exec() == QDialog::Accepted) {
    QString username = loginDlg.username();
    QString password = loginDlg.password();
    // 处理登录逻辑...
}
示例：日期选择输入框
cpp
运行
qui::CalendarLineEdit* dateEdit = new qui::CalendarLineEdit();
dateEdit->setPlaceholderText("选择日期");
// 选中日期后触发信号
connect(dateEdit, &qui::CalendarLineEdit::dateSelected, [](const QDate& date) {
    qDebug() << "选中日期：" << date.toString();
});
4. 宏与工具函数
库提供了大量便捷宏，简化常见操作：
QUI_APP_IS_RUNNING(UNIQUE_KEY)：防止程序重复启动（如上述主窗口示例）。
QUI_CREATE_TRAY：创建系统托盘（在窗口布局中添加此宏，或调用qui::Widget::showTrayMessage显示托盘消息）。
路径宏：QUI_CURRENT_PATH（程序目录）、QUI_DESKTOP_PATH（桌面路径）等。
cpp
运行
// 显示托盘消息
qui::Widget::showTrayMessage(mainWindow, "新消息通知", 5000);  // 显示5秒
5. 主题与样式设置
qui::Widget支持多种内置样式（如蓝色、黑色、扁平风格等），可动态切换：
cpp
运行
// 获取当前窗口并设置样式
qui::Widget* widget = qui::Widget::findSelf(mainWidget);
if (widget) {
    widget->setWindowStyle(qui::Widget::BlueStyle);  // 切换为蓝色样式
}
6. 自动更新功能
通过接口检查版本并触发更新：
cpp
运行
// 检查是否为最新版本
if (!qui::appIsLatestVersion()) {
    // 有更新，触发更新事件（需先注册更新事件）
    emit widget->updateAvailabel();
}
四、注意事项
许可证：遵循 MIT 许可证，使用时需保留原始版权和许可声明。
线程安全：qui::Ini类支持多线程安全，可通过QUI_INI_MUTEX_LOCKER宏加锁。
扩展功能：若需 SQL 支持，需定义宏QUI_SUPPORT_SQL（此时会引入 QSql 相关头文件）。
通过以上步骤，可快速利用libqui的封装能力简化 Qt 界面开发，减少重复编码。更多控件和接口的使用可参考change_logs.txt中记录的版本更新细节。使用libqui类库开发 Qt 应用程序可遵循以下步骤，结合核心功能模块举例说明：
一、环境要求
Qt 版本：需 ≥ 5.14.2（代码中明确校验：#error the qt version must be greater than or equal to 5.14.2）。
编码：默认使用 UTF-8（#pragma execution_character_set("utf-8")）。
编译：若使用动态库，需定义宏QUI_BUILD_DLL（编译库时）或QUI_DLL_IMPORT（使用库时，自动处理）。
二、基础配置
引入头文件在项目中包含核心头文件：
cpp
运行
#include "libqui/libqui/include/libqui.h"
链接库文件根据编译方式（静态 / 动态），在 Qt 项目文件（.pro）中配置库路径和链接选项，例如：
qmake
INCLUDEPATH += path/to/libqui/include
LIBS += -Lpath/to/libqui/lib -lqui  # 动态库或静态库链接
三、核心功能使用示例
1. 窗口创建与布局
libqui::Widget是基础窗口类，支持预定义布局（通过宏配置按钮、标题栏等），可创建模态 / 非模态窗口。
示例：创建主窗口使用QUI_MAIN_WINDOW_LAYOUT（默认包含 logo、菜单、最小化 / 最大化 / 关闭按钮等）：
cpp
运行
#include <QWidget>

// 自定义主窗口内容
class MyMainWidget : public QWidget {
public:
    MyMainWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("我的应用");
        // 添加自定义控件...
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 防止程序重复启动（使用唯一标识）
    QUI_APP_IS_RUNNING("MyAppUniqueKey");

    // 创建非模态主窗口（使用预定义布局）
    qui::Widget::doModeless<MyMainWidget, QUI_MAIN_WINDOW_LAYOUT>();

    return app.exec();
}
布局宏说明：预定义布局宏可直接使用，也可组合自定义（如QUI_SHOW_CLOSE_BTN | QUI_HIDE_WINDOW_TITLE）：
QUI_MAIN_WINDOW_LAYOUT：主窗口默认布局（含完整按钮组）。
QUI_DIALOG_WINDOW_LAYOUT：对话框布局（最小化 / 最大化 / 关闭按钮）。
QUI_FRAMELESS_WINDOW_LAYOUT：无边框窗口布局。
2. Ini 配置文件操作
通过qui::Ini类及相关宏（QUI_INI、QUI_INI_GROUP_LOCKER等）实现配置读写，支持多线程安全、加密及 JSON 类型。
示例：读写 Ini 配置
cpp
运行
// 初始化Ini（全局调用一次）
QUI_INI_INIT("config.ini");  // 指定配置文件路径

// 写入配置（支持基本类型、QStringList、QJsonObject等）
{
    QUI_INI_GROUP_LOCKER("User");  // 自动管理分组生命周期
    QUI_INI.setValue("Name", "张三");
    QUI_INI.setValue("Age", 25);
    QUI_INI.setValue("Hobbies", QStringList() << "读书" << "运动");
}

// 读取配置
QString name = QUI_INI.value("User", "Name").toString();
int age = QUI_INI.value("User", "Age").toInt();
QStringList hobbies = QUI_INI.value("User", "Hobbies").toStringList();
3. 控件使用
库中封装了多种扩展控件（如LoginDialog、CaptchaLabel、CalendarLineEdit等），直接实例化即可使用。
示例：使用登录对话框
cpp
运行
// 显示登录对话框（含验证码）
qui::LoginDialog loginDlg;
loginDlg.setWindowTitle("用户登录");
if (loginDlg.exec() == QDialog::Accepted) {
    QString username = loginDlg.username();
    QString password = loginDlg.password();
    // 处理登录逻辑...
}
示例：日期选择输入框
cpp
运行
qui::CalendarLineEdit* dateEdit = new qui::CalendarLineEdit();
dateEdit->setPlaceholderText("选择日期");
// 选中日期后触发信号
connect(dateEdit, &qui::CalendarLineEdit::dateSelected, [](const QDate& date) {
    qDebug() << "选中日期：" << date.toString();
});
4. 宏与工具函数
库提供了大量便捷宏，简化常见操作：
QUI_APP_IS_RUNNING(UNIQUE_KEY)：防止程序重复启动（如上述主窗口示例）。
QUI_CREATE_TRAY：创建系统托盘（在窗口布局中添加此宏，或调用qui::Widget::showTrayMessage显示托盘消息）。
路径宏：QUI_CURRENT_PATH（程序目录）、QUI_DESKTOP_PATH（桌面路径）等。
cpp
运行
// 显示托盘消息
qui::Widget::showTrayMessage(mainWindow, "新消息通知", 5000);  // 显示5秒
5. 主题与样式设置
qui::Widget支持多种内置样式（如蓝色、黑色、扁平风格等），可动态切换：
cpp
运行
// 获取当前窗口并设置样式
qui::Widget* widget = qui::Widget::findSelf(mainWidget);
if (widget) {
    widget->setWindowStyle(qui::Widget::BlueStyle);  // 切换为蓝色样式
}
6. 自动更新功能
通过接口检查版本并触发更新：
cpp
运行
// 检查是否为最新版本
if (!qui::appIsLatestVersion()) {
    // 有更新，触发更新事件（需先注册更新事件）
    emit widget->updateAvailabel();
}
四、注意事项
许可证：遵循 MIT 许可证，使用时需保留原始版权和许可声明。
线程安全：qui::Ini类支持多线程安全，可通过QUI_INI_MUTEX_LOCKER宏加锁。
扩展功能：若需 SQL 支持，需定义宏QUI_SUPPORT_SQL（此时会引入 QSql 相关头文件）。
通过以上步骤，可快速利用libqui的封装能力简化 Qt 界面开发，减少重复编码。更多控件和接口的使用可参考change_logs.txt中记录的版本更新细节。
```
