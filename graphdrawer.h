#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

#include <QPixmap>
#include <QSize>
#include <QPainter>

enum DrawingConstants {
    SIDE_PADDING_SMALL = 25,
    SIDE_PADDING_BIG = 70,
    POINT_RADIUS = 3,
    THIN_LINE = 1,
    THICK_LINE = 2,
    GRID_STEPS = 5,
    AXIS_TICK_LENGTH = 5,
    BIG_INDENT = 50,
    SMALL_INDENT = 5,
    Y_PADDING_RATIO = 1
};

struct Colors {

};

const QColor COLOR_BACKGROUND = QColor("#15121d");
const QColor COLOR_FRAME = QColor("#5d3d91");
const QColor COLOR_LINE = QColor("#cfbfff");
const QColor COLOR_TEXT = QColor("#cfbfff");
const QColor COLOR_MAX = QColor("#FF3333");
const QColor COLOR_MEDIAN = QColor("#FFFF00");
const QColor COLOR_MIN = QColor("#3399FF");

struct GraphData {
    const int* years; //
    const double* values;//
    int count;//
    double minValue;//
    double maxValue;//
    double medianValue;//
    int yearMin;
    int yearMax;
    QSize size;
};

struct DrawContext {
    QPainter* painter;
    int draw_w;
    int draw_h;
    int yearMin; //
    int yearMax;
    double minVal;
    double maxVal;
};

QPixmap draw_graph(const GraphData& data);

#endif
