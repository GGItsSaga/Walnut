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
    std::string errorMessage;

    bool isValidPassword(const std::string& pwd) const 
    {
        return pwd.length() >= 6; // Simple check for password length
    }

    void saveToFile() const 
    {
        std::ofstream outFile("accounts.txt", std::ios::app); // Append mode
        if (!outFile.is_open()) 
        {
            ImGui::OpenPopup("Error Opening File");
            //errorMessage = "Error: Failed to open file for writing.";
            return;
        }

        try 
        {
            outFile << username << " " << password << std::endl;
            outFile.close(); // Close the file after writing
        }
        catch (const std::exception& e) 
        {
            ImGui::OpenPopup("Error Writing File");
            //errorMessage = "Error while writing to file: " + std::string(e.what());
            outFile.close(); // Make sure to close the file in case of an exception
        }
    }

public:
    Account() : username(""), password("") {}

    std::string createAccount(const std::string& usr, const std::string& pwd)
    {
        // Check if the username already exists. If it's taken, return an error message.
        if (isUsernameTaken(usr))
        {
            return "Username taken";
        }

        username = usr;
        password = pwd;

        saveToFile(); // Save the account data to file
        return "Account created successfully";
    }
    
    // Checks if the username is taken or not. If it is, output an error and ask them to re-enter.
    bool isUsernameTaken(const std::string& usr) const
    {
        std::ifstream inFile("accounts.txt");
        std::string storedUser, storedPass;

        while (inFile >> storedUser >> storedPass)
        {
            if (storedUser == usr)
            {
                inFile.close();
                return true; // Username is already taken
            }
        }

        inFile.close();
        return false; // Username is not taken
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
