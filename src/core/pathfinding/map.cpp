#include "map.h"

#include "../common/logger.h"

using oneday::core::Logger;
#include <algorithm>
#include <fstream>
#include <sstream>

namespace oneday::pathfinding {

Map::Map(int width, int height)
    : m_width(width), m_height(height), m_data(width * height, CellType::Walkable) {
    Logger::info("Map created with size " + std::to_string(width) + "x" + std::to_string(height));
}

Map::~Map() {
    Logger::info("Map destroyed");
}

bool Map::isValidPosition(const Point& point) const {
    return point.x >= 0 && point.x < m_width && point.y >= 0 && point.y < m_height;
}

bool Map::isWalkable(const Point& point) const {
    if (!isValidPosition(point)) {
        return false;
    }

    return getCellType(point) == CellType::Walkable;
}

CellType Map::getCellType(const Point& point) const {
    if (!isValidPosition(point)) {
        return CellType::Obstacle;
    }

    return m_data[point.y * m_width + point.x];
}

void Map::setCellType(const Point& point, CellType type) {
    if (!isValidPosition(point)) {
        Logger::warning("Attempted to set cell type at invalid position (" +
                        std::to_string(point.x) + "," + std::to_string(point.y) + ")");
        return;
    }

    m_data[point.y * m_width + point.x] = type;
}

int Map::getWidth() const {
    return m_width;
}

int Map::getHeight() const {
    return m_height;
}

void Map::clear(CellType fillType) {
    std::fill(m_data.begin(), m_data.end(), fillType);
    Logger::info("Map cleared with fill type " + std::to_string(static_cast<int>(fillType)));
}

void Map::resize(int newWidth, int newHeight, CellType fillType) {
    if (newWidth <= 0 || newHeight <= 0) {
        Logger::error("Invalid map dimensions: " + std::to_string(newWidth) + "x" +
                      std::to_string(newHeight));
        return;
    }

    std::vector<CellType> newData(newWidth * newHeight, fillType);

    // ??????
    int copyWidth = std::min(m_width, newWidth);
    int copyHeight = std::min(m_height, newHeight);

    for (int y = 0; y < copyHeight; ++y) {
        for (int x = 0; x < copyWidth; ++x) {
            newData[y * newWidth + x] = m_data[y * m_width + x];
        }
    }

    m_width = newWidth;
    m_height = newHeight;
    m_data = std::move(newData);

    Logger::info("Map resized to " + std::to_string(newWidth) + "x" + std::to_string(newHeight));
}

void Map::setRectangle(const Point& topLeft, const Point& bottomRight, CellType type) {
    Point normalizedTopLeft = {std::min(topLeft.x, bottomRight.x),
                               std::min(topLeft.y, bottomRight.y)};

    Point normalizedBottomRight = {std::max(topLeft.x, bottomRight.x),
                                   std::max(topLeft.y, bottomRight.y)};

    for (int y = normalizedTopLeft.y; y <= normalizedBottomRight.y; ++y) {
        for (int x = normalizedTopLeft.x; x <= normalizedBottomRight.x; ++x) {
            setCellType({x, y}, type);
        }
    }

    Logger::debug("Rectangle set from (" + std::to_string(normalizedTopLeft.x) + "," +
                  std::to_string(normalizedTopLeft.y) + ") to (" +
                  std::to_string(normalizedBottomRight.x) + "," +
                  std::to_string(normalizedBottomRight.y) + ")");
}

void Map::setCircle(const Point& center, int radius, CellType type) {
    for (int y = center.y - radius; y <= center.y + radius; ++y) {
        for (int x = center.x - radius; x <= center.x + radius; ++x) {
            Point point = {x, y};

            if (isValidPosition(point)) {
                int dx = x - center.x;
                int dy = y - center.y;
                double distance = std::sqrt(dx * dx + dy * dy);

                if (distance <= radius) {
                    setCellType(point, type);
                }
            }
        }
    }

    Logger::debug("Circle set at (" + std::to_string(center.x) + "," + std::to_string(center.y) +
                  ") with radius " + std::to_string(radius));
}

void Map::setLine(const Point& start, const Point& end, CellType type) {
    // ??Bresenham????
    int dx = std::abs(end.x - start.x);
    int dy = std::abs(end.y - start.y);
    int x = start.x;
    int y = start.y;

    int x_inc = (end.x > start.x) ? 1 : -1;
    int y_inc = (end.y > start.y) ? 1 : -1;

    int error = dx - dy;

    while (true) {
        setCellType({x, y}, type);

        if (x == end.x && y == end.y) {
            break;
        }

        int error2 = 2 * error;

        if (error2 > -dy) {
            error -= dy;
            x += x_inc;
        }

        if (error2 < dx) {
            error += dx;
            y += y_inc;
        }
    }

    Logger::debug("Line set from (" + std::to_string(start.x) + "," + std::to_string(start.y) +
                  ") to (" + std::to_string(end.x) + "," + std::to_string(end.y) + ")");
}

std::vector<Point> Map::getNeighbors(const Point& point, bool includeDiagonal) const {
    std::vector<Point> neighbors;

    // 4????
    const std::vector<Point> directions4 = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // 8???????????
    const std::vector<Point> directions8 = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    const auto& directions = includeDiagonal ? directions8 : directions4;

    for (const Point& dir : directions) {
        Point neighbor = {point.x + dir.x, point.y + dir.y};

        if (isValidPosition(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

std::vector<Point> Map::getWalkableNeighbors(const Point& point, bool includeDiagonal) const {
    std::vector<Point> neighbors = getNeighbors(point, includeDiagonal);

    // ??????????    neighbors.erase(
        std::remove_if(neighbors.begin(), neighbors.end(),
                      [this](const Point& p) {
        return !isWalkable(p); }),
        neighbors.end()
    );

        return neighbors;
}

bool Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::error("Failed to open map file: " + filename);
        return false;
    }

    std::string line;
    std::vector<std::vector<CellType>> tempData;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;  // ????????        }

            std::vector<CellType> row;
            for (char c : line) {
                switch (c) {
                    case '.':
                    case ' ':
                        row.push_back(CellType::Walkable);
                        break;
                    case '#':
                    case 'X':
                        row.push_back(CellType::Obstacle);
                        break;
                    case 'S':
                        row.push_back(CellType::Start);
                        break;
                    case 'G':
                        row.push_back(CellType::Goal);
                        break;
                    default:
                        row.push_back(CellType::Walkable);
                        break;
                }
            }

            if (!row.empty()) {
                tempData.push_back(row);
            }
        }

        file.close();

        if (tempData.empty()) {
            Logger::error("No valid data found in map file: " + filename);
            return false;
        }

        // ????????    m_height = static_cast<int>(tempData.size());
        m_width = static_cast<int>(tempData[0].size());
        m_data.resize(m_width * m_height);

        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width && x < static_cast<int>(tempData[y].size()); ++x) {
                m_data[y * m_width + x] = tempData[y][x];
            }
        }

        Logger::info("Map loaded from file: " + filename + " (" + std::to_string(m_width) + "x" +
                     std::to_string(m_height) + ")");
        return true;
    }

    bool Map::saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::error("Failed to create map file: " + filename);
            return false;
        }

        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                CellType type = getCellType({x, y});

                switch (type) {
                    case CellType::Walkable:
                        file << '.';
                        break;
                    case CellType::Obstacle:
                        file << '#';
                        break;
                    case CellType::Start:
                        file << 'S';
                        break;
                    case CellType::Goal:
                        file << 'G';
                        break;
                    default:
                        file << '.';
                        break;
                }
            }
            file << '\n';
        }

        file.close();

        Logger::info("Map saved to file: " + filename);
        return true;
    }

    std::string Map::toString() const {
        std::ostringstream oss;

        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                CellType type = getCellType({x, y});

                switch (type) {
                    case CellType::Walkable:
                        oss << '.';
                        break;
                    case CellType::Obstacle:
                        oss << '#';
                        break;
                    case CellType::Start:
                        oss << 'S';
                        break;
                    case CellType::Goal:
                        oss << 'G';
                        break;
                    default:
                        oss << '?';
                        break;
                }
            }
            oss << '\n';
        }

        return oss.str();
    }

    void Map::printToConsole() const {
        Logger::info("Map contents:\n" + toString());
    }

    int Map::countCellsOfType(CellType type) const {
        return static_cast<int>(std::count(m_data.begin(), m_data.end(), type));
    }

    std::vector<Point> Map::findCellsOfType(CellType type) const {
        std::vector<Point> points;

        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                if (getCellType({x, y}) == type) {
                    points.push_back({x, y});
                }
            }
        }

        return points;
    }

    Point Map::findFirstCellOfType(CellType type) const {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                if (getCellType({x, y}) == type) {
                    return {x, y};
                }
            }
        }

        return {-1, -1};  // ????}

        void Map::floodFill(const Point& start, CellType newType, CellType targetType) {
            if (!isValidPosition(start)) {
                return;
            }

            if (targetType == CellType::Any) {
                targetType = getCellType(start);
            }

            if (getCellType(start) != targetType || targetType == newType) {
                return;
            }

            std::vector<Point> stack;
            stack.push_back(start);

            while (!stack.empty()) {
                Point current = stack.back();
                stack.pop_back();

                if (!isValidPosition(current) || getCellType(current) != targetType) {
                    continue;
                }

                setCellType(current, newType);

                // ??4????
                std::vector<Point> neighbors = getNeighbors(current, false);
                for (const Point& neighbor : neighbors) {
                    if (isValidPosition(neighbor) && getCellType(neighbor) == targetType) {
                        stack.push_back(neighbor);
                    }
                }
            }

            Logger::debug("Flood fill completed from (" + std::to_string(start.x) + "," +
                          std::to_string(start.y) + ")");
        }

    }  // namespace oneday::pathfinding
