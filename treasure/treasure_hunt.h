#ifndef __TREASURE_HUNT_H
#define __TREASURE_HUNT_H

#include <utility>

#include "treasure.h"
#include "member.h"

template<typename sideA, typename sideB>
using Encounter = std::pair<sideA&, sideB&>;

template<IsMember A, IsTreasure B>
constexpr void run(const Encounter<A, B>& e) {
    e.first.loot(std::move(e.second));
}

template<IsTreasure A, IsMember B>
constexpr void run(const Encounter<A, B>& e) {
    e.second.loot(std::move(e.first));
}

// Transfers loot from lost adventurer to the winner.
template<IsMember A, IsMember B>
constexpr void transferLoot(A& first, B& second) {
    auto loot = second.pay();
    // Creating a treasure with lost adventurer's loot.
    SafeTreasure<decltype(loot)> treasure(loot);
    first.loot(std::move(treasure));
}

template<IsMember A, IsMember B>
constexpr void runArmedVsArmed(const Encounter<A, B>& e)
requires (A::isArmed && B::isArmed) {
    if (e.first.getStrength() > e.second.getStrength())
        transferLoot(e.first, e.second);
    else if (e.first.getStrength() < e.second.getStrength())
        transferLoot(e.second, e.first);
}

template<IsMember A, IsMember B>
constexpr void runArmedVsArmed(const Encounter<A, B>& e)
requires (!(A::isArmed && B::isArmed)) {}

template<IsMember A, IsMember B>
constexpr void run(const Encounter<A, B>& e) {
    if (!e.first.isArmed && !e.second.isArmed)
        return;
    else if (e.first.isArmed && !e.second.isArmed)
        transferLoot(e.first, e.second);
    else if (!e.first.isArmed && e.second.isArmed)
        transferLoot(e.second, e.first);
    else
        runArmedVsArmed(e);
}

template<typename IsEncounter>
constexpr void expedition(IsEncounter e) {
    run(e);
}

template<typename IsEncounter, typename... Args>
constexpr void expedition(IsEncounter e, Args... args) {
    run(e);
    expedition(args...);
}

template<typename T>
concept EncounterSide = IsTreasure<T> || IsMember<T>;

#endif // __TREASURE_HUNT_H

