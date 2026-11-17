# Online Shop & Inventory Management System

A command-line shop written in C++ for **Object-Oriented Programming II**
(Department of Intelligence Computing, Dong-eui University, Fall 2026).

Customers browse products, fill a shopping cart and place orders. An
administrator manages products and stock. Everything (users, products,
orders) is saved in text files, so the data is still there the next time the
program runs.

**Instructor:** Asrat Kaleab Taye

## Team

| Member | Student ID | Responsibility (from the project proposal) |
|---|---|---|
| Jubayer Parvej Ahmed | 20253655 | `User`, `Customer`, `Admin`, `AuthService` |
| AJM Sazzad | 20253660 | `Product`, `Inventory` |
| Fahim Md Maktubat Shailkh | 20253622 | `Cart`, `CartItem`, `Order`, `OrderItem` |
| Tonmoy Abrar Jahin | 20253621 | `ShopApp`, `FileManager`, `ShopException` |

## Features

**Customer**

- Sign up and log in
- View all products and search by name (not case sensitive)
- Add products to a cart, remove them, view the cart and its total
- Checkout with a confirmation and a printed receipt
- View their own previous orders

**Administrator**

- Log in
- View the inventory
- Add, update and remove products
- Set the stock quantity of a product
- View all orders of all customers

**System**

- Role-based menus (the Customer and the Admin see different menus)
- Stock is checked when an item is added to the cart and again at checkout,
  so a product can never be oversold
- Users, products and orders are saved to files and loaded on startup
- Bad input and file problems are handled with exceptions; the program does
  not crash

## Requirements

- A C++17 compiler: `clang++` (macOS) or `g++` 8 or newer (Linux, Windows
  with MinGW). C++17 is needed for `<filesystem>`.
- No other libraries.

## Build and run

Open a terminal in the **project root** (the folder that contains `main.cpp`).

```bash
mkdir -p output
clang++ -std=c++17 -Wall -Wextra -g3 -Iinclude main.cpp src/*.cpp -o output/main
./output/main
```

(`g++` works the same way; just replace `clang++` with `g++`.)

**Always run the program from the project root.** The data files are found
through the relative path `data/`, so starting the program from another folder
would create a second, empty `data/` folder there.

### Default data

- **First run:** if there is no `data/products.txt`, three sample products
  are created (Laptop, Mouse, Keyboard).
- **Default administrator:** if there are no users at all, one admin account
  is created:

  | Username | Password |
  |---|---|
  | `admin` | `admin123` |

- **Reset everything:** stop the program and delete the `data/` folder. The
  next run starts fresh with the defaults above.

## How to use it

```
1. Signup  2. Login  3. Quit
```

Sign up creates a **customer** account. After logging in, the menu depends on
the role.

**Customer menu**

```
1. View products          5. View cart
2. Search products        6. Checkout
3. Add product to cart    7. View my orders
4. Remove product         8. Logout
```

**Admin menu**

```
1. View inventory         5. Update stock
2. Add product            6. View all orders
3. Update product         7. Logout
4. Remove product
```

### A short demo

1. Log in as `admin` / `admin123`, add a product, then log out.
2. Sign up as a customer and log in.
3. Search for the new product, add 2 of it to the cart, view the cart.
4. Try to add more than the stock: the program refuses.
5. Checkout and read the receipt. Log out and quit.
6. Start the program again: the order and the reduced stock are still there.
   Log in as `admin` and choose "View all orders".

## Project structure

```
OnlineShop/
├── main.cpp                 starts ShopApp
├── include/                 header files (class declarations)
│   ├── Exceptions.h         ShopException, OutOfStockException
│   ├── StringUtils.h        helpers for reading file records
│   ├── Product.h  Inventory.h
│   ├── User.h  Customer.h  Admin.h  AuthService.h
│   ├── CartItem.h  Cart.h
│   ├── OrderItem.h  Order.h
│   ├── FileManager.h
│   └── ShopApp.h
├── src/                     implementations (one .cpp per class)
├── data/                    saved data, created when the program runs
│   ├── users.txt
│   ├── products.txt
│   └── orders.txt
├── docs/
│   └── UML_Class_Diagram.pdf
└── README.md
```

## Design

### Classes

The program has 12 core classes and 2 custom exceptions.

