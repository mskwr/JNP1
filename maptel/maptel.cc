// Autorzy: Marcin Mordecki, Michał Skwarek.

#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include "maptel.h"

#ifdef NDEBUG
    const bool DEBUG = false;
#else
    const bool DEBUG = true;
#endif

namespace jnp1 {
    namespace {
        using std::unordered_map;
        using std::unordered_set;
        using std::string;
        using ulong = unsigned long;
        using dict = unordered_map<string, string>;

        // Tworzy mapę słowników.
        unordered_map<ulong, dict> &dictionaries() {
            static unordered_map<ulong, dict> dictionaries;
            return dictionaries;
        }

        // Sprawdza, czy podany wskaźnik nie wskazuje na NULL.
        [[maybe_unused]] bool check_invalid_pointer(char const *tel,
                                                    char const *func_name) {
            if (tel == nullptr) {
                if (DEBUG)
                    std::cerr << "maptel: " << func_name
                              << ": address of tel is NULL\n";
                return true;
            }
            return false;
        }

        // Sprawdza, czy napis zawiera niedozwolone znaki.
        [[maybe_unused]] bool check_invalid_chars(char const *tel,
                                                  char const *func_name,
                                                  size_t &length) {
            size_t i;

            for (i = 0; i <= TEL_NUM_MAX_LEN && tel[i] != 0; ++i) {
                if (!isdigit(tel[i])) {
                    if (DEBUG)
                        std::cerr << "maptel: " << func_name
                                  << ": invalid chars\n";
                    return true;
                }
            }
            // Zapisuje w zmiennej length długość numeru lub TEL_NUM_MAX_LEN,
            // jeśli jest równa lub większa tej stałej.
            length = i;
            return false;
        }

        // Sprawdza czy długość numeru jest poprawna i czy kończy się '\0'.
        [[maybe_unused]] bool check_invalid_length(char const *tel,
                                                   char const *func_name,
                                                   size_t length) {
            if (length == 0 || length > TEL_NUM_MAX_LEN || tel[length] != 0) {
                if (DEBUG)
                    std::cerr << "maptel: " << func_name
                              << ": invalid length of tel\n";

                return true;
            }

            return false;
        }

        // Sprawdza poprawność numeru telefonu.
        [[maybe_unused]] bool check_invalid_tel(char const *tel,
                                                char const *func_name) {
            size_t length = 0;
            return check_invalid_pointer(tel, func_name) ||
                   check_invalid_chars(tel, func_name, length) ||
                   check_invalid_length(tel, func_name, length);
        }

        // Sprawdza, czy słownik o zadanym numerze istnieje.
        [[maybe_unused]] bool dict_of_id_exists(ulong id) {
            return dictionaries().find(id) != dictionaries().end();
        }

        // Zwraca numer telefonu jako napis.
        string string_of_tel(char const *tel) {
            return string(tel);
        }

        // Zapisuje w tel_dst numer tel_src_s.
        void update(string &tel_before_change, string &tel_src_s,
                    char *tel_dst, [[maybe_unused]] size_t len) {
            assert(len > tel_src_s.size()); // Miejsce na znak '\0'.

            size_t tel_src_size = tel_src_s.size();

            for (size_t i = 0; i < tel_src_size; ++i)
                tel_dst[i] = tel_src_s[i];

            tel_dst[tel_src_size] = 0;

            if (DEBUG)
                std::cerr << "maptel: maptel_transform: " << tel_before_change
                          << " -> " << tel_dst << "\n";
        }
    }

    ulong maptel_create(void) {
        static ulong maptel_count = 0;

        if (DEBUG)
            std::cerr << "maptel: maptel_create()\n";

        ulong id_new_dict = maptel_count;
        dictionaries()[maptel_count++] = dict();

        if (DEBUG)
            std::cerr << "maptel: maptel_create: new map id = "
                      << id_new_dict << "\n";

        return id_new_dict;
    }

    void maptel_delete(ulong id_of_deleted) {
        if (DEBUG)
            std::cerr << "maptel: maptel_delete(" << id_of_deleted << ")\n";

        assert(dict_of_id_exists(id_of_deleted));

        auto rem = dictionaries().find(id_of_deleted);

        rem->second.clear();
        dictionaries().erase(rem);

        if (DEBUG)
            std::cerr << "maptel: maptel_delete: map "
                      << id_of_deleted << " deleted\n";
    }

    void maptel_insert(ulong id, char const *tel_src, char const *tel_dst) {
        if (DEBUG)
            std::cerr << "maptel: maptel_insert("
                      << id << ", " << tel_src << ", " << tel_dst << ")\n";

        assert(dict_of_id_exists(id));
        assert(!check_invalid_tel(tel_src, "maptel_insert") &&
               !check_invalid_tel(tel_dst, "maptel_insert"));

        string tel_src_s = string_of_tel(tel_src);
        string tel_dst_s = string_of_tel(tel_dst);
        auto dictionary = dictionaries().find(id);
        dictionary->second[tel_src_s] = tel_dst_s;

        if (DEBUG)
            std::cerr << "maptel: maptel_insert: inserted\n";
    }

    void maptel_erase(ulong id, char const *tel_src) {
        if (DEBUG)
            std::cerr << "maptel: maptel_erase("
                      << id << ", " << tel_src << ")\n";

        assert(dict_of_id_exists(id));
        assert(!check_invalid_tel(tel_src, "maptel_erase"));

        string tel_src_s = string_of_tel(tel_src);
        auto dictionary = dictionaries().find(id);
        auto tel_dst_ptr = dictionary->second.find(tel_src_s);

        if (tel_dst_ptr == dictionary->second.end()) {
            if (DEBUG)
                std::cerr << "maptel: maptel_erase: nothing to erase\n";

            return; // Jeśli numer nie był zmieniany, to funkcja nic nie robi.
        }

        dictionary->second.erase(tel_dst_ptr);

        if (DEBUG)
            std::cerr << "maptel: maptel_erase: erased\n";
    }

    void maptel_transform(ulong id, char const *tel_src, char *tel_dst,
                          size_t len) {
        if (DEBUG)
            std::cerr << "maptel: maptel_transform(" << id << ", " << tel_src
                      << ", " << &tel_dst << ", " << len << ")\n";

        assert(dict_of_id_exists(id));
        assert(!check_invalid_tel(tel_src, "maptel_transform") &&
               !check_invalid_pointer(tel_dst, "maptel_transform"));

        string tel_src_s = string_of_tel(tel_src);
        unordered_set<string> visited_numbers; // Zbiór przejrzanych telefonów.
        dict &dict_of_id = dictionaries()[id];
        auto phone_number = dict_of_id.find(tel_src_s);
        string new_tel = (phone_number == dict_of_id.end()) ?
                         tel_src_s : dict_of_id[tel_src_s];
        bool cycle = false;

        while (phone_number != dict_of_id.end() && !cycle) {
            visited_numbers.insert(new_tel);
            phone_number = dict_of_id.find(new_tel);

            if (phone_number != dict_of_id.end()) {
                new_tel = phone_number->second;
                cycle = visited_numbers.find(new_tel) != visited_numbers.end();
            }
        }

        if (cycle) {
            if (DEBUG)
                std::cerr << "maptel: maptel_transform: cycle detected\n";

            update(tel_src_s, tel_src_s, tel_dst, len);
        } else {
            update(tel_src_s, new_tel, tel_dst, len);
        }
    }
}
