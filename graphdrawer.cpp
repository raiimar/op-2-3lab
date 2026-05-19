#include "graphdrawer.h"
#include <QPainterPath>
#include <cmath>

static int map_to_x(int year, const DrawContext& dc) { // //
    double ratio = (double)(year - dc.yearMin) / (dc.yearMax - dc.yearMin);
    return qRound(ratio * (dc.draw_w - 1));
}

static int map_to_y(double value, const DrawContext& dc) {
    double ratio = (value - dc.minVal) / (dc.maxVal - dc.minVal);
    return (dc.draw_h - 1) - qRound(ratio * (dc.draw_h - 1));
}

static void draw_axes(const DrawContext& dc) {
    dc.painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    dc.painter->drawLine(0, dc.draw_h - 1, dc.draw_w - 1, dc.draw_h - 1);
    dc.painter->drawLine(0, 0, 0, dc.draw_h - 1);

    dc.painter->setPen(COLOR_TEXT);
    dc.painter->drawText(dc.draw_w / 2 - 2*SMALL_INDENT, dc.draw_h + BIG_INDENT, "Year");
    dc.painter->drawText(-BIG_INDENT, -2*SMALL_INDENT, "Value");
}

static void draw_grid_and_ticks(const DrawContext& dc) {
    dc.painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    QFontMetrics fm(dc.painter->font());

    for (int i = 0; i <= GRID_STEPS; ++i) {
        int year = dc.yearMin + i * (dc.yearMax - dc.yearMin) / GRID_STEPS;
        int x = map_to_x(year, dc);
        dc.painter->drawLine(x, 0, x, dc.draw_h - 1);
        dc.painter->drawLine(x, dc.draw_h - 1, x, dc.draw_h - 1 + AXIS_TICK_LENGTH);

        QString label = QString::number(year);
        int label_w = fm.horizontalAdvance(label);
        dc.painter->drawText(x - label_w / 2, dc.draw_h + AXIS_TICK_LENGTH + 2*SMALL_INDENT, label);
    }

    for (int i = 0; i <= GRID_STEPS; ++i) {
        double val = dc.minVal + i * (dc.maxVal - dc.minVal) / GRID_STEPS;
        int y = map_to_y(val, dc);
        dc.painter->drawLine(0, y, dc.draw_w - 1, y);
        dc.painter->drawLine(0, y, -AXIS_TICK_LENGTH, y);

        QString label = QString::number(val, 'f', 2);
        int label_w = fm.horizontalAdvance(label);
        dc.painter->drawText(-AXIS_TICK_LENGTH - SMALL_INDENT - label_w, y + fm.ascent() / 2, label);
    }
}

static void draw_data_line(const GraphData& data, const DrawContext& dc) {
    QPainterPath path;
    path.moveTo(map_to_x(data.years[0], dc), map_to_y(data.values[0], dc));
    for (int i = 1; i < data.count; ++i) {
        path.lineTo(map_to_x(data.years[i], dc), map_to_y(data.values[i], dc));
    }
    dc.painter->setPen(QPen(COLOR_LINE, THICK_LINE));
    dc.painter->drawPath(path);

    dc.painter->setBrush(COLOR_LINE);
    dc.painter->setPen(Qt::NoPen);
    for (int i = 0; i < data.count; ++i) {
        dc.painter->drawEllipse(QPoint(map_to_x(data.years[i], dc), map_to_y(data.values[i], dc)), POINT_RADIUS, POINT_RADIUS);
    }
}

static void draw_metric_markers(double actualMin, double actualMax, double medianVal, const DrawContext& dc) {
    QFontMetrics fm(dc.painter->font());

    dc.painter->setPen(QPen(COLOR_MAX, THICK_LINE));
    int yMax = map_to_y(actualMax, dc);
    dc.painter->drawLine(0, yMax, dc.draw_w - 1, yMax);
    QString maxStr = QString("Max: %1").arg(actualMax, 0, 'f', 3);
    dc.painter->drawText(dc.draw_w - fm.horizontalAdvance(maxStr) - SMALL_INDENT, yMax - SMALL_INDENT, maxStr);

    dc.painter->setPen(QPen(COLOR_MIN, THICK_LINE));
    int yMin = map_to_y(actualMin, dc);
    dc.painter->drawLine(0, yMin, dc.draw_w - 1, yMin);
    QString minStr = QString("Min: %1").arg(actualMin, 0, 'f', 3);
    dc.painter->drawText(dc.draw_w - fm.horizontalAdvance(minStr) - SMALL_INDENT, yMin - SMALL_INDENT, minStr);

    dc.painter->setPen(QPen(COLOR_MEDIAN, THICK_LINE));
    int yMed = map_to_y(medianVal, dc);
    dc.painter->drawLine(0, yMed, dc.draw_w - 1, yMed);
    QString medStr = QString("Median: %1").arg(medianVal, 0, 'f', 3);
    dc.painter->drawText(dc.draw_w - fm.horizontalAdvance(medStr) - SMALL_INDENT, yMed - SMALL_INDENT, medStr);
}

QPixmap draw_graph(const GraphData& data) {
    QPixmap pixmap(data.size);
    pixmap.fill(COLOR_BACKGROUND);
    QPainter painter(&pixmap);
    painter.translate(SIDE_PADDING_BIG, SIDE_PADDING_SMALL);

    DrawContext dc;
    dc.painter = &painter;
    dc.draw_w = data.size.width() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;
    dc.draw_h = data.size.height() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;
    dc.yearMin = data.yearMin;
    dc.yearMax = data.yearMax;

    double padding = (data.maxValue - data.minValue) * Y_PADDING_RATIO / 10;

    dc.minVal = data.minValue - padding;
    dc.maxVal = data.maxValue + padding;

    draw_axes(dc);
    draw_grid_and_ticks(dc);
    draw_data_line(data, dc);
    draw_metric_markers(data.minValue, data.maxValue, data.medianValue, dc);

    return pixmap;
}