| Class | What it does |
|---|---|
| `ShopApp` | Coordinator: loads the data, runs the login screen and the menus, saves changes |
| `User` | **Abstract** base class: id, username, password, and the pure virtual `displayMenu()` and `getRole()` |
| `Customer` | A user who shops. Owns one `Cart`. `checkout()` turns the cart into an `Order` |
| `Admin` | A user who manages the shop: add, update, remove products, set stock |
| `AuthService` | Signup and login. Owns all `User` objects (`vector<unique_ptr<User>>`) |
| `Product` | One product: id, name, price, stock. Refuses invalid values |
| `Inventory` | The collection of products and the **single source of truth for stock** |
| `CartItem` | One line in a cart: product, price, quantity |
| `Cart` | The items a customer wants to buy. Checks stock when adding |
| `OrderItem` | One line of a finished order, with the **price paid** (snapshot) |
| `Order` | A finished purchase: id, customer, date, items, total, receipt |
| `FileManager` | The only class that opens files. Saves and loads products, users and orders |
| `ShopException` | Base class of the shop's own errors (inherits `std::runtime_error`) |
| `OutOfStockException` | Not enough stock; inherits `ShopException`, knows the product and the numbers |

### Object-oriented concepts

| Concept | Where it is used |
|---|---|
| **Encapsulation** | Data members are private. `Product` and `CartItem` validate every change; `User` has no password getter, only `checkPassword()` |
| **Inheritance** | `Customer` and `Admin` inherit from `User`; `OutOfStockException` inherits from `ShopException` |
| **Abstraction** | `User` cannot be created directly; it only declares what every user must provide |
| **Runtime polymorphism** | `User* current = authService.login(...); current->displayMenu();` prints the Customer or the Admin menu, decided at run time |
| **Composition** | A `Customer` owns a `Cart`; a `Cart` contains `CartItem`s; an `Order` contains `OrderItem`s |
| **Smart pointers** | `AuthService` owns users with `unique_ptr`, so there is no manual `delete`; `login()` returns a non-owning `User*` |

### Exception handling

```
std::exception
  └── std::runtime_error
        └── ShopException            unknown product, bad quantity, file problems, ...
              └── OutOfStockException   more requested than in stock
```

- `ShopApp` catches the most specific exception first:
  `OutOfStockException`, then `ShopException`, then `std::exception`.
- A wrong password is **not** an exception: `login()` returns `nullptr`.
- A damaged data file stops the program at startup with the file name and
  line number, for example
  `Startup error: data/products.txt line 3: Invalid price in product record: 'abc'`.

### File I/O

All file access goes through `FileManager`. The other classes only know how
to turn themselves into one line of text (`toLine()`) and back (`fromLine()`).

| File | Format of one line | Example |
|---|---|---|
| `data/users.txt` | `userID,username,password,role` | `U002,kim,abc123,customer` |
| `data/products.txt` | `productID,name,price,stock` | `P001,Laptop,750000,10` |
| `data/orders.txt` | `orderID,customerID,date,productID:qty:unitPrice;...` | `O001,U002,2026-11-17,P001:2:750000;P002:1:15000` |

- New users and new orders are **appended** (`ios::app`), so old records are
  never erased.
- The products file is rewritten as a whole. It is first written to a
  temporary file and then renamed, so a failure half-way never destroys the
  old file.
- Every file is checked with `is_open()`; a missing data file on the first run
  is not an error.
- Names and passwords cannot contain a comma, because the files use commas as
  separators.

### Checkout

`Customer::checkout()` changes nothing unless everything is valid:

1. The cart must not be empty.
2. **Every** item is checked against the stock (before any stock is deducted).
3. If a price changed since the item was added, the cart is updated and the
   customer reviews the new total.
4. The `Order` is created, with the price of each item saved.
5. Only then is the stock deducted and the cart emptied.

`ShopApp` then saves the order and the new stock.

Because orders keep the price paid, changing a product's price later does not
change old orders.

## Testing checklist

Start from a fresh `data/` folder (delete it), then check:

- [ ] Wrong password shows "Login failed."
- [ ] Signup with an existing username, an empty username, or a comma in the
      username is refused
- [ ] A new customer can log in after the program is restarted
- [ ] Admin: add, update, remove a product and change the stock; restart and
      confirm the changes were saved
- [ ] Customer: search finds products whatever the letter case
- [ ] Adding more than the stock is refused (also when added in two steps)
- [ ] Typing letters instead of a number shows an error and the program goes on
- [ ] Checkout on an empty cart is refused
- [ ] After checkout the stock is lower, the cart is empty, a receipt is shown
- [ ] After a restart, the order is still listed for the customer and for the
      admin
- [ ] Editing a data file by hand into a wrong format gives a clear startup
      error with the line number

## Known limitations

- Passwords are stored as **plain text**, as in the Week 11 lecture. A real
  application would store a hash.
- A cart exists only while the program runs. It stays when the user logs out
  and in again, but it is lost when the program ends. Orders are saved; carts
  are not.
- It is a single-user console program: one person at a time.
- There are no payments, no delivery and no website; the scope is the command
  line application.
- Fields in the data files cannot contain a comma (and product IDs cannot
  contain `:` or `;`).

## Working with Git

```bash
git pull                      # get the latest work before you start
git add include src main.cpp  # stage your changes
git commit -m "Short, clear message"
git push
```

Only push code that builds. Put `output/` in `.gitignore` so the compiled
program is not committed.