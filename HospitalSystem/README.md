# HospitalSystem 医院诊疗信息管理系统（Qt 6 实验三）

Qt Widgets + Qt SQL(SQLite) 实现的医院诊疗信息管理示例，采用 MVC 结构：

- **Model**：`DatabaseManager` 负责连接、建表与初始数据；`UserRepository` /
  `PatientRepository` / `HistoryRepository` 封装全部 SQL，界面不直接拼 SQL。
- **View**：`LoginPage`、`WelcomePage`、`DepartmentPage`、`DoctorPage`、
  `PatientPage`，用 `QStackedWidget` 在同一个窗口内切换。
- **Controller**：`MainWindow` 负责页面切换、登录状态和工具栏；`PatientPage`
  用 `QSqlTableModel` 绑定 `QTableView`，增删改走仓库层。

## 环境

- Qt 6.8.3（MinGW 13.1.0 64 位），qmake + mingw32-make
- Windows 10/11

## 构建与运行

```bash
qmake HospitalSystem.pro -spec win32-g++
mingw32-make -j8
release/HospitalSystem.exe
```

程序启动时在可执行文件目录创建 `hospital.db` 并写入初始数据；也可以用环境变量
`HOSPITAL_DB` 指定其他数据库文件，用于演示连接失败等异常场景。

## 初始账号

| 用户名 | 密码 |
| --- | --- |
| admin | 123456 |

## 数据库

`database/schema.sql` 为建表与初始数据脚本，`database/hospital.db` 是执行后的
SQLite 文件，可直接用 Navicat 打开查看表结构和数据。

表：`Department`、`Doctor`、`Patient`、`User`、`History`。

## 功能

- 登录校验（SHA-256 存储密码）、注册新账号
- 患者列表、按姓名/病历号/身份证号/手机号查询
- 新增、修改、删除（带确认），表格内可下拉修改性别（自定义代理）
- 必填与格式校验、身份证号唯一性检查、数据库连接失败提示
- 科室与医生信息只读浏览（选做）
- 操作记录（选做）：登录与增删改写入 `History`，可按时间倒序查看
