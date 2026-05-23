#ifndef GRAPHDRAWER_H
#define GRAPHDRAWER_H

#include <QPixmap>
#include <QSize>
#include <QPainter>
#include "list.h"

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

struct GraphData {
    List* dataList;
    int columnIndex;
    double minValue;
    double maxValue;
    double medianValue;
    int yearMin;
    int yearMax;
    QSize size;
};

struct DrawContext {
    QPainter* painter;
    int draw_w;
    int draw_h;
    int yearMin;
    int yearMax;
    double minVal;
    double maxVal;
};

QPixmap draw_graph(const GraphData* data);

int map_to_x(int year, const DrawContext* dc);
int map_to_y(double value, const DrawContext* dc);
void draw_axes(const DrawContext* dc);
void draw_grid_and_ticks(const DrawContext* dc);
void draw_path(const GraphData* data, const DrawContext* dc);
void draw_points(const GraphData* data, const DrawContext* dc);
void draw_metric_markers(double actualMin, double actualMax, double medianVal, const DrawContext* dc);

#endif
