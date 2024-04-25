#pragma once
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <string>
#include <fstream>

class Account 
{
private:
    std::string username;
    std::string password;

    bool isValidPassword(const std::string& pwd) const 
    {
        return pwd.length() >= 6; // Simple check for password length
    }

    void saveToFile() const 
    {
        std::ofstream outFile("accounts.txt", std::ios::app); // Append mode
        if (!outFile.is_open()) 
        {
            std::cerr << "Error: Failed to open file for writing." << std::endl;
            throw std::runtime_error("Failed to open file for writing.");
        }

        try 
        {
            outFile << username << " " << password << std::endl;
            outFile.close(); // Close the file after writing
        }
        catch (const std::exception& e) 
        {
            std::cerr << "Error while writing to file: " << e.what() << std::endl;
            outFile.close(); // Make sure to close the file in case of an exception
            throw; // Re-throw the exception for the caller to handle
        }
    }

public:
    Account() : username(""), password("") {}

    bool createAccount(const std::string& usr, const std::string& pwd) 
    {
        username = usr;
        password = pwd;

        if (!isValidPassword(password)) 
        {
            throw std::invalid_argument("Password does not meet the requirements.");
        }

        saveToFile(); // Save the account data to file
        return true;
    }

    bool login(const std::string& usr, const std::string& pwd) 
    {
        std::ifstream inFile("accounts.txt");
        std::string storedUser, storedPass;

        while (inFile >> storedUser >> storedPass) 
        {
            if (storedUser == usr && storedPass == pwd) 
            {
                username = usr;
                password = pwd;
                inFile.close();
                return true; // Login successful
            }
        }

        inFile.close();
        return false; // Login failed
    }
};

#endif // ACCOUNT_H
