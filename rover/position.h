#ifndef POSITION_H
#define POSITION_H

using coordinate_t = int;

enum class Direction {NORTH, EAST, SOUTH, WEST};

class Position {
private:
    coordinate_t x;
    coordinate_t y;
    Direction direction;

public:
    coordinate_t get_x() const {
        return x;
    }

    coordinate_t get_y() const {
        return y;
    }

    Direction get_direction() const {
        return direction;
    }

    void move(coordinate_t new_x, coordinate_t new_y) {
        x = new_x;
        y = new_y;
    }

    void rotate(Direction new_direction) {
        direction = new_direction;
    }
};

#endif //POSITION_H
