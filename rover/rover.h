#ifndef ROVER_H
#define ROVER_H

#include "position.h"
#include "sensor.h"
#include "command.h"
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>

class rover_did_not_landed : public std::exception {
public:
    const char* what() const noexcept override {
        return "rover_did_not_landed";
    }
};

class Rover {
private:
    std::unordered_map<char, std::shared_ptr<command>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
    Position position;
    bool stopped;
    bool landed;

public:
    Rover(std::unordered_map<char, std::shared_ptr<command>> c,
          std::vector<std::unique_ptr<Sensor>> s) :
          commands(std::move(c)), sensors(std::move(s)),
          position(), stopped(false), landed(false) {}

    void land(std::pair<coordinate_t, coordinate_t> coords ,
              Direction direction) {
        // Łazik może lądować nieskończenie wiele razy.
        position.move(coords.first, coords.second);
        position.rotate(direction);
        landed = true;
        stopped = false;
    }

    void execute(const std::string &s) {
        if (!landed)
            throw rover_did_not_landed();

        stopped = false;

        for (char cmd : s) {
            if (!commands.contains(cmd) ||
                !commands[cmd]->execute(position, sensors)) {
                stopped = true;
                return;
            }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const Rover &that) {
        if (!that.landed)
            return os << "unknown";

        os << "(" << that.position.get_x() << ", "
           << that.position.get_y() << ") ";

        switch (that.position.get_direction()) {
            case Direction::NORTH:
                os << "NORTH";
                break;
            case Direction::EAST:
                os << "EAST";
                break;
            case Direction::SOUTH:
                os << "SOUTH";
                break;
            case Direction::WEST:
                os << "WEST";
                break;
        }

        if (that.stopped)
            os << " stopped";

        return os;
    }
};

class RoverBuilder {
private:
    std::unordered_map<char, std::shared_ptr<command>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;

public:
    RoverBuilder() = default;

    RoverBuilder(std::unordered_map<char, std::shared_ptr<command>> c,
                 std::vector<std::unique_ptr<Sensor>> s) :
        commands(std::move(c)), sensors(std::move(s)) {}

    RoverBuilder program_command(char chr, const command &cmd) {
        commands[chr] = std::move(cmd.clone());
        return {std::move(commands), std::move(sensors)};
    }

    RoverBuilder add_sensor(std::unique_ptr<Sensor> sensor) {
        sensors.push_back(std::move(sensor));
        return {std::move(commands), std::move(sensors)};
    }

    Rover build() {
        return {std::move(commands), std::move(sensors)};
    }
};

#endif //ROVER_H
