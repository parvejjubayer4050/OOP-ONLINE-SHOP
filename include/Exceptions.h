#pragma once

#include <stdexcept>
#include <string>

// ---------------------------------------------------------------
// Exceptions.h
//
// The two custom exception classes from the UML diagram.
// Header-only: these classes are small, so no Exceptions.cpp is needed.
//
//   std::exception
//     └── std::runtime_error
//           └── ShopException            (base for every shop error)
//                 └── OutOfStockException (not enough stock to buy)
//
// Note: we write std:: here instead of "using namespace std;".
// Never put "using namespace std;" in a header file, because every
// .cpp file that includes the header would be forced to use it too.
// ---------------------------------------------------------------


// ShopException
// General-purpose error for the whole application.
// Because it inherits from std::runtime_error, it already stores the
// message and provides what(), so we do not write our own.
//
// Where the team will throw it:
//   - FileManager : a file cannot be opened       -> ShopException
//   - Inventory   : product ID not found          -> ShopException
//   - Cart        : quantity is zero or negative  -> ShopException
//   - Product     : negative price or stock       -> ShopException
class ShopException : public std::runtime_error {
public:
    explicit ShopException(const std::string& message)
        : std::runtime_error(message) {}
};


// OutOfStockException
// Thrown when a customer asks for more of a product than is in stock.
// It is a ShopException, so "catch (const ShopException&)" also catches it.
//
// It remembers the details (which product, how many were requested, how
// many are available) so the menu code can show a helpful message
// without parsing the text of what().
//
// Where the team will throw it:
//   - Inventory::checkStock() / Cart::addItem() / checkout validation
class OutOfStockException : public ShopException {
private:
    std::string productName;
    int requested;
    int available;

public:
    OutOfStockException(const std::string& productName,
                        int requested,
                        int available)
        : ShopException("Not enough stock for '" + productName +
                        "': requested " + std::to_string(requested) +
                        ", available " + std::to_string(available)),
          productName(productName),
          requested(requested),
          available(available) {}

    const std::string& getProductName() const noexcept { return productName; }
    int getRequested() const noexcept { return requested; }
    int getAvailable() const noexcept { return available; }
};