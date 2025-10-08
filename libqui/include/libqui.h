#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets>
#include <queue>

#ifdef QUI_SUPPORT_SQL
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#endif // QUI_SUPPORT_SQL

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 2))
#error the qt version must be greater than or equal to 5.14.2
#endif

#if defined(QUI_BUILD_DLL)
#define QUI_DLL_EXPORT __declspec(dllexport)
#else
#define QUI_DLL_EXPORT __declspec(dllimport)
#endif

// 显示logo按钮
#define QUI_SHOW_LOGO_BTN 1

// 显示菜单按钮
#define QUI_SHOW_MENU_BTN 2

// 显示最小化按钮
#define QUI_SHOW_MIN_BTN 4

// 显示最大化(还原)按钮
#define QUI_SHOW_MAX_BTN 8

// 显示关闭按钮
#define QUI_SHOW_CLOSE_BTN 16

// 禁用最大化(还原)按钮
#define QUI_DISABLE_MAX_BTN 32

// 禁用关闭按钮
#define QUI_DISABLE_CLOSE_BTN 64

// 禁用大小手柄(窗口右下图标可实现拖拽)
#define QUI_DISABLE_SIZE_GRIP 128

// 隐藏窗口标题
#define QUI_HIDE_WINDOW_TITLE 256

// 关闭到托盘
#define QUI_CLOSE_TO_TRAY 512

// 主窗口关闭时,同时关闭所有子窗口
#define QUI_QUIT_ON_CLOSE 1024

// 检查更新
#define QUI_CHECK_UPDATE 2048

// 创建托盘
#define QUI_CREATE_TRAY 4096

// 主窗口布局
#define QUI_MAIN_WINDOW_LAYOUT (QUI_SHOW_LOGO_BTN | QUI_SHOW_MENU_BTN | QUI_SHOW_MIN_BTN | QUI_SHOW_MAX_BTN | QUI_SHOW_CLOSE_BTN | QUI_QUIT_ON_CLOSE | QUI_CHECK_UPDATE)

// 对话框窗口布局
#define QUI_DIALOG_WINDOW_LAYOUT (QUI_SHOW_MIN_BTN | QUI_SHOW_MAX_BTN | QUI_SHOW_CLOSE_BTN)

// 装饰物窗口布局
#define QUI_WIDGET_WINDOW_LAYOUT (QUI_DIALOG_WINDOW_LAYOUT | QUI_DISABLE_SIZE_GRIP)

// 简易窗口布局
#define QUI_SIMPLE_WINDOW_LAYOUT (QUI_SHOW_CLOSE_BTN | QUI_DISABLE_SIZE_GRIP)

// 无边框窗口布局
#define QUI_FRAMELESS_WINDOW_LAYOUT (QUI_WIDGET_WINDOW_LAYOUT | QUI_HIDE_WINDOW_TITLE)

// 当前路径
#define QUI_CURRENT_PATH qApp->applicationDirPath()

// 配置路径
#define QUI_CONFIG_PATH QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)

// 桌面路径
#define QUI_DESKTOP_PATH QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)

// 临时文件路径
#define QUI_TEMP_PATH QStandardPaths::writableLocation(QStandardPaths::TempLocation)

// 文件名
#define QUI_FILENAME (strrchr("\\" __FILE__, '\\') + 1)

// ini文件相关操作
#define QUI_INI_INIT qui::iniInit
#ifdef QUI_NOT_USE_INI_CREATE_GLOBAL_COPY
#define QUI_INI qui::iniGet()
#else
#define QUI_INI qui::iniCreateGlobalCopy(__FILE__)
#endif
#define QUI_INI_GROUP_LOCKER(gn, ...) qui::Ini::GroupLocker __qui_ini_g_locker__(QUI_INI, gn, ##__VA_ARGS__)
#define QUI_INI_READ_ARRAY_LOCKER(an, as) qui::Ini::ReadArrayLocker __qui_ini_ra_locker__(QUI_INI, an, as)
#define QUI_INI_WRITE_ARRAY_LOCKER(an, ...) qui::Ini::WriteArrayLocker __qui_ini_wa_locker__(QUI_INI, an, ##__VA_ARGS__)

// 日志相关操作
#define QUI_LOG_INIT qui::logInit
#define QUI_LOG qui::logGet()

// APP是否正在运行中
#define QUI_APP_IS_RUNNING(UNIQUE_KEY)  { \
	static QSharedMemory sm(UNIQUE_KEY); \
	if (!sm.create(sizeof(int))) { \
		qui::MsgBox::warning("警告", "程序已在运行中,请勿重复启动!"); \
		::exit(1); \
	} \
}

namespace qui {
	class IconHelper;
	class CustomButton;
	class Widget;
	class TreeWidget;
	class Ini;
	/**
	* @brief 扩展的QVariant类，支持JSON类型转换
	*
	* 该类继承自QVariant，增加了对QStringList、QJsonObject、QJsonArray和QByteArray的支持，
	*/
	class QUI_DLL_EXPORT Variant : protected QVariant {
	public:
		friend class Ini;

		enum class UserType {
			Range = QMetaType::User + 1,
		};

		// 构造函数
		inline Variant() : QVariant() {}
		inline ~Variant() {}
		inline Variant(int i) : QVariant(i) {}
		inline Variant(uint ui) : QVariant(ui) {}
		inline Variant(qlonglong ll) : QVariant(ll) {}
		inline Variant(qulonglong ull) : QVariant(ull) {}
		inline Variant(bool b) : QVariant(b) {}
		inline Variant(double d) : QVariant(d) {}
		inline Variant(float f) : QVariant(f) {}
		inline Variant(const char* s) : QVariant(s) {}
		inline Variant(const QString& s) : QVariant(s) {}
		inline Variant(const QStringList& strs) : QVariant(strs) {}
		inline Variant(const QLatin1String& s) : QVariant(s) {}
		inline Variant(const QJsonObject& jo) : QVariant(jo) {}
		inline Variant(const QJsonArray& ja) : QVariant(ja) {}
		inline Variant(const QByteArray& ba) : QVariant(ba) {}
		// 将会转换为x~y格式,使用toRange转换
		template<typename T, std::enable_if_t<std::is_arithmetic_v<T> ||
			std::is_same_v<T, QString>, int> = 0>
		inline Variant(const QPair<T, T>&p) : QVariant() {
			range_pair_ = p;
			d = QVariant::Private(static_cast<int>(UserType::Range));
		}

		// 基础类型转换
		using QVariant::toInt;
		using QVariant::toUInt;
		using QVariant::toLongLong;
		using QVariant::toULongLong;
		using QVariant::toBool;
		using QVariant::toDouble;
		using QVariant::toFloat;
		using QVariant::toString;

		// 扩展类型转换
		QStringList toStringList() const;
		QJsonObject toJsonObject() const;
		QJsonArray toJsonArray() const;
		QByteArray toByteArray() const;

		// 支持范围
		template<typename T, std::enable_if_t<std::is_arithmetic_v<T> ||
			std::is_same_v<T, QString>, int> = 0>
		inline QPair<T, T> toRange(bool* ok = nullptr) const {
			auto str = __super::toString();
			QPair<QVariant, QVariant> pair;
			if (!str.isEmpty()) {
				auto split = str.split("~", QString::SkipEmptyParts);
				if (split.size() == 2) {
					pair = qMakePair(split[0], split[1]);
				}

				if (ok) {
					*ok = split.size() == 2;
				}
			}
			else {
				if (ok) {
					*ok = false;
				}
				pair = range_pair_;
			}

			return { pair.first.value<T>(), pair.second.value<T>() };
		}

		// 类型信息查询
		using QVariant::type;
		using QVariant::typeName;
		using QVariant::canConvert;
		using QVariant::convert;
		using QVariant::isValid;
		using QVariant::isNull;
		using QVariant::userType;

	private:
		QPair<QVariant, QVariant> range_pair_;
	};

	using IniDataChangedCb = ::std::function<bool(const QString& group, const QString& key, const Variant& value)>;
	using IniTraverseArrayCb = ::std::function<bool(int index, const QString& key, const Variant& value)>;
	struct IniIconPaths {
		IniIconPaths() {}
		IniIconPaths(const QString& g, const QString& k, const QString& v, const QString& d)
			:group(g), key(k), value(v), description(d) {
		}
		~IniIconPaths() {}
		QString group;
		QString key;
		QString value;
		QString description;
	};

	//全局变量
	namespace global {
		QUI_DLL_EXPORT extern QList<Widget*> mainWidgets;
		QUI_DLL_EXPORT extern int sqlCount;
		QUI_DLL_EXPORT extern QSet<size_t> withMainWindowSameSizeSet;
		QUI_DLL_EXPORT extern const IniIconPaths defaultIniIconPaths;
	}

	/*
	* @brief 用户界面装饰物
	*/
	class QUI_DLL_EXPORT Widget : public QDialog {
		Q_OBJECT
	public:
		//按钮
		enum Button {
			//应用程序按钮
			AppButton,

			//菜单按钮
			MenuButton,

			//最小化按钮
			MinButton,

			//最大化按钮
			MaxButton,

			//还原按钮
			RestoreButton,

			//关闭按钮
			CloseButton
		};

		//样式
		enum Style {
			//银色样式
			SilveryStyle,

			//蓝色样式
			BlueStyle,

			//浅蓝色样式
			LightBlueStyle,

			//深蓝色样式
			DarkBlueStyle,

			//灰色样式
			GrayStyle,

			//浅灰色样式
			LightGrayStyle,

			//深灰色样式
			DarkGrayStyle,

			//黑色样式
			BlackStyle,

			//浅灰色样式
			LightBlackStyle,

			//深黑色样式
			DarkBlackStyle,

			//PS黑色样式
			PSBlackStyle,

			//黑色扁平样式
			FlatBlackStyle,

			//白色扁平样式
			FlatWhiteStyle
		};

	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		Widget(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~Widget();

		/*
		* @brief 构造
		* @param[in] mainWidget 主装饰物
		* @param[in] windowLayout 窗口布局
		* @param[in] parent 父对象
		*/
		template<class T>
		Widget(T mainWidget, int windowLayout, QWidget* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] mainWidget 主装饰物
		* @param[in] titleText 标题文本
		* @param[in] windowLayout 窗口布局
		* @param[in] parent 父对象
		*/
		template<class T>
		Widget(T mainWidget, const QString& titleText, int windowLayout, QWidget* parent = nullptr);

	public:
		/*
		* @brief 做模态
		* @tparam[in] T 主窗口类
		* @param[in] windowLayout 窗口布局
		* @tparam[in] args 主窗口类参数
		* @return int
		*/
		template<class T, int windowLayout, class... Args> static inline int doModal(Args&&... args)
		{
			auto widget = new Widget;
			widget->setWindowLayout(windowLayout);
			T* mainWidget = new T(std::forward<Args>(args)...);
			widget->setMainWidget(mainWidget);
			widget->setWindowTitle(mainWidget->windowTitle());
			return widget->exec();
		}

		/*
		* @brief 做模态
		* @param[in] mainWidget 主装饰物
		* @param[in] windowLayout 窗口布局
		* @return int
		*/
		template<class T> static inline int doModal(T mainWidget, int windowLayout)
		{
			return (new Widget(mainWidget, windowLayout))->exec();
		}

		/*
		* @brief 做模态
		* @param[in] mainWidget 主装饰物
		* @param[in] titleText 标题文本
		* @param[in] windowLayout 窗口标题
		* @return int
		*/
		template<class T> static inline int doModal(T mainWidget, const QString& titleText, int windowLayout)
		{
			return (new Widget(mainWidget, titleText, windowLayout))->exec();
		}

		/*
		* @brief 做非模态
		* @tparam[in] T 主窗口类
		* @param[in] windowLayout 窗口布局
		* @tparam[in] args 主窗口类参数
		* @return void
		*/
		template<class T, int windowLayout, class...Args> static inline void doModeless(Args&&...args)
		{
			auto widget = new Widget;
			widget->setWindowLayout(windowLayout);
			T* mainWidget = new T(std::forward<Args>(args)...);
			widget->setMainWidget(mainWidget);
			widget->setWindowTitle(mainWidget->windowTitle());
			widget->show();
		}

		/*
		* @brief 做非模态
		* @param[in] mainWidget 主装饰物
		* @param[in] windowLayout 窗口布局
		* @return void
		*/
		template<class T> static inline void doModeless(T mainWidget, int windowLayout)
		{
			(new Widget(mainWidget, windowLayout))->show();
		}

		/*
		* @brief 做非模态
		* @param[in] mainWidget 主装饰物
		* @param[in] titleText 标题文本
		* @param[in] windowLayout 窗口标题
		* @return void
		*/
		template<class T> static inline void doModeless(T mainWidget, const QString& titleText, int windowLayout)
		{
			(new Widget(mainWidget, titleText, windowLayout))->show();
		}

		/*
		* @brief 显示托盘消息
		* @param[in] mainWidget 主装饰物
		* @param[in] message 消息
		* @param[in] msec 毫秒
		* @return void
		*/
		static void showTrayMessage(QWidget* mainWidget, const QString& message, int msec = 3000);

		/*
		* @brief 设置事件过滤器
		* @param[in] mainWidget 主装饰物
		* @param[in] func 回调函数(对象, 事件)
		* @return void
		*/
		static void setEventFilter(QObject* mainWidget, ::std::function<bool(QObject* object, QEvent* event)>&& func);

		/*
		* @brief 设置原生事件
		* @param[in] mainWidget 主装饰物
		* @param[in] func 回调函数(事件类型, 消息, 结果)
		* @return void
		*/
		static void setNativeEvent(QObject* mainWidget, ::std::function<bool(const QByteArray& eventType, void* message, long* result)>&& func);

	public:
		/*
		* @brief 获取窗口布局
		* @return int
		*/
		int windowLayout() const;

		/*
		* @brief 获取窗口样式
		* @return int
		*/
		int windowStyle() const;

		/*
		* @brief 查找自己
		* @param[in] mainWidget 主装饰物
		* @retval !=nullptr 找到
		* @retval ==nullptr 未找到
		*/
		static Widget* findSelf(QWidget* mainWidget);

		/*
		* @brief 查找主装饰物
		* @return QList<T*>
		*/
		template<class T> static inline QList<T*> findMainWidget()
		{
			QList<T*> result;
			auto hashCode = typeid(T*).hash_code();
			for (auto& x : global::mainWidgets) {
				if (x->hashCode_ == hashCode) {
					result.append(dynamic_cast<T*>(x->mainWidget_));
				}
			}
			return result;
		}

		/*
		* @brief 是否包含主装饰物
		* @param[in] ifContainsShow 如果包含则显示
		* @return bool
		*/
		template<class T> static inline bool isContainsMainWidget(bool ifContainsShow = true) { 
			auto widgets = findMainWidget<T>();
			auto contains = !widgets.isEmpty();
			if (contains && ifContainsShow) {
				for (auto& x : widgets) {
					auto widget = qui::Widget::findSelf(x);
					if (widget) {
						if (!(widget->windowFlags() & Qt::WindowDoesNotAcceptFocus)) {
							if (!widget->isActiveWindow()) {
								widget->activateWindow();
							}
						}
						widget->isMaximized() ? widget->showMaximized() : widget->showNormal();
					}
				}
			}
			return contains;
		}

		/*
		* @brief 启用与主窗口相同大小
		* @return void
		* 示例:
		* qui::Widget::enableWithMainWindowSameSize<qui::SettingDialog>();
		*/
		template<class T> static inline void enableWithMainWindowSameSize() { 
			global::withMainWindowSameSizeSet.insert(typeid(T*).hash_code());
		}

		/*
		* @brief 禁用与主窗口相同大小
		* @return void
		* 示例:
		* qui::Widget::disableWithMainWindowSameSize<qui::SettingDialog>();
		*/
		template<class T> static inline void disableWithMainWindowSameSize() {
			global::withMainWindowSameSizeSet.remove(typeid(T*).hash_code());
		}

	public:
		/*
		* @brief 设置窗口布局
		* @param[in] layout 布局
		* @return void
		*/
		void setWindowLayout(int layout);

		/*
		* @brief 设置窗口样式
		* @param[in] style 样式
		* @return void
		*/
		void setWindowStyle(int style);

		/*
		* @brief 获取Qss颜色
		* @param[out] qssContent qss内容
		* @param[out] textColor 文本颜色
		* @param[out] panelColor 面板颜色
		* @param[out] borderColor 边框颜色
		* @param[out] normalColorStart 正常颜色起始
		* @param[out] normalColorEnd 正常颜色结束
		* @param[out] darkColorStart 深色颜色起始
		* @param[out] darkColorEnd 深色颜色结束
		* @param[out] highColor 高亮颜色
		* @return void
		*/
		void qssColor(const QString& qssContent, QString& textColor,
			QString& panelColor, QString& borderColor,
			QString& normalColorStart, QString& normalColorEnd,
			QString& darkColorStart, QString& darkColorEnd,
			QString& highColor) const;

		/*
		* @brief 设置装饰物图标
		* @param[in] button 按钮
		* @param[in] str unicode图标代码
		* @param[in] size 大小
		* @return void
		*/
		void setButtonIcon(Widget::Button button, QChar str, quint32 size = 9);

		/*
		* @brief 设置装饰物像素图
		* @param[in] button 按钮
		* @param[in] file 文件路径
		* @param[in] size 大小
		* @return void
		*/
		void setButtonPixmap(Widget::Button button, const QString& file, const QSize& size = QSize(16, 16));

		/*
		* @brief 设置装饰物可见
		* @param[in] button 按钮
		* @param[in] visible 是否可见
		* @return void
		*/
		void setButtonVisible(Widget::Button button, bool visible = true);

		/*
		* @brief 设置装饰物启用
		* @param[in] button 按钮
		* @param[in] enabled 是否启用
		* @return void
		*/
		void setButtonEnabled(Widget::Button button, bool enabled = true);

		/*
		* @brief 设置装饰物禁用
		* @param[in] button 按钮
		* @param[in] disable 是否禁用
		* @return void
		*/
		void setButtonDisabled(Widget::Button button, bool disabled = true);

		/*
		* @brief 设置主装饰物
		* @tparam[in] mainWidget 主装饰物
		* @return void
		*/
		template<class T> inline void setMainWidget(T mainWidget)
		{
			if (mainWidget_ == nullptr) {
				hashCode_ = typeid(T).hash_code();
				if (global::withMainWindowSameSizeSet.contains(hashCode_)) {
					for (const auto& x : global::mainWidgets) {
						if (x->windowLayout() & QUI_QUIT_ON_CLOSE) {
							mainWidget->resize(x->mainWidget_->size());
							break;
						}
					}
				}
				widget_->layout()->addWidget(mainWidget);
				mainWidget_ = mainWidget;
				resize(mainWidget_->width(), mainWidget_->height() + labelTitle_->height());
				mainWidget_->setAttribute(Qt::WA_DeleteOnClose, true);
				setWindowModality(mainWidget_->windowModality());
				auto flags = mainWidget_->windowFlags();
				flags |= Qt::FramelessWindowHint;
				setWindowFlags(flags);
				auto dialog = dynamic_cast<QDialog*>(mainWidget_);
				connect(mainWidget_, &QObject::destroyed, this, std::bind(&Widget::destoryedClose, this, std::placeholders::_1, dialog));
				adjustMenuLayout();
				if (!mainWidget_->pos().isNull()) {
					move(mainWidget_->pos());
				}
			}
		}

		/*
		* @brief 主装饰物
		* @return T
		*/
		template<class T> inline T mainWidget() const
		{
			return static_cast<T>(mainWidget_);
		}

		/*
		* @brief 显示
		* @return void
		*/
		void show();

		/*
		* @brief 设置固定大小
		* @param[in] size 大小
		* @return void
		*/
		void setFixedSize(const QSize& size);

		/*
		* @brief 设置固定大小
		* @param[in] width 宽度
		* @param[in] height 高度
		* @return void
		*/
		void setFixedSize(int width, int height);

		/*
		* @brief 设置固定宽度
		* @param[in] width 宽度
		* @return void
		*/
		void setFixedWidth(int width);

		/*
		* @brief 设置固定高度
		* @param[in] height
		* @return void
		*/
		void setFixedHeight(int height);

		/*
		* @brief 设置窗口标题
		* @param[in] title 标题
		* @return void
		*/
		void setWindowTitle(const QString& title);

	public slots:
		/*
		* @brief 进行模态对话框显示
		* @return int
		*/
		int exec() override;

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] object 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* object, QEvent* event) override;

		/*
		* @brief 原生事件
		* @param[in] eventType 事件类型
		* @param[in] message 消息
		* @param[in] result 结果
		* @return bool
		*/
		bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

		/*
		* @brief 调节菜单布局
		* @return void
		*/
		void adjustMenuLayout();

	private:
		QVBoxLayout* verticalLayout1_ = nullptr;
		QWidget* widgetMain_ = nullptr;
		QLabel* labelTitle_ = nullptr;
		QLabel* labelRedDot_ = nullptr;
		QWidget* titleLeft_ = nullptr;
		QWidget* titleRight_ = nullptr;
		QVBoxLayout* verticalLayout2_ = nullptr;
		QHBoxLayout* horizontalLayout4_ = nullptr;
		QHBoxLayout* horizontalLayout5_ = nullptr;
		QHBoxLayout* horizontalLayout_ = nullptr;
		CustomButton* btnIcon_ = nullptr;
		CustomButton* btnLogo_ = nullptr;
		CustomButton* btnUpdate_ = nullptr;
		QToolButton* btnMenu_ = nullptr;
		QPushButton* btnMenu_Min_ = nullptr;
		QPushButton* btnMenu_Max_ = nullptr;
		QPushButton* btnMenu_Close_ = nullptr;
		QWidget* widget_ = nullptr;
		QVBoxLayout* verticalLayout3_ = nullptr;

	private:
		int windowLayout_ = QUI_WIDGET_WINDOW_LAYOUT;
		int windowStyle_ = SilveryStyle;
		bool isMaximize_ = false;
		QRect windowLocation_;
		QWidget* mainWidget_ = nullptr;
		IconHelper* helper_ = nullptr;
		QPoint mousePoint_;
		bool mousePressed_ = false;
		size_t hashCode_ = 0;
		QTimer redDotTimer_;
		QTimer updateTimer_;
		bool updateWait_ = false;
		QTimer idleTimer_;
		bool isIdle_ = false;
		bool isResize_ = false;

