#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "context_state.h"
#include "entrypoint.h"
#include "iterator.h"
#include "data.h"
#include "graphdrawer.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <algorithm>
#include <cstring>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    doOperation(OPERATION_INITIALIZATION, &context, nullptr);

    connect(ui->buttonSelectFile, &QPushButton::clicked, this, &MainWindow::on_buttonSelectFile_clicked);
    connect(ui->buttonLoadData, &QPushButton::clicked, this, &MainWindow::on_buttonLoadData_clicked);
    connect(ui->buttonCalculateAndDraw, &QPushButton::clicked, this, &MainWindow::on_buttonCalculateAndDraw_clicked);
    connect(ui->comboBoxRegion, &QComboBox::currentTextChanged, this, &MainWindow::fill_table_by_region);

    ui->comboBoxColumn->clear();
    ui->comboBoxColumn->addItem("Year", COLUMN_YEAR);
    ui->comboBoxColumn->addItem("Natural population growth", COLUMN_NATURAL_POPULATION_GROWTH);
    ui->comboBoxColumn->addItem("Birth rate", COLUMN_BIRTH_RATE);
    ui->comboBoxColumn->addItem("Death rate", COLUMN_DEATH_RATE);
    ui->comboBoxColumn->addItem("General demographic weight", COLUMN_GENERAL_DEMOGRAPHIC_WEIGHT);
    ui->comboBoxColumn->addItem("Urbanization", COLUMN_URBANIZATION);

    ui->tableWidgetData->setColumnCount(TABLE_COLUMN_COUNT);
    ui->tableWidgetData->setHorizontalHeaderLabels({"Year", "Region", "Natural Growth", "Birth Rate", "Death Rate", "Demographic Weight", "Urbanization"});
    for (int col = 0; col < TABLE_COLUMN_COUNT; ++col) {
        ui->tableWidgetData->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    };
}

const char* MainWindow::get_status_string(Status status){
    const char* result = "Unknown error.";
    switch (status) {
    case STATUS_OK:
        result = "Success";
        break;
    case ERROR_FILE_OPEN:
        result = "Failed to open file.";
        break;
    case ERROR_FILE_READ:
        result = "Failed to read header.";
        break;
    case ERROR_INVALID_DATA:
        result = "Invalid data in file.";
        break;
    case ERROR_INVALID_PARAMS:
        result = "Invalid parameters provided.";
        break;
    case ERROR_EMPTY_RESULT:
        result = "No data for given region and years.";
        break;
    default:
        break;
    }
    return result;
}

MainWindow::~MainWindow() {
    doOperation(OPERATION_CLEAR_DATA, &context, nullptr);
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (context.plot.filteredData != NULL && context.plot.filteredData->size > 0) {
        update_graph_display();
    }
}

void MainWindow::update_graph_display() {
    GraphData graphData;
    graphData.dataList = context.plot.filteredData;
    graphData.columnIndex = context.plot.columnIndex;
    graphData.minValue = context.metrics.min;
    graphData.maxValue = context.metrics.max;
    graphData.medianValue = context.metrics.median;
    graphData.yearMin = context.plot.yearMin;
    graphData.yearMax = context.plot.yearMax;
    graphData.size = ui->graphLabel->size();

    QPixmap pix = draw_graph(&graphData);
    ui->graphLabel->setPixmap(pix);
}

void MainWindow::on_buttonSelectFile_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open CSV", "D:/vasilina/bau/c", "CSV files (*.csv)");
    if (!fileName.isEmpty()) {
        ui->lineEditFileName->setText(fileName);
    }
}

void MainWindow::on_buttonLoadData_clicked() {
    QString filePath = ui->lineEditFileName->text().trimmed();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first.");
        return;
    }

    AppParams params = {};
    std::strncpy(params.loadData.filePath, filePath.toUtf8().constData(), sizeof(params.loadData.filePath) - 1);
    params.loadData.filePath[sizeof(params.loadData.filePath) - 1] = '\0';

    doOperation(OPERATION_LOAD_DATA, &context, &params);

    int errorRows = context.rows.total - context.rows.valid;
    QString message = QString("Total rows: %1\nValid rows: %2\nErrors: %3\n\n%4")
                          .arg(context.rows.total)
                          .arg(context.rows.valid)
                          .arg(errorRows)
                          .arg(QString::fromUtf8(get_status_string(context.status)));
    QMessageBox::information(this, "Load Result", message);

    refresh_selectors();
}

