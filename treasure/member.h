#ifndef __MEMBER_H
#define __MEMBER_H

#include <cstddef>
#include <cstdint>
#include <concepts>

#include "treasure.h"

template<std::integral T, bool _isArmed>
class Adventurer {
public:
    using strength_t = uint32_t;

    static constexpr bool isArmed = _isArmed;

    constexpr Adventurer() requires (!isArmed) : strength(0), balance(0) {}

    constexpr Adventurer(T _strength) requires isArmed :
            strength(_strength), balance(0) {}

    constexpr T getStrength() const requires isArmed {
        return strength;
    }

    template<bool isTrapped>
    constexpr void loot(Treasure<T, isTrapped>&& treasure) {
        if (isTrapped) {
            if (strength > 0) {
                balance += treasure.getLoot();
                strength /= 2;
            }
        }
        else {
            balance += treasure.getLoot();
        }
    };

    constexpr T pay() {
        T temp = balance;
        balance = 0;
        return temp;
    }

private:
    T balance;
    strength_t strength;
};

template<typename T>
using Explorer = Adventurer<T, false>;

template<typename T, size_t CompletedExpeditions>
requires (CompletedExpeditions < 25)
class Veteran {
public:
    using strength_t = uint32_t;

    static constexpr bool isArmed = true;

    constexpr Veteran() : strength(fib(CompletedExpeditions)), balance(0) {};

    constexpr T getStrength() const {
        return strength;
    }

    constexpr void loot(Treasure<T, false>&& treasure) {
        balance += treasure.getLoot();
    }

    constexpr void loot(Treasure<T, true>&& treasure) {
        if (strength > 0)
            balance += treasure.getLoot();
    }

    constexpr T pay() {
        T temp = balance;
        balance = 0;
        return temp;
    }

private:
    T balance;
    strength_t strength;

    constexpr T fib(T n) {
        T fib0 = 0, fib1 = 1;

        for (T i = 0; i < n; ++i) {
            fib1 += fib0;
            fib0 = fib1 - fib0;
        }

        return fib0;
    }
};

template<typename T>
concept IsMember = requires(T a) {
    typename T::strength_t;
    {T::isArmed} -> std::convertible_to<bool>;
    {a.pay()} -> std::integral;
    a.loot(SafeTreasure<decltype(a.pay())>(0));
    a.loot(TrappedTreasure<decltype(a.pay())>(0));
    // Checks if isArmed is static.
    { [] () constexpr {return T::isArmed;} () };
};

#endif // __MEMBER_H