	signals:
		/*
		* @brief 样式改变
		* @param[in] style 样式
		* @return void
		*/
		void styleChanged(int style);

		/*
		* @brief 透明度改变
		* @param[in] opacity 透明度
		* @return void
		*/
		void opacityChanged(double opacity);

		/*
		* @brief 有可用更新
		* @return void
		*/
		void updateAvailabel();

	private slots:
		/*
		* @brief 销毁关闭
		* @param[in] object 对象
		* @param[in] widget 装饰物
		* @return void
		*/
		void destoryedClose(QObject* object, void* widget);

		/*
		* @brief 改变样式
		* @return void
		*/
		void changeStyle();

		/*
		* @brief 图标按钮点击
		* @return void
		*/
		void on_btnIcon_clicked();

		/*
		* @brief logo按钮点击
		* @return void
		*/
		void on_btnLogo_clicked();

		/*
		* @brief 最小化按钮点击
		* @return void
		*/
		void on_btnMenu_Min_clicked();

		/*
		* @brief 最大化按钮点击
		* @return void
		*/
		void on_btnMenu_Max_clicked();

		/*
		* @brief 菜单按钮点击
		* @return void
		*/
		void on_btnMenu_Close_clicked();
	};

	/*
	* @brief 跨线程调用
	*/
	class QUI_DLL_EXPORT Invoke : public QObject {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] type 类型
		* @param[in] closeSec 多少秒之后关闭
		* @param[in] parent 父对象
		*/
		Invoke(const QString& title, const QString& text, int type, bool defaultBtn, int closeSec = 0, QObject* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] title 标题
		* @param[in] type 类型
		* @param[in] placeholderText 占位符文本
		* @param[in] passwordEcho 密码回显
		* @param[in] closeSec 多少秒之后关闭
		* @param[in] resize 重定义的大小
		* @param[in] parent 父对象
		*/
		Invoke(const QString& title, int type, const QString& placeholderText = QString(), bool passwordEcho = false, int closeSec = 0, const QSize& resize = QSize(), QObject* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] title 标题
		* @param[in] type 类型
		* @param[in] selectableItems 可选择的条目
		* @param[in] defaultValue 默认值
		* @param[in] closeSec 多少秒之后关闭
		* @param[in] resize 重定义的大小
		* @param[in] parent 父对象
		*/
		Invoke(const QString& title, int type, const QStringList& selectableItems, const QString& defaultValue = QString(), int closeSec = 0, const QSize& resize = QSize(), QObject* parent = nullptr);

		/*
		* @brief 析构
		*/
		~Invoke();

	signals:
		/*
		* @brief 运行
		* @param[out] result 结果
		* @return void
		*/
		void run(bool* result);

		/*
		* @brief 运行
		* @param[out] result 结果
		* @param[out] value 值
		* @return void
		*/
		void run(bool* result, QString* value);

	private slots:
		/*
		* @brief 显示
		* @param[out] result 结果
		* @return void
		*/
		void show(bool* result);

		/*
		* @brief 显示
		* @param[out] result 结果
		* @param[out] value 值
		* @return void
		*/
		void show(bool* result, QString* value);

	private:
		struct {
			QString title;
			QString text;
			int type = 0;
			bool defaultBtn = false;
			int closeSec = 0;
		} messageBox_;

		struct {
			QString title;
			int type = 0;
			QString placeholderText;
			QStringList selectableItems;
			bool passwordEcho = false;
			int closeSec = 0;
			QSize size;
		} inputBox_;
	};

	/*
	* @brief 消息框(支持跨线程)
	* @note 命名与Windows API中的宏定义MessageBox有冲突,因此改变为MsgBox
	*/
	class QUI_DLL_EXPORT MsgBox : public QDialog
	{
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit MsgBox(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~MsgBox();

		/*
		* @brief 设置主图标
		* @param[in] str unicode图标代码
		* @param[in] size 大小
		* @return void
		*/
		void setMainIcon(QChar str, quint32 size = 9);

		/*
		* @brief 设置消息
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] type 类型
		* @param[in] defaultBtn 默认按钮
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		void setMessage(const QString& title, const QString& text, int type, bool defaultBtn = true, int closeSec = 0);

	public:
		/*
		* @brief 关于消息框
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		static void about(const QString& title, const QString& text, int closeSec = 0);

		/*
		* @brief 信息消息框
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		static void information(const QString& title, const QString& text, int closeSec = 0);

		/*
		* @brief 警告消息框
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		static void warning(const QString& title, const QString& text, int closeSec = 0);

		/*
		* @brief 严重消息框
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		static void critical(const QString& title, const QString& text, int closeSec = 0);

		/*
		* @brief 询问消息框
		* @param[in] title 标题
		* @param[in] text 文本
		* @param[in] defaultBtn 默认按钮(true确定,false取消)
		* @param[in] closeSec 多少秒之后关闭
		* @retval true 点击了(确定)
		* @retval false 点击了(取消)
		*/
		static bool question(const QString& title, const QString& text, bool defaultBtn = true, int closeSec = 0);

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] object 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private slots:
		/*
		* @brief 菜单按钮关闭点击
		* @return void
		*/
		void on_btnMenu_Close_clicked();

		/*
		* @brief 确定按钮点击
		* @return void
		*/
		void on_btnOk_clicked();

		/*
		* @brief 取消按钮点击
		* @return void
		*/
		void on_btnCancel_clicked();

		/*
		* @brief 检测秒
		* @return void
		*/
		void checkSec();

	private:
		QVBoxLayout* verticalLayout1_ = nullptr;
		QWidget* widgetTitle_ = nullptr;
		QHBoxLayout* horizontalLayout3_ = nullptr;
		QLabel* labelIcon_ = nullptr;
		QLabel* labelTitle_ = nullptr;
		QWidget* widgetMenu_ = nullptr;
		QHBoxLayout* horizontalLayout4_ = nullptr;
		QPushButton* btnMenu_Close_ = nullptr;
		QWidget* widgetMain_ = nullptr;
		QVBoxLayout* verticalLayout2_ = nullptr;
		QFrame* frame_ = nullptr;
		QVBoxLayout* verticalLayout4_ = nullptr;
		QHBoxLayout* horizontalLayout1_ = nullptr;
		QLabel* labelIconMain_ = nullptr;
		QSpacerItem* horizontalSpacer1_ = nullptr;
		QLabel* labelInfo_ = nullptr;
		QHBoxLayout* horizontalLayout2_ = nullptr;
		QSpacerItem* horizontalSpacer2_ = nullptr;
		QPushButton* btnOk_ = nullptr;
		QPushButton* btnCancel_ = nullptr;

	private:
		bool defaultBtn_ = false;
		int closeSec_ = 0;
		int currentSec_ = 0;
		IconHelper* helper_ = nullptr;
		QPoint mousePoint_;
		bool mousePressed_ = false;
		int type_ = 0;
	};

	/*
	* @brief 输入框(支持跨线程)
	*/
	class QUI_DLL_EXPORT InputBox : public QDialog
	{
		Q_OBJECT
	public:
		//输入框类型
		enum Type {
			//正常类型
			NormalType,

			//选择类型
			SelectType,

			//循环类型
			LoopType
		};

		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit InputBox(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~InputBox();

		/*
		* @brief 获取值
		* @return QString
		*/
		QString value() const;

		/*
		* @brief 设置主图标
		* @param[in] str unicode图标代码
		* @param[in] size 大小
		* @return void
		*/
		void setMainIcon(QChar str, quint32 size = 9);

		/*
		* @brief 设置参数
		* @param[in] title 标题
		* @param[in] type 类型
		* @param[in] placeholderText 占位符文本
		* @param[in] passwordEcho 密码回显
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		void setParameter(const QString& title, int type, const QString& placeholderText = QString(), bool passwordEcho = false, int closeSec = 0);

		/*
		* @brief 设置参数
		* @param[in] title 标题
		* @param[in] type 类型
		* @param[in] selectableItems 可选择的条目
		* @param[in] defaultValue 默认值
		* @param[in] closeSec 多少秒之后关闭
		* @return void
		*/
		void setParameter(const QString& title, int type, const QStringList& selectableItems, const QString& defaultValue = QString(), int closeSec = 0);

	public:
		/*
		* @brief 正常的输入框
		* @param[in] title 标题
		* @param[out] value 输入的值
		* @param[in] placeholderText 占位文本
		* @param[in] passwordEcho 密码回显
		* @param[in] closeSec 多少秒之后关闭
		* @param[in] resize 重定义的大小
		* @retval true 已输入内容
		* @retval false 未输入内容
		*/
		static bool normal(const QString& title, QString& value, const QString& placeholderText = QString(), bool passwordEcho = false, int closeSec = 0, const QSize& resize = QSize(280, 150));

		/*
		* @brief 选择的输入框
		* @param[in] title 标题
		* @param[out] value 选择的值
		* @param[in] selectableItems 可选择的条目
		* @param[in] defaultValue 默认值
		* @param[in] closeSec 多少秒之后关闭
		* @param[in] resize 重定义的大小
		* @retval true 已选择内容
		* @retval false 未选择内容
		*/
		static bool select(const QString& title, QString& value, const QStringList& selectableItems, const QString& defalutValue = QString(), int closeSec = 0, const QSize& resize = QSize(280, 150));

		/*
		* @brief 循环的输入框
		* @param[in] title 标题
		* @param[in] value 直到用户输入的值与此变量相等才结束循环
		* @param[in] placeholderText 占位文本
		* @param[in] passwordEcho 密码回显
		* @param[in] resize 重定义的大小
		* @return void
		*/
		static void loop(const QString& title, const QString& value, const QString& placeholderText = QString(), bool passwordEcho = false, const QSize& resize = QSize(280, 150));

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] object 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private slots:
		/*
		* @brief 菜单按钮关闭点击
		* @return void
		*/
		void on_btnMenu_Close_clicked();

		/*
		* @brief 确定按钮点击
		* @return void
		*/
		void on_btnOk_clicked();

		/*
		* @brief 取消按钮点击
		* @return void
		*/
		void on_btnCancel_clicked();

		/*
		* @brief 检测秒
		* @return void
		*/
		void checkSec();

	private:
		QVBoxLayout* verticalLayout1_ = nullptr;
		QWidget* widgetTitle_ = nullptr;
		QHBoxLayout* horizontalLayout1_ = nullptr;
		QLabel* labelIcon_ = nullptr;
		QLabel* labelTitle_ = nullptr;
		QLabel* labelTime_ = nullptr;
		QWidget* widgetMenu_ = nullptr;
		QHBoxLayout* horizontalLayout2_ = nullptr;
		QPushButton* btnMenu_Close_ = nullptr;
		QWidget* widgetMain_ = nullptr;
		QVBoxLayout* verticalLayout2_ = nullptr;
		QFrame* frame_ = nullptr;
		QVBoxLayout* verticalLayout3_ = nullptr;
		QLabel* labelInfo_ = nullptr;
		QLineEdit* textValue_ = nullptr;
		QComboBox* boxValue_ = nullptr;
		QHBoxLayout* layout_ = nullptr;
		QSpacerItem* horizontalSpacer_ = nullptr;
		QPushButton* btnOk_ = nullptr;
		QPushButton* btnCancel_ = nullptr;

	private:
		int closeSec_ = 0;
		int currentSec_ = 0;
		QString value_;
		IconHelper* helper_ = nullptr;
		Type type_ = NormalType;
		QString placeholderText_;
		QStringList selectableItems_;
		QPoint mousePoint_;
		bool mousePressed_ = false;
	};

	/*
	* @brief 图标辅助
	*/
	class QUI_DLL_EXPORT IconHelper : public QObject
	{
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit IconHelper(QObject* parent = nullptr);

		/*
		* @brief 析构
		*/
		~IconHelper();

		/*
		* @brief 设置图标
		* @param[in] label 标签
		* @param[in] c unicde图标代码
		* @param[in] size 大小
		* @return void
		*/
		void setIcon(QLabel* label, QChar c, quint32 size = 9);

		/*
		* @brief 设置图标
		* @param[in] button 按钮
		* @param[in] c unicode图标代码
		* @param[in] size 大小
		* @return void
		*/
		void setIcon(QAbstractButton* button, QChar c, quint32 size = 9);

		/*
		* @brief 获取像素图
		* @param[in] color 颜色
		* @param[in] c unicode图标代码
		* @param[in] size 大小
		* @param[in] pixmapWidth 像素图宽度
		* @param[in] pixmapHeight 像素图高度
		* @return QPixmap
		*/
		QPixmap pixmap(const QString& color, QChar c, quint32 size = 9, quint32 pixmapWidth = 10, quint32 pixmapHeight = 10);

		/*
		* @brief 获取像素图
		* @param[in|out] button 按钮
		* @param[in] normal 是否正常颜色(区分正常颜色和深色)
		* @return QPixmap
		*/
		QPixmap pixmap(QToolButton* button, bool normal);

		/*
		* @brief 设置样式
		* @param[in] widget 装饰物
		* @param[in] type 类型
		* @param[in] borderWidth 边框宽度
		* @param[in] borderColor 边框颜色
		* @param[in] normalBgColor 正常背景颜色
		* @param[in] darkBgColor 深色背景颜色
		* @param[in] normalTextColor 正常文本颜色
		* @param[in] darkTextColor 深色文本颜色
		* @return void
		*/
		void setStyle(QWidget* widget,
			const QString& type = "left",
			int borderWidth = 3,
			const QString& borderColor = "#029FEA",
			const QString& normalBgColor = "#292F38",
			const QString& darkBgColor = "#1D2025",
			const QString& normalTextColor = "#54626F",
			const QString& darkTextColor = "#FDFDFD");

		/*
		* @brief 设置样式
		* @param[in] widget 装饰物
		* @param[in] buttons 按钮列表
		* @param[in] pixmapChar 像素图代码(unicode图标代码)
		* @param[in] iconSize 图标大小
		* @param[in] iconWidth 图标宽度
		* @param[in] iconHeight 图标高度
		* @param[in] type 类型
		* @param[in] borderWidth 边框宽度
		* @param[in] borderColor 边框颜色
		* @param[in] normalBgColor 正常背景颜色
		* @param[in] darkBgColor 深色背景颜色
		* @param[in] normalTextColor 正常文本颜色
		* @param[in] darkTextColor 深色文本颜色
		* @return void
		*/
		void setStyle(QWidget* widget,
			const QList<QToolButton*>& buttons,
			const QList<int>& pixmapChar,
			quint32 iconSize = 9,
			quint32 iconWidth = 10,
			quint32 iconHeight = 10,
			const QString& type = "left",
			int borderWidth = 3,
			const QString& borderColor = "#029FEA",
			const QString& normalBgColor = "#292F38",
			const QString& darkBgColor = "#1D2025",
			const QString& normalTextColor = "#54626F",
			const QString& darkTextColor = "#FDFDFD");

		/*
		* @brief 设置样式
		* @param[in] frame 边框装饰物
		* @param[in] buttons 按钮列表
		* @param[in] pixmapChar 像素图代码(unicode图标代码)
		* @param[in] iconSize 图标大小
		* @param[in] iconWidth 图标宽度
		* @param[in] iconHeight 图标高度
		* @param[in] normalBgColor 正常背景颜色
		* @param[in] darkBgColor 深色背景颜色
		* @param[in] normalTextColor 正常文本颜色
		* @param[in] darkTextColor 深色文本颜色
		* @return void
		*/
		void setStyle(QFrame* frame,
			const QList<QToolButton*>& buttons,
			const QList<int>& pixmapChar,
			quint32 iconSize = 9,
			quint32 iconWidth = 10,
			quint32 iconHeight = 10,
			const QString& normalBgColor = "#2FC5A2",
			const QString& darkBgColor = "#3EA7E9",
			const QString& normalTextColor = "#EEEEEE",
			const QString& darkTextColor = "#FFFFFF");

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] object 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* watched, QEvent* event) override;

	private:
		QFont iconFont_;
		QList<QToolButton*> buttons_;
		QList<QPixmap> pixmapNormal_;
		QList<QPixmap> pixmapDark_;
	};

	/*
	* @brief 配置
	*/
	class QUI_DLL_EXPORT Config
	{
	public:
		//标题栏左上角图标
		static QChar AppIcon;

		//下拉菜单图标
		static QChar MenuIcon;

		//最小化图标
		static QChar MinIcon;

		//最大化图标
		static QChar MaxIcon;

		//还原图标
		static QChar RestoreIcon;

		//关闭图标
		static QChar CloseIcon;

		//全局字体名称
		static QString FontName;

		//全局字体大小
		static int FontSize;

		//文字颜色
		static QString TextColor;

		//面板颜色
		static QString PanelColor;

		//边框颜色
		static QString BorderColor;

		//正常状态开始颜色
		static QString NormalColorStart;

		//正常状态结束颜色
		static QString NormalColorEnd;

		//加深状态开始颜色
		static QString DarkColorStart;

		//加深状态结束颜色
		static QString DarkColorEnd;

		//高亮颜色
		static QString HighColor;
	};

	template<class T>
	Widget::Widget(T mainWidget, int windowLayout, QWidget* parent)
		:helper_(new IconHelper), QDialog(parent)
	{
		initControl();
		initForm();
		setWindowLayout(windowLayout);
		setMainWidget(mainWidget);
		setWindowTitle(mainWidget->windowTitle());
	}

	template<class T>
	Widget::Widget(T mainWidget, const QString& titleText, int windowLayout, QWidget* parent)
		:helper_(new IconHelper), QDialog(parent)
	{
		initControl();
		initForm();
		setWindowLayout(windowLayout);
		setMainWidget(mainWidget);
		setWindowTitle(titleText);
	}

	/*
	* @brief 自定义按钮
	* @note 仅供Widget库内部调用,外部不要使用.
	*/
	class QUI_DLL_EXPORT CustomButton : public QPushButton {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit CustomButton(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~CustomButton();
	};

	/*
	* @brief 关于对话框
	*/
	class QUI_DLL_EXPORT AboutDialog : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit AboutDialog(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~AboutDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @return int
		*/
		static int doModal();

		/*
		* @brief 做非模态
		* @return void
		*/
		static void doModeless();

	public Q_SLOTS:
		/*
		* @brief 进行模态对话框显示
		* @return int
		*/
		int exec() override;

	private slots:
		/*
		* @brief 按钮点击
		* @return void
		*/
		void on_pushButton_clicked();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private:
		Widget* widget_ = nullptr;
		QVBoxLayout* verticalLayout_ = nullptr;
		QGridLayout* gridLayout_ = nullptr;
		QSpacerItem* verticalSpacer_2_ = nullptr;
		QLabel* label_2_ = nullptr;
		QSpacerItem* horizontalSpacer_2_ = nullptr;
		QLabel* label_4_ = nullptr;
		QSpacerItem* horizontalSpacer_ = nullptr;
		QLabel* label_3_ = nullptr;
		QSpacerItem* verticalSpacer_ = nullptr;
		QPushButton* pushButton_ = nullptr;
		QLabel* label_ = nullptr;
	};

	/*
	* @brief 自定义滑块
	* @note 仅供Widget库内部调用,外部不要使用.
	*/
	class QUI_DLL_EXPORT CustomSlider : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		CustomSlider(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~CustomSlider();

		/*
		* @brief 设置值
		* @param[in] value 值
		* @return void
		*/
		void setValue(double value);

		/*
		* @brief 获取值
		* @return double
		*/
		double value() const;

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public Q_SLOTS:
		/*
		* @brief 进行模态对话框显示
		* @return int
		*/
		int exec() override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	signals:
		/*
		* @brief 更新透明度
		* @param[in] opacity 透明度
		* @return void
		*/
		void updateOpacity(double opacity);

	private slots:
		/*
		* @brief 水平滑块值改变
		* @param[in] value 值
		* @return void
		*/
		void on_horizontalSlider_valueChanged(int value);

	private:
		Widget* widget_ = nullptr;
		QHBoxLayout* horizontalLayout_ = nullptr;
		QLabel* label_ = nullptr;
		QSlider* horizontalSlider_ = nullptr;
		QLabel* label_2_ = nullptr;
	};

	/*
	* @brief 反馈对话框
	*/
	class QUI_DLL_EXPORT FeedbackDialog : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		* @note 需要配合mes服务器或invo_mes_plugin.dll使用
		*/
		FeedbackDialog(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~FeedbackDialog();

		/*
		* @brief 显示(非模态)
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @return int
		*/
		static int doModal();

		/*
		* @brief 做非模态
		* @return void
		*/
		static void doModeless();

	protected:
		/*
		* @brief 绘图事件
		* @param[in] event 事件
		* @return void
		*/
		void paintEvent(QPaintEvent* event) override;

	public Q_SLOTS:
		/*
		* @brief 模态显示
		* @retunr int
		*/
		int exec() override;

	private slots:
		/*
		* @brief 功能异常点击
		* @param[in] clicked 是否点击
		* @return void
		*/
		void on_dysfunction_clicked(bool clicked);

		/*
		* @brief 意见点击
		* @param[in] clicked 是否点击
		* @return void
		*/
		void on_suggestion_clicked(bool clicked);

		/*
		* @brief 其他点击
		* @param[in] clicked 是否点击
		* @return void
		*/
		void on_other_clicked(bool clicked);

		/*
		* @brief 提交按钮点击
		* @return void
		*/
		void on_submitButton_clicked();

		/*
		* @brief 询问文本改变
		* @return void
		*/
		void on_question_textChanged();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private:
		Widget* widget_ = nullptr;
		QVBoxLayout* verticalLayout_2_ = nullptr;
		QGroupBox* groupBox_ = nullptr;
		QHBoxLayout* horizontalLayout_ = nullptr;
		QCheckBox* dysfunction_ = nullptr;
		QCheckBox* suggestion_ = nullptr;
		QCheckBox* other_ = nullptr;
		QGroupBox* groupBox_2_ = nullptr;
		QVBoxLayout* verticalLayout_ = nullptr;
		QTextEdit* question_ = nullptr;
		QHBoxLayout* horizontalLayout_3_ = nullptr;
		QSpacerItem* horizontalSpacer_ = nullptr;
		QPushButton* submitButton_ = nullptr;
		QSpacerItem* horizontalSpacer_2_ = nullptr;
		QLabel fontCount_;
	};

	/*
	* @brief 进度对话框
	* @code
	* auto task = [](int& progress, const bool& terminate){
	*	int count = 100;
	*	for (int i = 0; i < count; ++i) {
	*		if (terminate) { //terminate为true,代表用户关闭加载对话框.
	*			return -1;
	*		}
	*		progress = (static_cast<float>(i) / count) * 100;
	*		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	*	}
	*	return 0;
	* }
	* ProgressDialog::doModal(task);
	* QUIprogressDialog::doModeless(task, [](int exit_code) { std::cout << "exit_code:" << exit_code << std::endl; });
	* @endcode
	*/
	class QUI_DLL_EXPORT ProgressDialog : public QDialog
	{
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] task 任务
		* @param[in] parent 父对象
		*/
		ProgressDialog(::std::function<int(int&, const bool&)>&& task, QWidget* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] task 任务
		* @param[in] callback 回调函数
		* @param[in] parent 父对象
		*/
		ProgressDialog(::std::function<int(int&, const bool&)>&& task, ::std::function<void(int)>&& callback, QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~ProgressDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @param[in] task 任务
		* @return int
		*/
		static int doModal(::std::function<int(int&, const bool&)>&& task);

		/*
		* @brief 做非模态
		* @param[in] task 任务
		* @param[in] callback 回调
		* @return void
		*/
		static void doModeless(::std::function<int(int&, const bool&)>&& task, ::std::function<void(int)>&& callback);

	public slots:
		/*
		* @brief 显示(模态)
		* @return int
		*/
		int exec() override;

	protected:
		/*
		* @brief 关闭事件
		* @param[in] event 事件
		* @return void
		*/
		void closeEvent(QCloseEvent* event) override;

	signals:
		/*
		* @brief 关闭窗口
		* @return void
		*/
		void closeWindow();

	private slots:
		/*
		* @brief 关闭窗口
		* @return void
		*/
		void close_window();

		/*
		* @brief 计时器超时
		* @return void
		*/
		void timer_timeout();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

		/*
		* @brief 异步任务
		* @return void
		*/
		void asyncTask();

	private:
		Widget* widget_ = nullptr;
		bool taskStarted_ = false;
		bool taskCompleted_ = false;
		bool termiante_ = false;
		int exitCode_ = 0;
		int progress_ = 0;
		QTimer timer_;
		::std::future<void> future_;
		::std::function<int(int&, const bool&)> task_ = nullptr;
		::std::function<void(int)> callback_ = nullptr;

	private:
		QVBoxLayout* verticalLayout_ = nullptr;
		QHBoxLayout* horizontalLayout_ = nullptr;
		QProgressBar* progressBar_ = nullptr;
	};

	/*
	* @brief 加载对话框
	* @code
	* auto task = []() {
	*	int count = 100;
	*	for (int i = 0; i < count; ++i) {
	*		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	*	}
	*	return 100;
	* }
	* auto exit_code = LoadingDialog::doModal(task);
	* qDebug() << "exit_code:" << exit_code; // exit_code is 100
	* @endcode
	*/

	class QUI_DLL_EXPORT LoadingDialog : public QDialog
	{
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] task 任务
		* @param[in] parent 父对象
		*/
		LoadingDialog(::std::function<int()>&& task, QWidget* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] task 任务
		* @param[in] callback 回调函数
		* @param[in] parent 父对象
		*/
		LoadingDialog(::std::function<int()>&& task, ::std::function<void(int)>&& callback, QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~LoadingDialog();

		/*
		* @brief 设置点数量
		* @param[in] count 数量
		* @return void
		*/
		void setDotCount(int count);

		/*
		* @brief 设置点颜色
		* @param[in] color 颜色
		* @return void
		*/
		void setDotColor(const QColor& color);

		/*
		* @brief 设置最大直径
		* @param[in] diameter 直径
		* @return void
		*/
		void setMaxDiameter(float diameter);

		/*
		* @brief 设置最小直径
		* @param[in] diameter 直径
		* @return void
		*/
		void setMinDiameter(float diameter);

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @param[in] task 任务
		* @return int
		*/
		static int doModal(::std::function<int()>&& task);

		/*
		* @brief 做非模态
		* @param[in] task 任务
		* @param[in] callback 回调函数
		* @return void
		*/
		static void doModeless(::std::function<int()>&& task, ::std::function<void(int)>&& callback = nullptr);

	public slots:
		/*
		* @brief 模态显示
		* @return int
		*/
		int exec() override;

	protected:
		/*
		* @brief 绘图事件
		* @param[in] event 事件
		* @return void
		*/
		void paintEvent(QPaintEvent* event) override;

		/*
		* @brief 调整大小事件
		* @param[in] event 事件
		* @return void
		*/
		void resizeEvent(QResizeEvent* event) override;

	private:
		/*
		* @brief 异步任务
		* @return void
		*/
		void asyncTask();

		/*
		* @brief 绘制点
		* @param[in] painter 绘画家
		* @return void
		*/
		void paintDot(QPainter& painter);

		/*
		* @brief 计算
		* @return void
		*/
		void caculate();

	signals:
		/*
		* @brief 关闭窗口
		* @return void
		*/
		void closeWindow();

	private slots:
		/*
		* @brief 关闭窗口
		* @return void
		*/
		void close_window();

		/*
		* @brief 刷新
		* @return void
		*/
		void refresh();

	private:
		struct Location {
		public:
			Location(float _x, float _y) { x = _x; y = _y; }
			float x;
			float y;
		};
		int index_ = 0;
		QColor dotColor_;
		int dotCount_ = 0;
		float minDiameter_ = 0.0f;
		float maxDiameter_ = 0.0f;
		int i_ = 0;
		int interval_ = 0;
		QTimer timer_;
		float squareWidth_ = 0.0f;
		float centerDistance_ = 0.0f;
		QList<float> radiiList_;
		QList<Location> locationList_;
		::std::future<void> future_;
		::std::function<int()> task_ = nullptr;
		::std::function<void(int)> callback_ = nullptr;
		bool taskStarted_ = false;
	};

	/*
	* @brief 文本查看器对话框
	*/
	class QUI_DLL_EXPORT TextViewerDialog : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] windowTitle 窗口标题
		* @param[in] filePath 文件路径
		* @param[in] fontSize 字体大小
		* @param[in] parent 父对象
		* @note 文件编码会自动识别,无需关注
		*/
		TextViewerDialog(const QString& windowTitle, const QString& filePath, int fontSize = 12, QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~TextViewerDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

		/*
		* @brief 纯文本编辑
		*/
		QPlainTextEdit* plainTextEdit = nullptr;

	public:
		/*
		* @brief 做模态
		* @param[in] windowTitle 标题
		* @param[in] filePath 文件路径
		* @param[in] fontSize 字体大小
		* @return int
		*/
		static int doModal(const QString& windowTitle, const QString& filePath, int fontSize = 12);

		/*
		* @brief 做非模态
		* @param[in] windowTitle 标题
		* @param[in] filePath 文件路径
		* @param[in] fontSize 字体大小
		* @return int
		*/
		static void doModeless(const QString& windowTitle, const QString& filePath, int fontSize = 12);

	public slots:
		/*
		* @brief 显示
		* @return void
		*/
		int exec() override;

	protected:
		/*
		* @brief 加载数据
		* @return void
		*/
		void onLoadingData();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private:
		Widget* widget_ = nullptr;
		QVBoxLayout* verticalLayout_ = nullptr;
		QLabel* label_ = nullptr;
		QHBoxLayout* horizontalLayout_ = nullptr;
		QSpacerItem* horizontalSpacer_ = nullptr;
		QString filePath_;
		::std::future<void> future_;
		bool quit_ = false;

	signals:
		void appendText(const QString& text);
		void loadingError(const QString& error);
		void loadingFinished();

	private slots:
		void onAppendText(const QString& text);
		void onLoadingError(const QString& error);
		void onLoadingFinished();
	};

	/*
	* @brief 设置服务端主机名
	* @param[in] host 主机名
	* @return void
	* @note 默认www.invomes.com
	*/
	QUI_DLL_EXPORT void setServerHostName(const QString& hostName);

	/*
	* @brief 获取服务端主机名
	* @return 服务端主机名
	*/
	QUI_DLL_EXPORT QString serverHostName();

	/*
	* @brief 设置服务端端口
	* @param[in] port 端口
	* @return void
	* @note 默认60682
	*/
	QUI_DLL_EXPORT void setServerPort(int port);

	/*
	* @brief 获取服务端端口
	* @return 服务端端口
	*/
	QUI_DLL_EXPORT int serverPort();

	/*
	* @brief 设置连接服务器超时时间
	* @param[in] timeout 超时时间(单位:毫秒)
	* @return void
	*/
	QUI_DLL_EXPORT void setConnectServerTimeout(int timeout);

	/*
	* @brief 获取连接服务器超时时间(默认:5000ms)
	* @return 超时时间
	*/
	QUI_DLL_EXPORT int connectServerTimeout();

	/*
	* @brief 注册更新事件
	* @param[in] func 函数
	* @param[in] period 周期(min)
	* @return void
	* @note 仅会注册生效一次,多次注册无效
	*/
	QUI_DLL_EXPORT void registerUpdateEvent(std::function<void()>&& func, int period = 5);

	/*
	* @brief 取消注册更新事件
	* @return void
	*/
	QUI_DLL_EXPORT void unregisterUpdateEvent();

	/*
	* @brief 写日志到数据库(异步接口)
	* @param[in] log 日志
	* @param[in] user 用户
	* @param[in] cb 回调函数,用于判断是否写入成功,参数:(错误代码:0成功非0失败,错误描述)
	* @param[in] async 是否异步执行
	* @param[in] address 上传的服务器地址,如果为空则以qui::serverHostName返回的地址为准
	* @param[in] port 上传的服务器端口,如果为0则以qui::serverPort返回的端口为准
	* @param[in] timeout 连接服务器超时时间(单位:毫秒),如果为0则以qui::connectServerTimeout返回的超时时间为准
	* @return void
	*/
	QUI_DLL_EXPORT void writeLogToDb(const QString& log, const QString& user = "anonymous",
		::std::function<void(int, const QString&)>&& cb = nullptr, bool async = true,
		const QString& address = QString(), int port = 0, int timeout = 0);

	/*
	* @brief 更新对话框
	*/
	class QUI_DLL_EXPORT UpdateDialog : public QDialog
	{
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		UpdateDialog(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~UpdateDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @return int
		*/
		static int doModal();

		/*
		* @brief 做非模态
		* @return void
		*/
		static void doModeless();

	public slots:
		/*
		* @brief 显示
		* @return int
		*/
		int exec() override;

	protected:
		/*
		* @brief 关闭事件
		* @param[in] event 事件
		* @return void
		*/
		void closeEvent(QCloseEvent* event) override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	signals:
		/*
		* @brief 更新最新版本
		* @param[in] version 版本
		* @return void
		*/
		void updateLatestVersion(const QString& version);

		/*
		* @brief 更新最新内容
		* @param[in] content 内容
		* @return void
		*/
		void updateLatestContent(const QString& content);

		/*
		* @brief 更新检测状态
		* @param[in] status 状态
		* @return void
		*/
		void updateCheckStatus(const QString& status);

		/*
		* @brief 更新进度状态
		* @param[in] status 状态
		* @return void
		*/
		void updateProgressStatus(const QString& status);

		/*
		* @brief 更新界面
		* @param[in] index 索引
		* @return void
		*/
		void updateInterface(int index);

		/*
		* @brief 更新进度条
		* @param[in] value 值
		* @return void
		*/
		void updateProgressBar(int value);

		/*
		* @brief 关闭所有窗口
		* @return void
		*/
		void closeAllWindow();

	private slots:
		/*
		* @brief 按钮点击
		* @return void
		*/
		void pushButton_clicked();

		/*
		* @brief 更新最新版本
		* @param[in] verison 版本
		* @return void
		*/
		void update_latest_version(const QString& version);

		/*
		* @brief 更新最新内容
		* @param[in] content 内容
		* @return void
		*/
		void update_latest_content(const QString& content);

		/*
		* @brief 更新检测状态
		* @param[in] status 状态
		* @return void
		*/
		void update_check_status(const QString& status);

		/*
		* @brief 更新进度状态
		* @param[in] status 状态
		* @return void
		*/
		void update_progress_status(const QString& status);

		/*
		* @brief 更新界面
		* @param[in] index 索引
		* @return void
		*/
		void update_interface(int index);

		/*
		* @brief 更新进度条
		* @param[in] value 值
		* @return void
		*/
		void update_progress_bar(int value);

		/*
		* @brief 关闭所有窗口
		* @return void
		*/
		void close_all_window();

	private:
		Widget* widget;
		struct Boolean {
			bool need_to_update_app = false;
			bool app_exit = false;
			bool update_thread_is_exit = false;
			bool is_closed = false;
		} boolean;

		QVBoxLayout* mainLayout;
		QWidget* appIconWidget;
		QVBoxLayout* verticalLayout_0;
		QSpacerItem* verticalSpacer_0;
		QHBoxLayout* horizontalLayout;
		QSpacerItem* horizontalSpacer;
		QLabel* appIcon;
		QSpacerItem* horizontalSpacer_2;
		QSpacerItem* verticalSpacer_2;

		QStackedWidget* stackedWidget;
		QWidget* checkUpdateWidget;
		QVBoxLayout* verticalLayout_2;
		QSpacerItem* verticalSpacer_4;
		QHBoxLayout* horizontalLayout_7;
		QSpacerItem* horizontalSpacer_13;
		QLabel* appName_0;
		QSpacerItem* horizontalSpacer_14;
		QHBoxLayout* horizontalLayout_8;
		QSpacerItem* horizontalSpacer_15;
		QLabel* appVer_0;
		QSpacerItem* horizontalSpacer_16;
		QHBoxLayout* horizontalLayout_9;
		QSpacerItem* horizontalSpacer_17;
		QLabel* avaiableVer_0;
		QSpacerItem* horizontalSpacer_18;
		QSpacerItem* verticalSpacer_5;

		QWidget* variableUpdateWidget;
		QVBoxLayout* verticalLayout;
		QSpacerItem* verticalSpacer;
		QHBoxLayout* horizontalLayout_2;
		QSpacerItem* horizontalSpacer_3;
		QLabel* appName_1;
		QSpacerItem* horizontalSpacer_4;
		QHBoxLayout* horizontalLayout_3;
		QSpacerItem* horizontalSpacer_5;
		QLabel* appVer_1;
		QSpacerItem* horizontalSpacer_6;
		QHBoxLayout* horizontalLayout_4;
		QSpacerItem* horizontalSpacer_7;
		QLabel* avaiableVer_1;
		QSpacerItem* horizontalSpacer_8;
		QHBoxLayout* horizontalLayout_6;
		QSpacerItem* horizontalSpacer_9;
		QPlainTextEdit* plainTextEdit;
		QSpacerItem* horizontalSpacer_10;
		QHBoxLayout* horizontalLayout_5;
		QSpacerItem* horizontalSpacer_11;
		QPushButton* pushButton;
		QSpacerItem* horizontalSpacer_12;
		QSpacerItem* verticalSpacer_3;

		QWidget* updateProgressWidget;
		QVBoxLayout* verticalLayout_3;
		QSpacerItem* verticalSpacer_6;
		QHBoxLayout* horizontalLayout_11;
		QSpacerItem* horizontalSpacer_19;
		QProgressBar* progressBar;
		QSpacerItem* horizontalSpacer_20;
		QHBoxLayout* horizontalLayout_10;
		QSpacerItem* horizontalSpacer_21;
		QLabel* progressStatus;
		QSpacerItem* horizontalSpacer_22;
		QSpacerItem* verticalSpacer_7;
	};

	/*
	* @brief 设置对话框
	*/
	class QUI_DLL_EXPORT SettingDialog : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] ini ini类
		* @param[in] callback ini数据改变回调
		* @param[in] iconPaths 图标路径
		* @param[in] hiddenGroups 需要隐藏的节,比如有config/name,menu/button,但是不想显示menu,那么就可以写menu,不显示它
		* @param[in] canBeModifiedPaths 可以被修改的路径(需要为全路径如app/properties/ver-config),如果设置了右击ver-config即可
		* 增加新节点,然后点击它的子节点可以被删除
		* @param[in] saveState 保存展开&折叠状态
		* @param[in] parent 父对象
		*/
		SettingDialog(qui::Ini* ini, qui::IniDataChangedCb&& callback = nullptr,
			const IniIconPaths& iconPaths = global::defaultIniIconPaths,
			const QStringList& hiddenGroups = QStringList(),
			const QStringList& canBeModifiedPaths = QStringList(),
			bool saveState = false,
			QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~SettingDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @param[in] ini ini类
		* @param[in] callback ini数据改变回调
		* @param[in] iconPaths 图标路径
		* @param[in] hiddenGroups 需要隐藏的节,比如有config/name,menu/button,但是不想显示menu,那么就可以写menu,不显示它
		* @param[in] canBeModifiedPaths 可以被修改的路径(需要为全路径如app/properties/ver-config),如果设置了右击ver-config即可
		* 增加新节点,然后点击它的子节点可以被删除
		* @param[in] saveState 保存展开&折叠状态
		* @return int
		*/
		static int doModal(qui::Ini* ini, qui::IniDataChangedCb&& callback = nullptr,
			const IniIconPaths& iconPaths = global::defaultIniIconPaths,
			const QStringList& hiddenGroups = QStringList(),
			const QStringList& canBeModifiedPaths = QStringList(),
			bool saveState = false);

		/*
		* @brief 做非模态
		* @param[in] ini ini类
		* @param[in] callback ini数据改变回调
		* @param[in] iconPaths 图标路径
		* @param[in] hiddenGroups 需要隐藏的节,比如有config/name,menu/button,但是不想显示menu,那么就可以写menu,不显示它
		* @param[in] canBeModifiedPaths 可以被修改的路径(需要为全路径如app/properties/ver-config),如果设置了右击ver-config即可
		* 增加新节点,然后点击它的子节点可以被删除
		* @param[in] saveState 保存展开&折叠状态
		* @return void
		*/
		static void doModeless(qui::Ini* ini, qui::IniDataChangedCb&& callback = nullptr,
			const IniIconPaths& iconPaths = global::defaultIniIconPaths,
			const QStringList& hiddenGroups = QStringList(),
			const QStringList& canBeModifiedPaths = QStringList(),
			bool saveState = false);

	public slots:
		/*
		* @brief 显示
		* @return int
		*/
		int exec();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private:
		Widget* widget = nullptr;
		QVBoxLayout* verticalLayout = nullptr;
		qui::TreeWidget* treeWidget = nullptr;
		qui::Ini* ini = nullptr;
		qui::IniDataChangedCb iniCallback = nullptr;
		qui::IniIconPaths iconPaths;
		QStringList hiddenGroups;
		QStringList canBeModifiedPaths;
		bool saveState = false;
	};

	/*
	* @brief 树控件,对QTreeWidget的二次封装,使用起来更方便更快捷
	* @usage 在Qt Designer中新建一个QTreeWidget将其提升为TreeWidget
	*/
	class QUI_DLL_EXPORT TreeWidget : public QTreeWidget
	{
		Q_OBJECT
	public:
		/*
		* @brief 布尔锁
		*/
		class BoolLock {
		public:
			BoolLock() :m_value(nullptr) {}

			BoolLock(bool& value) :m_value(&value) { *m_value = true; }

			~BoolLock() { if (m_value) *m_value = false; };

			void setVariable(bool& value) { m_value = &value; *m_value = true; };
		private:
			bool* m_value;
		};

		/*
		* @brief 计数锁
		*/
		class CountLock {
		public:
			CountLock() {}

			CountLock(int& count) { setCount(count); }

			~CountLock() { if (m_count) { m_mutex.lock(); --(*m_count); m_mutex.unlock(); } }

			void setCount(int& count) { m_count = &count; m_mutex.lock(); ++(*m_count); m_mutex.unlock(); }
		private:
			int* m_count = nullptr;

			::std::mutex m_mutex;
		};

		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		TreeWidget(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~TreeWidget();

		/*
		* @brief 初始化
		* @param[in] labels 标签
		* @param[in] saveState 保存展开&折叠状态
		* @return void
		*/
		void initialize(const QStringList& labels = { "键", "值", "描述" }, bool saveState = false);

		/*
		* @brief 初始化
		* @param[in] ini 初始化配置文件
		* @param[in] callback 初始化配置文件数据改变回调,用于判断输入的值是否符合规范
		* @param[in] iconPaths 图标路径
		* @param[in] hiddenGroups 需要隐藏的分组,比如有config/name,menu/button,但是不想显示menu,那么就可以写menu,不显示它
		* @param[in] canBeModifiedPaths 可以被修改的路径(需要为全路径如app/properties/ver-config),如果设置了右击ver-config即可
		* 增加新节点,然后点击它的子节点可以被删除
		* @param[in] saveState 保存展开&折叠状态
		* @return void
		* @note 修改ini数据是线程安全的
		*/
		void initialize(Ini* ini, IniDataChangedCb&& callback = nullptr,
			const IniIconPaths& iconPaths = IniIconPaths(),
			const QStringList& hiddenGroups = QStringList(),
			const QStringList& canBeModifiedPaths = QStringList(),
			bool saveState = false
		);

		/*
		* @brief 设置QRC统一资源定位器
		* @param[in] url 统一资源定位器
		* @return void
		*/
		void setQrcUrl(const QString& url);

		/*
		* @brief 获取QRC统一资源定位器
		* @return 统一资源定位器
		*/
		QString getQrcUrl() const;

		/*
		* @brief 保存项目状态
		* @return void
		*/
		void saveItemState();

		/*
		* @brief 还原项目状态
		* @return void
		*/
		void restoreItemState();

		/*
		* @brief 添加N级项目
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* ui.treeWidget->addLevelItem<0>("hello world");//此处hello world为键
		* ui.treeWidget->addLevelItem<0>(0, "world hello", "1", "2");//将world hello添加到索引为0的顶层中,在hello world的前面
		* ui.treeWidget->addLevelItem<0>("hello world", "2");//hello world下添加子节点2
		* ui.treeWidget->addLevelItem<0>("hello world", 0, "1");将1添加到父键hello world索引为0中,将会在2的前面
		* ui.treeWidget->addLevelItem<1>("hello world", "language", "cpp", "C++编程语言");//此处hello world为父键,language为子键,cpp为language键的值,c++编程语言为说明
		* //ui.treeWidget->addLevelItem<1>("hello world", "language"); //也可以
		* ui.treeWidget->addLevelItem<2>("hello world", "language", "cpp");//在language下添加一个cpp子节点
		* ui.treeWidget->addLevelItem<3>("hello world", "language", "cpp", "c++", QString(), "c加加");//...可以添加N个节点
		* @endcode
		* @note 以上代码执行如下图所示
		* _______________________________
		* |键           |值 |说明			|
		* |-----------------------------|
		* |world hello	|1	|2			|
		* |-----------------------------|
		* |hello world	|	|			|
		* |-----------------------------|
		* |  1			|	|			|
		* |-----------------------------|
		* |	 2			|	|			|
		* |-----------------------------|
		* |	 language	|cpp|C++编程语言	|
		* |-----------------------------|
		* |	   cpp		|	|			|
		* |-----------------------------|
		* |      c++	|	|C加加		|
		* -------------------------------
		*/
		template<int Level, class... Args> inline void addLevelItem(Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) >= Level + 1, "参数太少");
			Q_STATIC_ASSERT_X(sizeof...(args) <= Level + 4, "参数太多");
			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			if (Level == 0)
			{
				if (datas[Level].type() == QVariant::Type::Int)
				{
					//addListItem<0>(1, "hello", "world", "你好世界");
					Q_ASSERT_X(sizeof...(args) >= Level + 2 && sizeof...(args) <= Level + 3, __FUNCTION__, "参数不正确");
					int index = datas.front().toInt();
					datas.pop_front();
					auto&& strings = variants2Strings(datas);
					__super::insertTopLevelItem(index, new QTreeWidgetItem(strings));
				}
				else
				{
					Q_ASSERT_X(sizeof...(args) <= Level + 3, __FUNCTION__, "参数太多");
					auto&& strings = variants2Strings(datas);
					__super::addTopLevelItem(new QTreeWidgetItem(strings));
				}
			}
			else
			{
				//addListItem<0>("hello");
				//addListItem<0>(0, "hello");
				//addListItem<1>("hello", 0, "hi", "world", "hello");
				//addListItem<1>("hello", "hi", "world", "hello");
				bool searchIndex = (datas[Level].type() == QVariant::Type::Int);
				int newLevel = Level + (searchIndex ? 4 : 3);

				//补全
				while (datas.size() < newLevel)
				{
					datas.append(QString());
				}

				//提取说明
				QString explain = datas.back().toString();
				//删除说明
				datas.pop_back();

				//提取值
				QString value = datas.back().toString();
				//删除值
				datas.pop_back();

				//提取键
				QString key = datas.back().toString();
				//删除键
				datas.pop_back();

				int addIndex = 0;
				if (searchIndex)
				{
					addIndex = datas.back().toInt();
					//删除添加索引
					datas.pop_back();
				}

				Q_ASSERT_X(datas.size() == Level, __FUNCTION__, "参数太多");

				auto&& strings = variants2Strings(datas);
				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item) && item)
						{
							if (searchIndex)
							{
								item->insertChild(addIndex, new QTreeWidgetItem({ key, value, explain }));
							}
							else
							{
								item->addChild(new QTreeWidgetItem({ key, value, explain }));
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 添加多个项目
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* ui.treeWidget->addLevelItems<0>(QStringList{ "one", "two" }); //ok
		* ui.treeWidget->addLevelItems<0>(0, QStringList{ "three", "four" }, QStringList{ "三", "四" }); //ok
		* ui.treeWidget->addLevelItems<0>(1, QStringList{ "five", "six" }, QStringList{ "五", "六" }, QStringList{ "5", "6"});
		* ui.treeWidget->addLevelItems<1>("six", QStringList{ "seven", "eight" });
		* ui.treeWidget->addLevelItems<1>("six", 0, QStringList{ "七", "八" });
		* @endcode
		* @note 以上代码执行如下图所示
		* _______________________________
		* |键     |值 |说明				|
		* |-----------------------------|
		* |three  |三 |					|
		* |-----------------------------|
		* |five   |五 |5					|
		* |-----------------------------|
		* |six    |六 |6					|
		* |-----------------------------|
		* |  七   |	  |					|
		* |-----------------------------|
		* |  八   |   |					|
		* |-----------------------------|
		* |  seven|   |					|
		* |-----------------------------|
		* |  eight|   |					|
		* |-----------------------------|
		* |four   |四 |					|
		* |-----------------------------|
		* |one	  |	  |					|
		* |-----------------------------|
		* |tow	  |	  |					|
		* -------------------------------
		*/
		template<int Level, class... Args> inline void addLevelItems(Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) >= Level + 1, "参数太少");
			Q_STATIC_ASSERT_X(sizeof...(args) <= Level + 4, "参数太多");
			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			bool searchIndex = false;
			int addIndex = -1;
			if (Level == 0)
			{
				if (datas.front().type() == QVariant::Type::Int)
				{
					searchIndex = true;
					addIndex = datas.front().toInt();
					datas.pop_front();
				}

				QList<QStringList> strings;
				for (auto& x : datas)
				{
					Q_ASSERT_X(x.type() == QVariant::Type::StringList, __FUNCTION__, "项目数据类型必须为QStringList");
					strings.append(x.toStringList());
				}

				while (strings.size() < 3)
				{
					strings.append(QStringList());
				}

				QStringList keys = strings[0];
				QStringList values = strings[1];
				QStringList explains = strings[2];
				while (values.size() < keys.size())
				{
					values.append(QString());
				}

				while (explains.size() < keys.size())
				{
					explains.append(QString());
				}

				if (searchIndex)
				{
					for (int i = keys.size() - 1; i >= 0; --i)
					{
						__super::insertTopLevelItem(addIndex, new QTreeWidgetItem({ keys[i], values[i], explains[i] }));
					}
				}
				else
				{
					for (int i = 0; i < keys.size(); ++i)
					{
						__super::addTopLevelItem(new QTreeWidgetItem({ keys[i], values[i], explains[i] }));
					}
				}
			}
			else
			{
				searchIndex = (datas[Level].type() == QVariant::Type::Int);
				int newLevel = Level + (searchIndex ? 4 : 3);

				//补全
				while (datas.size() < newLevel)
				{
					datas.append(QStringList());
				}

				Q_ASSERT_X(datas.back().type() == QVariant::Type::StringList, __FUNCTION__, "项目键数据类型必须为QStringList");

				//提取说明
				QStringList explains = datas.back().toStringList();
				//删除说明
				datas.pop_back();

				Q_ASSERT_X(datas.back().type() == QVariant::Type::StringList, __FUNCTION__, "项目值数据类型必须为QStringList");

				//提取值
				QStringList values = datas.back().toStringList();
				//删除值
				datas.pop_back();

				Q_ASSERT_X(datas.back().type() == QVariant::Type::StringList, __FUNCTION__, "项目说明数据类型必须为QStringList");
				//提取键
				QStringList keys = datas.back().toStringList();
				//删除键
				datas.pop_back();

				Q_ASSERT_X(keys.size() != 0, __FUNCTION__, "项目键列表不可为空");

				if (searchIndex)
				{
					addIndex = datas.back().toInt();
					//删除添加索引
					datas.pop_back();
				}
				Q_ASSERT_X(datas.size() == Level, __FUNCTION__, "参数太多");

				//补全值
				while (values.size() < keys.size())
				{
					values.append(QString());
				}

				//补全说明
				while (explains.size() < keys.size())
				{
					explains.append(QString());
				}

				auto&& strings = variants2Strings(datas);

				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							if (searchIndex)
							{
								for (int j = keys.size() - 1; j >= 0; --j)
								{
									item->insertChild(addIndex, new QTreeWidgetItem({ keys[j], values[j], explains[j] }));
								}
							}
							else
							{
								for (int j = 0; j < keys.size(); ++j)
								{
									item->addChild(new QTreeWidgetItem({ keys[j], values[j], explains[j] }));
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief addLevelItemBeforeCurrentItem
		* @param[in] args 可变参数
		* @return void
		*/
		template<int Level, class... Args> inline void addLevelItemBeforeCurrentItem(Args&&... args)
		{
			QStringList datas = argsData<QString>(std::forward<Args>(args)...);
			if (Level == 0)
			{
				//最顶层项目
				__super::addTopLevelItem(new QTreeWidgetItem(datas));
			}
			else
			{
				//addLevelNItem<1>("1","2");
				//datas.size()必须大于Level + 1
				Q_STATIC_ASSERT(sizeof...(args) > Level + 1);

				//level1 level0_k+level1_k+level1_v+level_e=4
				//level2 level0_k+level1_k+level2_k+level2_v+level2_e=5
				int newLevel = Level + 3;

				//补全
				while (datas.size() < newLevel)
				{
					datas.append(QString());
				}

				//提取说明
				QString explain = datas.back();
				//删除说明
				datas.pop_back();

				//提取值
				QString value = datas.back();
				//删除值
				datas.pop_back();

				//提取键
				QString key = datas.back();
				//删除键
				datas.pop_back();

				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == datas[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), datas, index, &item, -2))
						{
							int count = item->childCount();
							if (count == 0)
							{
								item->addChild(new QTreeWidgetItem({ key, value, explain }));
							}
							else
							{
								for (int i = 0; i < item->childCount(); ++i)
								{
									if (item->child(i) == __super::currentItem())
									{
										auto tempIndex = i == 0 ? 0 : i - 1;
										item->insertChild(tempIndex, new QTreeWidgetItem({ key, value, explain }));
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 添加项目在当前项目之后
		* @param[in] args 可变参数
		* @return void
		*/
		template<int Level, class... Args> inline void addLevelItemAfterCurrentItem(Args&&... args)
		{
			if (sizeof...(args) != 0)
			{
				QStringList datas = argsData<QString>(std::forward<Args>(args)...);
				if (Level == 0)
				{
					//最顶层项目
					__super::addTopLevelItem(new QTreeWidgetItem(datas));
				}
				else
				{
					//addLevelNItem<1>("1","2");
					//datas.size()必须大于Level + 1
					Q_STATIC_ASSERT(sizeof...(args) > Level + 1);

					//level1 level0_k+level1_k+level1_v+level_e=4
					//level2 level0_k+level1_k+level2_k+level2_v+level2_e=5
					int newLevel = Level + 3;

					//补全
					while (datas.size() < newLevel)
					{
						datas.append(QString());
					}

					//提取说明
					QString explain = datas.back();
					//删除说明
					datas.pop_back();

					//提取值
					QString value = datas.back();
					//删除值
					datas.pop_back();

					//提取键
					QString key = datas.back();
					//删除键
					datas.pop_back();

					QTreeWidgetItem* item = nullptr;
					int index = 0;
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (__super::topLevelItem(i)->text(0) == datas[0])
						{
							if (findAllTreeItem(__super::topLevelItem(i), datas, index, &item, -2))
							{
								int count = item->childCount();
								if (count == 0)
								{
									item->addChild(new QTreeWidgetItem({ key,value,explain }));
								}
								else
								{
									for (int i = 0; i < item->childCount(); ++i)
									{
										if (item->child(i) == __super::currentItem())
										{
											auto tempIndex = i == item->childCount() ? item->childCount() : i + 1;
											item->insertChild(tempIndex, new QTreeWidgetItem({ key,value,explain }));
											break;
										}
									}
								}
								break;
							}
						}
					}
				}
			}
		}

		/*
		* @brief 删除项目
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* ui.treeWidget->addLevelItems<0>(QStringList{ "one", "two" }); //ok
		* ui.treeWidget->addLevelItems<0>(0, QStringList{ "three", "four" }, QStringList{ "三", "四" }); //ok
		* ui.treeWidget->addLevelItems<0>(1, QStringList{ "five", "six" }, QStringList{ "五", "六" }, QStringList{ "5", "6"});
		* ui.treeWidget->addLevelItems<1>("six", QStringList{ "seven", "eight" });
		* ui.treeWidget->addLevelItems<1>("six", 0, QStringList{ "七", "八" });
		*
		* ui.treeWidget->removeLevelItem<1>("six", "seven");
		* ui.treeWidget->removeLevelItem<1>("six", 2);
		* ui.treeWidget->removeLevelItem<0>(0);
		* @endcode
		* @note 以上代码执行如下图所示
		* _______________________________
		* |键     |值 |说明				|
		* |-----------------------------|
		* |five   |五 |5					|
		* |-----------------------------|
		* |six    |六 |6					|
		* |-----------------------------|
		* |  七   |	  |					|
		* |-----------------------------|
		* |  八   |   |					|
		* |-----------------------------|
		* |four   |四 |					|
		* |-----------------------------|
		* |one	  |	  |					|
		* |-----------------------------|
		* |tow	  |	  |					|
		* |-----------------------------|
		*/
		template<int Level, class... Args> inline void removeLevelItem(Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) == Level + 1, "参数个数不正确");
			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			if (Level == 0)
			{
				if (datas[0].type() == QVariant::Type::String)
				{
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (__super::topLevelItem(i)->text(0) == datas[0].toString())
						{
							__super::takeTopLevelItem(i);
							break;
						}
					}
				}
				else
				{
					takeTopLevelItem(datas[0].toInt());
				}
			}
			else
			{
				QVariant variant = datas.back();
				datas.pop_back();
				auto&& strings = variants2Strings(datas);
				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == datas[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							if (variant.type() == QVariant::Type::String)
							{
								auto&& str = variant.toString();
								for (int j = 0; j < item->childCount(); ++j)
								{
									if (item->child(j)->text(0) == str)
									{
										item->removeChild(item->child(j));
										break;
									}
								}
							}
							else
							{
								int num = variant.toInt();
								for (int j = 0; j < item->childCount(); ++j)
								{
									if (j == num)
									{
										item->removeChild(item->child(j));
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 删除项目
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* ui.treeWidget->addLevelItems<0>(QStringList{ "one", "two" }); //ok
		* ui.treeWidget->addLevelItems<0>(0, QStringList{ "three", "four" }, QStringList{ "三", "四" }); //ok
		* ui.treeWidget->addLevelItems<0>(1, QStringList{ "five", "six" }, QStringList{ "五", "六" }, QStringList{ "5", "6"});
		* ui.treeWidget->addLevelItems<1>("six", QStringList{ "seven", "eight" });
		* ui.treeWidget->addLevelItems<1>("six", 0, QStringList{ "七", "八" });
		*
		* ui.treeWidget->removeLevelItems<1>("six", QStringList{ "seven", "eight" });
		* @endcode
		* @note 以上代码执行如下图所示
		* _______________________________
		* |键     |值 |说明				|
		* |-----------------------------|
		* |three  |三 |					|
		* |-----------------------------|
		* |five   |五 |5					|
		* |-----------------------------|
		* |six    |六 |6					|
		* |-----------------------------|
		* |  七   |	  |					|
		* |-----------------------------|
		* |  八   |   |					|
		* |-----------------------------|
		* |four   |四 |					|
		* |-----------------------------|
		* |one	  |	  |					|
		* |-----------------------------|
		* |tow	  |	  |					|
		* |-----------------------------|
		*/
		template<int Level, class... Args> inline void removeLevelItems(Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) == Level + 1, "参数个数不正确");
			QList<QVariant> datas = argsDataVariant(std::forward<Args>(args)...);
			if (Level == 0)
			{
				Q_ASSERT_X(datas[0].type() == QVariant::Type::StringList ||
					datas[0].type() == QVariant::Type::List, __FUNCTION__, "参数数据类型不正确");

				if (datas[0].type() == QVariant::Type::StringList)
				{
					QStringList&& temp = datas[0].toStringList();
					for (int j = 0; j < temp.size(); ++j)
					{
						for (int i = 0; i < __super::topLevelItemCount(); ++i)
						{
							if (__super::topLevelItem(i)->text(0) == temp[j])
							{
								__super::takeTopLevelItem(i);
								break;
							}
						}
					}
				}
				else
				{
					QList<QVariant>&& temp = datas[0].toList();
					for (int i = temp.size() - 1; i >= 0; --i)
					{
						__super::takeTopLevelItem(temp[i].toInt());
					}
				}
			}
			else
			{
				QVariant variant = datas.back();
				datas.pop_back();
				Q_ASSERT_X(variant.type() == QVariant::Type::StringList ||
					variant.type() == QVariant::Type::List, __FUNCTION__, "参数数据类型不正确");

				auto&& strings = variants2Strings(datas);
				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							if (variant.type() == QVariant::Type::StringList)
							{
								QStringList&& strs = variant.toStringList();
								for (int k = 0; k < strs.size(); ++k)
								{
									for (int j = 0; j < item->childCount(); ++j)
									{
										if (item->child(j)->text(0) == strs[k])
										{
											item->removeChild(item->child(j));
											break;
										}
									}
								}
							}
							else
							{
								QList<QVariant>&& nums = variant.toList();
								//需要从后往前删除
								for (int k = nums.size() - 1; k >= 0; --k)
								{
									item->takeChild(nums[k].toInt());
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 设置级别项目
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* ui.treeWidget->addLevelItems<0>(QStringList{ "one", "two" }); //ok
		* ui.treeWidget->addLevelItems<0>(0, QStringList{ "three", "four" }, QStringList{ "三", "四" }); //ok
		* ui.treeWidget->addLevelItems<0>(1, QStringList{ "five", "six" }, QStringList{ "五", "六" }, QStringList{ "5", "6" });
		* ui.treeWidget->addLevelItems<1>("six", QStringList{ "seven", "eight" });
		* ui.treeWidget->addLevelItems<1>("six", 0, QStringList{ "七", "八" });
		*
		* ui.treeWidget->setLevelItem<1>("six", "七", 1, "seven");
		* ui.treeWidget->setLevelItem<1>("six", "七", 2, "中文&英文");
		* ui.treeWidget->setLevelItem<1>("six", 1, 1, "eight");//第一个1代表八的下标,第二个1代表列数
		* ui.treeWidget->setLevelItem<1>("six", 1, 2, "中文&英文");//第一个1代表八的下标,第二个2代表列数
		* @endcode
		* @note 以上代码执行如下图所示
		* _______________________________
		* |键     |值    |说明		    |
		* |-----------------------------|
		* |three  |三    |				|
		* |-----------------------------|
		* |five   |五    |5				|
		* |-----------------------------|
		* |six    |六    |6				|
		* |-----------------------------|
		* |  七   |seven |中文&英文		|
		* |-----------------------------|
		* |  八   |eight |中文&英文		|
		* |-----------------------------|
		* |four   |四    |				|
		* |-----------------------------|
		* |one	  |	     |				|
		* |-----------------------------|
		* |tow	  |	     |				|
		* |-----------------------------|
		*/
		template<int Level, class... Args> inline void setLevelItem(Args&&... args)
		{
			constexpr size_t argsSize = sizeof...(args);
			Q_STATIC_ASSERT_X(argsSize == Level + 3, "参数个数不正确");
			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);

			//文本内容
			Q_ASSERT_X(datas.back().type() == QVariant::Type::String, __FUNCTION__, "文本内容数据类型不正确");
			QString text = datas.back().toString();
			datas.pop_back();

			//第几列
			Q_ASSERT_X(datas.back().type() == QVariant::Type::Int, __FUNCTION__, "列数数据类型不正确");
			int column = datas.back().toInt();
			datas.pop_back();

			//键(string->text or int->index)
			Q_ASSERT_X(datas.back().type() == QVariant::Type::String || datas.back().type() == QVariant::Type::Int,
				__FUNCTION__, "键数据类型不正确");

			QVariant key = datas.back();
			datas.pop_back();

			if (Level == 0)
			{
				if (key.type() == QVariant::Type::String)
				{
					QString&& str = key.toString();
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (__super::topLevelItem(i)->text(0) == str)
						{
							__super::topLevelItem(i)->setText(column, text);
							break;
						}
					}
				}
				else
				{
					int num = key.toInt();
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (i == num)
						{
							__super::topLevelItem(i)->setText(column, text);
							break;
						}
					}
				}
			}
			else
			{
				auto&& strings = variants2Strings(datas);
				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							bool isString = (key.type() == QVariant::Type::String);
							QString str = key.toString();
							int num = key.toInt();
							for (int j = 0; j < item->childCount(); ++j)
							{
								if (isString)
								{
									if (item->child(j)->text(0) == str)
									{
										item->child(j)->setText(column, text);
										break;
									}
								}
								else
								{
									if (j == num)
									{
										item->child(j)->setText(column, text);
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 设置级别控件
		* @param[in] args 可变参数
		* @return void
		* @note 控件无法用文字画出,不做演示.
		* @par
		* @code
		* QComboBox* combo = new QComboBox;
		* combo->addItems({"1", "2", "3"});
		* ui.treeWidget->setLevelWidget("example", "combo_box", 1, combox);
		* @endcode
		*/
		template<int Level, class... Args> inline void setLevelWidget(Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) == Level + 3, "参数个数不正确");
			QList<_QVariant> datas = argsData<_QVariant>(std::forward<Args>(args)...);

			//控件
			Q_ASSERT_X(datas.back().type() == _QVariant::Type::Invalid, __FUNCTION__, "控件数据类型不正确");
			QWidget* widget = datas.back().toWidgetPtr();
			datas.pop_back();

			//第几列
			Q_ASSERT_X(datas.back().type() == _QVariant::Type::Int, __FUNCTION__, "列数数据类型不正确");
			int column = datas.back().toInt();
			datas.pop_back();

			//键或索引
			Q_ASSERT_X(datas.back().type() == _QVariant::Type::String || datas.back().type() == _QVariant::Type::Int,
				__FUNCTION__, "键数据类型不正确");
			_QVariant key = datas.back();
			datas.pop_back();

			if (Level == 0)
			{
				if (key.type() == _QVariant::Type::String)
				{
					QString&& str = key.toString();
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (__super::topLevelItem(i)->text(0) == str)
						{
							setItemWidget(__super::topLevelItem(i), column, widget);
							break;
						}
					}
				}
				else
				{
					int num = key.toInt();
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (i == num)
						{
							setItemWidget(__super::topLevelItem(i), column, widget);
							break;
						}
					}
				}
			}
			else
			{
				auto&& strings = _variants2Strings(datas);
				QTreeWidgetItem* item = nullptr;
				int index = 0;
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							bool isString = (key.type() == _QVariant::Type::String);
							QString str = key.toString();
							int num = key.toInt();
							for (int j = 0; j < item->childCount(); ++j)
							{
								if (isString)
								{
									if (item->child(j)->text(0) == str)
									{
										setItemWidget(item->child(j), column, widget);
										break;
									}
								}
								else
								{
									if (j == num)
									{
										setItemWidget(item->child(j), column, widget);
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}

		/*
		* @brief 设置级别图标
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* setLevelIcon<0>("example", "key.ico", "value.ico", "explain.ico");//value.ico&explain.ico可选
		* setLevelIcon<0>(0, "key.ico", "value.ico", "explain.ico");
		* setLevelIcon<1>("example", "one", "key.ico", "value.ico", "explain.ico");
		* setLevelIcon<1>("example", 0, "key.ico", "value.ico", "explain.ico");
		* @endcode
		*/
		template<int Level, class... Args> inline void setLevelIcon(Args&&... args)
		{
			if (m_maskIcon) return;

			Q_STATIC_ASSERT_X(sizeof...(args) >= Level + 2 && sizeof...(args) <= Level + 4, "参数个数不正确");

			CountLock lock(m_maskOnItemChangedCount);

			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			int newLevel = Level + 4;

			//补全
			while (datas.size() < newLevel)
			{
				datas.append(QVariant());
			}

			//提取说明图标
			QString explainIcon = datas.back().toString();
			datas.pop_back();

			//提取值图标
			QString valueIcon = datas.back().toString();
			datas.pop_back();

			//提取键图标
			QString keyIcon = datas.back().toString();
			datas.pop_back();

			//提取键
			QVariant key = datas.back();
			datas.pop_back();

			Q_ASSERT_X(key.type() == QVariant::Type::String || key.type() == QVariant::Type::Int, __FUNCTION__, "键数据类型错误");

			QTreeWidgetItem* item = nullptr;
			int index = 0;
			bool isString = key.type() == QVariant::Type::String;
			QString keyString;
			int keyIndex = 0;
			if (isString)
			{
				keyString = key.toString();
			}
			else
			{
				keyIndex = key.toInt();
			}

			if (Level == 0)
			{
				if (isString)
				{
					for (int i = 0; i < __super::topLevelItemCount(); ++i)
					{
						if (__super::topLevelItem(i)->text(0) == keyString)
						{
							item = __super::topLevelItem(i);
							break;
						}
					}
				}
				else
				{
					item = __super::topLevelItem(keyIndex);
				}
			}
			else
			{
				auto&& strings = variants2Strings(datas);
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item))
						{
							if (isString)
							{
								for (int j = 0; j < item->childCount(); ++j)
								{
									if (item->child(j)->text(0) == keyString)
									{
										item = item->child(j);
										break;
									}
								}
							}
							else
							{
								item = item->child(keyIndex);
							}
							break;
						}
					}
				}
			}

			if (item)
			{
				if (!keyIcon.isEmpty())
				{
					item->setIcon(0, convertToQrcUrl(keyIcon));
				}

				if (!valueIcon.isEmpty())
				{
					item->setIcon(1, convertToQrcUrl(valueIcon));
				}

				if (!explainIcon.isEmpty())
				{
					item->setIcon(2, convertToQrcUrl(explainIcon));
				}
			}
		}

		/*
		* @brief 设置级别图标
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* setLevelIcons<0>(QStringList{ "one", "two" }, "key.ico", "value.ico", "explain.ico");//value.ico&explain.ico可选
		* setLevelIcons<0>(QList<QVariant>{ 1, 2 }, "key.ico", "value.ico", "explain.ico");
		* setLevelIcons<1>("example", QStringList{ "one", "two"}, "key.ico", "value.ico", "explain.ico");
		* setLevelIcons<1>("example", QList<QVariant>{ 1, 2 }, "key.ico", "value.ico", "explain.ico");
		* @endcode
		*/
		template<int Level, class... Args> inline void setLevelIcons(Args&&... args)
		{
			if (m_maskIcon) return;

			Q_STATIC_ASSERT_X(sizeof...(args) >= Level + 2 && sizeof...(args) <= Level + 4, "参数个数不正确");

			CountLock lock(m_maskOnItemChangedCount);

			QList<QVariant> datas = argsDataVariant(std::forward<Args>(args)...);

			int newLevel = Level + 4;

			//补全
			while (datas.size() < newLevel)
			{
				datas.append(QVariant());
			}

			//提取说明图标
			QString explainIcon = datas.back().toString();
			datas.pop_back();

			//提取值图标
			QString valueIcon = datas.back().toString();
			datas.pop_back();

			//提取键图标
			QString keyIcon = datas.back().toString();
			datas.pop_back();

			//提取键
			QVariant key = datas.back();
			datas.pop_back();

			Q_ASSERT_X(key.type() == QVariant::Type::StringList || key.type() == QVariant::Type::List, __FUNCTION__, "键数据类型错误");

			QTreeWidgetItem* item = nullptr;
			int index = 0;
			bool isStringList = key.type() == QVariant::Type::StringList;
			QStringList keyStrings;
			QList<QVariant> keyIndexs;
			if (isStringList)
			{
				keyStrings = key.toStringList();
			}
			else
			{
				keyIndexs = key.toList();
			}

			QList<QTreeWidgetItem*> iconItems;
			if (Level == 0)
			{
				if (isStringList)
				{
					int j = 0;
					for (auto& x : keyStrings)
					{
						for (int i = 0; i < __super::topLevelItemCount(); ++i)
						{
							if (__super::topLevelItem(i)->text(0) == x)
							{
								iconItems.append(__super::topLevelItem(j++));
								break;
							}
						}
					}
				}
				else
				{
					for (auto& x : keyIndexs)
					{
						iconItems.append(__super::topLevelItem(x.toInt()));
					}
				}
			}
			else
			{
				auto&& strings = variants2Strings(datas);
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == strings[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), strings, index, &item) && item)
						{
							if (isStringList)
							{
								int k = 0;
								for (auto& x : keyStrings)
								{
									for (int j = 0; j < item->childCount(); ++j)
									{
										if (item->child(j)->text(0) == x)
										{
											iconItems.append(item->child(k++));
											break;
										}
									}
								}
							}
							else
							{
								for (auto& x : keyIndexs)
								{
									iconItems.append(item->child(x.toInt()));
								}
							}
							break;
						}
					}
				}
			}

			for (auto& item : iconItems)
			{
				if (!item)
				{
					continue;
				}

				if (!keyIcon.isEmpty())
				{
					item->setIcon(0, convertToQrcUrl(keyIcon));
				}

				if (!valueIcon.isEmpty())
				{
					item->setIcon(1, convertToQrcUrl(valueIcon));
				}

				if (!explainIcon.isEmpty())
				{
					item->setIcon(2, convertToQrcUrl(explainIcon));
				}
			}
		}

		/*
		* @brief 当前的项目是多少级
		* @tparam Level 级别
		* @return bool
		*/
		template<int Level> inline [[nodiscard]] bool currentItemIsLevel() const
		{
			return getItemLevel() == Level;
		}

		/*
		* @brief 获取顶部项目
		* @return QTreeWidgetItem*
		*/
		[[nodiscard]] QTreeWidgetItem* topItem() const;

		/*
		* @brief 获取顶部文本
		* @param[in] index 列索引
		* @return QString
		*/
		[[nodiscard]] QString topText(int index = 0) const;

		/*
		* @brief 父项目
		* @return QTreeWidgetItem*
		*/
		[[nodiscard]] QTreeWidgetItem* parentItem() const;

		/*
		* @brief 父文本
		* @param[in] index 索引
		* @return QString
		*/
		[[nodiscard]] QString parentText(int index = 0) const;

		/*
		* @brief 当前文本
		* @param[in] index 索引
		* @return QString
		*/
		[[nodiscard]] QString currentText(int index = 0) const;

		/*
		* @brief 当前行
		* @return int
		*/
		[[nodiscard]] int currentRow() const;

		/*
		* @brief 子项目
		* @return QTreeWidgetImte*
		*/
		[[nodiscard]] QTreeWidgetItem* childItem() const;

		/*
		* @brief 子文本
		* @param[in] index 索引
		* @return QString
		*/
		[[nodiscard]] QString childText(int index = 0) const;

		/*
		* @brief 当前级别键
		* @return QStringList
		*/
		[[nodiscard]] QStringList currentLevelKeys() const;

		/*
		* @brief 当前级别值
		* @return QStringList
		*/
		[[nodiscard]] QStringList currentLevelValues() const;

		/*
		* @brief 当前级别大小
		* @return int
		*/
		[[nodiscard]] int currentLevelSize() const;

		/*
		* @brief 当前子键
		* @return QStringList
		*/
		[[nodiscard]] QStringList currentChildKeys() const;

		/*
		* @brief 当前子值
		* @return QStringList
		*/
		[[nodiscard]] QStringList currentChildValues() const;

		/*
		* @brief 当前子键大小
		* @return int
		*/
		[[nodiscard]] int currentChildKeysSize() const;

		/*
		* @brief 当前父键
		* @return QStringList
		*/
		[[nodiscard]] QStringList currentParentKeys() const;

		/*
		* @brief 当前父键大小
		* @return int
		*/
		[[nodiscard]] int currentParentKeysSize() const;

		/*
		* @brief 上一个项目
		* @return QTreeWidgetItem*
		*/
		[[nodiscard]] QTreeWidgetItem* previousItem() const;

		/*
		* @brief 上一个文本
		* @param[in] column 行
		* @return QString
		*/
		[[nodiscard]] QString previousText(int column = 0) const;

		/*
		* @brief 上一行
		* @return itn
		*/
		[[nodiscard]] int previousRow() const;

		/*
		* @brief 上一列
		* @return int
		*/
		[[nodiscard]] int previousColumn() const;

		/*
		* @brief 设置屏蔽图标
		* @param[in] mask 是否屏蔽
		* @return void
		*/
		void setMaskIcon(bool mask);

		/*
		* @brief 设置双击拦截
		* @param[in] on 是否拦截
		* @return void
		*/
		void setDoubleClickedIntercept(bool on);

		/*
		* @brief 是否双击拦截
		* @retval true 是
		* @retval false 否
		*/
		bool isDoubleClickedIntercept() const;

		/*
		* @brief 设置键是否可编辑
		* @param[in] topKey 顶层键名称
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* setLevelKeyEditPrivilege("example", 0, 1, 2, true);
		* @endcode
		*/
		template<class... Args> inline void setLevelKeyEditPrivilege(const QString& topKey, Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) != 0, "参数个数不正确");

			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			QVariant variant = datas.back();
			datas.pop_back();

			Q_ASSERT_X(variant.type() == QVariant::Type::Bool, __FUNCTION__, "最后一个参数必须为布尔类型");

			auto ints = variants2Ints(datas);

			for (const auto& x : ints)
			{
				QString&& childKey = QString("%1_%2").arg(topKey).arg(x);
				if (!m_editPrivilege.contains(childKey))
				{
					EditPrivilege privilege;
					privilege.key = variant.toBool();
					m_editPrivilege.insert(childKey, privilege);
				}
				else
				{
					m_editPrivilege[childKey].key = variant.toBool();
				}
			}
		}

		/*
		* @brief 设置值是否可编辑
		* @param[in] topKey 顶层键名称
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* setLevelValueEditPrivilege("example", 0, 1, 2, true);
		* @endcode
		*/
		template<class... Args> inline void setLevelValueEditPrivilege(const QString& topKey, Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) != 0, "参数个数不正确");

			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			QVariant variant = datas.back();
			datas.pop_back();

			Q_ASSERT_X(variant.type() == QVariant::Type::Bool, __FUNCTION__, "最后一个参数必须为布尔类型");

			auto ints = variants2Ints(datas);

			for (const auto& x : ints)
			{
				QString&& childKey = QString("%1_%2").arg(topKey).arg(x);
				if (!m_editPrivilege.contains(childKey))
				{
					EditPrivilege privilege;
					privilege.value = variant.toBool();
					m_editPrivilege.insert(childKey, privilege);
				}
				else
				{
					m_editPrivilege[childKey].value = variant.toBool();
				}
			}
		}

		/*
		* @brief 设置说明是否可编辑
		* @param[in] topKey 顶层键名称
		* @param[in] args 可变参数
		* @return void
		* @par
		* @code
		* setLevelExplainEditPrivilege("example", 0, 1, 2, true);
		* @endcode
		*/
		template<class... Args> inline void setLevelExplainEditPrivilege(const QString& topKey, Args&&... args)
		{
			Q_STATIC_ASSERT_X(sizeof...(args) != 0, "参数个数不正确");

			QList<QVariant> datas = argsData<QVariant>(std::forward<Args>(args)...);
			QVariant variant = datas.back();
			datas.pop_back();

			Q_ASSERT_X(variant.type() == QVariant::Type::Bool, __FUNCTION__, "最后一个参数必须为布尔类型");

			auto ints = variants2Ints(datas);

			for (const auto& x : ints)
			{
				QString&& childKey = QString("%1_%2").arg(topKey).arg(x);
				if (!m_editPrivilege.contains(childKey))
				{
					EditPrivilege privilege;
					privilege.explain = variant.toBool();
					m_editPrivilege.insert(childKey, privilege);
				}
				else
				{
					m_editPrivilege[childKey].explain = variant.toBool();
				}
			}
		}

		::std::function<void(QTreeWidgetItem* item, int row, int column)> onItemSelectionChanged = nullptr;

		::std::function<void(QTreeWidgetItem* item, int row, int column)> onItemPressed = nullptr;

		::std::function<bool(QTreeWidgetItem* item, int row, int column)> onItemDoubleClicked = nullptr;

		::std::function<bool(QTreeWidgetItem* item, int row, int column)> onItemChanged = nullptr;

		/*
		* @brief 屏蔽项目改变
		* @param[in] lock 计数锁
		* @return void
		*/
		void maskOnItemChanged(CountLock& lock);

		/*
		* @brief 正在恢复项目文本
		* @return bool
		*/
		bool restoringItemText() const;

	public:
		/*
		* @brief 获取最顶层项目
		* @return 项目列表
		*/
		QList<QTreeWidgetItem*> getTopLevelItems() const;

		/*
		* @brief 获取最顶层项目键
		* @return 键
		*/
		QStringList getTopLevelItemKeys() const;

		/*
		* @brief 获取最顶层项目数量
		* @return 数量
		*/
		int getTopLevelItemCount() const;

		/*
		* @brief 查找N级项目
		* @param[in] args 可变参数
		* @retval nullptr 未找到
		* @retval !=nullptr 找到
		* @note 可变参数的最后一个参数,如果为整数,则通过下标来返回可变参数最后一个前一个的子项目.
		*/
		template<class... Args> inline QTreeWidgetItem* findLevelItem(Args&&... args)
		{
			QTreeWidgetItem* item = nullptr;
			if (sizeof...(args) != 0)
			{
				QList<QVariant>&& variants = argsData<QVariant>(std::forward<Args>(args)...);
				bool ok = false;
				int itemIndex = variants.back().toInt(&ok);
				if (!ok)
				{
					//如果最后一个不为整数,则代表未使用下标查询子项目
					itemIndex = -1;
				}
				else
				{
					//如果最后一个为整数,则代表使用下标查询子项目
					//防止越界
					if (itemIndex < 0)
					{
						itemIndex = 0;
					}
				}

				int findIndex = 0;
				QStringList findDatas;
				int size = ok ? variants.size() - 1 : variants.size();
				for (int i = 0; i < size; ++i)
				{
					findDatas.append(variants[i].toString());
				}

				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == findDatas[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), findDatas, findIndex, &item, itemIndex))
						{
							break;
						}
					}
				}
			}
			return item;
		}

		/*
		* @brief 查找N级子项目
		* @param[in] args 可变参数
		* @retval QList<QTreeWidgetItem*>().size()为0 未找到
		* @retval QList<QTreeWidgetItem*>().size()不为0 找到
		*/
		template<class... Args> inline QList<QTreeWidgetItem*> findLevelChildren(Args&&... args)
		{
			QList<QTreeWidgetItem*> items;
			QTreeWidgetItem* item = nullptr;
			if (sizeof...(args) != 0)
			{
				int findIndex = 0;
				QStringList findDatas = argsData<QString>(std::forward<Args>(args)...);
				for (int i = 0; i < __super::topLevelItemCount(); ++i)
				{
					if (__super::topLevelItem(i)->text(0) == findDatas[0])
					{
						if (findAllTreeItem(__super::topLevelItem(i), findDatas, findIndex, &item, -2))
						{
							break;
						}
					}
				}

				if (item)
				{
					for (int i = 0; i < item->childCount(); ++i)
					{
						items.append(item->child(i));
					}
				}
			}
			return items;
		}

		/*
		* @brief 获取项目级别
		* @return 级别
		*/
		int getItemLevel() const
		{
			QTreeWidgetItem* item = __super::currentItem();
			if (!item) {
				return -1;
			}

			int level = 0;
			while (item && item->parent()) {
				item = item->parent();
				++level;
			}
			return level;
		}

	protected:
		/*
		* @brief 变量转字符串
		* @param[in] variants 变量
		* @return QStringList
		*/
		inline QStringList variants2Strings(const QList<QVariant>& variants) const
		{
			QStringList strings;
			for (int i = 0; i < variants.size(); ++i)
			{
				Q_ASSERT_X(variants[i].type() == QVariant::Type::String, __FUNCTION__, "传入参数数据类型不正确");
				strings.append(variants[i].toString());
			}
			return strings;
		}

		/*
		* @brief 变量
		*/
		class _QVariant : public QVariant {
		public:
			_QVariant(int i) :QVariant(i) {}

			_QVariant(uint ui) :QVariant(ui) {}

			_QVariant(qlonglong ll) :QVariant(ll) {}

			_QVariant(qulonglong ull) :QVariant(ull) {}

			_QVariant(const char* string) :QVariant(string) {}

			_QVariant(const QString& string) :QVariant(string) {}

			_QVariant(QWidget* widget) :QVariant(), m_widget(widget) { }

			QWidget* toWidgetPtr() const { return m_widget; }
		private:
			QWidget* m_widget = nullptr;
		};

		/*
		* @brief 变量转字符串
		* @param[in] _variants 变量
		* @return QStringList
		*/
		inline QStringList _variants2Strings(const QList<_QVariant>& _variants) const
		{
			QStringList strings;
			for (int i = 0; i < _variants.size(); ++i)
			{
				Q_ASSERT_X(_variants[i].type() == QVariant::Type::String, __FUNCTION__, "传入参数数据类型不正确");
				strings.append(_variants[i].toString());
			}
			return strings;
		}

		/*
		* @brief 变量转整形
		* @param[in] variants 变量
		* @return QList<int>
		*/
		inline QList<int> variants2Ints(const QList<QVariant>& variants) const
		{
			QList<int> ints;
			for (int i = 0; i < variants.size(); ++i)
			{
				Q_ASSERT_X(variants[i].type() == QVariant::Type::Int, __FUNCTION__, "传入参数数据类型不正确");
				ints.append(variants[i].toInt());
			}
			return ints;
		}

		/*
		* @brief 获取项目行号
		* @param[in] item 项目
		* @retval -1 没获取到
		* @retval >=0 获取到
		*/
		int getItemRow(const QTreeWidgetItem* item) const;

		/*
		* @brief 转换为qrcurl
		* @param[in] icon 图标
		* @return const QIcon&
		*/
		const QIcon& convertToQrcUrl(const QString& icon);

		/*
		* @brief 查找所有的树项目
		* @param[in] item 要查找的项目
		* @param[in] findDatas 查找的数据
		* @param[in] findIndex 查找的索引
		* @param[in] findResult 找到的项目
		* @param[in] itemIndex 索引
		* index为-1,代表查找单个项目
		* index为-2,代表查找所有项目
		* index为>0,代表通过索引查找单个项目
		* @retval true 找到
		* @retval false 未找到
		*/
		bool findAllTreeItem(const QTreeWidgetItem* item,
			const QStringList& findDatas,
			int& findIndex,
			QTreeWidgetItem** findResult,
			int itemIndex = -1);

		/*
		* @brief 可变参数数据
		* @param[in] args 可变参数
		* @QList<T>
		*/
		template<class T, class... Args> inline QList<T> argsData(Args&&... args)
		{
			QList<T> datas;
			std::initializer_list<int>{(datas.append(args), 0)...};
			return datas;
		}

		/*
		* @brief 可变参数数据变量
		* @param[in] args 可变参数
		* @return QList<QVariant>
		*/
		template<class... Args> inline QList<QVariant> argsDataVariant(Args&&... args)
		{
			QList<QVariant> datas;
			std::initializer_list<int>{(datas.append(QVariant(args)), 0)...};
			return datas;
		}

	private slots:
		/*
		* @brief 选择项目改变槽
		* @return void
		*/
		void itemSelectionChangedSlot();

		/*
		* @brief 按下项目槽
		* @param[in] item 项目
		* @param[in] column 列
		* @return void
		*/
		void itemPressedSlot(QTreeWidgetItem* item, int column);

		/*
		* @brief 双击项目槽
		* @param[in] item 项目
		* @param[in] column 列
		* @return void
		*/
		void itemDoubleClickedSlot(QTreeWidgetItem* item, int column);

		/*
		* @brief 项目改变槽
		* @param[in] item 项目
		* @param[in] column 列
		* @return void
		*/
		void itemChangedSlot(QTreeWidgetItem* item, int column);

	private:
		QString m_qrcUrl;
		bool m_itemOpen = false;
		QTreeWidgetItem* m_previousPtr = nullptr;
		QTreeWidgetItem* m_previousItem = nullptr;
		int m_previousRow = 0;
		int m_previousColumn = 0;
		QString m_previousValue;
		bool m_doubleClickedIntercept = true;
		int m_maskOnItemChangedCount = 0;
		bool m_restoringItemText = false;
		bool m_maskIcon = false;
		struct EditPrivilege {
			bool key;
			bool value;
			bool explain;
			int level;
			EditPrivilege() :key(false), value(false),
				explain(false), level(0) {}
			~EditPrivilege() {}
		};
		QMap<QString, EditPrivilege> m_editPrivilege;
		struct ItemState {
			bool saveState = false;
			QString fileName;
		} m_itemState;
		QMap<QString, QIcon> m_iconCache;
		std::unique_ptr<Ini> m_ini;
		QString m_parentObjectName;
	};

	//定义PageData
	using PageData = QList<QList<QString>>;

	/*
	* @brief 分页装饰物
	*/
	class QUI_DLL_EXPORT PageWidget : public QWidget {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		PageWidget(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~PageWidget();

		/*
		* @brief 初始化
		* @param[in] tableWidget 表格装饰物
		* @param[in] blockSize 块大小
		* @return void
		*/
		void initialize(QTableWidget* tableWidget, int blockSize = 3);

		/*
		* @brief 获取块大小
		* @return int
		*/
		int blockSize() const;

		/*
		* @brief 设置最大页
		* @param[in] maxPage 最大页
		* @return void
		*/
		void setMaxPage(int maxPage);

		/*
		* @brief 获取最大页
		* @return int
		*/
		int maxPage() const;

		/*
		* @brief 设置当前页
		* @param[in] currentPage 当前页
		* @param[in] signalEmitted 是否发射信号
		* @return void
		*/
		void setCurrentPage(int currentPage, bool signalEmitted = false);

		/*
		* @brief 获取当前页
		* @return int
		*/
		int currentPage() const;

		/*
		* @brief 设置页数据
		* @param[in] data 数据
		* @return void
		*/
		void setPageData(const PageData& data);

	protected:
		/*
		* @brief 设置块大小
		* @param[in] blockSize 块大小
		* @return void
		*/
		void setBlockSize(int blockSize);

		/*
		* @brief 更新页标签
		* @return void
		*/
		void updatePageLabels();

		/*
		* @brief 事件过滤器
		* @param[in] object 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* object, QEvent* event) override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	signals:
		/*
		* @brief 页已经改变
		* @param[in] page 第几页
		* @return void
		*/
		void pageChanged(int page);

	private:
		QHBoxLayout* horizontalLayout = nullptr;
		QSpacerItem* horizontalSpacer = nullptr;
		QLabel* previousPageLabel = nullptr;
		QWidget* leftPagesWidget = nullptr;
		QLabel* leftSeparateLabel = nullptr;
		QWidget* centerPagesWidget = nullptr;
		QLabel* rightSeparateLabel = nullptr;
		QWidget* rightPagesWidget = nullptr;
		QLabel* nextPageLabel = nullptr;
		QLabel* label = nullptr;
		QLineEdit* pageLineEdit = nullptr;
		QLabel* label_2 = nullptr;
		QSpacerItem* horizontalSpacer_2 = nullptr;

	private:
		int m_blockSize = 3;
		int m_maxPage = 0;
		int m_currentPage = 1;
		QList<QLabel*>* m_pageLabels = nullptr;
		QTableWidget* m_tableWidget = nullptr;
	};

	/*
	* @brief ini文件操作
	* 此类是为了解决QSettings无法将中文的group或key正常显示在文件中,类内部调用Windows API接口实现,
	* 大部分接口都与QSettings一样,内部编码为ANSI(WINXP下为gb2312,WIN7~WIN11为gbk).
	* 提供INI文件读写功能的类，支持分组、数组操作和数据加密, 内部与QSettings完全一致
	* 该类封装了Windows INI文件操作API，提供线程安全的INI文件读写功能，
	* 支持嵌套分组、数组操作、注释管理和数据加密，适用于配置文件管理场景。
	*/
	class QUI_DLL_EXPORT Ini
	{
	public:
		// RAII操作

		// 分组锁
		class GroupLocker {
		public:
			explicit GroupLocker(qui::Ini* ini, const QString& groupName, bool isEndAll = false) : ini_(ini), is_end_all_(isEndAll) {
				ini_->beginGroup(groupName);
			}
			~GroupLocker() { 
				is_end_all_ ? ini_->endAllGroup() : ini_->endGroup();
			}
		private:
			GroupLocker(const GroupLocker&) = delete; 
			GroupLocker& operator=(const GroupLocker&) = delete;
			qui::Ini* ini_;
			bool is_end_all_;
		};

		// 读数组锁
		class ReadArrayLocker {
		public:
			explicit ReadArrayLocker(qui::Ini* ini, const QString& arrayName, int* arraySize) : ini_(ini) {
				Q_ASSERT(arraySize != nullptr);
				*arraySize = ini_->beginReadArray(arrayName);
			}
			~ReadArrayLocker() {
				ini_->endArray();
			}
		private:
			ReadArrayLocker(const ReadArrayLocker&) = delete; 
			ReadArrayLocker& operator=(const ReadArrayLocker&) = delete;
			qui::Ini* ini_;
		};

		// 写数组锁
		class WriteArrayLocker {
		public:
			explicit WriteArrayLocker(qui::Ini* ini, const QString& arrayName, int arraySize = -1) : ini_(ini) {
				ini_->beginWriteArray(arrayName, arraySize);
			}
			~WriteArrayLocker() {
				ini_->endArray();
			}
		private:
			WriteArrayLocker(const WriteArrayLocker&) = delete; 
			WriteArrayLocker& operator=(const WriteArrayLocker&) = delete;
			qui::Ini* ini_;
		};

		/**
		 * @brief 构造函数
		 * @param[in] filePath INI文件路径，为空则创建内存INI
		 * @param[in] encryptData 是否启用数据加密
		 */
		explicit Ini(const QString& filePath = QString(), bool encryptData = false);

		/**
		 * @brief 析构函数
		 */
		~Ini();

		/*
		* @brief 拷贝构造函数
		*/
		Ini(const Ini& other);

		/*
		* @brief 赋值构造函数
		*/
		Ini& operator=(const Ini& other);

		// 删除移动构造
		Ini(Ini&&) = delete;

		// 删除赋值移动构造
		Ini& operator=(const Ini&&) = delete;

		/**
		 * @brief 获取当前INI文件路径
		 * @return INI文件路径
		 */
		QString filePath() const;

		//=====================================================================
		// 组操作
		//=====================================================================

		/**
		 * @brief 开始一个新的分组
		 * @param[in] prefix 分组前缀
		 *
		 * 后续的键值操作将在此分组下进行，可以嵌套调用以创建多级分组。
		 */
		void beginGroup(const QString& prefix);

		/**
		 * @brief 结束当前分组
		 *
		 * 返回到上一级分组上下文。
		 */
		void endGroup();

		/*
		* @brief 结束所有分组
		*/
		void endAllGroup();

		/**
		 * @brief 获取当前分组路径
		 * @return 当前分组路径
		 */
		QString group() const;

		//=====================================================================
		// 数组操作
		//=====================================================================

		/**
		 * @brief 开始读取数组
		 * @param[in] prefix 数组前缀
		 * @return 数组大小
		 *
		 * 用于读取现有数组，需要配合setArrayIndex和endArray使用。
		 */
		int beginReadArray(const QString& prefix);

		/**
		 * @brief 开始写入数组
		 * @param[in] prefix 数组前缀
		 * @param[in] size 数组大小，-1表示自动计算
		 *
		 * 用于写入新数组，需要配合setArrayIndex和endArray使用。
		 */
		void beginWriteArray(const QString& prefix, int size = -1);

		/**
		 * @brief 结束数组操作
		 *
		 * 完成数组读写操作，保存数组大小信息。
		 */
		void endArray();

		/**
		 * @brief 设置当前数组索引
		 * @param[in] i 数组索引
		 *
		 * 在beginReadArray或beginWriteArray之后调用，设置当前操作的数组元素索引。
		 */
		void setArrayIndex(int i);

		/*
		* @brief 遍历数组
		* @param[in] prefix 数组前缀
		* @param[in] func 回调函数(index, key, value)返回false停止遍历
		* @note 需要注意,此接口将会调用beginReadArray和endArray,如果已经调用了begin则会调用失败
		*/
		void traverseArray(const QString& prefix, IniTraverseArrayCb&& func);

		//=====================================================================
		// 键值操作
		//=====================================================================

		/**
		 * @brief 设置键值
		 * @param[in] key 键名
		 * @param[in] value 值
		 */
		void setValue(const QString& key, const Variant& value);

		/**
		 * @brief 获取键值
		 * @param[in] key 键名
		 * @param[in] defaultValue 默认值
		 * @return 键对应的值，若不存在则返回默认值
		 */
		Variant value(const QString& key, const Variant& defaultValue = Variant()) const;

		//=====================================================================
		// 注释操作
		//=====================================================================

		/**
		 * @brief 设置键的注释
		 * @param[in] key 键名
		 * @param[in] comment 注释内容
		 */
		void setComment(const QString& key, const QString& comment);

		/**
		 * @brief 获取键的注释
		 * @param[in] key 键名
		 * @param[in] defaultComment 默认注释
		 * @return 键对应的注释，若不存在则返回默认注释
		 */
		QString comment(const QString& key, const QString& defaultComment = QString()) const;

		//=====================================================================
		// 便利方法
		//=====================================================================

		/**
		 * @brief 设置键值并添加注释
		 * @param[in] key 键名
		 * @param[in] value 值
		 * @param[in] comment 注释内容
		 *
		 * 相当于连续调用setValue和setComment。
		 */
		void newValue(const QString& key, const Variant& value, const QString& comment = QString());

		//=====================================================================
		// 键管理
		//=====================================================================

		/**
		 * @brief 移除键
		 * @param[in] key 键名
		 */
		void remove(const QString& key);

		/**
		 * @brief 重命名键
		 * @param[in] oldKeyPath 旧键路径
		 * @param[in] newKeyName 新键名
		 *
		 * 示例:
		 * rename("config/key0", "key"); //第二个参数代表新名称, 将key0改为key, 不要写成config/key
		 */
		void rename(const QString& oldKeyPath, const QString& newKeyName);

		/**
		 * @brief 检查键是否存在
		 * @param[in] key 键名
		 * @return 存在返回true，否则返回false
		 */
		bool contains(const QString& key) const;

		/*
		* @brief 是否为分组
		* @param[in] key 键名
		* @return 是返回true, 否则返回false
		*/
		bool isGroup(const QString& key) const;

		/*
		* @brief 是否为数组
		* @param[in] key 键名
		* @return 是否返回true, 否则返回false
		*/
		bool isArray(const QString& key) const;

		//=====================================================================
		// 查询方法
		//=====================================================================

		/**
		 * @brief 获取所有键
		 * @return 包含所有键的字符串列表
		 */
		QStringList allKeys() const;

		/**
		 * @brief 获取当前分组的子键
		 * @return 包含当前分组所有子键的字符串列表
		 */
		QStringList childKeys() const;

		/**
		 * @brief 获取当前分组的子组
		 * @return 包含当前分组所有子组的字符串列表
		 */
		QStringList childGroups() const;

		/*
		* @brief 启用键排序
		* @param enable 是否启用
		*/
		void enableKeySort(bool enable = true);

		/*
		* @brief 上下文的数量
		* @return 上下文的数量
		*/
		int ctxCount() const;

	protected:
		/**
		 * @brief 构建完整的组名和键名
		 * @param[in] key 输入键名
		 * @param[in] groupName 输出组名
		 * @param[in] keyName 输出键名
		 */
		void buildGroupAndKeyName(const QString& key, QString& groupName, QString& keyName) const;

		/**
		 * @brief 检查键是否存在的内部方法
		 * @param[in] key 键名
		 * @param[in] flag 检查标志
		 * @return 存在返回true，否则返回false
		 */
		bool contains(const QString& key, int flag) const;

		//=====================================================================
		// 加密相关
		//=====================================================================

		/**
		 * @brief Base64编码
		 * @param[in] data 待编码数据
		 * @param[in] length 数据长度
		 * @return 编码后的字符串
		 */
		std::string base64Encode(const uint8_t* data, size_t length) const;

		/**
		 * @brief Base64解码
		 * @param[in] encoded 待解码字符串
		 * @return 解码后的数据
		 */
		std::vector<uint8_t> base64Decode(const std::string& encoded) const;

		/**
		 * @brief 判断字符是否为Base64字符
		 * @param[in] c 待判断字符
		 * @return 是返回true，否则返回false
		 */
		bool isBase64(uchar c) const;

		/*
		* @brief 创建加密
		*/
		void createCrypt();

		/*
		* @brief 销毁加密
		*/
		void destroyCrypt();

		/*
		* @brief 加密数据
		* @param[in] data 需要加密的数据
		* @return 加密后的数据
		*/
		QString encryptData(const QString& data) const;

		/*
		* @brief 解密数据
		* @param[in] data 需要解密的数据
		* @return 解密后的数据
		*/
		QString decryptData(const QString& data) const;

		/*
		* @brief 获取分组子属性
		* @param[in] group 分组
		* @return 分组子属性
		*/
		QVector<QPair<QString, QString>> childProperties(const QString& group) const;

		// 上下文
		struct Ctx {
			QString group;
			QString arrayPrefix;
			int arrayIndex = -1;
			bool inArray = false;
			bool autoSize = true;
			int maxIndex = -1;
			bool writeArray = false;
		};

		/*
		* @brief 获取上下文
		* @return 上下文
		*/
		Ctx* ctx() const;

		/*
		* @brief 清空上下文
		* @note 如果在子线程中进行操作ini文件了,等待线程退出的时候,需要调用此函数来清理上下文
		* @note 只会清理子线程的上下文,主线程的不会被清理
		*/
		void clearCtx();

		/*
		* @brief 文件上锁
		*/
		void fileLock() const;

		/*
		* @brief 文件解锁
		*/
		void fileUnlock() const;

		/*
		* @brief 快速读取
		* @param[in] group 分组
		* @param[in] key 键(完整的键名)内部以\区分层级, 与QSettings一致
		* @param[in] defaultValue 默认值
		* @return 读取的值
		*/
		QString fastRead(const QString& group, const QString& key, const QString& defaultValue = QString()) const;

		/*
		* @brief 快速写入
		* @param[in] group 分组
		* @param[in] key 键(完整的键名)内部以\区分层级, 与QSettings一致
		* @param[in] value 写入的值
		*/
		void fastWrite(const QString& group, const QString& key, const QString& value) const;

		/*
		* @brief 快速删除
		* @param[in] group 分组
		* @param[in] key 键(完整的键名), 如果为空则删除group, 内部以\区分层级, 与QSettings一致
		*/
		void fastRemove(const QString& group, const QString& key) const;

		/*
		* @brief 快速重命名
		* @param[in] group 分组
		* @param[in] oldKeyPath 旧键路径(完整的键名)内部以\区分层级, 与QSettings一致
		* @param[in] newKeyName 新键名
		*/
		void fastRename(const QString& group, const QString& oldKeyPath, const QString& newKeyName);

	private:
		/*
		* @brief 读取文件数据
		* @param[in] group 分组
		* @param[in] key 键
		* @param[in] defaultValue 默认值
		* @param[in] filePath 文件路径
		* @param[out] result 读取的结果
		* @return 返回读取到的值
		*/
		QString readFileData(const QString& group, const QString& key, const QString& defaultValue, const QString& filePath, bool* result = nullptr) const;

		/*
		* @brief 写入文件数据
		* @param[in] group 分组
		* @param[in] key 键
		* @param[in] value 值
		* @param[in] filePath 文件路径
		* @return 成功返回true, 失败返回false
		*/
		bool writeFileData(const QString& group, const QString& key, const QString& value, const QString& filePath) const;

		/*
		* @brief 移除文件数据
		* @param[in] group 分组
		* @param[in] key 键
		* @param[in] filePath 文件路径
		* @return 成功返回true, 失败返回false
		*/
		bool removeFileData(const QString& group, const QString& key, const QString& filePath) const;

		/*
		* @brief 包含文件数据
		* @param[in] group 分组
		* @param[in] key 键
		* @param[in] filePath 文件路径
		* @return 成功返回true, 失败返回false
		*/
		bool containsFileData(const QString& group, const QString& key, const QString& filePath) const;

	private:
		//=====================================================================
		// 成员变量
		//=====================================================================
		mutable QMap<Qt::HANDLE, Ctx> ctx_map_;        // 线程上下文映射
		mutable QMutex ctx_mutex_;                     // 线程上下文互斥锁
		mutable QRecursiveMutex* recursive_mutex_;     // 递归互斥锁，保证线程安全
		QString ini_file_;                             // INI文件路径
		QString comment_file_;                         // INI注释文件路径
		bool encrypt_data_;                            // 是否加密数据标志
		bool key_sort_;                                // 是否键排序
		ULONG_PTR crypt_prov_ = 0;                     // 加密服务提供者句柄
		ULONG_PTR crypt_hash_ = 0;                     // 哈希对象句柄
		ULONG_PTR crypt_key_ = 0;                      // 加密密钥句柄
	};

	/*
	* @brief ini初始化
	* @param[in] filePath 文件路径
	* @param[in] encryptData 是否加密数据
	* @return Ini*
	* @note 如果需要指定具体参数,必须先调用此接口,否则不会生效
	*/
	QUI_DLL_EXPORT Ini* iniInit(const QString& filePath = QString(), bool encryptData = false);

	/*
	* @brief 获取ini
	* @return Ini*
	* @note 如果没有调用_qui_ini_init,则在%localappdata%/$(ProjectName)/data目录下创建文件config.ini
	*/
	QUI_DLL_EXPORT Ini* iniGet();

	/*
	* @brief 创建全局ini副本
	* @param unique 唯一的字符串,用于区分不同的Ini
	* @return Ini*
	* @note 如果没有调用_qui_ini_init,则在%localappdata%/$(ProjectName)/data目录下创建文件config.ini
	* 这个接口将会根据填入的文件路径(__FILE__),进行复制ini_init所初始化的全局变量,然后可以根据所在文件
	* 进行beginGroup和endGroup,多线程安全.
	*/
	QUI_DLL_EXPORT Ini* iniCreateGlobalCopy(const char* unique);

	/*
	* @brief 销毁全局ini副本
	* @param unique 唯一的字符串,用于区分不同的Ini
	* @note 多线程安全
	*/
	QUI_DLL_EXPORT void iniDestoryGlobalCopy(const char* unique);

	/*
	* @brief 日志
	*/
	class QUI_DLL_EXPORT Log {
	public:
		//日志级别
		enum Level {
			//跟踪级别
			TraceLevel,

			//调试级别
			DebugLevel,

			//信息级别
			InfoLevel,

			//警告级别
			WarnLevel,

			//错误级别
			ErrorLevel,

			//严重级别
			CriticalLevel,

			//关闭级别
			OffLevel
		};

		/*
		* @brief 构造
		* @param[in] logName 日志名
		* @param[in] filePath 文件路径
		* @note 如果filePath为空,则在%localappdata%/$(ProjectName)/log目录下创建文件log_xxxx-xx-xx.log
		*/
		Log(const QString& logName = "log", const QString& filePath = QString());

		/*
		* @brief 析构
		*/
		~Log();

		/*
		* @brief 拷贝构造
		*/
		Log(const Log& other);


		/*
		* @brief 赋值构造
		*/
		Log& operator=(const Log& other);

		/*
		* @brief 日志名
		* @return QString
		*/
		QString logName() const;

		/*
		* @brief 获取文件路径
		* @return QString
		*/
		QString filePath() const;

		/*
		* @brief 跟踪
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void trace(const char* fmt, ...);

		/*
		* @brief 调试
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void debug(const char* fmt, ...);

		/*
		* @brief 信息
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void info(const char* fmt, ...);

		/*
		* @brief 警告
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void warn(const char* fmt, ...);

		/*
		* @brief 错误
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void error(const char* fmt, ...);

		/*
		* @brief 严重
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void critical(const char* fmt, ...);

		/*
		* @brief 跟踪
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qtrace(const QString& s, Args&&... args)
		{
			format(TraceLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 调试
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qdebug(const QString& s, Args&&... args)
		{
			format(DebugLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 信息
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qinfo(const QString& s, Args&&... args)
		{
			format(InfoLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 警告
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qwarn(const QString& s, Args&&... args)
		{
			format(WarnLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 错误
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qerror(const QString& s, Args&&... args)
		{
			format(ErrorLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 严重
		* @param[in] s 字符串或QString::arg中%1,%2格式化
		* @tparam[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void qcritical(const QString& s, Args&&... args)
		{
			format(CriticalLevel, s, std::forward<Args>(args)...);
		}

		/*
		* @brief 刷新
		* @return void
		*/
		void flush();

		/*
		* @brief 在什么级别下自动刷新
		* @param[in] level 级别
		* @return void
		*/
		void flushOn(int level);

		/*
		* @brief 刷新级别
		* @return int
		*/
		int flushLevel() const;

		/*
		* @brief 设置输出级别
		* @param[in] level 级别
		* @return void
		* @note 默认为InfoLevel,大于等于InfoLevel级别才会输出到文件内
		*/
		void setOutputLevel(int level);

		/*
		* @brief 获取输出级别
		* @return int
		*/
		int outputLevel() const;

		/*
		* @brief 设置控制台输出
		* @param[in] on 是否开启
		* @return void
		*/
		void setConsoleOutput(bool on);

		/*
		* @brief 是否控制台输出
		* @return bool
		*/
		bool isConsoleOutput() const;

		/*
		* @brief 设置移除天数
		* @param[in] days 天数,-1永不移除
		* @return void
		*/
		void setRemoveDays(int days);

		/*
		* @brief 获取移除天数
		* @return int
		*/
		int removeDays() const;

	private:
		/*
		* @brief 格式化
		* @param[in] format 格式化(例如qt中的"%1,%2,%3,%4"...)
		* @param[in] args 可变参数
		* @return void
		*/
		template<typename... Args> inline void format(int level, const QString& fmt, Args&&... args)
		{
			QString s(fmt);
			if (sizeof...(args)) {
				QStringList list;
				std::initializer_list<int>{(list.append(QString("%1").arg(args)), 0)...};
				for (int i = 0; i < list.size(); ++i) {
					s.replace(QString::asprintf("%%%d", i + 1), list[i]);
				}
			}
			output(level, s);
		}

		/*
		* @brief 输出
		* @param[in] level 级别
		* @param[in] s 字符串
		* @return void
		*/
		void output(int level, const QString& s);

	private:
		void* m_log = nullptr;
		int m_remove_days = 30;
		int m_output_level = InfoLevel;
		bool m_console_output = false;
		QString m_path;
	};

	/*
	* @brief 日志初始化
	* @param[in] log_name 日志名
	* @param[in] file_path 文件路径
	* @return Log*
	* @note 如果需要指定具体参数,必须先调用此接口,否则不会生效
	*/
	QUI_DLL_EXPORT Log* logInit(const QString& logName = "quilog", const QString& filePath = QString());

	/*
	* @brief 获取日志
	* @return Log*
	* @note 如果没有调用_qui_log_init,则在%localappdata%/$(ProjectName)/log目录下创建文件log_xxxx-xx-xx.log
	*/
	QUI_DLL_EXPORT Log* logGet();

	/*
	* @brief 重写移动事件
	* 比如对某个窗口设置了无边框属性,需要重写鼠标移动事件,调用此类即可实现移动.
	*/
	class QUI_DLL_EXPORT _OverrideMoveEvent : public QObject {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		* @param[in] leftButton 限定鼠标左键
		* @param[in] inContainer 限定在容器内
		*/
		_OverrideMoveEvent(QWidget* parent, bool leftButton = true, bool inContainer = true);

		/*
		* @brief 析构
		*/
		~_OverrideMoveEvent();

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] watched 对象
		* @param[in] event 事件
		* @return bool
		*/
		bool eventFilter(QObject* watched, QEvent* event) override;

	private:
		//最后按下的坐标
		QPoint lastPoint_;

		//鼠标是否按下
		bool pressed_ = false;

		//限定鼠标左键
		bool leftButton_ = true;

		//限定在容器内
		bool inContainer_ = true;

		//移动的控件
		QWidget* widget_ = nullptr;
	};

	/*
	* @brief 重写移动事件
	* @param[in] widget 装饰物
	* @param[in] leftButton 限定鼠标左键
	* @param[in] inContainer 限定在容器内
	* @return void
	*/
	QUI_DLL_EXPORT void overrideMoveEvent(QWidget* widget, bool leftButton = true, bool inContainer = true);

	/*
	* @brief 虚拟键盘,只能给Qt界面发送
	*/
	class QUI_DLL_EXPORT VirtualKeyboard : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		VirtualKeyboard(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~VirtualKeyboard();

		/*
		* @brief 显示(非模态)
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @return int
		*/
		static int doModal();

		/*
		* @brief 做非模态
		* @return void
		*/
		static void doModeless();

	public slots:
		/*
		* @brief 显示(模态)
		* @return int
		*/
		int exec() override;

	private slots:
		/*
		* @brief 按钮键点击
		* @return void
		*/
		void keyButtonClicked();

		/*
		* @brief 字母键点击
		* @return void
		*/
		void keyLetterButtonClicked();

		/*
		* @brief 数字键点击
		* @return void
		*/
		void keyNumberButtonClicked();

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

		/*
		* @brief 初始化样式
		* @return void
		*/
		void initStyleSheet();

		/*
		* @brief 处理焦点
		* @return bool
		*/
		bool processFocus();

	private:
		Widget* widget_ = nullptr;
		QVBoxLayout* verticalLayout = nullptr;
		QHBoxLayout* horizontalLayout = nullptr;
		QPushButton* pushButton_58 = nullptr;
		QPushButton* pushButton_ = nullptr;
		QPushButton* pushButton_2 = nullptr;
		QPushButton* pushButton_3 = nullptr;
		QPushButton* pushButton_4 = nullptr;
		QPushButton* pushButton_6 = nullptr;
		QPushButton* pushButton_8 = nullptr;
		QPushButton* pushButton_5 = nullptr;
		QPushButton* pushButton_7 = nullptr;
		QPushButton* pushButton_10 = nullptr;
		QPushButton* pushButton_9 = nullptr;
		QPushButton* pushButton_55 = nullptr;
		QPushButton* pushButton_56 = nullptr;
		QPushButton* pushButton_backspace = nullptr;
		QHBoxLayout* horizontalLayout_2 = nullptr;
		QPushButton* pushButton_tab = nullptr;
		QPushButton* pushButton_13 = nullptr;
		QPushButton* pushButton_16 = nullptr;
		QPushButton* pushButton_18 = nullptr;
		QPushButton* pushButton_14 = nullptr;
		QPushButton* pushButton_17 = nullptr;
		QPushButton* pushButton_15 = nullptr;
		QPushButton* pushButton_12 = nullptr;
		QPushButton* pushButton_11 = nullptr;
		QPushButton* pushButton_20 = nullptr;
		QPushButton* pushButton_19 = nullptr;
		QPushButton* pushButton_52 = nullptr;
		QPushButton* pushButton_54 = nullptr;
		QPushButton* pushButton_53 = nullptr;
		QHBoxLayout* horizontalLayout_3 = nullptr;
		QPushButton* pushButton_capsLock = nullptr;
		QPushButton* pushButton_25 = nullptr;
		QPushButton* pushButton_27 = nullptr;
		QPushButton* pushButton_29 = nullptr;
		QPushButton* pushButton_24 = nullptr;
		QPushButton* pushButton_22 = nullptr;
		QPushButton* pushButton_26 = nullptr;
		QPushButton* pushButton_28 = nullptr;
		QPushButton* pushButton_23 = nullptr;
		QPushButton* pushButton_21 = nullptr;
		QPushButton* pushButton_40 = nullptr;
		QPushButton* pushButton_41 = nullptr;
		QPushButton* pushButton_enter = nullptr;
		QHBoxLayout* horizontalLayout_4 = nullptr;
		QPushButton* pushButton_shift = nullptr;
		QPushButton* pushButton_35 = nullptr;
		QPushButton* pushButton_34 = nullptr;
		QPushButton* pushButton_31 = nullptr;
		QPushButton* pushButton_33 = nullptr;
		QPushButton* pushButton_36 = nullptr;
		QPushButton* pushButton_32 = nullptr;
		QPushButton* pushButton_30 = nullptr;
		QPushButton* pushButton_37 = nullptr;
		QPushButton* pushButton_38 = nullptr;
		QPushButton* pushButton_39 = nullptr;
		QPushButton* pushButton_up = nullptr;
		QHBoxLayout* horizontalLayout_5 = nullptr;
		QPushButton* pushButton_ctrl = nullptr;
		QPushButton* pushButton_win = nullptr;
		QPushButton* pushButton_alt = nullptr;
		QPushButton* pushButton_space = nullptr;
		QPushButton* pushButton_47 = nullptr;
		QPushButton* pushButton_46 = nullptr;
		QPushButton* pushButton_60 = nullptr;
		QWidget* m_focusWidget = nullptr;
		QVector<QPushButton*> m_letterKeys;
		QVector<QPushButton*> m_numberKeys;
		QMap<QString, Qt::Key> m_symbolKeys;
		QList<QPushButton*> m_btns;
	};

	//支持SQL,需要导入Sql模块
