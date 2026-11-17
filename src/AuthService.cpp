#include "AuthService.h"
#include "Admin.h"
#include "Customer.h"
#include "Exceptions.h"

#include <algorithm>
#include <cctype>
using namespace std;

// ---------------------------------------------------------------
// Constructor: load the saved users
// ---------------------------------------------------------------
AuthService::AuthService(FileManager& fileManager)
    : users(fileManager.loadUsers()),
      fileManager(fileManager) {

    // First run: create the default admin so the shop can be managed.
    if (users.empty()) {
        unique_ptr<User> admin = make_unique<Admin>("U001", "admin", "admin123");
        fileManager.saveUser(*admin);           // save first: if this throws,
        users.push_back(std::move(admin));      // nothing is half-created
    }
}

// ---------------------------------------------------------------
// Private helper
// ---------------------------------------------------------------
string AuthService::generateNextId() const {
    int highest = 0;
    for (const unique_ptr<User>& u : users) {
        const string& id = u->getId();
        // Only look at IDs shaped like "U" + up to 9 digits
        if (id.size() < 2 || id.size() > 10 || id[0] != 'U')
            continue;
        bool allDigits = all_of(id.begin() + 1, id.end(), [](char c) {
            return isdigit(static_cast<unsigned char>(c)) != 0;
        });
        if (!allDigits)
            continue;
        highest = max(highest, stoi(id.substr(1)));
    }

    string number = to_string(highest + 1);
    if (number.size() < 3)
        number = string(3 - number.size(), '0') + number;
    return "U" + number;
}

// ---------------------------------------------------------------
// Signup (the same idea as signupUser() in Assignment 2, with classes)
// ---------------------------------------------------------------
User* AuthService::signup(const string& username, const string& password) {
    if (findUser(username) != nullptr)
        throw ShopException("Username already taken: " + username);

    // The Customer constructor rejects an empty or comma-containing
    // username/password (ShopException).
    unique_ptr<User> user = make_unique<Customer>(generateNextId(), username, password);

    // Save to the file BEFORE adding to memory: if saving fails, the
    // exception leaves the in-memory list unchanged, so memory and file
    // always agree.
    fileManager.saveUser(*user);

    users.push_back(std::move(user));
    return users.back().get();
}

// ---------------------------------------------------------------
// Login (the same idea as loginUser() in Assignment 2)
// ---------------------------------------------------------------
User* AuthService::login(const string& username, const string& password) {
    User* user = findUser(username);
    if (user != nullptr && user->checkPassword(password))
        return user;
    return nullptr;     // unknown username or wrong password (same result on purpose,
                        // so the program does not reveal which usernames exist)
}

User* AuthService::findUser(const string& username) const {
    for (const unique_ptr<User>& u : users) {
        if (u->getUsername() == username)
            return u.get();
    }
    return nullptr;
}