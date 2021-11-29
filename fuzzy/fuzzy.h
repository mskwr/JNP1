// Autorzy: Agata Chrzanowska, Michał Skwarek

#ifndef __FUZZY_H
#define __FUZZY_H

#include <compare>
#include <cmath>
#include <ostream>
#include <set>
#include <initializer_list>

using real_t = double;

class TriFuzzyNum {
    public:
        // Konstruktor na podstawie podanych wartości.
        constexpr TriFuzzyNum(real_t a, real_t b, real_t c)
            : l(a), m(b), u(c) {
            fix_order();
        }

        // Konstruktor kopiujący.
        constexpr TriFuzzyNum(const TriFuzzyNum& that) = default;

        // Konstruktor przenoszący.
        constexpr TriFuzzyNum(TriFuzzyNum&& that) = default;

        // Destruktor.
        constexpr ~TriFuzzyNum() = default;

        // Operator kopiujący.
        constexpr TriFuzzyNum& operator=(const TriFuzzyNum& that) = default;

        // Operator przenoszący.
        constexpr TriFuzzyNum& operator=(TriFuzzyNum&& that) = default;

        constexpr real_t lower_value() const {
            return l;
        }

        constexpr real_t modal_value() const {
            return m;
        }

        constexpr real_t upper_value() const {
            return u;
        }

        constexpr TriFuzzyNum& operator+=(const TriFuzzyNum& that) {
            l += that.l;
            m += that.m;
            u += that.u;
            return *this;
        }

        constexpr TriFuzzyNum operator+(const TriFuzzyNum& that) const {
            return TriFuzzyNum(*this) += that;
        }

        constexpr TriFuzzyNum& operator-=(const TriFuzzyNum& that) {
            l -= that.u;
            m -= that.m;
            u -= that.l;
            return *this;
        }

        constexpr TriFuzzyNum operator-(const TriFuzzyNum& that) const {
            return TriFuzzyNum(*this) -= that;
        }

        constexpr TriFuzzyNum& operator*=(const TriFuzzyNum& that) {
            l *= that.l;
            m *= that.m;
            u *= that.u;
            // Mnożenie może zaburzyć kolejność l, m, u.
            fix_order();
            return *this;
        }

        constexpr TriFuzzyNum operator*(const TriFuzzyNum& that) const {
            return TriFuzzyNum(*this) *= that;
        }

        constexpr auto operator<=>(const TriFuzzyNum& that) const {
            // Obliczanie wartości x, y, z według podanych wzorów.
            real_t z1 = get_z();
            real_t y1 = get_y(z1);
            real_t x1 = get_x(z1);
            real_t z2 = that.get_z();
            real_t y2 = that.get_y(z2);
            real_t x2 = that.get_x(z2);

            if (x1 - y1 / 2 < x2 - y2 / 2)
                return -1;
            if (x1 - y1 / 2 > x2 - y2 / 2)
                return 1;
            if (1 - y1 < 1 - y2)
                return -1;
            if (1 - y1 > 1 - y2)
                return 1;
            if (m < that.m)
                return -1;
            if (m > that.m)
                return 1;
            return 0;
        }

        constexpr auto operator==(const TriFuzzyNum& that) const {
            if (l == that.l && m == that.m && u == that.u)
                return 1;
            return 0;
        }

        constexpr auto operator!=(const TriFuzzyNum& that) const {
            if (l != that.l || m != that.m || u != that.u)
                return 1;
            return 0;
        }

        friend std::ostream& operator<<(std::ostream& os,
                                        const TriFuzzyNum& that) {
            os << "(" << that.lower_value() << ", " << that.modal_value()
               << ", " << that.upper_value() << ")";
            return os;
        }

    private:
        real_t l, m, u;

        // Sortuje elementy l, m, u.
        constexpr void fix_order() {
            if (l > m)
                std::swap(l, m);
            if (m > u)
                std::swap(m, u);
            if (l > m)
                std::swap(l, m);
        }

        constexpr real_t get_x(real_t z) const {
            return ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l
                   + sqrt(1 + (m - l) * (m - l)) * u) / z;
        }

        constexpr real_t get_y(real_t z) const {
            return (u - l) / z;
        }

        constexpr real_t get_z() const {
            return (u - l) + sqrt(1 + (u - m) * (u - m))
                   + sqrt(1 + (m - l) * (m - l));
        }
};

class TriFuzzyNumSet {
    public:
        // Konstruktor bezparametrowy.
        TriFuzzyNumSet() = default;

        // Konstruktor na podstawie podanych wartości.
        TriFuzzyNumSet(std::initializer_list<TriFuzzyNum> nums) : set(nums) {}

        // Konstruktor kopiujący.
        TriFuzzyNumSet(const TriFuzzyNumSet& that) = default;

        // Konstruktor przenoszący.
        TriFuzzyNumSet(TriFuzzyNumSet&& that) = default;

        // Destruktor.
        ~TriFuzzyNumSet() = default;

        // Operator kopiujący.
        TriFuzzyNumSet& operator=(const TriFuzzyNumSet& that) = default;

        // Operator przenoszący.
        TriFuzzyNumSet& operator=(TriFuzzyNumSet&& that) = default;

        // Wstawianie w wersji kopiującej.
        void insert(const TriFuzzyNum& that) {
            set.insert(that);
        }

        // Wstawianie w wersji przenoszącej.
        void insert(TriFuzzyNum&& that) {
            set.insert(that);
        }

        void remove(const TriFuzzyNum& that) {
            if (set.contains(that))
                set.erase(that);
        }

        TriFuzzyNum arithmetic_mean() const {
            if (set.empty()) {
                throw std::length_error(
                        "TriFuzzyNumSet::arithmetic_mean - the set is empty.");
            }
            else {
                real_t l_sum = 0, m_sum = 0, u_sum = 0;
                auto set_size = (double)set.size();

                for (const TriFuzzyNum& fnum : set) {
                    l_sum += fnum.lower_value();
                    m_sum += fnum.modal_value();
                    u_sum += fnum.upper_value();
                }

                return {l_sum / set_size, m_sum / set_size, u_sum / set_size};
            }
        }

    private:
        std::multiset<TriFuzzyNum> set;
};

consteval TriFuzzyNum crisp_number(real_t v) {
    return {v, v, v};
}

inline constinit const TriFuzzyNum crisp_zero = crisp_number(0);

#endif
