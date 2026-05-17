#include "graphdrawer.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

static int map_to_x(int year, const DrawParams& dp) {
    double ratio = (double)(year - dp.yearMin) / (dp.yearMax - dp.yearMin);
    return qRound(ratio * (dp.draw_w - 1));
}

static int map_to_y(double value, const DrawParams& dp) {
    double ratio = (value - dp.minVal) / (dp.maxVal - dp.minVal);
    return (dp.draw_h - 1) - qRound(ratio * (dp.draw_h - 1));
}

static void draw_axes(QPainter* painter, int draw_w, int draw_h) {
    painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    painter->drawLine(0, draw_h - 1, draw_w - 1, draw_h - 1);
    painter->drawLine(0, 0, 0, draw_h - 1);

    painter->setPen(COLOR_TEXT);
    painter->drawText(draw_w / 2 - INDENT, draw_h + 5 * INDENT, "Year");
    painter->drawText(-5 * INDENT, -INDENT, "Value");
}

static void draw_grid_and_ticks(QPainter* painter, const DrawParams& dp) {
    painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    QFontMetrics fm(painter->font());

    for (int i = 0; i <= GRID_STEPS; ++i) {
        int year = dp.yearMin + i * (dp.yearMax - dp.yearMin) / GRID_STEPS;
        int x = map_to_x(year, dp);
        painter->drawLine(x, 0, x, dp.draw_h - 1);
        painter->drawLine(x, dp.draw_h - 1, x, dp.draw_h - 1 + AXIS_TICK_LENGTH);

        QString label = QString::number(year);
        int label_w = fm.horizontalAdvance(label);
        painter->drawText(x - label_w / 2, dp.draw_h + AXIS_TICK_LENGTH + 4*LABEL_OFFSET, label);
    }

    for (int i = 0; i <= GRID_STEPS; ++i) {
        double val = dp.minVal + i * (dp.maxVal - dp.minVal) / GRID_STEPS;
        int y = map_to_y(val, dp);
        painter->drawLine(0, y, dp.draw_w - 1, y);
        painter->drawLine(0, y, -AXIS_TICK_LENGTH, y);

        QString label = QString::number(val, 'f', 2);
        int label_w = fm.horizontalAdvance(label);
        painter->drawText(-AXIS_TICK_LENGTH - LABEL_OFFSET - label_w, y + fm.ascent() / 2, label);
    }
}

static void draw_data_line(QPainter* painter, const int* years, const double* values, int count, const DrawParams& dp) {
    QPainterPath path;
    path.moveTo(map_to_x(years[0], dp), map_to_y(values[0], dp));
    for (int i = 1; i < count; ++i) {
        path.lineTo(map_to_x(years[i], dp), map_to_y(values[i], dp));
    }
    painter->setPen(QPen(COLOR_LINE, THICK_LINE));
    painter->drawPath(path);

    painter->setBrush(COLOR_LINE);
    painter->setPen(Qt::NoPen);
    for (int i = 0; i < count; ++i) {
        painter->drawEllipse(QPoint(map_to_x(years[i], dp), map_to_y(values[i], dp)), POINT_RADIUS, POINT_RADIUS);
    }
}

static void draw_metric_markers(QPainter* painter, double minVal, double maxVal, double medianVal, const DrawParams& dp) {
    QFontMetrics fm(painter->font());
    painter->setPen(QPen(COLOR_MAX, THICK_LINE));
    int yMax = map_to_y(maxVal, dp);
    painter->drawLine(0, yMax, dp.draw_w - 1, yMax);
    QString maxStr = QString("Max: %1").arg(maxVal, 0, 'f', 3);
    painter->drawText(dp.draw_w - fm.horizontalAdvance(maxStr) - LABEL_OFFSET, yMax - LABEL_OFFSET, maxStr);

    painter->setPen(QPen(COLOR_MIN, THICK_LINE));
    int yMin = map_to_y(minVal, dp);
    painter->drawLine(0, yMin, dp.draw_w - 1, yMin);
    QString minStr = QString("Min: %1").arg(minVal, 0, 'f', 3);
    painter->drawText(dp.draw_w - fm.horizontalAdvance(minStr) - LABEL_OFFSET, yMin - LABEL_OFFSET, minStr);

    painter->setPen(QPen(COLOR_MEDIAN, THICK_LINE));
    int yMed = map_to_y(medianVal, dp);
    painter->drawLine(0, yMed, dp.draw_w - 1, yMed);
    QString medStr = QString("Median: %1").arg(medianVal, 0, 'f', 3);
    painter->drawText(dp.draw_w - fm.horizontalAdvance(medStr) - LABEL_OFFSET, yMed - LABEL_OFFSET, medStr);
}

QPixmap draw_graph(const GraphData& data) {
    QPixmap pixmap(data.size);
    pixmap.fill(COLOR_BACKGROUND);
    QPainter painter(&pixmap);
    int draw_x = SIDE_PADDING_BIG;
    int draw_y = SIDE_PADDING_SMALL;

    DrawParams dp;
    dp.draw_w = data.size.width() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;
    dp.draw_h = data.size.height() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;
    dp.yearMin = data.yearMin;
    dp.yearMax = data.yearMax;
    dp.minVal = data.minValue;
    dp.maxVal = data.maxValue;

    painter.translate(draw_x, draw_y);

    draw_axes(&painter, dp.draw_w, dp.draw_h);
    draw_grid_and_ticks(&painter, dp);
    draw_data_line(&painter, data.years, data.values, data.count, dp);
    draw_metric_markers(&painter, data.minValue, data.maxValue, data.medianValue, dp);

    return pixmap;
}
