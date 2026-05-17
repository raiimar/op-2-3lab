#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

#include <QPixmap>
#include <QSize>

const int SIDE_PADDING_SMALL = 25;
const int SIDE_PADDING_BIG = 70;

const int POINT_RADIUS = 3;
const int INDENT = 15;
const int TEXT_OFFSET = 6;
const int THIN_LINE = 1;
const int THICK_LINE = 2;
const int GRID_STEPS = 5;          // количество линий сетки
const int AXIS_TICK_LENGTH = 5;    // длина засечек на осях
const int LABEL_OFFSET = 5;        // отступ подписей от осей

const QColor COLOR_BACKGROUND = QColor("#15121d");
const QColor COLOR_FRAME = QColor("#5d3d91");
const QColor COLOR_LINE = QColor("#cfbfff");
const QColor COLOR_TEXT = QColor("#cfbfff");
const QColor COLOR_MAX = QColor("#FF3333");
const QColor COLOR_MEDIAN = QColor("#FFFF00");
const QColor COLOR_MIN = QColor("#3399FF");

struct GraphData {
    const int* years;
    const double* values;
    int count;
    double minValue;
    double maxValue;
    double medianValue;
    int yearMin;
    int yearMax;
    QSize size;
};

QPixmap draw_graph(const GraphData& data);

#endif
