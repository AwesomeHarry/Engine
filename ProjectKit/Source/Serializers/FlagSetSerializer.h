#include <json.hpp>
#include "Util/FlagSet.h"

namespace nlohmann {
    template <typename T>
    struct adl_serializer<FlagSet<T>> {
        static void to_json(json& j, const FlagSet<T>& flagset) {
            j = flagset.to_string();
        }

        static void from_json(const json& j, FlagSet<T>& flagset) {
            if (!j.is_string()) {
                throw json::type_error::create(302, "Type must be string, but is " + std::string(j.type_name()));
            }

            std::string bitstring = j.get<std::string>();

            // Ensure the bitstring length matches the FlagSet size
            if (bitstring.length() != flagset.size()) {
                throw json::other_error::create(500, "Bitstring length does not match FlagSet size");
            }

            flagset.reset(); // Clear all flags

            // Set flags based on the bitstring
            for (size_t i = 0; i < bitstring.length(); ++i) {
                if (bitstring[i] == '1') {
                    flagset.set(static_cast<T>(i));
                }
                else if (bitstring[i] != '0') {
                    throw json::parse_error::create(101, "Invalid character in bitstring");
                }
            }
        }
    };
}
