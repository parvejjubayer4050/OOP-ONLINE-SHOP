#pragma once

#include <memory>
#include <string>
#include <vector>

#include "FileManager.h"
#include "User.h"

// ---------------------------------------------------------------
// AuthService
//
// Handles signup and login and OWNS all User objects while the
// program runs:
//
//     vector<unique_ptr<User>> users     (owner)
//     User* login(...)                   (non-owning pointer into that vector)
//
// The vector owns the users, so nobody ever has to call delete. The
// pointer returned by login() only borrows a user: do not delete it, and
// do not keep it after the AuthService is destroyed.
//
// The file is touched only through FileManager:
//     - on startup       : FileManager::loadUsers()
//     - on every signup  : FileManager::saveUser()   (appends one line)
//
// A wrong password is a normal result, not an error, so login() returns
// nullptr instead of throwing. Problems that should never happen in
// normal use (empty username, duplicate username, file errors) throw
// ShopException.
//
// First run: if there are no users at all, one Admin account is created
// so that somebody can log in and manage the shop:
//     username "admin", password "admin123"
// ---------------------------------------------------------------
class AuthService {
private:
    std::vector<std::unique_ptr<User>> users;
    FileManager& fileManager;       // not owned; must outlive the AuthService

    // Next free ID in the style "U001", "U002", ...
    std::string generateNextId() const;

public:
    // Loads the saved users (and creates the default admin on the first run).
    // Throws ShopException if users.txt is damaged or cannot be written.
    explicit AuthService(FileManager& fileManager);

    // Creates a new CUSTOMER account and saves it to the file.
    // Returns a pointer to the new user (owned by this AuthService).
    // Throws ShopException if the username is already taken, or the
    // username/password is empty or contains a comma.
    User* signup(const std::string& username, const std::string& password);

    // Returns the user if the username and password match, otherwise nullptr.
    User* login(const std::string& username, const std::string& password);

    // Returns the user with this username, or nullptr if there is none.
    User* findUser(const std::string& username) const;

    int getUserCount() const { return static_cast<int>(users.size()); }
};