#ifdef QUI_SUPPORT_SQL
#if defined(QT_DEBUG)
#define _qui_sql_print QMessageLogger().debug().noquote()
#else
#define _qui_sql_print QMessageLogger().noDebug()
#endif
	struct SqlQuery {
		/*
		* @brief 构造
		*/
		inline SqlQuery() {}

		/*
		* @brief 构造
		* @param[in] result 结果
		* @param[in] query 查询
		*/
		inline SqlQuery(bool result, const QSqlQuery& query) {
			this->result = result;
			this->query = query;
		}

		/*
		* @brief 析构
		*/
		inline ~SqlQuery() {

		}

		/*
		* @brief 重载bool
		* @return bool
		*/
		inline operator bool() {
			return result;
		}

		/*
		* @brief 重载->
		* @return QSqlQuery*
		*/
		inline QSqlQuery* operator->() {
			return &query;
		}
		bool result = false;
		QSqlQuery query;
	};

	/*
	* @brief Sql
	*/
	class Sql {
	public:
		/*
		* @brief 事务
		*/
		enum Transaction {
			//开始事务
			DatabaseTransactionStart,

			//提交事务
			DatabaseTransactionCommit,

			//回滚事务
			DatabaseTransactionRollback
		};

		/*
		* @brief 构造
		* @param[in] driver 驱动
		*/
		inline Sql(const QString& driver)
			:mutex_(new QMutex)
		{
			databaseNew(driver);
		}

		/*
		* @brief 析构
		*/
		inline ~Sql() {
			for (auto iter = query_.begin(); iter != query_.end(); ++iter) {
				if (iter.value()) {
					delete iter.value();
					iter.value() = nullptr;
				}
			}
			databaseRelease();
			if (mutex_) {
				delete mutex_;
				mutex_ = nullptr;
			}
		}

		/*
		* @brief 拷贝构造
		* @param[in] other 其他
		*/
		inline Sql(const Sql& other)
			:mutex_(new QMutex)
		{
			driverName_ = other.driverName_;
			connectionName_ = QString("sql_%1").arg(global::sqlCount++);
			database_ = QSqlDatabase::cloneDatabase(other.database_, connectionName_);
			auto& db = other.database_;
			databaseOpen(db.databaseName(), db.userName(), db.password(), db.hostName(), db.port());
		}

		/*
		* @brief 赋值构造
		* @param[in] other 其他
		* @return QUISql&
		*/
		inline Sql& operator=(const Sql& other) {
			if (this != &other) {
				mutex_ = new QMutex;
				driverName_ = other.driverName_;
				connectionName_ = QString("sql_%1").arg(global::sqlCount++);
				database_ = QSqlDatabase::cloneDatabase(other.database_, connectionName_);
				auto& db = other.database_;
				databaseOpen(db.databaseName(), db.userName(), db.password(), db.hostName(), db.port());
			}
			return *this;
		}

		/*
		* @brief 获取驱动名
		* @return QString
		*/
		inline QString driverName() const {
			return driverName_;
		}

		/*
		* @brief 获取连接名
		* @return QString
		*/
		inline QString connectionName() const {
			return connectionName_;
		}

		/*
		* @brief 获取最终错误
		* @return QString
		*/
		inline QString lastError() const {
			return lastError_[GetCurrentThreadId()];
		}

		/*
		* @brief 数据库是否有效
		* @return bool
		*/
		inline bool databaseIsValid() const {
			return database_.isValid();
		}

		/*
		* @brief 数据库是否打开
		* @return bool
		*/
		inline bool databaseIsOpen() const {
			return database_.isOpen();
		}

		/*
		* @brief 打开数据库
		* @param[in] databaseName 数据库名称
		* @param[in] userName 用户名
		* @param[in] password 密码
		* @param[in] hostName 主机名
		* @param[in] port 端口
		* @return bool
		*/
		inline bool databaseOpen(const QString& databaseName, const QString& userName = QString(),
			const QString& password = QString(), const QString& hostName = "localhost", int port = 3306) {
			bool result = false;
			do
			{
				if (!database_.isValid()) {
					setLastError("Database driver not loaded.");
					break;
				}

				if (database_.isOpen()) {
					database_.close();
				}

				database_.setHostName(hostName);
				database_.setPort(port);
				database_.setDatabaseName(databaseName);
				database_.setUserName(userName);
				database_.setPassword(password);
				if (database_.connectionName() == "QODBC") {
					auto&& statement = QString("DRIVER={SQL SERVER};SERVER=%1,%2;DATABASE=%3;UID=%4;PWD=%5;")
						.arg(database_.hostName(), QString::number(database_.port()), database_.databaseName(), database_.userName(), database_.password());
					database_.setDatabaseName(statement);
					database_.setUserName(QString());
					database_.setPassword(QString());
					database_.setHostName(QString());
					database_.setPort(0);
				}

				if (!database_.open()) {
					setLastError(database_.lastError().text());
					break;
				}
				result = true;
			} while (false);
			return result;
		}

		/*
		* @brief 关闭数据库
		* @return void
		*/
		inline void databaseClose() {
			if (database_.isOpen()) {
				database_.close();
			}
		}

		/*
		* @brief 开始事务
		* @return bool
		*/
		inline bool transactionStart() {
			bool result = false;
			do
			{
				if (!database_.transaction()) {
					setLastError(database_.lastError().text());
					break;
				}
				result = true;
			} while (false);
			return result;
		}

		/*
		* @brief 提交事务
		* @return bool
		*/
		inline bool transactionCommit() {
			bool result = false;
			do
			{
				if (!database_.commit()) {
					setLastError(database_.lastError().text());
					break;
				}
				result = true;
			} while (false);
			return result;
		}

		/*
		* @brief 回滚事务
		* @return bool
		*/
		inline bool transactionRollback() {
			bool result = false;
			do
			{
				if (!database_.rollback()) {
					setLastError(database_.lastError().text());
					break;
				}
				result = true;
			} while (false);
			return result;
		}

		/*
		* @brief 执行查询
		* @param[in] format 格式化字符串(QString::arg中的%1,%2,%3)
		* @tparam[in] args 可变参数
		* @return SqlQuery
		*/
		template<class... Args>
		inline SqlQuery queryExec(const QString& format, Args&&... args)
		{
			bool result = false;
			auto tid = threadId();
			QString statement;
			if (argsSize(std::forward<Args&&>(args)...) == 0) {
				statement = format;
			}
			else {
				statement = argsFormat(format, std::forward<Args&&>(args)...);
			}

			mutex_->lock();
			if (!query_.contains(tid)) {
				query_.insert(tid, new QSqlQuery(database_));
			}
			result = query_[tid]->exec(statement);
			mutex_->unlock();

			_qui_sql_print << "sql_statement:{\n" << query_[tid]->lastQuery() << "\n};";
			_qui_sql_print << "sql_execute_result:" << result;
			if (!result) {
				setLastError(query_[tid]->lastError().text());
				_qui_sql_print << "sql_execute_failure_cause:" << query_[tid]->lastError().text();
			}
			_qui_sql_print << endl;
			return SqlQuery(result, *query_[tid]);
		}

		/*
		* @brief 准备查询
		* @param[in] format 格式化字符串(QSqlQuery中的?,?,?)
		* @tparam[in] args 可变参数
		* @return SqlQuery
		*/
		template<class... Args>
		inline SqlQuery queryPrepare(const QString& format, Args&&... args) {
			bool result = false;
			auto tid = threadId();
			mutex_->lock();
			if (!query_.contains(tid)) {
				query_.insert(tid, new QSqlQuery(database_));
			}
			result = query_[tid]->prepare(format);
			mutex_->unlock();

			_qui_sql_print << "sql_prepare_result:" << result;
			if (!result) {
				setLastError(query_[tid]->lastError().text());
				_qui_sql_print << "sql_prepare_failure_cause:" << query_[tid]->lastError().text();
				return SqlQuery(result, *query_[tid]);
			}

			auto&& variants = argsData<QVariant>(std::forward<Args&&>(args)...);
			for (auto& x : variants) {
				query_[tid]->addBindValue(x);
			}
			mutex_->lock();
			result = query_[tid]->exec();
			mutex_->unlock();
			_qui_sql_print << "sql_statement:{\n" << query_[tid]->lastQuery() << "\n};";
			_qui_sql_print << "sql_execute_result:" << result;
			if (!result) {
				setLastError(query_[tid]->lastError().text());
				_qui_sql_print << "sql_execute_failure_cause:" << query_[tid]->lastError().text();
			}
			_qui_sql_print << endl;
			return SqlQuery(result, *query_[tid]);
		}

		/*
		* @brief 获取表字段列表
		* @param[in] tableName 表名
		* @param[out] fields 字段
		* @return bool
		* @note 目前仅支持MySQL
		*/
		inline bool getTableFieldList(const QString& tableName, QStringList& fields) {
			auto&& query = queryExec(QString("desc `%1`;").arg(tableName));
			if (!query) {
				setLastError(query->lastError().text());
				return false;
			}

			while (query->next()) {
				auto&& record = query->record();
				fields.append(record.value("Field").toString());
			}
			return true;
		}

		/*
		* @brief 获取找到的行数量
		* @param[out] count 数量
		* @return bool
		* @note 目前仅支持MySQL
		*/
		inline bool getFoundRowsCount(qint64& count) {
			auto&& query = queryExec("SELECT FOUND_ROWS();");
			if (!query) {
				setLastError(query->lastError().text());
				return false;
			}

			if (query->next()) {
				count = query->record().value("FOUND_ROWS()").toULongLong();
			}
			return true;
		}

		/*
		* @brief 获取数据库表列表
		* @param[out] tables 表
		* @return bool
		*/
		inline bool getTableList(QStringList& tables) {
			auto&& query = queryExec("show tables;");
			if (!query) {
				setLastError(query->lastError().text());
				return false;
			}

			while (query->next()) {
				auto&& table = query->record().value(QString("Tables_in_%1").arg(database_.databaseName())).toString();
				tables.append(table);
			}
			return true;
		}

		/*
		* @brief 拷贝表结构
		* @param[in] srcTableName 原始表名
		* @param[in] dstTableName 目标表名
		* @return bool
		*/
		inline bool copyTableStructure(const QString& srcTableName, const QString& dstTableName) {
			return queryExec("create table `%1` like `%2`;", dstTableName, srcTableName);
		}

	protected:
		/*
		* @brief 设置最终错误
		* @param[in] error 错误信息
		* @return void
		*/
		inline void setLastError(const QString& error) {
			mutex_->lock();
			lastError_[GetCurrentThreadId()] = error;
			mutex_->unlock();
		}

		/*
		* @brief 获取线程ID
		* @return quint32
		*/
		inline quint32 threadId() const {
			return GetCurrentThreadId();
		}

		/*
		* @brief 获取可变参数大小
		* @tparam[in] args 可变参数
		* @return size_t
		*/
		template<class... Args>
		inline size_t argsSize(Args&&... args) {
			return sizeof...(args);
		}

		/*
		* @brief 可变参数格式化
		* @param[in] format 格式化字符串
		* @tparam[in] args 可变参数
		* @return QString
		*/
		template<class... Args>
		inline QString argsFormat(const QString& format, Args&&... args) {
			QStringList list;
			std::initializer_list<int>{(list.append(QString("%1").arg(args)), 0)...};
			QString statement(format);
			for (int i = 0; i < list.size(); ++i) {
				statement.replace(QString("%%1").arg(i + 1), list[i]);
			}
			return statement;
		}

		/*
		* @brief 展开可变参数数据列表
		* @tparam[in] args 可变参数
		* @return QList<T>
		*/
		template<class T, class... Args>
		inline QList<T> argsData(Args&&... args) {
			QList<T> datas;
			std::initializer_list<int>{(datas.append(args), 0)...};
			return datas;
		}

		/*
		* @brief 新建数据库
		* @param[in] driver 驱动
		* @return bool
		*/
		inline bool databaseNew(const QString& driver) {
			bool result = false;
			do
			{
				if (!database_.isValid()) {
					driverName_ = driver;
					connectionName_ = QString("sql_%1").arg(global::sqlCount++);
					database_ = QSqlDatabase::addDatabase(driverName_, connectionName_);
					if (!database_.isValid()) {
						setLastError(database_.lastError().text());
						break;
					}
				}
				result = true;
			} while (false);
			return result;
		}

		/*
		* @brief 释放数据库
		* @return void
		*/
		inline void databaseRelease() {
			databaseClose();
			if (database_.isValid()) {
				database_.~QSqlDatabase();
				QSqlDatabase::removeDatabase(connectionName_);
			}
		}

	private:
		QString driverName_;
		QString connectionName_;
		QSqlDatabase database_;
		QMutex* mutex_ = nullptr;
		QMap<quint32, QString> lastError_;
		QMap<quint32, QSqlQuery*> query_;
	};

