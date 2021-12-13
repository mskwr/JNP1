#ifndef __TREASURE_H
#define __TREASURE_H

#include <concepts>

template<typename T, bool _isTrapped>
requires (std::integral<T>)
class Treasure {
public:
    static constexpr bool isTrapped = _isTrapped;

    constexpr Treasure(T _value) : value(_value) {};

    constexpr T evaluate() const {
        return value;
    }

    constexpr T getLoot() {
        T temp = value;
        value = 0;
        return temp;
    }

private:
    T value;
};

template<typename T>
using SafeTreasure = Treasure<T, false>;

template<typename T>
using TrappedTreasure = Treasure<T, true>;

template<typename T>
concept IsTreasure = requires(T a) {
    Treasure(a);
};

#endif // __TREASURE_H

