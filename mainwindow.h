#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "appcontext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum TableColumns {
    COL_YEAR,
    COL_REGION,
    COL_NATURAL_GROWTH,
    COL_BIRTH_RATE,
    COL_DEATH_RATE,
    COL_DEMOGRAPHIC_WEIGHT,
    COL_URBANIZATION,
    TABLE_COLUMN_COUNT
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void on_buttonSelectFile_clicked();
    void on_buttonLoadData_clicked();
    void on_buttonCalculateAndDraw_clicked();
    void refresh_selectors();
    void fill_table_by_region(const QString& region);


private:
    Ui::MainWindow *ui;
    AppContext context;
};

#endif
