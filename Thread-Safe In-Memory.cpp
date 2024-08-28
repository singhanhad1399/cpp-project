#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include <thread>

// Key-value store class
class KeyValueStore {
public:
    // Constructor
    KeyValueStore(const std::string& filename) : filename_(filename) {
        load();
    }

    // Destructor
    ~KeyValueStore() {
        save();
    }

    // Insert or update key-value pair
    void insert(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        store_[key] = value;
    }

    // Delete key-value pair
    void erase(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        store_.erase(key);
    }

    // Retrieve value by key
    std::string get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = store_.find(key);
        if (it != store_.end()) {
            return it->second;
        } else {
            return "Key not found";
        }
    }

private:
    // Save the current state to a file
    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream file(filename_);
        if (file.is_open()) {
            for (const auto& pair : store_) {
                file << pair.first << "=" << pair.second << "\n";
            }
            file.close();
        } else {
            std::cerr << "Failed to open file for saving\n";
        }
    }

    // Load the state from a file
    void load() {
        std::ifstream file(filename_);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    store_[key] = value;
                }
            }
            file.close();
        } else {
            std::cerr << "Failed to open file for loading\n";
        }
    }

    std::unordered_map<std::string, std::string> store_;
    std::mutex mutex_;
    std::string filename_;
};

// Test the KeyValueStore class
void testKeyValueStore() {
    KeyValueStore kvStore("store.txt");

    // Inserting key-value pairs
    kvStore.insert("name", "Alice");
    kvStore.insert("age", "30");

    // Retrieve values
    std::cout << "Name: " << kvStore.get("name") << std::endl;
    std::cout << "Age: " << kvStore.get("age") << std::endl;

    // Deleting a key-value pair
    kvStore.erase("age");

    // Retrieve value after deletion
    std::cout << "Age after deletion: " << kvStore.get("age") << std::endl;
}

// Main function to run the test
int main() {
    // Run the test in a separate thread
    std::thread t(testKeyValueStore);
    t.join(); // Wait for the test to finish

    return 0;
}
