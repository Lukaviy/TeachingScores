#ifndef KEYID_H
#define KEYID_H

#include <utility>

namespace ts {
    template<typename T, typename K = unsigned int>
    class KeyId {
        friend T;
    public:
        using underlying_type = K;

        explicit KeyId(K id) noexcept : m_id(id) {}

        explicit operator K() const noexcept {
            return m_id;
        }

        bool operator<(const KeyId& b) const noexcept {
            return m_id < b.m_id;
        }

        bool operator==(const KeyId& b) const noexcept {
            return m_id == b.m_id;
        }

        bool operator!=(const KeyId& b) const noexcept {
            return m_id != b.m_id;
        }

    private:
        K m_id;
    };

    template<typename T, typename K = unsigned int>
    class IdGenerator {
    public:
        using Id = KeyId<T, K>;

        IdGenerator() : m_lastId(K{}) {}
        IdGenerator(Id&& initValue) : m_lastId(std::move(initValue)) {}

        Id newId() noexcept {
            return Id(++m_lastId);
        }

    private:
        K m_lastId;
    };
}

template<typename T, typename K>
struct std::hash<ts::KeyId<T, K>> {
    std::size_t operator()(ts::KeyId<T> const& s) const noexcept {
        return std::hash(static_cast<K>(s));
    }
};


#endif // KEYID_H
