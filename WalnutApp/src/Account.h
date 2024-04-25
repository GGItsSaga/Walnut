#pragma once
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <string>
#include <fstream> 

class Account {
private:
    std::string username;
    std::string password;

    bool isValidPassword(const std::string& pwd) const {
        return pwd.length() >= 6; // Simple check for password length
    }

    // Helper method to persist data
    void saveToFile() const {
        std::ofstream outFile("accounts.txt", std::ios::app); // Append mode
        if (outFile) {
            outFile << username << " " << password << std::endl;
        }
        outFile.close();
    }

public:
    Account() : username(""), password("") {}

    Account(const std::string& username, const std::string& password)
        : username(username), password(password) {}

    bool createAccount() {
        std::cout << "Enter your account name: ";
        std::getline(std::cin, username);

        std::string check;
        bool valid = false;

        while (!valid) {
            std::cout << "\nEnter your password (8 characters minimum): ";
            std::getline(std::cin, password);
            std::cout << "\nRe-enter your password: ";
            std::getline(std::cin, check);

            if (check != password) {
                std::cout << "\nPasswords do not match, try again\n";
                continue;
            }
            if (!isValidPassword(password)) {
                std::cout << "\nPassword does not meet the requirements, try again\n";
                continue;
            }
            valid = true;
            saveToFile(); // Save the account data to file
        }

        return valid;
    }

    bool login() {
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);
        std::cout << "Enter your password: ";
        std::getline(std::cin, password);

        std::ifstream inFile("accounts.txt");
        std::string storedUser, storedPass;

        while (inFile >> storedUser >> storedPass) {
            if (storedUser == username && storedPass == password) {
                return true; // Login successful
            }
        }

        return false; // Login failed
    }

    void displayAccount() const {
        std::cout << "Account Username: " << username << std::endl;
    }
};

#endif // ACCOUNT_H
