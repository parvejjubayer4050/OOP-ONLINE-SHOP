#include "User.h"
#include "Admin.h"
#include "Customer.h"
#include "Exceptions.h"

#include <vector>
using namespace std;

namespace {

// id, username and password are stored in a comma-separated file, so they
// must be non-empty and must not contain a comma.
void checkField(const string& value, const string& fieldName) {
    if (value.empty())
        throw ShopException("User " + fieldName + " cannot be empty");
    if (value.find(',') != string::npos)
        throw ShopException("User " + fieldName + " cannot contain a comma");
}

// "a,b,c,d" -> {"a","b","c","d"}  (keeps empty fields, so "a,b,," has 4 parts)
vector<string> splitByComma(const string& line) {
    vector<string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = line.find(',', start);
        if (pos == string::npos) {
            parts.push_back(line.substr(start));
            break;
        }
        parts.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

} // namespace


User::User(const string& id, const string& username, const string& password) {
    checkField(id, "ID");
    checkField(username, "username");
    checkField(password, "password");

    this->id = id;
    this->username = username;
    this->password = password;
}

string User::toLine() const {
    return id + "," + username + "," + password + "," + getRole();
}

unique_ptr<User> User::fromLine(const string& line) {
    string clean = line;
    // Ignore a trailing '\r' (files saved with Windows line endings)
    if (!clean.empty() && clean.back() == '\r')
        clean.pop_back();

    vector<string> parts = splitByComma(clean);
    if (parts.size() != 4)
        throw ShopException("Bad user record (expected 4 fields): '" + clean + "'");

    const string& id = parts[0];
    const string& username = parts[1];
    const string& password = parts[2];
    const string& role = parts[3];

    // The role field decides which kind of object to create.
    // The constructors do the final validation (empty / comma checks).
    if (role == "customer")
        return make_unique<Customer>(id, username, password);
    if (role == "admin")
        return make_unique<Admin>(id, username, password);

    throw ShopException("Unknown user role '" + role + "' in record: '" + clean + "'");
}