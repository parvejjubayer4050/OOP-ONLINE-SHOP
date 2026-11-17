#pragma once

#include <memory>
#include <string>

// ---------------------------------------------------------------
// User  (abstract base class)
//
// Holds what every user has in common: id, username and password.
// It cannot be created directly. The two roles, Customer and Admin,
// inherit from it and provide their own menu and role name.
//
// Runtime polymorphism (the key demo of the project):
//
//     User* current = authService.login(name, pass);
//     current->displayMenu();   // prints the Customer OR the Admin menu,
//                               // decided at run time
//
// File record format (one user per line, used by FileManager):
//     userID,username,password,role
//     U001,parvej,1234,customer
//     U002,admin,admin123,admin
//
// (Passwords are saved as plain text, as in the Week 11 lecture. A real
//  application would store a hash instead.)
// ---------------------------------------------------------------
class User {
private:
    std::string id;         // e.g. "U001"
    std::string username;   // must not contain a comma
    std::string password;   // must not contain a comma

public:
    // Throws ShopException if any field is empty or contains a comma.
    User(const std::string& id,
         const std::string& username,
         const std::string& password);

    virtual ~User() = default;      // needed so deleting a User* deletes the
                                    // real Customer/Admin object correctly

    // ----- getters -----
    const std::string& getId() const { return id; }
    const std::string& getUsername() const { return username; }

    // The password has no getter: other classes only ask "is this right?"
    bool checkPassword(const std::string& attempt) const { return password == attempt; }

    // ----- role-specific behavior (each subclass must provide these) -----
    virtual void displayMenu() const = 0;
    virtual std::string getRole() const = 0;     // "customer" or "admin"

    // ----- file format -----
    std::string toLine() const;                  // "U001,parvej,1234,customer"

    // Builds a Customer or an Admin from one line of users.txt, depending
    // on the role field. Throws ShopException if the line is malformed or
    // the role is unknown.
    static std::unique_ptr<User> fromLine(const std::string& line);
};