int MainWindow::collect_calculate_params(AppParams& params, QString& errorMsg) {
    int success = 1;

    QString regionName = ui->comboBoxRegion->currentText().trimmed();
    if (regionName == "ALL REGIONS") {
        errorMsg = "Please select a region.";
        success = 0;
    }

    if (success) {
        int startYear = ui->comboBoxYearFrom->currentData().toInt();
        int endYear = ui->comboBoxYearTo->currentData().toInt();
        if (startYear > endYear) {
            errorMsg = "Start year cannot be greater than end year.";
            success = 0;
        } else {
            params.calculateMetrics.columnIndex = ui->comboBoxColumn->currentData().toInt();
            params.calculateMetrics.startYear = startYear;
            params.calculateMetrics.endYear = endYear;
            strncpy(params.calculateMetrics.region, regionName.toUtf8().constData(), sizeof(params.calculateMetrics.region) - 1);
            params.calculateMetrics.region[sizeof(params.calculateMetrics.region) - 1] = '\0';
        }
    }
    return success;
}

void MainWindow::on_buttonCalculateAndDraw_clicked() {
    AppParams params = {};
    QString errorMsg;

    if (!collect_calculate_params(params, errorMsg)) {
        QMessageBox::warning(this, "Calculation error", errorMsg);
        ui->lineEditMin->clear();
        ui->lineEditMax->clear();
        ui->lineEditMedian->clear();
    } else {
        doOperation(OPERATION_CALCULATE_METRICS, &context, &params);
        if (context.status != STATUS_OK) {
            QMessageBox::warning(this, "Calculation error", QString::fromUtf8(get_status_string(context.status)));
            ui->lineEditMin->clear();
            ui->lineEditMax->clear();
            ui->lineEditMedian->clear();
            return;
        }

        ui->lineEditMin->setText(QString::number(context.metrics.min, 'f', 3));
        ui->lineEditMax->setText(QString::number(context.metrics.max, 'f', 3));
        ui->lineEditMedian->setText(QString::number(context.metrics.median, 'f', 3));
        update_graph_display();
    }
}

void MainWindow::collect_unique_data(QSet<QString>& regions, QSet<int>& years) {
    Iterator it = iterator_create(context.dataList);
    while (iterator_has_next(&it)) {
        DataRow* row = (DataRow*)iterator_get(&it);
        if (row != nullptr) {
            regions.insert(QString::fromUtf8(row->region));
            years.insert(row->year);
        }
        iterator_next(&it);
    }
}

void MainWindow::refresh_selectors() {
    ui->comboBoxRegion->clear();
    ui->comboBoxYearFrom->clear();
    ui->comboBoxYearTo->clear();

    QSet<QString> regions;
    QSet<int> years;
    collect_unique_data(regions, years);

    ui->comboBoxRegion->addItem("ALL REGIONS");
    QList<QString> sortedRegions = regions.values();
    std::sort(sortedRegions.begin(), sortedRegions.end());
    for (const QString& reg : sortedRegions) {
        ui->comboBoxRegion->addItem(reg);
    }

    QList<int> sortedYears = years.values();
    std::sort(sortedYears.begin(), sortedYears.end());
    for (int y : sortedYears) {
        ui->comboBoxYearFrom->addItem(QString::number(y), y);
        ui->comboBoxYearTo->addItem(QString::number(y), y);
    }

    if (!sortedYears.isEmpty()) {
        ui->comboBoxYearFrom->setCurrentIndex(0);
        ui->comboBoxYearTo->setCurrentIndex(sortedYears.size() - 1);
    }

    ui->comboBoxRegion->setCurrentIndex(0);
    fill_table_by_region(ui->comboBoxRegion->currentText());
}

void MainWindow::fill_table_by_region(const QString& region) {
    ui->tableWidgetData->setRowCount(0);

    int row = 0;
    Iterator it = iterator_create(context.dataList);
    while (iterator_has_next(&it)) {
        DataRow* data = (DataRow*)iterator_get(&it);
        if (data != nullptr) {
            QString rowRegion = QString::fromUtf8(data->region);
            if (region == "ALL REGIONS" || rowRegion == region) {
                ui->tableWidgetData->insertRow(row);
                ui->tableWidgetData->setItem(row, COL_YEAR, new QTableWidgetItem(QString::number(data->year)));
                ui->tableWidgetData->setItem(row, COL_REGION, new QTableWidgetItem(rowRegion));
                ui->tableWidgetData->setItem(row, COL_NATURAL_GROWTH, new QTableWidgetItem(QString::number(data->natural_population_growth)));
                ui->tableWidgetData->setItem(row, COL_BIRTH_RATE, new QTableWidgetItem(QString::number(data->birth_rate)));
                ui->tableWidgetData->setItem(row, COL_DEATH_RATE, new QTableWidgetItem(QString::number(data->death_rate)));
                ui->tableWidgetData->setItem(row, COL_DEMOGRAPHIC_WEIGHT, new QTableWidgetItem(QString::number(data->general_demographic_weight)));
                ui->tableWidgetData->setItem(row, COL_URBANIZATION, new QTableWidgetItem(QString::number(data->urbanization)));
                ++row;
            }
        }
        iterator_next(&it);
    }
}
