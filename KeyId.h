#ifndef KEYID_H
#define KEYID_H

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
    class UniqueIdProvider {
    public:
        using Id = KeyId<T, K>;
        static Id generateId() noexcept {
            static K current_id = 0;
            return Id(current_id++);
        }
    };
}

template<typename T, typename K>
struct std::hash<ts::KeyId<T, K>> {
    std::size_t operator()(ts::KeyId<T> const& s) const noexcept {
        return std::hash(static_cast<K>(s));
    }
};


#endif // KEYID_H