#endif // QUI_SUPPORT_SQL

	/*
	* @brief 线程池
	*/
	class QUI_DLL_EXPORT ThreadPool {
	public:
		/*
		* @brief 构造
		* @param[in] threads 线程数量
		*/
		inline ThreadPool(size_t threads) : stop_(false) {
			for (size_t i = 0; i < threads; ++i)
				workers_.emplace_back([this]() {
				for (;;) {
					::std::function<void()> task;
					{
						::std::unique_lock<::std::mutex> lock(queue_mutex_);
						condition_.wait(lock, [this]() {
							return stop_ || !tasks_.empty();
							});

						if (stop_ && tasks_.empty()) {
							return;
						}
						task = ::std::move(tasks_.front());
						tasks_.pop();
					}
					task();
				}
			});
		}

		/*
		* @brief 入队,将任务添加到队列当中
		* @tparam f 函数
		* @tparam args 参数
		* @return auto
		*/
		template<class F, class... Args>
#if _HAS_CXX17 //兼容旧版本,大于等于C++17版本,result_of_t被废弃,替换为std::invoke_result_t
		inline auto enqueue(F&& f, Args&&... args) -> ::std::future<typename ::std::invoke_result_t<F, Args...>>
#else
		inline auto enqueue(F&& f, Args&&... args) -> ::std::future<typename ::std::result_of_t<F(Args...)>>
#endif
		{
#if _HAS_CXX17
			using return_type = typename std::invoke_result_t<F, Args...>;
#else
			using return_type = typename std::result_of_t<F(Args...)>;
#endif

			auto task = std::make_shared<std::packaged_task<return_type()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock(queue_mutex_);
				if (stop_) {
					throw std::runtime_error("enqueue on stopped thread pool");
				}

				tasks_.emplace([task]() { (*task)(); });
			}
			condition_.notify_one();
			return res;
		}

		/*
		* @brief 析构
		*/
		inline ~ThreadPool() {
			{
				::std::unique_lock<::std::mutex> lock(queue_mutex_);
				stop_ = true;
			}
			condition_.notify_all();
			for (::std::thread& worker : workers_) {
				worker.join();
			}
		}
	private:
		::std::vector<::std::thread> workers_;
		::std::queue<::std::function<void()>> tasks_;
		::std::mutex queue_mutex_;
		::std::condition_variable condition_;
		bool stop_;
	};

	/*
	* @brief 日期验证器 格式:2025-05-06
	*/
	class QUI_DLL_EXPORT DateValidator : public QValidator {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		*/
		explicit DateValidator(QObject* parent = nullptr);

		/*
		* @brief 析构
		*/
		~DateValidator();

		/*
		* @brief 验证
		* @param[in|out] input 输入
		* @param[in|out] pos 位置
		* @return State
		*/
		State validate(QString& input, int& pos) const override;
	};

	/*
	* @brief 日期时间验证器 格式:2025-05-06 00:00:00
	*/
	class QUI_DLL_EXPORT DateTimeValidator : public QValidator {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		*/
		explicit DateTimeValidator(QObject* parent = nullptr);

		/*
		* @brief 析构
		*/
		~DateTimeValidator();

		/*
		* @brief 验证
		* @param[in|out] input 输入
		* @param[in|out] pos 位置
		* @return State
		*/
		State validate(QString& input, int& pos) const override;
	};

	/*
	* @brief 日历行编辑器
	*/
	class QUI_DLL_EXPORT CalendarLineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		/*
		* @param[in] 构造
		* @param[in] parent 父对象
		*/
		explicit CalendarLineEdit(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~CalendarLineEdit() override;

	protected:
		/*
		* @brief 绘图事件
		* @param[in] event 事件
		* @return void
		*/
		void paintEvent(QPaintEvent* event) override;

	private slots:
		/*
		* @brief 图标点击槽
		* @return void
		*/
		void onIconClicked();

		/*
		* @brief 日期点击槽
		* @param[in] date 日期
		* @return void
		*/
		void onDateClicked(const QDate& date);

	private:
		QCalendarWidget* calendarWidget = nullptr;
		QToolButton* iconButton = nullptr;
		int frameWidth = 0;
	};

	/*
	* @brief 搜索行编辑器
	*/
	class QUI_DLL_EXPORT SearchLineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		/*
		* @param[in] 构造
		* @param[in] parent 父对象
		*/
		explicit SearchLineEdit(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~SearchLineEdit() override;

	protected:
		/*
		* @brief 绘图事件
		* @param[in] event 事件
		* @return void
		*/
		void paintEvent(QPaintEvent* event) override;

	signals:
		/*
		* @brief 图标点击信号
		* @return void
		*/
		void iconClicked();

	private:
		QToolButton* iconButton = nullptr;
		int frameWidth = 0;
	};

	/*
	* @brief 选择字段装饰物
	*/
	class QUI_DLL_EXPORT SelectFieldWidget : public QWidget {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit SelectFieldWidget(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~SelectFieldWidget();

		/*
		* @brief 设置字段配置
		* @param[in] field 字段
		* @param[in] conditions 条件
		* @param[in] lineEdit 行编辑器
		* @return void
		*/
		void setFieldConfig(const QString& field, const QStringList& conditions, QLineEdit* lineEdit);

		/*
		* @brief 设置字段多个条目
		* @param[in] items 条目
		* @return void
		*/
		void setFieldItems(const QStringList& items);

		/*
		* @brief 字段的文本
		* @return QString
		*/
		QString fieldText() const;

		/*
		* @brief 条件的文本
		* @return QString
		*/
		QString conditionText() const;

		/*
		* @brief 值的文本
		* @return QString
		*/
		QString valueText() const;

		/*
		* @brief 设置默认的条件
		* @param[in] conditons 条件
		* @return void
		*/
		void setDefaultConditions(const QStringList& conditions);

		/*
		* @brief 默认的条件
		* @return QStringList
		*/
		QStringList defaultConditions() const;

	private slots:
		/*
		* @brief 字段组合框当前文本改变槽
		* @param[in] text 文本
		* @return void
		*/
		void on_fieldComboBox_currentTextChanged(const QString& text);

	private:
		QComboBox* field_combo_box_ = nullptr;
		QComboBox* condition_combo_box_ = nullptr;
		QLineEdit* value_line_edit_ = nullptr;
		QMap<QString, QPair<QStringList, QLineEdit*>> field_configs_;
		QStringList default_conditions_ = { "等于", "不等于", "大于", "大于等于", "小于", "小于等于", "包含", "不包含" };
	};

	/*
	* @brief 文本加载器
	*/
	class QUI_DLL_EXPORT TextLoader : public QObject {
		Q_OBJECT
	public:
		// 日志块结构，用于存储分块信息
		struct TextBlock {
			QString text;        // 块文本内容
			qint64 position;     // 在文件中的起始位置
			int blockNumber;     // 块号
			int startLineNumber; // 块内起始行号
			int lineCount;       // 块内行数
			bool isFinal;        // 是否为最后一块
			QStringList lines;   // 块内各行内容
			TextBlock() : position(0), blockNumber(0), startLineNumber(0), lineCount(0), isFinal(false) {}
		};

		// 搜索结果结构
		struct SearchResult {
			int blockNumber;     // 所在块号
			int lineNumber;      // 行号
			int positionInLine;  // 行内位置
			QString lineText;    // 行文本
			QString searchText;  // 搜索文本，用于高亮
			int blockLineIndex;  // 在块内的行索引
			SearchResult() : blockNumber(0), lineNumber(0), positionInLine(0), blockLineIndex(0) {}
		};

		/*
		* @brief 构造
		*/
		explicit TextLoader(QObject* parent = nullptr);

		/*
		* @brief 析构
		*/
		~TextLoader();

		/*
		* @brief 加载文件
		* @param[in] fileName 文件名
		* @param[in] chunkSize 块大小
		* @return void
		*/
		void loadFile(const QString& fileName, qint64 chunkSize = 32 * 1024);

		/*
		* @brief 搜索
		* @param[in] text 文本
		* @param[in] caseSensitive 区分大小写
		* @param[in] isRegex 是否为正则表达式
		* @return void
		*/
		void search(const QString& text, bool caseSensitive = false, bool isRegex = false);

		/*
		* @brief 取消搜索
		* @return void
		*/
		void cancelSearch();

	signals:
		/*
		* @brief 块已加载完成
		* @param[in] block 日志块
		* @return void
		*/
		void blockLoaded(const TextBlock& block);

		/*
		* @brief 搜索结果已找到
		* @param[in] result 搜索结果
		* @return void
		*/
		void searchResultFound(const SearchResult& result);

		/*
		* @brief 搜索已结束
		* @param[in] totalMatches 总计匹配多少次
		* @return void
		*/
		void searchFinished(int totalMatches);

		/*
		* @brief 加载已完成
		* @return void
		*/
		void loadingFinished();

		/*
		* @brief 文件大小已确定
		* @param[in] size 大小
		* @return void
		*/
		void fileSizeDetermined(qint64 size);

		/*
		* @brief 进度已更新
		* @param[in] percent 百分比
		* @return void
		*/
		void progressUpdated(int percent);

	public slots:
		/*
		* @brief 停止加载
		* @return void
		*/
		void stopLoading();

	private:
		QString m_fileName;
		qint64 m_chunkSize;
		bool m_stopRequested;
		bool m_searchMode;
		QString m_searchText;
		bool m_caseSensitive;
		bool m_isRegex;
		qint64 m_fileSize;
		::std::thread m_loadThread;
		::std::thread m_searchThread;
		::std::mutex m_loadMutex;
		::std::mutex m_searchMutex;
		::std::atomic<bool> m_destroying;
	};

	/*
	* @brief 纯文本查看器
	*/
	class QUI_DLL_EXPORT PlainTextViewer : public QPlainTextEdit {
		Q_OBJECT
	public:
		// 行号区域类，用于显示行号
		class LineNumberArea : public QWidget {
		public:
			/*
			* @brief 构造
			* @param[in] editor 编辑器
			*/
			LineNumberArea(PlainTextViewer* viewer);

			/*
			* @brief 大小提示
			* @return QSize
			*/
			QSize sizeHint() const override;

		protected:
			/*
			* @brief 绘图事件
			* @param[in] event 事件
			* @return void
			*/
			void paintEvent(QPaintEvent* event) override;

		private:
			PlainTextViewer* m_viewer;
		};

		/*
		* @brief 构造
		* @parma[in] parent 父对象
		*/
		explicit PlainTextViewer(QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~PlainTextViewer();

		/*
		* @brief 加载文件
		* @param[in] fileName 文件名
		* @return void
		*/
		void loadFile(const QString& fileName);

		/*
		* @brief 搜索
		* @param[in] text 搜索的文本
		* @param[in] caseSensitive 是否区分大小写
		* @param[in] isRegex 是否为正则表达式
		* @return void
		*/
		void search(const QString& text, bool caseSensitive = false, bool isRegex = false);

		/*
		* @brief 下个搜索结果
		* @return void
		*/
		void nextSearchResult();

		/*
		* @brief 上个搜索结果
		* @return void
		*/
		void prevSearchResult();

		/*
		* @brief 清除搜索
		* @return void
		*/
		void clearSearch();

		/*
		* @brief 是否正在加载中
		* @return bool
		*/
		bool isLoading() const;

		/*
		* @brief 是否正在搜索中
		* @return bool
		*/
		bool isSearching() const;

		/*
		* @brief 当前行
		* @return int
		*/
		int currentLine() const;

		/*
		* @brief 总计行
		* @return int
		*/
		int totalLines() const;

	protected:
		/*
		* @brief 行号区域宽度
		* @return int
		*/
		int lineNumberAreaWidth() const;

		/*
		* @brief 获取块的全局行号
		* @param[in] blockNumber 行号
		* @return int
		*/
		int getGlobalLineNumberForBlock(int blockNumber) const;

		/*
		* @brief 更新块到全局行映射
		* @return void
		*/
		void updateBlockToGlobalLineMapping();

		/*
		* @brief 行号区域绘图事件
		* @param[in] event 事件
		* @return void
		*/
		void lineNumberAreaPaintEvent(QPaintEvent* event);

	signals:
		/*
		* @brief 加载已启动
		* @return void
		*/
		void loadingStarted();

		/*
		* @brief 加载进度
		* @param[in] percent 进度
		* @return void
		*/
		void loadingProgress(int percent);

		/*
		* @brief 加载已完成
		* @return void
		*/
		void loadingFinished();

		/*
		* @brief 搜索已启动
		* @return void
		*/
		void searchStarted();

		/*
		* @brief 搜索进度
		* @param[in] matchesFound 已经匹配了多次
		* @return void
		*/
		void searchProgress(int matchesFound);

		/*
		* @brief 搜索已完成
		* @param[in] totalMatches 总计匹配了多少次
		* @return void
		*/
		void searchFinished(int totalMatches);

		/*
		* @brief 文件大小已确定
		* @param[in] size 大小
		* @return void
		*/
		void fileSizeDetermined(qint64 size);

	protected:
		/*
		* @brief 调整大小事件
		* @param[in] event 事件
		* @return void
		*/
		void resizeEvent(QResizeEvent* event) override;

	private slots:
		/*
		* @brief 处理已加载的文本块
		* @param[in] block 已加载的文本块数据
		* @return void
		*/
		void handleBlockLoaded(const TextLoader::TextBlock& block);

		/*
		 * @brief 处理搜索结果找到事件
		 * @param[in] result 搜索到的结果信息，包含位置和匹配内容
		 * @return void
		 */
		void handleSearchResultFound(const TextLoader::SearchResult& result);

		/*
		 * @brief 处理搜索完成事件
		 * @param[in] totalMatches 搜索过程中找到的匹配项总数
		 * @return void
		 */
		void handleSearchFinished(int totalMatches);

		/*
		 * @brief 处理文件加载完成事件
		 * @return void
		 */
		void handleLoadingFinished();

		/*
		 * @brief 处理文件大小确定事件
		 * @param[in] size 文件大小（字节）
		 * @return void
		 */
		void handleFileSizeDetermined(qint64 size);

		/*
		 * @brief 处理进度更新事件
		 * @param[in] percent 当前处理进度百分比 (0-100)
		 * @return void
		 */
		void handleProgressUpdated(int percent);

		/*
		 * @brief 更新可视区域内的文本块显示
		 * 当视图滚动或窗口大小改变时调用，重新计算并渲染可见区域的文本块
		 * @return void
		 */
		void updateVisibleBlocks();

		/*
		* @brief 处理滚动条值变化事件
		* @param[in] value 滚动条当前值
		* @return void
		*/
		void onScrollBarValueChanged(int value);

		/*
		 * @brief 限制UI更新频率，避免频繁更新导致性能问题
		 * @return void
		 */
		void throttleUiUpdate();

		/*
		 * @brief 更新行号区域宽度以适应新的块数量
		 * @param[in] newBlockCount 新的块数量
		 * @return void
		 */
		void updateLineNumberAreaWidth(int newBlockCount);

		/*
		 * @brief 高亮当前行
		 * @return void
		 */
		void highlightCurrentLine();

		/*
		 * @brief 更新行号区域显示
		 * @param[in] rect 需要更新的区域
		 * @param[in] dy 垂直滚动偏移量
		 * @return void
		 */
		void updateLineNumberArea(const QRect& rect, int dy);

	private:
		/*
		* @brief 验证块索引是否有效
		* @param[in] index 待验证的块索引
		* @return bool 索引有效返回true，无效返回false
		*/
		bool isValidBlockIndex(int index) const;

		/*
		 * @brief 根据行号查找对应的块
		 * @param[in] lineNumber 全局行号
		 * @return int 块索引，未找到时返回-1
		 */
		int findBlockByLineNumber(int lineNumber) const;

		/*
		 * @brief 获取行在块内的偏移量
		 * @param[in] blockIndex 块索引
		 * @param[in] globalLineNumber 全局行号
		 * @return int 行在块内的偏移量
		 */
		int getLineOffsetInBlock(int blockIndex, int globalLineNumber) const;

		/*
		 * @brief 根据块索引和块内行偏移计算全局行号
		 * @param[in] blockIndex 块索引
		 * @param[in] localLineOffset 块内行偏移
		 * @return int 全局行号
		 */
		int getGlobalLineNumber(int blockIndex, int localLineOffset) const;

		/*
		 * @brief 更新行号到块的映射关系
		 * @return void
		 */
		void updateLineNumberMapping();

		/*
		 * @brief 计算文本中的行数
		 * @param[in] text 待计算的文本
		 * @return int 文本中的行数
		 */
		int countLines(const QString& text) const;

		/*
		 * @brief 定位到搜索结果位置
		 * @param[in] result 搜索结果
		 * @return void
		 */
		void locateSearchResult(const TextLoader::SearchResult& result);

	private:
		TextLoader* m_textLoader;
		//QThread* m_loaderThread;
		QString m_currentFile;
		qint64 m_fileSize;
		bool m_isLoading;
		bool m_isSearching;
		QVector<TextLoader::TextBlock> m_logBlocks;
		QList<TextLoader::SearchResult> m_searchResults;
		int m_currentSearchResultIndex;
		QTimer* m_scrollTimer;
		QTimer* m_uiUpdateTimer;
		QQueue<TextLoader::TextBlock> m_pendingBlocks;
		int m_visibleBlockStart;
		int m_visibleBlockEnd;
		int m_maxVisibleBlocks;
		QString m_visibleText; 
		bool m_needsFullUpdate;
		int m_totalLineCount; 
		QMap<int, int> m_blockLineStartMap; 
		QCache<int, QString> m_blockCache;
		QString m_searchText;
		int m_lastScrollValue;
		bool m_scrollingInProgress;
		QMap<int, QPair<int, int>> m_visibleLineMap;
		LineNumberArea* m_lineNumberArea;
		QMap<int, int> m_blockToGlobalLineMap; 
	};

	/*
	* @brief 日志查看器对话框,用于处理大文件
	*/
	class QUI_DLL_EXPORT LogViewerDialog : public QDialog {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] windowTitle 窗口标题
		* @param[in] filePath 文件路径
		* @param[in] parent 父对象
		* @note 文件编码会自动识别,无需关注
		*/
		LogViewerDialog(const QString& windowTitle, const QString& filePath, QWidget* parent = nullptr);

		/*
		* @brief 构造
		* @param[in] windowTitle 窗口标题
		* @param[in] log 日志
		* @param[in] parent 父对象
		* @note 文件编码会自动识别,无需关注
		*/
		LogViewerDialog(const QString& windowTitle, qui::Log* log, QWidget* parent = nullptr);

		/*
		* @brief 析构
		*/
		~LogViewerDialog();

		/*
		* @brief 显示
		* @return void
		*/
		void show();

	public:
		/*
		* @brief 做模态
		* @param[in] windowTitle 标题
		* @param[in] filePath 文件路径
		* @return int
		*/
		static int doModal(const QString& windowTitle, const QString& filePath);

		/*
		* @brief 做模态
		* @param[in] windowTitle 标题
		* @param[in] log 日志
		* @return int
		*/
		static int doModal(const QString& windowTitle, qui::Log* log);

		/*
		* @brief 做非模态
		* @param[in] windowTitle 标题
		* @param[in] filePath 文件路径
		* @return int
		*/
		static void doModeless(const QString& windowTitle, const QString& filePath);

		/*
		* @brief 做非模态
		* @param[in] windowTitle 标题
		* @param[in] log 日志
		* @return int
		*/
		static void doModeless(const QString& windowTitle, qui::Log* log);

	public slots:
		/*
		* @brief 显示
		* @return void
		*/
		int exec() override;

	private:
		/*
		* @brief 初始化控件
		* @return void
		*/
		void initControl();

		/*
		* @brief 初始化窗体
		* @return void
		*/
		void initForm();

	private:
		Widget* widget_ = nullptr;
		PlainTextViewer* textViewer_ = nullptr;
		QStatusBar* statusBar_ = nullptr;
		QLabel* statusLabel_ = nullptr;
		QLineEdit* searchEdit_ = nullptr;

	private slots:
		void onSearchStarted();
		void onLoadingFinished();
		void onSearchFinished(int totalMatches);
	};

	/*
	* @brief 登录对话框, 会自动记录账号密码
	*/
	class QUI_DLL_EXPORT LoginDialog : public QDialog {
		Q_OBJECT
	public:
		class DeleteButtonDelegate : public QStyledItemDelegate {
		public:
			/*
			* @brief 构造函数
			* @param[in] onDelete 删除回调函数
			* @param[in] parent 父对象
			*/
			explicit DeleteButtonDelegate(::std::function<void(const QString& text)> onDelete, QObject* parent = nullptr);

			/*
			* @brief 绘制视图项
			* @param[in] painter 绘图工具
			* @param[in] option 视图项样式选项
			* @param[in] index 模型索引
			*/
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

			/*
			* @brief 处理编辑事件
			* @param[in] event 事件对象
			* @param[in] model 数据模型
			* @param[in] option 视图项样式选项
			* @param[in] index 模型索引
			* @return 是否处理了事件
			*/
			bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
		private:
			::std::function<void(const QString& text)> on_delete_;
		};

		class CustomLineEdit : public QLineEdit {
		public:
			/*
			* @brief 构造函数
			* @param[in] type 输入框类型
			* @param[in] parent 父对象
			*/
			explicit CustomLineEdit(int type, QWidget* parent = nullptr);
		protected:
			/*
			* @brief 绘制事件
			* @param[in] event 绘制事件对象
			*/
			void paintEvent(QPaintEvent* event) override;
		private:
			QIcon icon_;
			int type_;
		};

		/*
		* @brief 构造函数
		* @param[in] authCb 认证回调函数
		* @param[in] parent 父对象
		*/
		explicit LoginDialog(::std::function<bool(const QString& account, const QString& password, QString& errmsg)>&& authCb, QWidget* parent = nullptr);

		/*
		* @brief 显示
		* @return void
		*/
		void show();

		/*
		* @brief 做非模态
		* @param[in] authCb 认证回调函数
		*/
		static void doModeless(::std::function<bool(const QString& account, const QString& password, QString& errmsg)>&& authCb);

		/*
		* @brief 做模态
		* @param[in] authCb 认证回调函数
		* @return 0失败; 1成功
		*/
		static int doModal(::std::function<bool(const QString& account, const QString& password, QString& errmsg)>&& authCb);

		/*
		* @brief 简单的模态对话框内部已封装完成用户的认证操作,账号输入$setaddr密码输入1.可设置认证服务器地址
		* @param[out] address 返回认证的服务器地址,如果为localhost则代表为离线模式认证
		* @param[out] account 返回认证输入的账号
		* @param[in] timeout 连接服务器超时时间(单位:毫秒),如果为0则以qui::connectServerTimeout返回的超时时间为准
		* @return 0失败; 1成功
		*/
		static int easy(QString* address = nullptr, QString* account = nullptr, int timeout = 0);

		/*
		* @brief 启用失败退出
		* @param[in] enable 是否启用
		* @return void
		*/
		void enableFailExit(bool enable);

		/*
		* @brief 是否失败退出
		* @return bool
		*/
		bool isFailExit() const;

	protected:
		/*
		* @brief 事件过滤器
		* @param[in] obj 目标对象
		* @param[in] event 事件对象
		* @return 是否过滤了事件
		*/
		bool eventFilter(QObject* obj, QEvent* event) override;

	private:
		/*
		* @brief 初始化UI
		*/
		void setupUI();

		/*
		* @brief 应用样式
		*/
		void applyStyle();

		/*
		* @brief 加载用户名列表
		*/
		void loadUsernames();

		/*
		* @brief 保存用户名
		* @param[in] username 要保存的用户名
		*/
		void saveUsername(const QString& username);

		/*
		* @brief 删除用户名
		* @param[in] username 要删除的用户名
		*/
		void deleteUsername(const QString& username);

	private:
		Widget* widget_;            // 装饰物
		QComboBox* username_box_;   // 用户名下拉框
		QLineEdit* password_edit_;  // 密码输入框
		QPushButton* login_button_; // 登录按钮
		QLabel* status_label_;      // 状态提示标签
		::std::function<bool(const QString&, const QString&, QString&)> authcb_; // 认证回调函数
		bool fail_exit_ = false;    // 失败退出

	public slots:
		/*
		* @brief 显示
		* @return void
		*/
		int exec() override;

	private slots:
		/*
		* @brief 处理删除用户事件
		* @param[in] username 要删除的用户名
		*/
		void onDeleteUser(const QString& username);

		/*
		* @brief 处理登录按钮点击事件
		*/
		void onLoginClicked();
	};

	/*
	* @brief 验证码标签
	*/
	class QUI_DLL_EXPORT CaptchaLabel : public QLabel {
		Q_OBJECT
	public:
		/*
		* @brief 构造
		* @param[in] parent 父对象
		*/
		explicit CaptchaLabel(QWidget* parent = nullptr);

		/*
		* @brief 返回验证码文本
		* @return QString
		*/
		QString captchaText() const;

		/*
		* @brief 刷新验证码
		*/
		void refreshCaptcha();

	protected:
		/*
		* @brief 绘图事件
		* @param[in] event 事件
		*/
		void paintEvent(QPaintEvent* event) override;

		/*
		* @brief 鼠标点击事件
		* @param[in] event 事件
		*/
		void mousePressEvent(QMouseEvent* event) override;

		/*
		* @brief 重新调整大小事件
		* @param[in] event 事件
		*/
		void resizeEvent(QResizeEvent* event) override;

	private:
		/*
		* @brief 获取随机颜色
		* @return 随机颜色
		*/
		QColor getRandomColor() const;

		/*
		* @brief 生成随机文本
		* @return 随机文本
		*/
		QString generateRandomText();

		/*
		* @brief 获取缩放系数
		* @return 缩放系数
		*/
		qreal getScaleFactor() const;

	private:
		QString m_captchaText;
	};

	/*
	* @brief APP是否为最新版本
	* @param[in] address 上传的服务器地址,如果为空则以qui::serverHostName返回的地址为准
	* @param[in] port 上传的服务器端口,如果为0则以qui::serverPort返回的端口为准
	* @param[in] timeout 连接服务器超时时间(单位:毫秒),如果为0则以qui::connectServerTimeout返回的超时时间为准
	* @return bool
	*/
	QUI_DLL_EXPORT bool appIsLatestVersion(const QString& address = QString(), int port = 0, int timeout = 0);
}
