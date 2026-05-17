#include "graphdrawer.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

// ---------- вспомогательные функции отображения координат ----------
static int map_to_x(int year, int yearMin, int yearMax, int width) {
    double ratio = (yearMax > yearMin) ? (double)(year - yearMin) / (yearMax - yearMin) : 0.0;
    return qRound(ratio * (width - 1));
}

static int map_to_y(double value, double minVal, double maxVal, int height) {
    double range = maxVal - minVal;
    double ratio = (range > 0.0) ? (value - minVal) / range : 0.0;
    return (height - 1) - qRound(ratio * (height - 1));
}

// ---------- отрисовка пустой области (нет данных) ----------
static void draw_empty_graph(QPainter* painter, const QSize& size) {
    painter->fillRect(0, 0, size.width(), size.height(), COLOR_BACKGROUND);
    painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    painter->drawRect(0, 0, size.width() - 1, size.height() - 1);
}

// ---------- отрисовка осей и подписей ----------
static void draw_axes(QPainter* painter, int draw_w, int draw_h, int total_w, int total_h) {
    painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    painter->drawLine(0, draw_h - 1, draw_w - 1, draw_h - 1);  // ось X
    painter->drawLine(0, 0, 0, draw_h - 1);                    // ось Y

    painter->setPen(COLOR_TEXT);
    painter->drawText(draw_w / 2 + INDENT, draw_h + 2 * INDENT, "Year");
    painter->drawText(INDENT, SIDE_PADDING_SMALL - INDENT, "Value");
}

// ---------- отрисовка сетки и числовых меток на осях ----------
static void draw_grid_and_ticks(QPainter* painter, int draw_w, int draw_h,
                                int yearMin, int yearMax,
                                double minVal, double maxVal) {
    painter->setPen(QPen(COLOR_FRAME, THIN_LINE));
    QFontMetrics fm(painter->font());

    // Вертикальные линии (по годам)
    for (int i = 0; i <= GRID_STEPS; ++i) {
        int year = yearMin + i * (yearMax - yearMin) / GRID_STEPS;
        int x = map_to_x(year, yearMin, yearMax, draw_w);
        painter->drawLine(x, 0, x, draw_h - 1);                      // линия сетки
        painter->drawLine(x, draw_h - 1, x, draw_h - 1 + AXIS_TICK_LENGTH); // засечка

        QString label = QString::number(year);
        int label_w = fm.horizontalAdvance(label);
        painter->drawText(x - label_w / 2, draw_h + AXIS_TICK_LENGTH + LABEL_OFFSET, label);
    }

    // Горизонтальные линии (по значениям)
    for (int i = 0; i <= GRID_STEPS; ++i) {
        double val = minVal + i * (maxVal - minVal) / GRID_STEPS;
        int y = map_to_y(val, minVal, maxVal, draw_h);
        painter->drawLine(0, y, draw_w - 1, y);                      // линия сетки
        painter->drawLine(0, y, -AXIS_TICK_LENGTH, y);               // засечка

        QString label = QString::number(val, 'f', 2);
        int label_w = fm.horizontalAdvance(label);
        painter->drawText(-AXIS_TICK_LENGTH - LABEL_OFFSET - label_w, y + fm.ascent() / 2, label);
    }
}

// ---------- отрисовка линии графика и точек ----------
static void draw_data_line(QPainter* painter, const int* years, const double* values, int count,
                           int yearMin, int yearMax, double minVal, double maxVal,
                           int draw_w, int draw_h) {
    if (count < 1) return;

    QPainterPath path;
    int firstX = map_to_x(years[0], yearMin, yearMax, draw_w);
    int firstY = map_to_y(values[0], minVal, maxVal, draw_h);
    path.moveTo(firstX, firstY);

    for (int i = 1; i < count; ++i) {
        int x = map_to_x(years[i], yearMin, yearMax, draw_w);
        int y = map_to_y(values[i], minVal, maxVal, draw_h);
        path.lineTo(x, y);
    }

    painter->setPen(QPen(COLOR_LINE, THICK_LINE));
    painter->drawPath(path);

    // Рисуем точки
    painter->setBrush(COLOR_LINE);
    painter->setPen(Qt::NoPen);
    for (int i = 0; i < count; ++i) {
        int x = map_to_x(years[i], yearMin, yearMax, draw_w);
        int y = map_to_y(values[i], minVal, maxVal, draw_h);
        painter->drawEllipse(QPoint(x, y), POINT_RADIUS, POINT_RADIUS);
    }
}

// ---------- отрисовка горизонтальных линий для min / max / median ----------
static void draw_metric_markers(QPainter* painter, double minVal, double maxVal, double medianVal,
                                int yearMin, int yearMax, double dataMin, double dataMax,
                                int draw_w, int draw_h) {
    QFontMetrics fm(painter->font());
    painter->setPen(QPen(COLOR_MAX, THICK_LINE));
    int yMax = map_to_y(maxVal, dataMin, dataMax, draw_h);
    painter->drawLine(0, yMax, draw_w - 1, yMax);
    QString maxStr = QString("Max: %1").arg(maxVal, 0, 'f', 3);
    painter->drawText(draw_w - fm.horizontalAdvance(maxStr) - LABEL_OFFSET,
                      yMax - LABEL_OFFSET, maxStr);

    painter->setPen(QPen(COLOR_MIN, THICK_LINE));
    int yMin = map_to_y(minVal, dataMin, dataMax, draw_h);
    painter->drawLine(0, yMin, draw_w - 1, yMin);
    QString minStr = QString("Min: %1").arg(minVal, 0, 'f', 3);
    painter->drawText(draw_w - fm.horizontalAdvance(minStr) - LABEL_OFFSET,
                      yMin - LABEL_OFFSET, minStr);

    painter->setPen(QPen(COLOR_MEDIAN, THICK_LINE));
    int yMed = map_to_y(medianVal, dataMin, dataMax, draw_h);
    painter->drawLine(0, yMed, draw_w - 1, yMed);
    QString medStr = QString("Median: %1").arg(medianVal, 0, 'f', 3);
    painter->drawText(draw_w - fm.horizontalAdvance(medStr) - LABEL_OFFSET,
                      yMed - LABEL_OFFSET, medStr);
}

// ---------- главная функция отрисовки (один return) ----------
QPixmap draw_graph(const GraphData& data) {
    QPixmap pixmap(data.size);
    pixmap.fill(COLOR_BACKGROUND);
    QPainter painter(&pixmap);

    if (data.count == 0) {
        draw_empty_graph(&painter, data.size);
    } else {
        int draw_x = SIDE_PADDING_BIG;
        int draw_y = SIDE_PADDING_SMALL;
        int draw_w = data.size.width() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;
        int draw_h = data.size.height() - SIDE_PADDING_BIG - SIDE_PADDING_SMALL;

        painter.save();
        painter.translate(draw_x, draw_y);

        draw_axes(&painter, draw_w, draw_h, data.size.width(), data.size.height());
        draw_grid_and_ticks(&painter, draw_w, draw_h,
                            data.yearMin, data.yearMax,
                            data.minValue, data.maxValue);
        draw_data_line(&painter, data.years, data.values, data.count,
                       data.yearMin, data.yearMax, data.minValue, data.maxValue,
                       draw_w, draw_h);
        draw_metric_markers(&painter,
                            data.minValue, data.maxValue, data.medianValue,
                            data.yearMin, data.yearMax, data.minValue, data.maxValue,
                            draw_w, draw_h);

        painter.restore();
    }

    return pixmap;
}
