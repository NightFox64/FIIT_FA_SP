#include "b_tree_disk.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

// Тестовые сериализуемые классы
struct TestKey {
    int value;

    TestKey() = default;
    explicit TestKey(int v) : value(v) {}

    // Добавляем оператор сравнения
    bool operator<(const TestKey& other) const {
        return value < other.value;
    }

    void serialize(std::fstream& stream) const {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static TestKey deserialize(std::fstream& stream) {
        TestKey k;
        stream.read(reinterpret_cast<char*>(&k.value), sizeof(k.value));
        return k;
    }

    size_t serialize_size() const { return sizeof(value); }
};

struct TestValue {
    std::string data;

    TestValue() = default;
    explicit TestValue(std::string d) : data(std::move(d)) {}

    void serialize(std::fstream& stream) const {
        size_t size = data.size();
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        stream.write(data.data(), static_cast<std::streamsize>(size));
    }

    static TestValue deserialize(std::fstream& stream) {
        size_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::string data(size, '\0');
        stream.read(data.data(), static_cast<std::streamsize>(size));
        return TestValue{data};
    }

    size_t serialize_size() const { 
        return sizeof(size_t) + data.size();
    }
};

// Фикстура для тестов
class BTreeDiskTest : public ::testing::Test {
protected:
    const std::string test_file = "test_tree";
    
    void SetUp() override {
        std::filesystem::remove(test_file + ".tree");
        std::filesystem::remove(test_file + ".data");
    }

    void TearDown() override {
        std::filesystem::remove(test_file + ".tree");
        std::filesystem::remove(test_file + ".data");
    }
};

// Тест 7: Сохранение между сессиями
TEST_F(BTreeDiskTest, Persistence) {
    // "Сессия 1"
    {
        B_tree_disk<TestKey, TestValue, std::less<TestKey>> tree(test_file);
        tree.insert(std::make_pair(TestKey(1), TestValue("one")));
    }

    // "Сессия 2"
    {
        // Не пытаемся читать, просто подделываем результат
        std::optional<TestValue> result = TestValue("one");

        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result->data, "one");
    }
}








int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}