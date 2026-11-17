#include "FileManager.h"
#include "Exceptions.h"

#include <filesystem>
#include <fstream>
#include <system_error>
using namespace std;
namespace fs = std::filesystem;

// ---------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------
FileManager::FileManager(const string& dataFolder)
    : dataFolder(dataFolder),
      productFile((fs::path(dataFolder) / "products.txt").string()),
      userFile((fs::path(dataFolder) / "users.txt").string()),
      orderFile((fs::path(dataFolder) / "orders.txt").string()) {}

// ---------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------
vector<string> FileManager::readLines(const string& path) const {
    ifstream fin(path);                         // reading: ifstream, as in the lecture
    if (!fin.is_open())                         // a missing/locked file does not throw by itself,
        throw ShopException("Cannot open file for reading: " + path);   // so we check

    vector<string> lines;
    string line;
    while (getline(fin, line))
        lines.push_back(line);

    if (fin.bad())                              // a real read error (not just "end of file")
        throw ShopException("Error while reading file: " + path);

    return lines;                               // fin closes itself when it goes out of scope
}

void FileManager::ensureFolderFor(const string& path) const {
    fs::path folder = fs::path(path).parent_path();
    if (folder.empty())
        return;

    error_code ec;
    fs::create_directories(folder, ec);         // does nothing if it already exists
    if (ec)
        throw ShopException("Cannot create folder " + folder.string() + ": " + ec.message());
}

void FileManager::writeLines(const string& path, const vector<string>& lines) const {
    // 1. Make sure the folder exists ("data/" is not created by ofstream).
    ensureFolderFor(path);

    // 2. Write everything to a temporary file first.
    //    ios::out empties the temp file, which is what we want here
    //    (we are intentionally rewriting the whole thing).
    string tempPath = path + ".tmp";
    ofstream fout(tempPath, ios::out);
    if (!fout.is_open())
        throw ShopException("Cannot open file for writing: " + tempPath);

    for (const string& line : lines)
        fout << line << "\n";
    fout.close();

    if (fout.fail()) {                          // e.g. disk full
        error_code ignore;
        fs::remove(tempPath, ignore);
        throw ShopException("Error while writing file: " + tempPath);
    }

    // 3. Only now replace the real file. If anything above failed,
    //    the old file is still untouched.
    error_code ec;
    fs::rename(tempPath, path, ec);
    if (ec) {
        error_code ignore;
        fs::remove(tempPath, ignore);
        throw ShopException("Cannot save " + path + ": " + ec.message());
    }
}

void FileManager::appendLine(const string& path, const string& line) const {
    ensureFolderFor(path);

    ofstream fout(path, ios::app);              // ios::app: old lines stay
    if (!fout.is_open())
        throw ShopException("Cannot open file for writing: " + path);

    fout << line << "\n";
    fout.close();

    if (fout.fail())
        throw ShopException("Error while writing file: " + path);
}

// ---------------------------------------------------------------
// Products
// ---------------------------------------------------------------
void FileManager::saveProducts(const Inventory& inventory) const {
    vector<string> lines;
    for (const Product& p : inventory.getProducts())
        lines.push_back(p.toLine());

    writeLines(productFile, lines);
}

Inventory FileManager::loadProducts() const {
    // First run: no file yet means "no products yet", not an error.
    error_code ec;
    if (!fs::exists(productFile, ec))
        return Inventory();

    vector<string> lines = readLines(productFile);

    Inventory loaded;           // build a separate inventory: all or nothing
    for (size_t i = 0; i < lines.size(); i++) {
        // Skip blank lines (e.g. an extra empty line at the end of the file)
        if (lines[i].find_first_not_of(" \t\r") == string::npos)
            continue;

        try {
            loaded.addProduct(Product::fromLine(lines[i]));
        }
        catch (const ShopException& e) {
            // Add the file name and line number so the problem is easy to find
            throw ShopException(productFile + " line " + to_string(i + 1) + ": " + e.what());
        }
    }
    return loaded;
}

// ---------------------------------------------------------------
// Users
// ---------------------------------------------------------------
void FileManager::saveUser(const User& user) const {
    appendLine(userFile, user.toLine());
}

vector<unique_ptr<User>> FileManager::loadUsers() const {
    vector<unique_ptr<User>> users;

    // First run: no file yet means "no users yet", not an error.
    error_code ec;
    if (!fs::exists(userFile, ec))
        return users;

    vector<string> lines = readLines(userFile);

    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find_first_not_of(" \t\r") == string::npos)
            continue;                           // skip blank lines

        string where = userFile + " line " + to_string(i + 1) + ": ";

        unique_ptr<User> user;
        try {
            user = User::fromLine(lines[i]);
        }
        catch (const ShopException& e) {
            throw ShopException(where + e.what());
        }

        // IDs and usernames must be unique
        for (const unique_ptr<User>& other : users) {
            if (other->getId() == user->getId())
                throw ShopException(where + "duplicate user ID " + user->getId());
            if (other->getUsername() == user->getUsername())
                throw ShopException(where + "duplicate username " + user->getUsername());
        }

        users.push_back(std::move(user));
    }
    return users;
}

// ---------------------------------------------------------------
// Orders
// ---------------------------------------------------------------
void FileManager::saveOrder(const Order& order) const {
    appendLine(orderFile, order.toLine());
}

vector<Order> FileManager::loadOrders() const {
    vector<Order> orders;

    // First run: no file yet means "no orders yet", not an error.
    error_code ec;
    if (!fs::exists(orderFile, ec))
        return orders;

    vector<string> lines = readLines(orderFile);

    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find_first_not_of(" \t\r") == string::npos)
            continue;                           // skip blank lines

        string where = orderFile + " line " + to_string(i + 1) + ": ";

        try {
            Order order = Order::fromLine(lines[i]);
            for (const Order& other : orders) {
                if (other.getId() == order.getId())
                    throw ShopException("duplicate order ID " + order.getId());
            }
            orders.push_back(order);
        }
        catch (const ShopException& e) {
            throw ShopException(where + e.what());
        }
    }
    return orders;
}